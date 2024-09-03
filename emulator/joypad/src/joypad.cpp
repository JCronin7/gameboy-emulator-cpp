#include "joypad.h"

using namespace emu;
using namespace emu::gameboy;

void
Control::decodeDirectionKey(char key)
{
    switch (to_upper(key))
    {
    case kConfigJoypadLeft:
        _directionInputs.left ^= 1;
        return;
    case kConfigJoypadUp:
        _directionInputs.up ^= 1;
        return;
    case kConfigJoypadRight:
        _directionInputs.right ^= 1;
        return;
    case kConfigJoypadDown:
        _directionInputs.down ^= 1;
    default:
        return;
    }
}

void
Control::decodeActionKey(char key)
{
    switch (to_upper(key))
    {
    case kConfigJoypadSelect:
        _actionInputs.select ^= 1;
        return;
    case kConfigJoypadStart:
        _actionInputs.start ^= 1;
        return;
    case kConfigJoypadA:
        _actionInputs.a ^= 1;
        return;
    case kConfigJoypadB:
        _actionInputs.b ^= 1;
    default:
        return;
    }
}

Control::Control(std::shared_ptr<GameboyMemory> memory): _memory(memory)
{
    auto updateFn = [&](char key, __unused int, __unused int)
    {
        decodeDirectionKey(key);
        decodeActionKey(key);
    
        controlBase regState = { .base = *_register };
        require_or(regState.directionSelect || regState.actionSelect, return);

        uint8_t oldButtonState = regState.buttons;
        if (regState.directionSelect == false)
        {
            *_register |= _directionInputs.base & 0xF;
        }
        else if (regState.actionSelect == false)
        {
            *_register |= _actionInputs.base & 0xF;
        }

        // High to low transition
        regState.base = *_register;
        if ((regState.buttons ^ oldButtonState) & oldButtonState)
        {
            assert(_interruptFlag != std::nullopt)
            _memory->requestInterrupt(*_interruptFlag);
            if (_interruptOccured)
            {
                _interruptOccured();
            }
        }
    };
    HAL_DisplaySetKeyboardEventCallback(updateFn);
    _register = &(memory->getIORegisters()->ctrl);
    *_register = UINT8_MAX;

    memory->registerIOSignaler([this](__unused uint16_t, __unused std::optional<uint8_t>) {
        //assert(joypadInputRegister->actionSelect || joypadInputRegister->directionSelect);
        controlBase regState = { .base = *_register };
        if (regState.actionSelect == 0)
        {
            *_register |= _actionInputs.base & 0xF;
        }
        else if (regState.directionSelect == 0)
        {
            *_register |= _directionInputs.base & 0xF;
        }
    }, kControlRegister, UINT16_MAX);
}
