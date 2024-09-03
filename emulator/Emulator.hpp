#ifndef emu__Emulator_hpp
#define emu__Emulator_hpp

#include "platform.h"

#include "apu.h"
#include "cpu.h"
#include "hal.h"
#include "joypad.h"
#include "mmu.h"
#include "ppu.h"
#include "timer.h"

namespace emu {
class Emulator
{
public:
    enum class Type: uint8_t
    {
        gameboy,
    };
public:
    Emulator(Type type);
    ~Emulator();

    bool Activate(const char *gameFile);
    void Run();
    void DumpState();

private:
    std::shared_ptr<gameboy::GameboyMemory>  _memory;
    std::optional<gameboy::CentralProcessor> _cpu;
    std::optional<gameboy::PixelProcessor>   _ppu;
    std::optional<gameboy::Control>          _control;
    std::optional<gameboy::GameboyTimer>     _timer;
};
} // namespace emu

#endif /* emu__Emulator_hpp */