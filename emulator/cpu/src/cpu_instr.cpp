/**
 * @file cpu_instr.c
 * @author Jake Cronin (jakecronin97@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "cpu_instr.h"

#define __OPCODE_ARGS0(_cpu)       ::emu::gameboy::CentralProcessor &_cpu, __unused ::std::optional<uint16_t>
#define __OPCODE_ARGS1(_cpu, _imm) ::emu::gameboy::CentralProcessor &_cpu, ::std::optional<uint16_t> _imm

#define OPCODE_ARGS(_cpu, ...) CONCAT(__OPCODE_ARGS, __ARG_CNT(__VA_ARGS__))(_cpu, ##__VA_ARGS__)

using namespace std;
using namespace emu::gameboy::instruction;
using namespace emu::gameboy::detail::cpu;

GameboyInstructionSet8bit::GameboyInstructionSet8bit(): InstructionSet(kInstructionArraySize)
{
    initMoveImmediateGroup();
    initMoveRegisterGroup();
    initLoadHLGroup();
    initStoreHLGroup();
    initLoadAGroup();
    initStoreAGroup();
    initMoveImmediate16bitGroup();
    initLoadStoreSpecial();
    initPushGroup();
    initPopGroup();
    initAddGroup();
    initAdcGroup();
    initSubGroup();
    initSbcGroup();
    initAndGroup();
    initOrGroup();
    initXorGroup();
    initCpGroup();
    initIncGroup();
    initDecGroup();
    initAdd16bitGroup();
    initInc16bitGroup();
    initDec16bitGroup();
    initDaa();
    initCpl();
    initCcf();
    initScf();
    initNop();
    initHalt();
    initInterruptControlGroup();
    initRotateAGroup();
    initJpGroup();
    initJrGroup();
    initCallGroup();
    initRstGroup();
    initReturnGroup();
}

void
GameboyInstructionSet8bit::initMoveImmediateGroup()
{
    static constexpr auto mv = [](uint8_t& reg, optional<uint16_t> imm) { reg = truncate(*imm); };
    static constexpr uint8_t sz = sizeof(uint8_t);
    _instructions[0x3E] = instruction("LD A,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.a, imm); }, 2, sz);
    _instructions[0x06] = instruction("LD B,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.b, imm); }, 2, sz);
    _instructions[0x0E] = instruction("LD C,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.c, imm); }, 2, sz);
    _instructions[0x16] = instruction("LD D,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.d, imm); }, 2, sz);
    _instructions[0x1E] = instruction("LD E,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.e, imm); }, 2, sz);
    _instructions[0x26] = instruction("LD H,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.h, imm); }, 2, sz);
    _instructions[0x2E] = instruction("LD L,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.l, imm); }, 2, sz);
}

void
GameboyInstructionSet8bit::initMoveRegisterGroup()
{
    static constexpr auto mv = [](uint8_t &dst, uint8_t src) { dst = src; };
    _instructions[0x7F] = instruction("LD A,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.a); }, 1);
    _instructions[0x78] = instruction("LD A,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.b); }, 1);
    _instructions[0x79] = instruction("LD A,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.c); }, 1);
    _instructions[0x7A] = instruction("LD A,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.d); }, 1);
    _instructions[0x7B] = instruction("LD A,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.e); }, 1);
    _instructions[0x7C] = instruction("LD A,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.h); }, 1);
    _instructions[0x7D] = instruction("LD A,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.a, cpu._registers.l); }, 1);
    _instructions[0x40] = instruction("LD B,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.b); }, 1);
    _instructions[0x41] = instruction("LD B,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.c); }, 1);
    _instructions[0x42] = instruction("LD B,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.d); }, 1);
    _instructions[0x43] = instruction("LD B,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.e); }, 1);
    _instructions[0x44] = instruction("LD B,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.h); }, 1);
    _instructions[0x45] = instruction("LD B,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.l); }, 1);
    _instructions[0x48] = instruction("LD C,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.b); }, 1);
    _instructions[0x49] = instruction("LD C,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.c); }, 1);
    _instructions[0x4A] = instruction("LD C,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.d); }, 1);
    _instructions[0x4B] = instruction("LD C,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.e); }, 1);
    _instructions[0x4C] = instruction("LD C,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.h); }, 1);
    _instructions[0x4D] = instruction("LD C,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.l); }, 1);
    _instructions[0x50] = instruction("LD D,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.b); }, 1);
    _instructions[0x51] = instruction("LD D,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.c); }, 1);
    _instructions[0x52] = instruction("LD D,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.d); }, 1);
    _instructions[0x53] = instruction("LD D,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.e); }, 1);
    _instructions[0x54] = instruction("LD D,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.h); }, 1);
    _instructions[0x55] = instruction("LD D,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.l); }, 1);
    _instructions[0x58] = instruction("LD E,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.b); }, 1);
    _instructions[0x59] = instruction("LD E,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.c); }, 1);
    _instructions[0x5A] = instruction("LD E,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.d); }, 1);
    _instructions[0x5B] = instruction("LD E,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.e); }, 1);
    _instructions[0x5C] = instruction("LD E,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.h); }, 1);
    _instructions[0x5D] = instruction("LD E,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.l); }, 1);
    _instructions[0x60] = instruction("LD H,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.b); }, 1);
    _instructions[0x61] = instruction("LD H,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.c); }, 1);
    _instructions[0x62] = instruction("LD H,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.d); }, 1);
    _instructions[0x63] = instruction("LD H,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.e); }, 1);
    _instructions[0x64] = instruction("LD H,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.h); }, 1);
    _instructions[0x65] = instruction("LD H,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.l); }, 1);
    _instructions[0x68] = instruction("LD L,B", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.b); }, 1);
    _instructions[0x69] = instruction("LD L,C", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.c); }, 1);
    _instructions[0x6A] = instruction("LD L,D", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.d); }, 1);
    _instructions[0x6B] = instruction("LD L,E", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.e); }, 1);
    _instructions[0x6C] = instruction("LD L,H", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.h); }, 1);
    _instructions[0x6D] = instruction("LD L,L", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.l); }, 1);
    _instructions[0x47] = instruction("LD B,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.b, cpu._registers.a); }, 1);
    _instructions[0x4F] = instruction("LD C,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.c, cpu._registers.a); }, 1);
    _instructions[0x57] = instruction("LD D,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.d, cpu._registers.a); }, 1);
    _instructions[0x5F] = instruction("LD E,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.e, cpu._registers.a); }, 1);
    _instructions[0x67] = instruction("LD H,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.h, cpu._registers.a); }, 1);
    _instructions[0x6F] = instruction("LD L,A", [](OPCODE_ARGS(cpu)) { mv(cpu._registers.l, cpu._registers.a); }, 1);
}

void
GameboyInstructionSet8bit::initLoadHLGroup()
{
    static constexpr auto ld = [](CentralProcessor& cpu, uint8_t& dst) { dst = cpu.getMemoryManager()->read(cpu._registers.hl); };
    _instructions[0x7E] = instruction("LD A,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.a); }, 2);
    _instructions[0x46] = instruction("LD B,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.b); }, 2);
    _instructions[0x4E] = instruction("LD C,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.c); }, 2);
    _instructions[0x56] = instruction("LD D,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.d); }, 2);
    _instructions[0x5E] = instruction("LD E,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.e); }, 2);
    _instructions[0x66] = instruction("LD H,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.h); }, 2);
    _instructions[0x6E] = instruction("LD L,(HL)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.l); }, 2);
}

void
GameboyInstructionSet8bit::initStoreHLGroup()
{
    static constexpr auto st = [](CentralProcessor& cpu, uint8_t src) { cpu.getMemoryManager()->write(cpu._registers.hl, src); };
    _instructions[0x77] = instruction("LD (HL),A", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.a); }, 2);
    _instructions[0x70] = instruction("LD (HL),B", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.b); }, 2);
    _instructions[0x71] = instruction("LD (HL),C", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.c); }, 2);
    _instructions[0x72] = instruction("LD (HL),D", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.d); }, 2);
    _instructions[0x73] = instruction("LD (HL),E", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.e); }, 2);
    _instructions[0x74] = instruction("LD (HL),H", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.h); }, 2);
    _instructions[0x75] = instruction("LD (HL),L", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.l); }, 2);
    _instructions[0x36] = instruction("LD (HL),%x", [](OPCODE_ARGS(cpu, imm)) { st(cpu, truncate(*imm)); }, 3,
                                      sizeof(uint8_t));
}

void
GameboyInstructionSet8bit::initLoadAGroup()
{
    static constexpr auto ld = [](CentralProcessor& cpu, uint16_t src) { cpu._registers.a = cpu.getMemoryManager()->read(src); };
    _instructions[0x0A] = instruction("LD A,(BC)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.bc); }, 2);
    _instructions[0x1A] = instruction("LD A,(DE)", [](OPCODE_ARGS(cpu)) { ld(cpu, cpu._registers.de); }, 2);
    _instructions[0xFA] = instruction("LD A,(%x)", [](OPCODE_ARGS(cpu, imm)) { ld(cpu, *imm); }, 4, 
                                      sizeof(uint16_t));
}

void
GameboyInstructionSet8bit::initStoreAGroup()
{
    static constexpr auto st = [](CentralProcessor& cpu, uint16_t dst) { cpu.getMemoryManager()->write(dst, cpu._registers.a); };
    _instructions[0x02] = instruction("LD (BC),A", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.bc); }, 2);
    _instructions[0x12] = instruction("LD (DE),A", [](OPCODE_ARGS(cpu)) { st(cpu, cpu._registers.de); }, 2);
    _instructions[0xEA] = instruction("LD (%x),A", [](OPCODE_ARGS(cpu, imm)) { st(cpu, *imm); }, 4, 
                                      sizeof(uint16_t));
}

void
GameboyInstructionSet8bit::initMoveImmediate16bitGroup()
{
    static constexpr auto mv = [](uint16_t& dst, uint16_t imm) { dst = imm; };
    static constexpr uint8_t sz = sizeof(uint16_t);
    _instructions[0x01] = instruction("LD BC,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.bc, *imm); }, 3, sz);
    _instructions[0x11] = instruction("LD DE,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.de, *imm); }, 3, sz);
    _instructions[0x21] = instruction("LD HL,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.hl, *imm); }, 3, sz);
    _instructions[0x31] = instruction("LD SP,%x", [](OPCODE_ARGS(cpu, imm)) { mv(cpu._registers.stackPointer, *imm); }, 3, sz);
}

void
GameboyInstructionSet8bit::initLoadStoreSpecial()
{
    _instructions[0x3A] = instruction("LDD A,(HL)", [](OPCODE_ARGS(cpu)) { 
        cpu._registers.a = cpu.getMemoryManager()->read(cpu._registers.hl--);
    }, 2);
    _instructions[0x2A] = instruction("LDI A,(HL)", [](OPCODE_ARGS(cpu)) { 
        cpu._registers.a = cpu.getMemoryManager()->read(cpu._registers.hl++);
    }, 2);
    _instructions[0xE2] = instruction("LD (C),A", [](OPCODE_ARGS(cpu)) { 
        cpu.getMemoryManager()->write(0xFF00 + extend(cpu._registers.c), cpu._registers.a);
    }, 2);
    _instructions[0xF2] = instruction("LD A,(C)", [](OPCODE_ARGS(cpu)) {
        cpu._registers.a = cpu.getMemoryManager()->read(0xFF00 + extend(cpu._registers.c));
    }, 2);
    _instructions[0x32] = instruction("LDD (HL),A", [](OPCODE_ARGS(cpu)) {
        cpu.getMemoryManager()->write(cpu._registers.hl--, cpu._registers.a);
    }, 2);
    _instructions[0x22] = instruction("LDI (HL),A", [](OPCODE_ARGS(cpu)) {
        cpu.getMemoryManager()->write(cpu._registers.hl++, cpu._registers.a);
    }, 2);
    _instructions[0xE0] = instruction("LDH (%x),A", [](OPCODE_ARGS(cpu, imm)) {
        cpu.getMemoryManager()->write(0xFF00 + truncate(*imm), cpu._registers.a);
    }, 3, sizeof(uint8_t));
    _instructions[0xF0] = instruction("LDH A,(%x)", [](OPCODE_ARGS(cpu, imm)) {
        cpu._registers.a = cpu.getMemoryManager()->read(0xFF00 + truncate(*imm));
    }, 3, sizeof(uint8_t));
    _instructions[0xF9] = instruction("LD SP,HL", [](OPCODE_ARGS(cpu)) {
        cpu._registers.stackPointer = cpu._registers.hl;
    }, 2);
    _instructions[0x08] = instruction("LD (%x),SP", [](OPCODE_ARGS(cpu, imm)) {
        cpu.getMemoryManager()->write(*imm, cpu._registers.stackPointer);
    }, 5, sizeof(uint16_t));
    _instructions[0xF8] = instruction("LDHL SP,%x", [](OPCODE_ARGS(cpu, imm)) {
        auto &flags = cpu._registers.f;
        uint8_t immByte = truncate(*imm);
        uint8_t lowNibbleSum = (truncate(cpu._registers.stackPointer) & 0xF) + (immByte & 0xF);
        flags.halfCarry = static_cast<bool>(lowNibbleSum >> 4);
        flags.carry = add_overflow(truncate(cpu._registers.stackPointer), immByte);
        flags.subtract = false;
        flags.zero = false;
        cpu._registers.hl = cpu._registers.stackPointer + sign_extend(immByte);
    }, 3, sizeof(uint8_t));
}

void
GameboyInstructionSet8bit::initPushGroup()
{
    static constexpr auto push = [](CentralProcessor& cpu, uint16_t value) {
        cpu.pushToStack(value);
    };
    _instructions[0xF5] = instruction("PUSH AF", [](OPCODE_ARGS(cpu)) { push(cpu, cpu._registers.af & 0xFFF0); }, 4);
    _instructions[0xC5] = instruction("PUSH BC", [](OPCODE_ARGS(cpu)) { push(cpu, cpu._registers.bc); }, 4);
    _instructions[0xD5] = instruction("PUSH DE", [](OPCODE_ARGS(cpu)) { push(cpu, cpu._registers.de); }, 4);
    _instructions[0xE5] = instruction("PUSH HL", [](OPCODE_ARGS(cpu)) { push(cpu, cpu._registers.hl); }, 4);
}

void
GameboyInstructionSet8bit::initPopGroup()
{
    static constexpr auto pop = [](CentralProcessor& cpu) -> uint16_t {
        return cpu.popFromStack();
    };
    _instructions[0xF1] = instruction("POP AF", [](OPCODE_ARGS(cpu)) { cpu._registers.af = pop(cpu) & 0xFFF0; }, 3);
    _instructions[0xC1] = instruction("POP BC", [](OPCODE_ARGS(cpu)) { cpu._registers.bc = pop(cpu); }, 3);
    _instructions[0xD1] = instruction("POP DE", [](OPCODE_ARGS(cpu)) { cpu._registers.de = pop(cpu); }, 3);
    _instructions[0xE1] = instruction("POP HL", [](OPCODE_ARGS(cpu)) { cpu._registers.hl = pop(cpu); }, 3);
}

void
GameboyInstructionSet8bit::initAddGroup()
{
    static constexpr auto add = [](CentralProcessor& cpu, uint8_t value) {
        auto &flags = cpu._registers.f;
        uint8_t lowNibbleSum = (cpu._registers.a & 0xF) + (value & 0xF);
        flags.halfCarry = static_cast<bool>(lowNibbleSum >> 4);
        flags.carry = add_overflow(cpu._registers.a, value, &cpu._registers.a);
        flags.zero = !cpu._registers.a;
        flags.subtract = false;
    };
    _instructions[0x87] = instruction("ADD A,A", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.a); }, 1);
    _instructions[0x80] = instruction("ADD A,B", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.b); }, 1);
    _instructions[0x81] = instruction("ADD A,C", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.c); }, 1);
    _instructions[0x82] = instruction("ADD A,D", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.d); }, 1);
    _instructions[0x83] = instruction("ADD A,E", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.e); }, 1);
    _instructions[0x84] = instruction("ADD A,H", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.h); }, 1);
    _instructions[0x85] = instruction("ADD A,L", [](OPCODE_ARGS(cpu)) { add(cpu, cpu._registers.l); }, 1);
    _instructions[0xC6] = instruction("ADD A,%x", [](OPCODE_ARGS(cpu, imm)) { add(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0x86] = instruction("ADD A,(HL)", [](OPCODE_ARGS(cpu, imm)) { 
        add(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initAdcGroup()
{
    static constexpr auto adc = [](CentralProcessor& cpu, uint8_t value) {
        auto &flags = cpu._registers.f;
        uint8_t lowNibbleSum = (cpu._registers.a & 0xF) + (value & 0xF) + flags.carry;
        flags.halfCarry = static_cast<bool>(lowNibbleSum >> 4);
        flags.carry = add3_overflow(cpu._registers.a, value, flags.carry, &cpu._registers.a);
        flags.zero = !cpu._registers.a;
        flags.subtract = false;
    };
    _instructions[0x8F] = instruction("ADC A,A", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.a); }, 1);
    _instructions[0x88] = instruction("ADC A,B", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.b); }, 1);
    _instructions[0x89] = instruction("ADC A,C", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.c); }, 1);
    _instructions[0x8A] = instruction("ADC A,D", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.d); }, 1);
    _instructions[0x8B] = instruction("ADC A,E", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.e); }, 1);
    _instructions[0x8C] = instruction("ADC A,H", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.h); }, 1);
    _instructions[0x8D] = instruction("ADC A,L", [](OPCODE_ARGS(cpu)) { adc(cpu, cpu._registers.l); }, 1);
    _instructions[0xCE] = instruction("ADC A,%x", [](OPCODE_ARGS(cpu, imm)) { adc(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0x8E] = instruction("ADC A,(HL)", [](OPCODE_ARGS(cpu, imm)) {
        adc(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initSubGroup()
{
    static constexpr auto sub = [](CentralProcessor& cpu, uint8_t value) {
        auto &flags = cpu._registers.f;
        uint8_t lowNibbleDiff = (cpu._registers.a & 0xF) - (value & 0xF);
        flags.halfCarry = static_cast<bool>(lowNibbleDiff >> 4);
        flags.carry = sub_overflow(cpu._registers.a, value, &cpu._registers.a);
        flags.zero = !cpu._registers.a;
        flags.subtract = true;
    };
    _instructions[0x97] = instruction("SUB A,A", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.a); }, 1);
    _instructions[0x90] = instruction("SUB A,B", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.b); }, 1);
    _instructions[0x91] = instruction("SUB A,C", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.c); }, 1);
    _instructions[0x92] = instruction("SUB A,D", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.d); }, 1);
    _instructions[0x93] = instruction("SUB A,E", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.e); }, 1);
    _instructions[0x94] = instruction("SUB A,H", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.h); }, 1);
    _instructions[0x95] = instruction("SUB A,L", [](OPCODE_ARGS(cpu)) { sub(cpu, cpu._registers.l); }, 1);
    _instructions[0xD6] = instruction("SUB A,%x", [](OPCODE_ARGS(cpu, imm)) { sub(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0x96] = instruction("SUB A,(HL)", [](OPCODE_ARGS(cpu)) { 
        sub(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initSbcGroup()
{
    static constexpr auto sbc = [](CentralProcessor& cpu, uint8_t value) {
        auto &flags = cpu._registers.f;
        uint8_t lowNibbleDiff = (cpu._registers.a & 0xF) - (value & 0xF) - flags.carry;
        flags.halfCarry = static_cast<bool>(lowNibbleDiff >> 4);
        flags.carry = sub3_overflow(cpu._registers.a, value, flags.carry, &cpu._registers.a);
        flags.zero = !cpu._registers.a;
        flags.subtract = true;
    };
    _instructions[0x9F] = instruction("SBC A,A", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.a); }, 1);
    _instructions[0x98] = instruction("SBC A,B", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.b); }, 1);
    _instructions[0x99] = instruction("SBC A,C", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.c); }, 1);
    _instructions[0x9A] = instruction("SBC A,D", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.d); }, 1);
    _instructions[0x9B] = instruction("SBC A,E", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.e); }, 1);
    _instructions[0x9C] = instruction("SBC A,H", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.h); }, 1);
    _instructions[0x9D] = instruction("SBC A,L", [](OPCODE_ARGS(cpu)) { sbc(cpu, cpu._registers.l); }, 1);
    _instructions[0xDE] = instruction("SBC A,%x", [](OPCODE_ARGS(cpu, imm)) { sbc(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0x9E] = instruction("SBC A,(HL)", [](OPCODE_ARGS(cpu)) { 
        sbc(cpu, cpu.getMemoryManager()->read(cpu._registers.hl));
    }, 2);
}

void
GameboyInstructionSet8bit::initAndGroup()
{
    static constexpr auto and_a = [](CentralProcessor& cpu, uint8_t value) {
        cpu._registers.a &= value;
        cpu._registers.setFlags(0, 1, 0, !cpu._registers.a);
    };
    _instructions[0xA7] = instruction("AND A,A", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.a); }, 1);
    _instructions[0xA0] = instruction("AND A,B", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.b); }, 1);
    _instructions[0xA1] = instruction("AND A,C", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.c); }, 1);
    _instructions[0xA2] = instruction("AND A,D", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.d); }, 1);
    _instructions[0xA3] = instruction("AND A,E", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.e); }, 1);
    _instructions[0xA4] = instruction("AND A,H", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.h); }, 1);
    _instructions[0xA5] = instruction("AND A,L", [](OPCODE_ARGS(cpu)) { and_a(cpu, cpu._registers.l); }, 1);
    _instructions[0xE6] = instruction("AND A,%x", [](OPCODE_ARGS(cpu, imm)) { and_a(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0xA6] = instruction("AND A,(HL)", [](OPCODE_ARGS(cpu)) { 
        and_a(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initOrGroup()
{
    static constexpr auto or_a = [](CentralProcessor& cpu, uint8_t value) {
        cpu._registers.a |= value;
        cpu._registers.setFlags(0, 0, 0, !cpu._registers.a);
    };
    _instructions[0xB7] = instruction("OR A,A", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.a); }, 1);
    _instructions[0xB0] = instruction("OR A,B", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.b); }, 1);
    _instructions[0xB1] = instruction("OR A,C", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.c); }, 1);
    _instructions[0xB2] = instruction("OR A,D", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.d); }, 1);
    _instructions[0xB3] = instruction("OR A,E", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.e); }, 1);
    _instructions[0xB4] = instruction("OR A,H", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.h); }, 1);
    _instructions[0xB5] = instruction("OR A,L", [](OPCODE_ARGS(cpu)) { or_a(cpu, cpu._registers.l); }, 1);
    _instructions[0xF6] = instruction("OR A,%x", [](OPCODE_ARGS(cpu, imm)) { or_a(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0xB6] = instruction("OR A,(HL)", [](OPCODE_ARGS(cpu)) { 
        or_a(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initXorGroup()
{
    static constexpr auto xor_a = [](CentralProcessor& cpu, uint8_t value) {
        cpu._registers.a ^= value;
        cpu._registers.setFlags(0, 0, 0, !cpu._registers.a);
    };
    _instructions[0xAF] = instruction("XOR A,A", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.a); }, 1);
    _instructions[0xA8] = instruction("XOR A,B", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.b); }, 1);
    _instructions[0xA9] = instruction("XOR A,C", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.c); }, 1);
    _instructions[0xAA] = instruction("XOR A,D", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.d); }, 1);
    _instructions[0xAB] = instruction("XOR A,E", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.e); }, 1);
    _instructions[0xAC] = instruction("XOR A,H", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.h); }, 1);
    _instructions[0xAD] = instruction("XOR A,L", [](OPCODE_ARGS(cpu)) { xor_a(cpu, cpu._registers.l); }, 1);
    _instructions[0xEE] = instruction("XOR A,%x", [](OPCODE_ARGS(cpu, imm)) { xor_a(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0xAE] = instruction("XOR A,(HL)", [](OPCODE_ARGS(cpu)) { 
        xor_a(cpu, cpu.getMemoryManager()->read(cpu._registers.hl)); 
    }, 2);
}

void
GameboyInstructionSet8bit::initCpGroup()
{
    static constexpr auto cp = [](CentralProcessor& cpu, uint8_t value) {
        auto &flags = cpu._registers.f;
        uint8_t lowNibbleDiff = (cpu._registers.a & 0xF) - (value & 0xF);
        flags.halfCarry = static_cast<bool>(lowNibbleDiff >> 4);
        uint8_t retval = 0;
        flags.carry = sub_overflow(cpu._registers.a, value, &retval);
        flags.zero = !retval;
        flags.subtract = true;
    };
    _instructions[0xBF] = instruction("CP A,A", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.a); }, 1);
    _instructions[0xB8] = instruction("CP A,B", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.b); }, 1);
    _instructions[0xB9] = instruction("CP A,C", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.c); }, 1);
    _instructions[0xBA] = instruction("CP A,D", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.d); }, 1);
    _instructions[0xBB] = instruction("CP A,E", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.e); }, 1);
    _instructions[0xBC] = instruction("CP A,H", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.h); }, 1);
    _instructions[0xBD] = instruction("CP A,L", [](OPCODE_ARGS(cpu)) { cp(cpu, cpu._registers.l); }, 1);
    _instructions[0xFE] = instruction("CP A,%x", [](OPCODE_ARGS(cpu, imm)) { cp(cpu, truncate(*imm)); }, 2, 
                                      sizeof(uint8_t));
    _instructions[0xBE] = instruction("CP A,(HL)", [](OPCODE_ARGS(cpu)) {
        cp(cpu, cpu.getMemoryManager()->read(cpu._registers.hl));
    }, 2);
}

void
GameboyInstructionSet8bit::initIncGroup()
{
    static constexpr auto inc = [](CentralProcessor& cpu, uint8_t& value) {
        auto &flags = cpu._registers.f;
        flags.halfCarry = static_cast<uint8_t>((value & 0xF) == 0xF);
        value += 1;
        flags.subtract = false;
        flags.zero = !value;
    };
    _instructions[0x3C] = instruction("INC A", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.a); }, 1);
    _instructions[0x04] = instruction("INC B", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.b); }, 1);
    _instructions[0x0C] = instruction("INC C", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.c); }, 1);
    _instructions[0x14] = instruction("INC D", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.d); }, 1);
    _instructions[0x1C] = instruction("INC E", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.e); }, 1);
    _instructions[0x24] = instruction("INC H", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.h); }, 1);
    _instructions[0x2C] = instruction("INC L", [](OPCODE_ARGS(cpu)) { inc(cpu, cpu._registers.l); }, 1);
    _instructions[0x34] = instruction("INC (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t memval = cpu.getMemoryManager()->read(cpu._registers.hl);
        inc(cpu, memval);
        cpu.getMemoryManager()->write(cpu._registers.hl, memval); 
    }, 3);
}

void
GameboyInstructionSet8bit::initDecGroup()
{
    static constexpr auto dec = [](CentralProcessor& cpu, uint8_t& value) {
        auto &flags = cpu._registers.f;
        flags.halfCarry = static_cast<uint8_t>((value & 0xF) == 0);
        value -= 1;
        flags.subtract = true;
        flags.zero = !value;
    };
    _instructions[0x3D] = instruction("DEC A", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.a); }, 1);
    _instructions[0x05] = instruction("DEC B", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.b); }, 1);
    _instructions[0x0D] = instruction("DEC C", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.c); }, 1);
    _instructions[0x15] = instruction("DEC D", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.d); }, 1);
    _instructions[0x1D] = instruction("DEC E", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.e); }, 1);
    _instructions[0x25] = instruction("DEC H", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.h); }, 1);
    _instructions[0x2D] = instruction("DEC L", [](OPCODE_ARGS(cpu)) { dec(cpu, cpu._registers.l); }, 1);
    _instructions[0x35] = instruction("DEC (HL)", [](OPCODE_ARGS(cpu)) { 
        uint8_t memval = cpu.getMemoryManager()->read(cpu._registers.hl);
        dec(cpu, memval);
        cpu.getMemoryManager()->write(cpu._registers.hl, memval); 
    }, 3);
}

void
GameboyInstructionSet8bit::initAdd16bitGroup()
{
    static constexpr auto add_hl = [](CentralProcessor& cpu, uint16_t value) {
        auto &flags = cpu._registers.f;
        uint16_t lowSum = (cpu._registers.hl & 0xFFF) + (value & 0xFFF);
        flags.halfCarry = static_cast<bool>(lowSum >> 12);
        flags.carry = add_overflow(cpu._registers.hl, value, &cpu._registers.hl);
        flags.subtract = false;
    };
    _instructions[0x09] = instruction("ADD HL,BC", [](OPCODE_ARGS(cpu)) { add_hl(cpu, cpu._registers.bc); }, 2);
    _instructions[0x19] = instruction("ADD HL,DE", [](OPCODE_ARGS(cpu)) { add_hl(cpu, cpu._registers.de); }, 2);
    _instructions[0x29] = instruction("ADD HL,HL", [](OPCODE_ARGS(cpu)) { add_hl(cpu, cpu._registers.hl); }, 2);
    _instructions[0x39] = instruction("ADD HL,SP", [](OPCODE_ARGS(cpu)) { add_hl(cpu, cpu._registers.stackPointer); }, 2);
    _instructions[0xE8] = instruction("ADD SP,%x", [](OPCODE_ARGS(cpu, imm)) {
        auto &flags = cpu._registers.f;
        uint8_t immByte = truncate(*imm);
        uint16_t lowNibbleSum = (cpu._registers.stackPointer & 0xF) + (immByte & 0xF);
        flags.halfCarry = static_cast<bool>(lowNibbleSum >> 4);
        flags.carry = add_overflow(truncate(cpu._registers.stackPointer), immByte);
        flags.subtract = false;
        flags.zero = false;
        cpu._registers.stackPointer += sign_extend(immByte);
    }, 4, sizeof(uint8_t));
}

void
GameboyInstructionSet8bit::initInc16bitGroup()
{
    _instructions[0x03] = instruction("INC BC", [](OPCODE_ARGS(cpu)) { cpu._registers.bc++; }, 2);
    _instructions[0x13] = instruction("INC DE", [](OPCODE_ARGS(cpu)) { cpu._registers.de++; }, 2);
    _instructions[0x23] = instruction("INC HL", [](OPCODE_ARGS(cpu)) { cpu._registers.hl++; }, 2);
    _instructions[0x33] = instruction("INC SP", [](OPCODE_ARGS(cpu)) { cpu._registers.stackPointer++; }, 2);
}

void
GameboyInstructionSet8bit::initDec16bitGroup()
{
    _instructions[0x0B] = instruction("DEC BC", [](OPCODE_ARGS(cpu)) { cpu._registers.bc--; }, 2);
    _instructions[0x1B] = instruction("DEC DE", [](OPCODE_ARGS(cpu)) { cpu._registers.de--; }, 2);
    _instructions[0x2B] = instruction("DEC HL", [](OPCODE_ARGS(cpu)) { cpu._registers.hl--; }, 2);
    _instructions[0x3B] = instruction("DEC SP", [](OPCODE_ARGS(cpu)) { cpu._registers.stackPointer--; }, 2);
}

void
GameboyInstructionSet8bit::initDaa()
{
    static constexpr auto daa = [](OPCODE_ARGS(cpu)) {
        flags &flags = cpu._registers.f;
        /// Borrowed from https://forums.nesdev.org/viewtopic.php?t=15944
        if (flags.subtract == false)
        {
            if (flags.carry || cpu._registers.a > 0x99)
            {
                cpu._registers.a += 0x60; 
                flags.carry = static_cast<uint8_t>(true);
            }
            if (flags.halfCarry || (cpu._registers.a & 0x0f) > 0x09)
                cpu._registers.a += 0x6;
        }
        else
        {
            if (flags.carry)
                cpu._registers.a -= 0x60;
            if (flags.halfCarry)
                cpu._registers.a -= 0x6;
        };
        flags.zero = static_cast<uint8_t>(cpu._registers.a == 0);
        flags.halfCarry = static_cast<uint8_t>(false);
    };
    _instructions[0x27] = instruction("DAA", daa, 1);
}

void
GameboyInstructionSet8bit::initCpl()
{
    _instructions[0x2F] = instruction("CPL", [](OPCODE_ARGS(cpu)) {
        flags &flags = cpu._registers.f;
        cpu._registers.a = ~cpu._registers.a;
        flags.subtract = true;
        flags.halfCarry = true;
    }, 1);
}

void
GameboyInstructionSet8bit::initCcf()
{
    _instructions[0x3F] = instruction("CCF", [](OPCODE_ARGS(cpu)) {
        flags &flags = cpu._registers.f;
        flags.carry = ~flags.carry;
        flags.subtract = false;
        flags.halfCarry = false;
    }, 1);
}

void
GameboyInstructionSet8bit::initScf()
{
    _instructions[0x37] = instruction("SCF", [](OPCODE_ARGS(cpu)) {
        flags &flags = cpu._registers.f;
        flags.carry = true;
        flags.subtract = false;
        flags.halfCarry = false;
    }, 1);
}

void
GameboyInstructionSet8bit::initNop()
{
    /// @todo Need to implement this
    _instructions[0x00] = instruction("NOP", [](OPCODE_ARGS(cpu)) {  }, 1);
}

void
GameboyInstructionSet8bit::initHalt()
{
    _instructions[0x76] = instruction("HALT", [](OPCODE_ARGS(cpu)) { 
        cpu._halt = true; }, 1);
}

void
GameboyInstructionSet8bit::initInterruptControlGroup()
{
    _instructions[0xF3] = instruction("DI", [](OPCODE_ARGS(cpu)) { 
        cpu.setInterruptState(CentralProcessor::interruptState::disabled); 
    }, 1);
    _instructions[0xFB] = instruction("EI", [](OPCODE_ARGS(cpu)) {
        cpu.setInterruptState(CentralProcessor::interruptState::enabled);
    }, 1);
}

void
GameboyInstructionSet8bit::initRotateAGroup()
{
    _instructions[0x07] = instruction("RLCA", [](OPCODE_ARGS(cpu)) {
        cpu._registers.f.carry = static_cast<bool>(cpu._registers.a & 0x80);
        cpu._registers.a = (cpu._registers.a << 1) | cpu._registers.f.carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, 0);
    }, 1);
    _instructions[0x17] = instruction("RLA", [](OPCODE_ARGS(cpu)) {
        bool carry = static_cast<bool>(cpu._registers.a & 0x80);
        cpu._registers.a = (cpu._registers.a << 1) | cpu._registers.f.carry;
        cpu._registers.f.carry = carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, 0);
    }, 1);
    _instructions[0x0F] = instruction("RRCA", [](OPCODE_ARGS(cpu)) {
        cpu._registers.f.carry = static_cast<bool>(cpu._registers.a & 0x1);
        cpu._registers.a = (cpu._registers.a >> 1) | (cpu._registers.f.carry << 7);
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, 0);
     }, 1);
    _instructions[0x1F] = instruction("RRA", [](OPCODE_ARGS(cpu)) {
        bool carry = static_cast<bool>(cpu._registers.a & 0x1);
        cpu._registers.a = (cpu._registers.a >> 1) | (cpu._registers.f.carry << 7);
        cpu._registers.f.carry = carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, 0);
     }, 1);
}

void
GameboyInstructionSet8bit::initJpGroup()
{
    static constexpr uint8_t kJpInstructionBranchPenalty = 1;
    static constexpr auto jp = [](CentralProcessor &cpu, uint16_t relativeAddress, bool condition = true) {
        require_or(condition, return);
        cpu._registers.programCounter = relativeAddress;
        cpu._branchPenalty = kJpInstructionBranchPenalty;
    };
    static constexpr uint8_t sz = sizeof(uint16_t);
    _instructions[0xC3] = instruction("JP %x",    [](OPCODE_ARGS(cpu, imm)) { jp(cpu, *imm); }, 3, sz);
    _instructions[0xC2] = instruction("JP NZ,%x", [](OPCODE_ARGS(cpu, imm)) { jp(cpu, *imm, !cpu._registers.f.zero);  }, 3, sz);
    _instructions[0xCA] = instruction("JP Z,%x",  [](OPCODE_ARGS(cpu, imm)) { jp(cpu, *imm, cpu._registers.f.zero);   }, 3, sz);
    _instructions[0xD2] = instruction("JP NC,%x", [](OPCODE_ARGS(cpu, imm)) { jp(cpu, *imm, !cpu._registers.f.carry); }, 3, sz);
    _instructions[0xDA] = instruction("JP C,%x",  [](OPCODE_ARGS(cpu, imm)) { jp(cpu, *imm, cpu._registers.f.carry);  }, 3, sz);
    _instructions[0xE9] = instruction("JP HL", [](OPCODE_ARGS(cpu)) { cpu._registers.programCounter = cpu._registers.hl; }, 1);
}

void
GameboyInstructionSet8bit::initJrGroup()
{
    static constexpr uint8_t kJrInstructionBranchPenalty = 1;
    static constexpr auto jr = [] (CentralProcessor& cpu, uint16_t rel_address, bool condition) {
        require_or(condition, return);
        cpu._registers.programCounter += sign_extend(truncate(rel_address));
        cpu._branchPenalty = kJrInstructionBranchPenalty;
    };
    static constexpr uint8_t sz = sizeof(uint8_t);
    _instructions[0x18] = instruction("JR %x",    [](OPCODE_ARGS(cpu, imm)) { jr(cpu, *imm, true); }, 2, sz);
    _instructions[0x20] = instruction("JR NZ,%x", [](OPCODE_ARGS(cpu, imm)) { jr(cpu, *imm, !cpu._registers.f.zero);  }, 2, sz);
    _instructions[0x28] = instruction("JR Z,%x",  [](OPCODE_ARGS(cpu, imm)) { jr(cpu, *imm, cpu._registers.f.zero);   }, 2, sz);
    _instructions[0x30] = instruction("JR NC,%x", [](OPCODE_ARGS(cpu, imm)) { jr(cpu, *imm, !cpu._registers.f.carry); }, 2, sz);
    _instructions[0x38] = instruction("JR C,%x",  [](OPCODE_ARGS(cpu, imm)) { jr(cpu, *imm, cpu._registers.f.carry);  }, 2, sz);
}

void
GameboyInstructionSet8bit::initCallGroup()
{
    static constexpr uint8_t kCallInstructionBranchPenalty = 3;
    static constexpr auto call = [](CentralProcessor& cpu, uint16_t imm, bool condition = true) {
        require_or(condition, return);
        cpu.pushToStack(cpu._registers.programCounter);
        cpu._registers.programCounter = imm;
        cpu._branchPenalty = kCallInstructionBranchPenalty;
    };
    static constexpr uint8_t sz = sizeof(uint16_t);
    _instructions[0xCD] = instruction("CALL %x", [](OPCODE_ARGS(cpu, imm)) { call(cpu, *imm); }, 3, sz);
    _instructions[0xC4] = instruction("CALL NZ,%x", [](OPCODE_ARGS(cpu, imm)) { call(cpu, *imm, !cpu._registers.f.zero);  }, 3, sz);
    _instructions[0xCC] = instruction("CALL Z,%x",  [](OPCODE_ARGS(cpu, imm)) { call(cpu, *imm, cpu._registers.f.zero);   }, 3, sz);
    _instructions[0xD4] = instruction("CALL NC,%x", [](OPCODE_ARGS(cpu, imm)) { call(cpu, *imm, !cpu._registers.f.carry); }, 3, sz);
    _instructions[0xDC] = instruction("CALL C,%x",  [](OPCODE_ARGS(cpu, imm)) { call(cpu, *imm, cpu._registers.f.carry);  }, 3, sz);
}

void
GameboyInstructionSet8bit::initRstGroup()
{
    static constexpr auto reset = [](CentralProcessor& cpu, uint16_t vector) {
        cpu.pushToStack(cpu._registers.programCounter);
        cpu._registers.programCounter = vector;
    };
    _instructions[0xC7] = instruction("RST 00H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x00);  }, 4);
    _instructions[0xCF] = instruction("RST 08H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x08);  }, 4);
    _instructions[0xD7] = instruction("RST 10H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x10);  }, 4);
    _instructions[0xDF] = instruction("RST 18H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x18);  }, 4);
    _instructions[0xE7] = instruction("RST 20H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x20);  }, 4);
    _instructions[0xEF] = instruction("RST 28H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x28);  }, 4);
    _instructions[0xF7] = instruction("RST 30H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x30);  }, 4);
    _instructions[0xFF] = instruction("RST 38H", [](OPCODE_ARGS(cpu, imm)) { reset(cpu, 0x38);  }, 4);
}

void
GameboyInstructionSet8bit::initReturnGroup()
{
    static constexpr uint8_t kReturnInstructionBranchPenalty = 3;
    static auto ret = [](CentralProcessor& cpu, bool condition = true) {
        require_or(condition, return);
        cpu._registers.programCounter = cpu.popFromStack();
        cpu._branchPenalty = kReturnInstructionBranchPenalty;
    };
    _instructions[0xC9] = instruction("RET",  [](OPCODE_ARGS(cpu)) { ret(cpu); }, 1);
    _instructions[0xC0] = instruction("RET NZ", [](OPCODE_ARGS(cpu, imm)) { ret(cpu, !cpu._registers.f.zero);  }, 2);
    _instructions[0xC8] = instruction("RET Z",  [](OPCODE_ARGS(cpu, imm)) { ret(cpu, cpu._registers.f.zero);   }, 2);
    _instructions[0xD0] = instruction("RET NC", [](OPCODE_ARGS(cpu, imm)) { ret(cpu, !cpu._registers.f.carry); }, 2);
    _instructions[0xD8] = instruction("RET C",  [](OPCODE_ARGS(cpu, imm)) { ret(cpu, cpu._registers.f.carry);  }, 2);
    
    static constexpr auto reti = [](CentralProcessor& cpu) {
        cpu.setInterruptState(CentralProcessor::interruptState::enabled);
        cpu._registers.programCounter = cpu.popFromStack();
    };
    _instructions[0xD9] = instruction("RETI",  [](OPCODE_ARGS(cpu)) { reti(cpu); }, 4);
}

GameboyInstructionSet16bit::GameboyInstructionSet16bit(): InstructionSet(kInstructionArraySize)
{
    initSwapGroup();
    initRlcGroup();
    initRlGroup();
    initRrcGroup();
    initRrGroup();
    initSlaGroup();
    initSraGroup();
    initSrlGroup();
    initBitGroup();
    initSetGroup();
    initResGroup();
}

void
GameboyInstructionSet16bit::initSwapGroup()
{
    static constexpr auto swap = [](CentralProcessor& cpu, uint8_t &value) {
        value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
        cpu._registers.setFlags(0, 0, 0, !value);
    };
    _instructions[0x37] = instruction("SWAP A", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.a); }, 2);
    _instructions[0x30] = instruction("SWAP B", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.b); }, 2);
    _instructions[0x31] = instruction("SWAP C", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.c); }, 2);
    _instructions[0x32] = instruction("SWAP D", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.d); }, 2);
    _instructions[0x33] = instruction("SWAP E", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.e); }, 2);
    _instructions[0x34] = instruction("SWAP H", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.h); }, 2);
    _instructions[0x35] = instruction("SWAP L", [](OPCODE_ARGS(cpu)) { swap(cpu, cpu._registers.l); }, 2);
    _instructions[0x36] = instruction("SWAP (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t value = cpu.getMemoryManager()->read(cpu._registers.hl);
        swap(cpu, value);
        cpu.getMemoryManager()->write(cpu._registers.hl, value);
    }, 4);
}

void
GameboyInstructionSet16bit::initRlcGroup()
{
    static constexpr auto rlc = [](CentralProcessor& cpu, uint8_t &value) {
        cpu._registers.f.carry = static_cast<bool>(value & 0x80);
        value = (value << 1) | cpu._registers.f.carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x07] = instruction("RLC A", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.a); }, 2);
    _instructions[0x00] = instruction("RLC B", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.b); }, 2);
    _instructions[0x01] = instruction("RLC C", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.c); }, 2);
    _instructions[0x02] = instruction("RLC D", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.d); }, 2);
    _instructions[0x03] = instruction("RLC E", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.e); }, 2);
    _instructions[0x04] = instruction("RLC H", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.h); }, 2);
    _instructions[0x05] = instruction("RLC L", [](OPCODE_ARGS(cpu)) { rlc(cpu, cpu._registers.l); }, 2);
    _instructions[0x06] = instruction("RLC (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        rlc(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initRlGroup()
{
    static constexpr auto rl = [](CentralProcessor& cpu, uint8_t &value) {
        bool carry = static_cast<bool>(value & 0x80);
        value = (value << 1) | cpu._registers.f.carry;
        cpu._registers.f.carry = carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x17] = instruction("RL A", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.a); }, 2);
    _instructions[0x10] = instruction("RL B", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.b); }, 2);
    _instructions[0x11] = instruction("RL C", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.c); }, 2);
    _instructions[0x12] = instruction("RL D", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.d); }, 2);
    _instructions[0x13] = instruction("RL E", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.e); }, 2);
    _instructions[0x14] = instruction("RL H", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.h); }, 2);
    _instructions[0x15] = instruction("RL L", [](OPCODE_ARGS(cpu)) { rl(cpu, cpu._registers.l); }, 2);
    _instructions[0x16] = instruction("RL (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        rl(cpu, mem_value);
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initRrcGroup()
{
    static constexpr auto rrc = [](CentralProcessor& cpu, uint8_t &value) {
        cpu._registers.f.carry = static_cast<bool>(value & 0x1);
        value = (value >> 1) | (cpu._registers.f.carry << 7);
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x0F] = instruction("RRC A", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.a); }, 2);
    _instructions[0x08] = instruction("RRC B", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.b); }, 2);
    _instructions[0x09] = instruction("RRC C", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.c); }, 2);
    _instructions[0x0A] = instruction("RRC D", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.d); }, 2);
    _instructions[0x0B] = instruction("RRC E", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.e); }, 2);
    _instructions[0x0C] = instruction("RRC H", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.h); }, 2);
    _instructions[0x0D] = instruction("RRC L", [](OPCODE_ARGS(cpu)) { rrc(cpu, cpu._registers.l); }, 2);
    _instructions[0x0E] = instruction("RRC (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        rrc(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initRrGroup()
{
    static constexpr auto rr = [](CentralProcessor& cpu, uint8_t &value) {
        bool carry = static_cast<bool>(value & 0x1);
        value = (value >> 1) | (cpu._registers.f.carry << 7);
        cpu._registers.f.carry = carry;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x1F] = instruction("RR A", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.a); }, 2);
    _instructions[0x18] = instruction("RR B", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.b); }, 2);
    _instructions[0x19] = instruction("RR C", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.c); }, 2);
    _instructions[0x1A] = instruction("RR D", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.d); }, 2);
    _instructions[0x1B] = instruction("RR E", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.e); }, 2);
    _instructions[0x1C] = instruction("RR H", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.h); }, 2);
    _instructions[0x1D] = instruction("RR L", [](OPCODE_ARGS(cpu)) { rr(cpu, cpu._registers.l); }, 2);
    _instructions[0x1E] = instruction("RR (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        rr(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initSlaGroup()
{
    static constexpr auto sla = [](CentralProcessor& cpu, uint8_t &value) {
        cpu._registers.f.carry = static_cast<bool>(value & 0x80);
        value = (value << 1) & 0xFE;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x27] = instruction("SLA A", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.a); }, 2);
    _instructions[0x20] = instruction("SLA B", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.b); }, 2);
    _instructions[0x21] = instruction("SLA C", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.c); }, 2);
    _instructions[0x22] = instruction("SLA D", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.d); }, 2);
    _instructions[0x23] = instruction("SLA E", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.e); }, 2);
    _instructions[0x24] = instruction("SLA H", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.h); }, 2);
    _instructions[0x25] = instruction("SLA L", [](OPCODE_ARGS(cpu)) { sla(cpu, cpu._registers.l); }, 2);
    _instructions[0x26] = instruction("SLA (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        sla(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initSraGroup()
{
    static constexpr auto sra = [](CentralProcessor& cpu, uint8_t &value) {
        cpu._registers.f.carry = static_cast<bool>(value & 0x1);
        value = ((value >> 1) & 0x7F) | (value & 0x80);
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x2F] = instruction("SRA A", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.a); }, 2);
    _instructions[0x28] = instruction("SRA B", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.b); }, 2);
    _instructions[0x29] = instruction("SRA C", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.c); }, 2);
    _instructions[0x2A] = instruction("SRA D", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.d); }, 2);
    _instructions[0x2B] = instruction("SRA E", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.e); }, 2);
    _instructions[0x2C] = instruction("SRA H", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.h); }, 2);
    _instructions[0x2D] = instruction("SRA L", [](OPCODE_ARGS(cpu)) { sra(cpu, cpu._registers.l); }, 2);
    _instructions[0x2E] = instruction("SRA (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        sra(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initSrlGroup()
{
    static constexpr auto srl = [](CentralProcessor& cpu, uint8_t &value) {
        cpu._registers.f.carry = static_cast<bool>(value & 0x1);
        value = (value >> 1) & 0x7F;
        cpu._registers.setFlags(cpu._registers.f.carry, 0, 0, !value);
    };
    _instructions[0x3F] = instruction("SRL A", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.a); }, 2);
    _instructions[0x38] = instruction("SRL B", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.b); }, 2);
    _instructions[0x39] = instruction("SRL C", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.c); }, 2);
    _instructions[0x3A] = instruction("SRL D", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.d); }, 2);
    _instructions[0x3B] = instruction("SRL E", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.e); }, 2);
    _instructions[0x3C] = instruction("SRL H", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.h); }, 2);
    _instructions[0x3D] = instruction("SRL L", [](OPCODE_ARGS(cpu)) { srl(cpu, cpu._registers.l); }, 2);
    _instructions[0x3E] = instruction("SRL (HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        srl(cpu, mem_value);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initBitGroup()
{
    static constexpr auto bit = [](CentralProcessor& cpu, uint8_t &value, uint8_t bit) {
        cpu._registers.setFlags(cpu._registers.f.carry, 1, 0, !get_bit(value, bit));
    };
    _instructions[0x47] = instruction("BIT 0,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 0); }, 2);
    _instructions[0x40] = instruction("BIT 0,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 0); }, 2);
    _instructions[0x41] = instruction("BIT 0,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 0); }, 2);
    _instructions[0x42] = instruction("BIT 0,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 0); }, 2);
    _instructions[0x43] = instruction("BIT 0,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 0); }, 2);
    _instructions[0x44] = instruction("BIT 0,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 0); }, 2);
    _instructions[0x45] = instruction("BIT 0,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 0); }, 2);
    _instructions[0x46] = instruction("BIT 0,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 0);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);
    
    _instructions[0x4F] = instruction("BIT 1,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 1); }, 2);
    _instructions[0x48] = instruction("BIT 1,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 1); }, 2);
    _instructions[0x49] = instruction("BIT 1,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 1); }, 2);
    _instructions[0x4A] = instruction("BIT 1,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 1); }, 2);
    _instructions[0x4B] = instruction("BIT 1,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 1); }, 2);
    _instructions[0x4C] = instruction("BIT 1,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 1); }, 2);
    _instructions[0x4D] = instruction("BIT 1,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 1); }, 2);
    _instructions[0x4E] = instruction("BIT 1,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 1);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x57] = instruction("BIT 2,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 2); }, 2);
    _instructions[0x50] = instruction("BIT 2,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 2); }, 2);
    _instructions[0x51] = instruction("BIT 2,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 2); }, 2);
    _instructions[0x52] = instruction("BIT 2,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 2); }, 2);
    _instructions[0x53] = instruction("BIT 2,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 2); }, 2);
    _instructions[0x54] = instruction("BIT 2,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 2); }, 2);
    _instructions[0x55] = instruction("BIT 2,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 2); }, 2);
    _instructions[0x56] = instruction("BIT 2,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 2);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x5F] = instruction("BIT 3,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 3); }, 2);
    _instructions[0x58] = instruction("BIT 3,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 3); }, 2);
    _instructions[0x59] = instruction("BIT 3,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 3); }, 2);
    _instructions[0x5A] = instruction("BIT 3,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 3); }, 2);
    _instructions[0x5B] = instruction("BIT 3,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 3); }, 2);
    _instructions[0x5C] = instruction("BIT 3,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 3); }, 2);
    _instructions[0x5D] = instruction("BIT 3,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 3); }, 2);
    _instructions[0x5E] = instruction("BIT 3,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 3);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x67] = instruction("BIT 4,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 4); }, 2);
    _instructions[0x60] = instruction("BIT 4,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 4); }, 2);
    _instructions[0x61] = instruction("BIT 4,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 4); }, 2);
    _instructions[0x62] = instruction("BIT 4,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 4); }, 2);
    _instructions[0x63] = instruction("BIT 4,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 4); }, 2);
    _instructions[0x64] = instruction("BIT 4,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 4); }, 2);
    _instructions[0x65] = instruction("BIT 4,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 4); }, 2);
    _instructions[0x66] = instruction("BIT 4,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 4);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x6F] = instruction("BIT 5,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 5); }, 2);
    _instructions[0x68] = instruction("BIT 5,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 5); }, 2);
    _instructions[0x69] = instruction("BIT 5,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 5); }, 2);
    _instructions[0x6A] = instruction("BIT 5,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 5); }, 2);
    _instructions[0x6B] = instruction("BIT 5,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 5); }, 2);
    _instructions[0x6C] = instruction("BIT 5,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 5); }, 2);
    _instructions[0x6D] = instruction("BIT 5,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 5); }, 2);
    _instructions[0x6E] = instruction("BIT 5,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 5);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x77] = instruction("BIT 6,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 6); }, 2);
    _instructions[0x70] = instruction("BIT 6,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 6); }, 2);
    _instructions[0x71] = instruction("BIT 6,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 6); }, 2);
    _instructions[0x72] = instruction("BIT 6,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 6); }, 2);
    _instructions[0x73] = instruction("BIT 6,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 6); }, 2);
    _instructions[0x74] = instruction("BIT 6,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 6); }, 2);
    _instructions[0x75] = instruction("BIT 6,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 6); }, 2);
    _instructions[0x76] = instruction("BIT 6,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 6);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);

    _instructions[0x7F] = instruction("BIT 7,A", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.a, 7); }, 2);
    _instructions[0x78] = instruction("BIT 7,B", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.b, 7); }, 2);
    _instructions[0x79] = instruction("BIT 7,C", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.c, 7); }, 2);
    _instructions[0x7A] = instruction("BIT 7,D", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.d, 7); }, 2);
    _instructions[0x7B] = instruction("BIT 7,E", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.e, 7); }, 2);
    _instructions[0x7C] = instruction("BIT 7,H", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.h, 7); }, 2);
    _instructions[0x7D] = instruction("BIT 7,L", [](OPCODE_ARGS(cpu)) { bit(cpu, cpu._registers.l, 7); }, 2);
    _instructions[0x7E] = instruction("BIT 7,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        bit(cpu, mem_value, 7);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 3);
}

void
GameboyInstructionSet16bit::initSetGroup()
{
    _instructions[0xC7] = instruction("SET 0,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 0); }, 2);
    _instructions[0xC0] = instruction("SET 0,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 0); }, 2);
    _instructions[0xC1] = instruction("SET 0,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 0); }, 2);
    _instructions[0xC2] = instruction("SET 0,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 0); }, 2);
    _instructions[0xC3] = instruction("SET 0,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 0); }, 2);
    _instructions[0xC4] = instruction("SET 0,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 0); }, 2);
    _instructions[0xC5] = instruction("SET 0,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 0); }, 2);
    _instructions[0xC6] = instruction("SET 0,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 0);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xCF] = instruction("SET 1,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 1); }, 2);
    _instructions[0xC8] = instruction("SET 1,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 1); }, 2);
    _instructions[0xC9] = instruction("SET 1,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 1); }, 2);
    _instructions[0xCA] = instruction("SET 1,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 1); }, 2);
    _instructions[0xCB] = instruction("SET 1,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 1); }, 2);
    _instructions[0xCC] = instruction("SET 1,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 1); }, 2);
    _instructions[0xCD] = instruction("SET 1,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 1); }, 2);
    _instructions[0xCE] = instruction("SET 1,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 1);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xD7] = instruction("SET 2,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 2); }, 2);
    _instructions[0xD0] = instruction("SET 2,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 2); }, 2);
    _instructions[0xD1] = instruction("SET 2,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 2); }, 2);
    _instructions[0xD2] = instruction("SET 2,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 2); }, 2);
    _instructions[0xD3] = instruction("SET 2,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 2); }, 2);
    _instructions[0xD4] = instruction("SET 2,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 2); }, 2);
    _instructions[0xD5] = instruction("SET 2,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 2); }, 2);
    _instructions[0xD6] = instruction("SET 2,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 2);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xDF] = instruction("SET 3,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 3); }, 2);
    _instructions[0xD8] = instruction("SET 3,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 3); }, 2);
    _instructions[0xD9] = instruction("SET 3,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 3); }, 2);
    _instructions[0xDA] = instruction("SET 3,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 3); }, 2);
    _instructions[0xDB] = instruction("SET 3,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 3); }, 2);
    _instructions[0xDC] = instruction("SET 3,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 3); }, 2);
    _instructions[0xDD] = instruction("SET 3,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 3); }, 2);
    _instructions[0xDE] = instruction("SET 3,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 3);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xE7] = instruction("SET 4,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 4); }, 2);
    _instructions[0xE0] = instruction("SET 4,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 4); }, 2);
    _instructions[0xE1] = instruction("SET 4,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 4); }, 2);
    _instructions[0xE2] = instruction("SET 4,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 4); }, 2);
    _instructions[0xE3] = instruction("SET 4,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 4); }, 2);
    _instructions[0xE4] = instruction("SET 4,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 4); }, 2);
    _instructions[0xE5] = instruction("SET 4,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 4); }, 2);
    _instructions[0xE6] = instruction("SET 4,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 4);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xEF] = instruction("SET 5,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 5); }, 2);
    _instructions[0xE8] = instruction("SET 5,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 5); }, 2);
    _instructions[0xE9] = instruction("SET 5,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 5); }, 2);
    _instructions[0xEA] = instruction("SET 5,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 5); }, 2);
    _instructions[0xEB] = instruction("SET 5,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 5); }, 2);
    _instructions[0xEC] = instruction("SET 5,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 5); }, 2);
    _instructions[0xED] = instruction("SET 5,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 5); }, 2);
    _instructions[0xEE] = instruction("SET 5,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 5);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xF7] = instruction("SET 6,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 6); }, 2);
    _instructions[0xF0] = instruction("SET 6,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 6); }, 2);
    _instructions[0xF1] = instruction("SET 6,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 6); }, 2);
    _instructions[0xF2] = instruction("SET 6,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 6); }, 2);
    _instructions[0xF3] = instruction("SET 6,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 6); }, 2);
    _instructions[0xF4] = instruction("SET 6,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 6); }, 2);
    _instructions[0xF5] = instruction("SET 6,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 6); }, 2);
    _instructions[0xF6] = instruction("SET 6,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 6);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xFF] = instruction("SET 7,A", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.a, 7); }, 2);
    _instructions[0xF8] = instruction("SET 7,B", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.b, 7); }, 2);
    _instructions[0xF9] = instruction("SET 7,C", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.c, 7); }, 2);
    _instructions[0xFA] = instruction("SET 7,D", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.d, 7); }, 2);
    _instructions[0xFB] = instruction("SET 7,E", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.e, 7); }, 2);
    _instructions[0xFC] = instruction("SET 7,H", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.h, 7); }, 2);
    _instructions[0xFD] = instruction("SET 7,L", [](OPCODE_ARGS(cpu)) { set_bit(cpu._registers.l, 7); }, 2);
    _instructions[0xFE] = instruction("SET 7,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        set_bit(mem_value, 7);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

void
GameboyInstructionSet16bit::initResGroup()
{
    _instructions[0x87] = instruction("RES 0,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 0); }, 2);
    _instructions[0x80] = instruction("RES 0,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 0); }, 2);
    _instructions[0x81] = instruction("RES 0,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 0); }, 2);
    _instructions[0x82] = instruction("RES 0,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 0); }, 2);
    _instructions[0x83] = instruction("RES 0,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 0); }, 2);
    _instructions[0x84] = instruction("RES 0,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 0); }, 2);
    _instructions[0x85] = instruction("RES 0,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 0); }, 2);
    _instructions[0x86] = instruction("RES 0,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 0);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0x8F] = instruction("RES 1,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 1); }, 2);
    _instructions[0x88] = instruction("RES 1,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 1); }, 2);
    _instructions[0x89] = instruction("RES 1,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 1); }, 2);
    _instructions[0x8A] = instruction("RES 1,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 1); }, 2);
    _instructions[0x8B] = instruction("RES 1,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 1); }, 2);
    _instructions[0x8C] = instruction("RES 1,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 1); }, 2);
    _instructions[0x8D] = instruction("RES 1,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 1); }, 2);
    _instructions[0x8E] = instruction("RES 1,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 1);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0x97] = instruction("RES 2,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 2); }, 2);
    _instructions[0x90] = instruction("RES 2,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 2); }, 2);
    _instructions[0x91] = instruction("RES 2,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 2); }, 2);
    _instructions[0x92] = instruction("RES 2,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 2); }, 2);
    _instructions[0x93] = instruction("RES 2,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 2); }, 2);
    _instructions[0x94] = instruction("RES 2,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 2); }, 2);
    _instructions[0x95] = instruction("RES 2,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 2); }, 2);
    _instructions[0x96] = instruction("RES 2,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 2);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0x9F] = instruction("RES 3,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 3); }, 2);
    _instructions[0x98] = instruction("RES 3,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 3); }, 2);
    _instructions[0x99] = instruction("RES 3,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 3); }, 2);
    _instructions[0x9A] = instruction("RES 3,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 3); }, 2);
    _instructions[0x9B] = instruction("RES 3,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 3); }, 2);
    _instructions[0x9C] = instruction("RES 3,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 3); }, 2);
    _instructions[0x9D] = instruction("RES 3,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 3); }, 2);
    _instructions[0x9E] = instruction("RES 3,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 3);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xA7] = instruction("RES 4,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 4); }, 2);
    _instructions[0xA0] = instruction("RES 4,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 4); }, 2);
    _instructions[0xA1] = instruction("RES 4,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 4); }, 2);
    _instructions[0xA2] = instruction("RES 4,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 4); }, 2);
    _instructions[0xA3] = instruction("RES 4,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 4); }, 2);
    _instructions[0xA4] = instruction("RES 4,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 4); }, 2);
    _instructions[0xA5] = instruction("RES 4,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 4); }, 2);
    _instructions[0xA6] = instruction("RES 4,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 4);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xAF] = instruction("RES 5,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 5); }, 2);
    _instructions[0xA8] = instruction("RES 5,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 5); }, 2);
    _instructions[0xA9] = instruction("RES 5,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 5); }, 2);
    _instructions[0xAA] = instruction("RES 5,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 5); }, 2);
    _instructions[0xAB] = instruction("RES 5,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 5); }, 2);
    _instructions[0xAC] = instruction("RES 5,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 5); }, 2);
    _instructions[0xAD] = instruction("RES 5,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 5); }, 2);
    _instructions[0xAE] = instruction("RES 5,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 5);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xB7] = instruction("RES 6,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 6); }, 2);
    _instructions[0xB0] = instruction("RES 6,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 6); }, 2);
    _instructions[0xB1] = instruction("RES 6,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 6); }, 2);
    _instructions[0xB2] = instruction("RES 6,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 6); }, 2);
    _instructions[0xB3] = instruction("RES 6,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 6); }, 2);
    _instructions[0xB4] = instruction("RES 6,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 6); }, 2);
    _instructions[0xB5] = instruction("RES 6,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 6); }, 2);
    _instructions[0xB6] = instruction("RES 6,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 6);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);

    _instructions[0xBF] = instruction("RES 7,A", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.a, 7); }, 2);
    _instructions[0xB8] = instruction("RES 7,B", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.b, 7); }, 2);
    _instructions[0xB9] = instruction("RES 7,C", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.c, 7); }, 2);
    _instructions[0xBA] = instruction("RES 7,D", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.d, 7); }, 2);
    _instructions[0xBB] = instruction("RES 7,E", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.e, 7); }, 2);
    _instructions[0xBC] = instruction("RES 7,H", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.h, 7); }, 2);
    _instructions[0xBD] = instruction("RES 7,L", [](OPCODE_ARGS(cpu)) { clear_bit(cpu._registers.l, 7); }, 2);
    _instructions[0xBE] = instruction("RES 7,(HL)", [](OPCODE_ARGS(cpu)) {
        uint8_t mem_value = cpu.getMemoryManager()->read(cpu._registers.hl);
        clear_bit(mem_value, 7);
        cpu.getMemoryManager()->write(cpu._registers.hl, mem_value);
    }, 4);
}

GameboyInstructionSetStop::GameboyInstructionSetStop(): InstructionSet(1)
{
    _instructions[0x00] = instruction("STOP", [](OPCODE_ARGS(cpu)) { log_error("Yet to be implemented!"); }, 1);
}
