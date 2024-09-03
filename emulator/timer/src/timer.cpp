#include "timer.h"

using namespace std;
using namespace emu::gameboy;

GameboyTimer::GameboyTimer(shared_ptr<GameboyMemory> memory, uint8_t interruptFlag)
{
    _memory = memory;
    _timerExpiredInterruptFlag = interruptFlag;
    _registers = &memory->getIORegisters()->timer;
    uint8_t clockSelect = _registers->control.clockSelect;
    _absoluteCounter = convertClockSelectToCount(clockSelect);
    
    auto registerWriteOccured = [&](uint16_t address, __unused optional<uint8_t>) {
        switch (address)
        {
        case kDividerRegister:
            _registers->divider = 0;
            break;
        case kControlRegister:
            _absoluteCounter = convertClockSelectToCount(_registers->control.clockSelect);
        case kCounterRegister:
        case kModuloRegister:
            break;
        default:
            assert(0);
        }
    };
    _ioSignalerId = _memory->registerIOSignaler(registerWriteOccured, kDividerRegister, 0xFFFC);
}

GameboyTimer::~GameboyTimer()
{ 
    _memory->deregisterIOSignaler(_ioSignalerId);
}

void 
GameboyTimer::mCycleUpdate(uint8_t mCycles)
{
    const uint8_t dividerMaxValue = kConfigMCycleHz(kConfigSingleSpeedMode) / 16384;
    
    // Divider logic
    _dividerCounter += mCycles;
    if (_dividerCounter >= dividerMaxValue)
    {
        _dividerCounter -= dividerMaxValue;
        _registers->divider++;
    }
    
    // Counter logic
    require_or(_registers->control.enable, return);

    bool underFlow = sub_overflow(_absoluteCounter, static_cast<uint64_t>(mCycles), &_absoluteCounter);
    require_or(_absoluteCounter == 0 || underFlow, return);
    
    const uint64_t countMaxValue = convertClockSelectToCount(_registers->control.clockSelect);
double_expire:
    _absoluteCounter += countMaxValue;
    if ((++_registers->counter) == 0)
    {
        _registers->counter = _registers->modulo;
        _memory->requestInterrupt(_timerExpiredInterruptFlag);
    }
    require_or((_absoluteCounter != 0) && (_absoluteCounter <= countMaxValue), goto double_expire);
}

/* static */
uint64_t 
GameboyTimer::convertClockSelectToCount(uint8_t clockSelect)
{
    static constexpr uint16_t kCounterIncrementTable[] = {
        [0] = 256,
        [1] = 4,
        [2] = 16,
        [3] = 64,
    };
    return static_cast<uint64_t>(kCounterIncrementTable[clockSelect]);
}
