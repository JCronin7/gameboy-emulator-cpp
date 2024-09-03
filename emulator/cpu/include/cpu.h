#ifndef _CPU_H_
#define _CPU_H_

#include "platform.h"
#include "mmu.h"

namespace emu::gameboy::instruction {
class instruction;
class GameboyInstructionSet8bit;
class GameboyInstructionSet16bit;
class GameboyInstructionSetStop;
} // namespace emu::gameboy::instruction

namespace emu::gameboy {
namespace detail::cpu {
struct flags
{
    uint8_t             : 4;
    uint8_t carry       : 1;
    uint8_t halfCarry   : 1;
    uint8_t subtract    : 1;
    uint8_t zero        : 1;
}; 
static_assert(sizeof(flags) == 1);
struct registers
{
public:
    union { struct { uint8_t c; uint8_t b; }; uint16_t bc = 0; };
    union { struct { uint8_t e; uint8_t d; }; uint16_t de = 0; };
    union { struct { uint8_t l; uint8_t h; }; uint16_t hl = 0; };
    union { struct { flags   f; uint8_t a; }; uint16_t af = 0; };
    uint16_t stackPointer = 0xFFFE;
    uint16_t programCounter = 0;
    
    registers(): bc(0), de(0), hl(0), af(0), stackPointer(0xFFFE), programCounter(0) { }
    void setFlags(bool carry, bool halfCarry, bool subtract, bool zero)
    {
        f.halfCarry = halfCarry;
        f.carry = carry;
        f.subtract = subtract;
        f.zero = zero;
    }
};
class DebugObject
{
private:
    static constexpr uint32_t kMaxDebugRecords = 64;
    
public:
    struct Record
    {
        char nameBuffer[16];
        registers registerState;
        
        Record(const char *name, registers registerState, std::optional<uint16_t> arg = std::nullopt)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
            (arg == std::nullopt) ? snprintf(nameBuffer, 16, name) : snprintf(nameBuffer, 10, name, *arg);
            this->registerState = registerState;
#pragma clang diagnostic pop
        }
    };

public:
    void insertRecord(const char *name, registers registerState, std::optional<uint16_t> arg = std::nullopt)
    {
        if (_records.size() == kMaxDebugRecords)
        {
            _records.pop();
        }
        _records.push(std::shared_ptr<Record>(new Record(name, registerState, arg)));
    }
    
    std::shared_ptr<Record> getNextRecord()
    { 
        require_or(_records.size() > 0, return nullptr);
        auto retval = _records.front();
        _records.pop();
        return retval;
    }

private:
    std::queue<std::shared_ptr<Record>> _records;
};
}

class CentralProcessor
{
public:
    using registers = detail::cpu::registers;
    using DebugObject = detail::cpu::DebugObject;

private:
    using GameboyInstructionSet8bit = instruction::GameboyInstructionSet8bit;
    using GameboyInstructionSet16bit = instruction::GameboyInstructionSet16bit;
    using GameboyInstructionSetStop = instruction::GameboyInstructionSetStop;

private:
    static constexpr uint8_t kInterruptMCycles = 5;
    static constexpr uint8_t kHaltMCycles = 1;

public:
    enum class interruptState : uint8_t
    {
        disabled,
        enabling,
        enabled,
    };
    enum class printStateOption : uint8_t
    {
        none,
        compact,
        full,
    };

public:
    CentralProcessor(std::shared_ptr<GameboyMemory> memory);
    ~CentralProcessor();

    void reset();
    void wake(); 

    void printInstructions();
    void printState(printStateOption opt);

    const ::emu::gameboy::instruction::instruction *getNextInstruction();
    uint8_t executeNextInstruction(const ::emu::gameboy::instruction::instruction *instruction);
    void dotCycleUpdate();
    
    registers& getRegisters() { return _registers; }
    void setInterruptState(interruptState newState) { _interruptState = newState; }
    
    std::shared_ptr<GameboyMemory> getMemoryManager() { return _memory; }
    
    void pushToStack(uint16_t value);
    uint16_t popFromStack();
    
    void setBranchPenalty(uint8_t branchPenalty) { _branchPenalty = branchPenalty; }

private:
    bool checkForInterrupt();
    uint8_t readProgramCounter() { return _memory->read(_registers.programCounter++); }
    
private:
    std::shared_ptr<GameboyMemory>      _memory;
    /// @todo Need to update instruction set
    const GameboyInstructionSet8bit *   _instructions8bit = nullptr;
    const GameboyInstructionSet16bit *  _instructions16bit = nullptr;
    const GameboyInstructionSetStop *   _stopInstruction = nullptr;
    registers                           _registers;
    uint64_t                            _mCycleCount = 0;
    interruptState                      _interruptState = interruptState::disabled;
    uint8_t                             _branchPenalty = 0;
    bool                                _halt = false;
    bool                                _power = true;
    DebugObject                         _debugOject;
    
    friend class instruction::GameboyInstructionSet8bit;
    friend class instruction::GameboyInstructionSet16bit;
    friend class instruction::GameboyInstructionSetStop;
};

namespace instruction {
class instruction
{
private:
    using action_t = void (*)(CentralProcessor&, std::optional<uint16_t>);

public:
    instruction(): _name(nullptr), _action(nullptr), _cycles(0), _immSize(0) { }
    instruction(const char *name, action_t action, uint8_t cycles): 
        _name(name), 
        _action(action), 
        _cycles(cycles),
        _immSize(0) 
    { }
    instruction(const char *name, action_t action, uint8_t cycles, uint8_t immSize): 
        _name(name), 
        _action(action), 
        _cycles(cycles), 
        _immSize(immSize) 
    { }

    uint8_t invoke(CentralProcessor& cpu, std::optional<uint16_t> imm) const
    {
        cpu.setBranchPenalty(0);
        switch (_immSize)
        {
        case 0:
            _action(cpu, std::nullopt);
            break;
        case 1:
            assert((*imm & 0xFF00) == 0);
        case 2:
            _action(cpu, imm);
            break;
        default:
            assert(_immSize);
        }
        return _cycles;
    }
    const char *getName() const { return _name; }
    uint8_t getCycles() const { return _cycles; }
    uint8_t getImmediateSize() const { return _immSize;}

    void print(uint8_t opcode) const 
    {
        require_or(_action, printf("0x%x\t%s\n", opcode, "Not implemented"); return);
        char sep[3] = "\t\t";
        if (strlen(_name) >= 8)
        {
            snprintf(sep, 3, "\t");
        }
        printf("0x%x\t%s%s%u\t%u\n", opcode, _name, sep, _immSize, _cycles);
    }

private:
    const char *_name = nullptr;
    action_t    _action = nullptr;
    uint8_t     _cycles = 0;
    uint8_t     _immSize = 0;
    uint8_t     _branchPenalty = 0;
};
} // namespace instruction
} // namespace emu::gameboy

#endif /* _CPU_H_ */