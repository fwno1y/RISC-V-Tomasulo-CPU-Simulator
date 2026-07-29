#ifndef RISC_V_CPU_ALU_H
#define RISC_V_CPU_ALU_H
#include <cstdint>

#include "Instruction.h"

class ALU {
public:
    ALU() = default;
    static uint32_t execute(InstructionType op, uint32_t op1, uint32_t op2, uint32_t pc, uint32_t imm);
    static bool execute_branch(InstructionType op, uint32_t op1, uint32_t op2);
};

#endif //RISC_V_CPU_ALU_H