#include "Emulator.hpp"

using namespace emu; 
using namespace emu::gameboy;

Emulator::Emulator(Type type): _memory(new GameboyMemory)
{
    /// Only support one type of emulator for now. 
    assert(type == Type::gameboy);
    log_error("Emulator version %d.%d", kSoftwareVersionMajor, kSoftwareVersionMinor);
    
}

Emulator::~Emulator() { }
    
bool 
Emulator::Activate(const char *gameFile)
{
    require_or(_memory->activate(gameFile), return false);
    
    _ppu.emplace(_memory);
    _ppu->configureInterrupts(kConfigVBlankInterruptFlag, kConfigLCDInterruptFlag);
    _ppu->reset();
    _ppu->disable();

    _control.emplace(_memory);
    _control->configureInterrupt(kConfigJoypadInterruptFlag);
    _control->setInterruptCallback([this](){ _cpu->wake(); });
    
    _timer.emplace(_memory, kConfigTimerInterruptFlag);
    
    _cpu.emplace(_memory);
#if (EMULATOR_LOG_LEVEL == kLogLevelDebug)
    _cpu->printInstructions();
#endif // (EMULATOR_LOG_LEVEL == kLogLevelDebug)
    
    return true;
}

void 
Emulator::Run()
{
    auto instruction = _cpu->getNextInstruction();

    uint8_t mCycles = instruction->getCycles();
    _ppu->mCycleUpdate(mCycles);
    _timer->mCycleUpdate(mCycles);
    _memory->mCycleUpdate(mCycles);

    mCycles = _cpu->executeNextInstruction(instruction);
    _ppu->mCycleUpdate(mCycles);
    _timer->mCycleUpdate(mCycles);
    _memory->mCycleUpdate(mCycles);
}

void
Emulator::DumpState()
{
    _memory->dumpToFile();
    _cpu->printState(CentralProcessor::printStateOption::compact);
}
