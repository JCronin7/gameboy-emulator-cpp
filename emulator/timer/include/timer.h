#ifndef _TIMER_H_
#define _TIMER_H_

#include "platform.h"
#include "mmu.h"

namespace emu::gameboy {
class GameboyTimer
{
private:
    /// @brief Timer_MCycleStep is called at (1 << 22) Hz, but needs to increment DIV at (1 << 14) Hz
    static constexpr uint32_t kStepDivision     = 1 << (22 - 14);
    
    static constexpr uint16_t kDividerRegister  = 0xFF04;
    static constexpr uint16_t kCounterRegister  = 0xFF05;
    static constexpr uint16_t kModuloRegister   = 0xFF06;
    static constexpr uint16_t kControlRegister  = 0xFF07;
    
    static constexpr uint8_t kDividerMaxMCycleValue = 64;
    
public:
    GameboyTimer(std::shared_ptr<GameboyMemory> memory, uint8_t interruptFlag);
    ~GameboyTimer();
    
    void mCycleUpdate(uint8_t mCycles);

private:
    static uint64_t convertClockSelectToCount(uint8_t clockSelect);
    
private:
    std::shared_ptr<GameboyMemory>  _memory;
    io::registers::timerRegisters * _registers;
    uint64_t                        _absoluteCounter = 256ull;
    uint8_t                         _dividerCounter = 0;
    uint8_t                         _timerExpiredInterruptFlag = 0;
    uint8_t                         _ioSignalerId = 0;
};
} // namespace emu::gameboy

#endif /* _TIMER_H_ */
