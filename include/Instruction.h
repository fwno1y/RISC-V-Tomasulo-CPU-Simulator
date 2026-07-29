#ifndef RISC_V_CPU_INSTRUCTION_H
#define RISC_V_CPU_INSTRUCTION_H
#include <cstdint>

enum class InstructionType {
    ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU, ADDI, ANDI, ORI, XORI, SLLI, SRLI,
    SRAI, SLTI, SLTIU, LB, LBU, LH, LHU, LW, SB, SH, SW, BEQ, BGE, BGEU, BLT, BLTU, BNE,
    JAL, JALR, AUIPC, LUI, MUL,HALT,
};

struct Instruction {
    uint32_t pc = 0;
    uint32_t raw_bits = 0;
    InstructionType type = InstructionType::HALT;
    uint32_t rd = 0, rs1 = 0, rs2 = 0;
    int32_t imm = 0;

    Instruction() = default;
};

Instruction decode(uint32_t pc, uint32_t raw_bits);

#endif //RISC_V_CPU_INSTRUCTION_H