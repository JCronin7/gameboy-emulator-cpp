#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include "hal.h"
#include "mmu.h"
#include "platform.h"

namespace emu::gameboy {
class Control
{
public:
    using interruptCallback = std::function<void(void)>;

private:
    using controlBase = io::ctrl::controlBase;
    using controlDirectionInputs = io::ctrl::controlDirectionInputs;
    using controlActionInputs = io::ctrl::controlActionInputs;
    
private:
    static constexpr controlBase kDefaultState = { .buttons = 0xF, .actionSelect = 1, .directionSelect = 1 };
    static constexpr uint16_t kControlRegister = 0xFF00;

public:
    Control(std::shared_ptr<GameboyMemory> memory);
    ~Control() { }
    
    void configureInterrupt(uint8_t interruptFlag) { _interruptFlag = interruptFlag; }
    void setInterruptCallback(interruptCallback cb) { _interruptOccured = cb; }
    
private:
    void decodeDirectionKey(char key);
    void decodeActionKey(char key);

private:
    std::shared_ptr<GameboyMemory> _memory = nullptr;
    uint8_t *                      _register = nullptr;
    controlDirectionInputs         _directionInputs;
    controlActionInputs            _actionInputs;
    interruptCallback              _interruptOccured;
    std::optional<uint8_t>         _interruptFlag = std::nullopt;
};
} // namespace emu::gameboy

#endif /* _JOYPAD_H_ */