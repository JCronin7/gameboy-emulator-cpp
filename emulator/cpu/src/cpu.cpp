#include "cpu.h"
#include "cpu_instr.h"

using namespace emu::gameboy;
using namespace emu::gameboy::instruction;

static auto kIntDelay = ::emu::gameboy::instruction::instruction("INT", [](__unused CentralProcessor &, __unused std::optional<uint16_t>) { }, 5);
static auto kHaltDelay = ::emu::gameboy::instruction::instruction("HDEL", [](__unused CentralProcessor &, __unused std::optional<uint16_t>) { }, 1);

CentralProcessor::CentralProcessor(std::shared_ptr<GameboyMemory> memory): _memory(memory)
{
    _instructions8bit = new GameboyInstructionSet8bit;
    _instructions16bit = new GameboyInstructionSet16bit;
    _stopInstruction = new GameboyInstructionSetStop;
}

CentralProcessor::~CentralProcessor()
{ 
    delete _instructions8bit;
    delete _instructions16bit;
    delete _stopInstruction;
}

void 
CentralProcessor::reset()
{
    _registers.bc = 0;
    _registers.de = 0;
    _registers.hl = 0;
    _registers.af = 0;
    _registers.stackPointer = 0xFFFE;
    _registers.programCounter = 0;
}

void
CentralProcessor::wake()
{
    _power = true;
}

void
CentralProcessor::printInstructions()
{
    if (_instructions8bit)
    {
        _instructions8bit->printAll();
    }
    if (_instructions16bit)
    {
        _instructions16bit->printAll();
    }
    if (_stopInstruction)
    {
        _stopInstruction->printAll();
    }
}

void 
CentralProcessor::printState(CentralProcessor::printStateOption opt)
{
    std::function<void (std::shared_ptr<DebugObject::Record>)> printFn; 
    switch (opt)
    {
    case printStateOption::none:
        printFn = [](__unused std::shared_ptr<DebugObject::Record>){};
        break;
    case printStateOption::compact:
        printFn = [](std::shared_ptr<DebugObject::Record> obj) { 
            printf("0x%04x:  %s \n", obj->registerState.programCounter, obj->nameBuffer); 
        };
        break;
    case printStateOption::full:
        printFn = [](std::shared_ptr<DebugObject::Record> obj) {
            auto &reg = obj->registerState;
            printf("%s:\n", obj->nameBuffer);
            printf("\tProgram Counter = 0x%X, Stack Pointer = 0x%X\n"
                   "\tRegister b = 0x%X, Register c = 0x%X\n"
                   "\tRegister d = 0x%X, Register e = 0x%X\n"
                   "\tRegister h = 0x%X, Register h = 0x%X\n"
                   "\tRegister a = 0x%X, fZero %u, fSubtract %u, fCarry %u, fHalfCarry %u\n",
                   reg.programCounter, reg.stackPointer, reg.b, reg.c, reg.d, reg.e, reg.h, reg.l,
                   reg.a, reg.f.zero, reg.f.subtract, reg.f.carry, reg.f.halfCarry);
        };
        break;
    }
    while (auto obj = _debugOject.getNextRecord())
    {
        printFn(obj);
    }
}

const ::emu::gameboy::instruction::instruction *
CentralProcessor::getNextInstruction()
{
    require_or(!checkForInterrupt(), return &kIntDelay);
    require_or(_halt == false, return &kHaltDelay);
    /// Get next opcode from program counter.
    /// If the instruction at the program counter, this is opcode, otherwise prefix code.
    uint8_t opcodeOrPrefix = readProgramCounter();
    switch (opcodeOrPrefix)
    {
    case GameboyInstructionSet16bit::kInstructionSetCode:
        return _instructions16bit->getInstruction(readProgramCounter());
    case GameboyInstructionSetStop::kInstructionSetCode:
        return _stopInstruction->getInstruction(readProgramCounter());
    default:
        assert(static_cast<uint16_t>(opcodeOrPrefix) < GameboyInstructionSet8bit::kInstructionArraySize);
        return _instructions8bit->getInstruction(opcodeOrPrefix);
    }
}

uint8_t 
CentralProcessor::executeNextInstruction(const ::emu::gameboy::instruction::instruction *instruction)
{
    /// Get operand from program counter.
    std::optional<uint16_t> operand;
    if (instruction->getImmediateSize() >= sizeof(uint8_t))
    {
        operand.emplace(static_cast<uint16_t>(readProgramCounter()));
    }
    if (instruction->getImmediateSize() == sizeof(uint16_t))
    {
        *operand |= (static_cast<uint16_t>(readProgramCounter()) << 8);
    }
    
    if (_registers.programCounter == 50123)
    {
        log_error();
    }

    _debugOject.insertRecord(instruction->getName(), _registers, operand);
    instruction->invoke(*this, operand);
    return _branchPenalty;
}

bool
CentralProcessor::checkForInterrupt()
{
    /// @todo Need to confirm this optimization is okay, if not halted, and not enabled, no need to check flags
    bool interrupts_enabled = (_interruptState == interruptState::enabled);
    require_or(_halt == true || interrupts_enabled, return false);
    // Check if interrupt is enabled
    uint8_t interrupt_enable = _memory->getInterruptEnable()->value;
    uint8_t interrupt_flags = _memory->getIORegisters()->interruptFlag;
    assert(_halt == false || interrupt_enable);
    if (interrupt_enable & interrupt_flags)
    {
        _halt = false;
    }
    require_or(interrupts_enabled, return false);
    // Iterate through interrupts from lowest vector to highest to match priority scheme
    uint8_t interrupt_flag = 0x01;
    uint8_t interrupt_vector = 0x40;
    while ((interrupt_enable & interrupt_flags & interrupt_flag) == false)
    {
        interrupt_vector += 0x8;
        require_or((interrupt_flag <<= 1) != 0x20, return false);
    }
    // Clear interrupt flag, disable interrupts
    _memory->write(0xFF0F, interrupt_flags & ~(interrupt_flag));
    _interruptState = interruptState::disabled;

    // execute `call <vector>`
    pushToStack(_registers.programCounter);
    _registers.programCounter = interrupt_vector;
    _debugOject.insertRecord("INT", _registers);
    return true;
}

void
CentralProcessor::dotCycleUpdate()
{
    require_or(_interruptState != interruptState::enabling, 
               _interruptState = interruptState::enabled; return);
}

void 
CentralProcessor::pushToStack(uint16_t value)
{
    _registers.stackPointer -= sizeof(uint16_t);
    _memory->write<uint16_t>(_registers.stackPointer, value);
}

uint16_t 
CentralProcessor::popFromStack()
{
    auto retval = getMemoryManager()->read<uint16_t>(_registers.stackPointer);
    _registers.stackPointer += sizeof(uint16_t);
    return retval;
}
