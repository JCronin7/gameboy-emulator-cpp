/**
 * @file cpu_instr.h
 * @author Jake Cronin (jakecronin97@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _CPU_INSTR_H_
#define _CPU_INSTR_H_

#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "mmu.h"
#include "platform.h"

namespace emu::gameboy::instruction {
class InstructionSet
{
public:
    InstructionSet(uint16_t size): _instructions(new instruction[size]), _instructionsSize(size) { }
    ~InstructionSet() { delete[] _instructions; }
    
    const instruction *getInstruction(uint8_t opcode) const 
    { 
        assert(opcode < _instructionsSize); 
        return &(_instructions[opcode]);
    };
    
    void printAll() const
    {
        for (uint16_t i = 0; i < _instructionsSize; i++)
        {
            _instructions[i].print(i);
        }
    }
    
protected:
    static uint8_t truncate(uint16_t value) { return static_cast<uint8_t>(value & 0xFF); };
    static uint16_t extend(uint8_t value) { return static_cast<uint16_t>(value); }
    static int16_t sign_extend(uint8_t value) { return static_cast<int16_t>(static_cast<int8_t>(value)); }
    
protected:
    instruction *   _instructions = nullptr;
    size_t          _instructionsSize = 0;
};

class GameboyInstructionSet8bit: public InstructionSet
{
public:
    static constexpr uint16_t kInstructionArraySize = 256;

public:
    GameboyInstructionSet8bit();
    ~GameboyInstructionSet8bit() { }
    
private:
    /// @brief Move a numerical value into a register.
    void initMoveImmediateGroup();
    /// @brief Move the contents of a register into another.
    void initMoveRegisterGroup();
    /// @brief Read memory at the address in HL, and write the value to a register.
    void initLoadHLGroup();
    /// @brief Write the value in a register to the address stored in HL.
    void initStoreHLGroup();
    /// @brief Read memory and store the result in register A. 
    void initLoadAGroup();
    /// @brief Write the value in register A to memory.
    void initStoreAGroup();
    /// @brief Move an integer value into a wide register.
    void initMoveImmediate16bitGroup();
    /// @brief Special load/store instructions. 
    void initLoadStoreSpecial();
    /// @brief Push a value onto the stack.
    void initPushGroup();
    /// @brief Pop a value off the stack. 
    void initPopGroup();
    /// @brief Add a value to register A and set flags. 
    void initAddGroup();
    /// @brief Add a value (plus carry) to register A and set flags. 
    void initAdcGroup();
    /// @brief Subtract a value from register A and set flags. 
    void initSubGroup();
    /// @brief Subtract a value (minus carry) from register A and set flags. 
    void initSbcGroup();
    /// @brief Perform logical AND with register A and another value. 
    void initAndGroup();
    /// @brief Perform logical OR with register A and another value. 
    void initOrGroup();
    /// @brief Perform logical XOR with register A and another value. 
    void initXorGroup();
    /// @brief Compare register A with another value, sets flags and discards result. 
    void initCpGroup();
    /// @brief Increment an 8-bit register value. 
    void initIncGroup();
    /// @brief decrement an 8-bit register value. 
    void initDecGroup();
    /// @brief Perform 16-bit addition.
    void initAdd16bitGroup();
    /// @brief Increment a 16-bit register.
    void initInc16bitGroup();
    /// @brief Decrement a 16-bit register.
    void initDec16bitGroup();
    /// @brief Perform decimal adjust on register A.
    void initDaa();
    void initCpl();
    void initCcf();
    void initScf();
    void initNop();
    void initHalt();
    void initInterruptControlGroup();
    /// @brief Perform left/right bit rotations on register A.
    void initRotateAGroup();
    void initJpGroup();
    void initJrGroup();
    void initCallGroup();
    void initRstGroup();
    void initReturnGroup();
};

class GameboyInstructionSet16bit: public InstructionSet
{
public:
    static constexpr uint8_t kInstructionSetCode = 0xCB;
    static constexpr uint16_t kInstructionArraySize = 256;

public:
    GameboyInstructionSet16bit();
    ~GameboyInstructionSet16bit() { }
    
private:
    void initSwapGroup();
    
    void initRlcGroup();
    void initRlGroup();
    void initRrcGroup();
    void initRrGroup();
    void initSlaGroup();
    void initSraGroup();
    void initSrlGroup();
    
    void initBitGroup();
    void initSetGroup();
    void initResGroup();
};

class GameboyInstructionSetStop: public InstructionSet
{
public:
    static constexpr uint8_t kInstructionSetCode = 0x10;
    static constexpr uint16_t kInstructionArraySize = 1;

public:
    GameboyInstructionSetStop();
    ~GameboyInstructionSetStop() { }
};

} // namespace emu::gameboy::instruction

#endif /* _CPU_INSTR_H_ */