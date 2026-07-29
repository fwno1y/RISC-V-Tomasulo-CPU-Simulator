#include "../include/ALU.h"

uint32_t ALU::execute(InstructionType op, uint32_t op1, uint32_t op2, uint32_t pc, uint32_t imm) {
    switch (op) {
        case InstructionType::ADDI:
        case InstructionType::ADD:
            return op1 + op2;
        case InstructionType::SUB:
            return op1 - op2;
        case InstructionType::ANDI:
        case InstructionType::AND:
            return op1 & op2;
        case InstructionType::ORI:
        case InstructionType::OR:
            return op1 | op2;
        case InstructionType::XORI:
        case InstructionType::XOR:
            return op1 ^ op2;
        case InstructionType::SLLI:
        case InstructionType::SLL:
            return op1 << (op2 & 0x1f);
        case InstructionType::SRLI:
        case InstructionType::SRL:
            return op1 >> (op2 & 0x1f);
        case InstructionType::SRAI:
        case InstructionType::SRA:
            return static_cast<uint32_t>(static_cast<int32_t>(op1) >> (op2 & 0x1f));
        case InstructionType::SLTI:
        case InstructionType::SLT:
            return static_cast<int32_t> (op1) < static_cast<int32_t>(op2) ? 1 : 0;
        case InstructionType::SLTIU:
        case InstructionType::SLTU:
            return op1 < op2 ? 1 : 0;
        case InstructionType::LUI:
            return imm;
        case InstructionType::AUIPC:
            return pc + imm;
        default:
            return 0;
    }
}

bool ALU::execute_branch(InstructionType op, uint32_t op1, uint32_t op2) {
    switch (op) {
        case InstructionType::BEQ:
            return op1 == op2;
        case InstructionType::BGE:
            return static_cast<int32_t>(op1) >= static_cast<int32_t>(op2);
        case InstructionType::BGEU:
            return op1 >= op2;
        case InstructionType::BLT:
            return static_cast<int32_t>(op1) < static_cast<int32_t>(op2);
        case InstructionType::BLTU:
            return op1 < op2;
        case InstructionType::BNE:
            return op1 != op2;
        default:
            return false;
    }
}
