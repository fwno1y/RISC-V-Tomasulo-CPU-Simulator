#include "../include/Instruction.h"

#include <complex.h>
#include <numeric>

Instruction decode(uint32_t pc, uint32_t raw_bits) {
    Instruction instruction;
    instruction.pc = pc;
    instruction.raw_bits = raw_bits;
    if (raw_bits == 0x0ff00513) {
        instruction.type = InstructionType::HALT;
        return instruction;
    }
    uint32_t opcode = raw_bits & 0x7f; //取后七位
    instruction.rd = raw_bits >> 7 & 0x1f;
    instruction.rs1 = raw_bits >> 15 & 0x1f;
    instruction.rs2 = raw_bits >> 20 & 0x1f;
    uint32_t funct3 = raw_bits >> 12 & 0x7;
    uint32_t funct7 = raw_bits >> 25 & 0x7f;
    int32_t imm_i = static_cast<int32_t>(raw_bits) >> 20;
    int32_t imm_s = (static_cast<int32_t>(raw_bits) >> 25 << 5) | (raw_bits >> 7 & 0x1f);
    uint32_t imm_b = ((raw_bits >> 31 & 1) << 12) | ((raw_bits >> 7 & 1) << 11) | ((raw_bits >> 25 & 0x3f) << 5) | ((raw_bits >> 8 & 0xf) << 1);
    if (imm_b & 0x1000) {
        imm_b |= 0xffffe000;
    }
    uint32_t imm_u = raw_bits & 0xfffff000;
    uint32_t imm_j = ((raw_bits >> 31 & 1) << 20) | ((raw_bits >> 12 & 0xff) << 12) | ((raw_bits >> 20 & 1) << 11) | ((raw_bits >> 21 & 0x3ff) << 1);
    if (imm_j & 0x100000) {
        imm_j |= 0xffe00000;
    }
    switch (opcode) {
        case 0b0110011 : {
            if (funct3 == 0) {
                if (funct7 == 0x20) {
                    instruction.type = InstructionType::SUB;
                }
                else {
                    instruction.type = InstructionType::ADD;
                }
            }
            else if (funct3 == 7) {
                instruction.type = InstructionType::AND;
            }
            else if (funct3 == 6) {
                instruction.type = InstructionType::OR;
            }
            else if (funct3 == 4) {
                instruction.type = InstructionType::XOR;
            }
            else if (funct3 == 1) {
                instruction.type = InstructionType::SLL;
            }
            else if (funct3 == 5) {
                if (funct7 == 0x20) {
                    instruction.type = InstructionType::SRA;
                }
                else {
                    instruction.type = InstructionType::SRL;
                }
            }
            else if (funct3 == 2) {
                instruction.type = InstructionType::SLT;
            }
            else if (funct3 == 3) {
                instruction.type = InstructionType::SLTU;
            }
            break;
        }
        case 0b0010011 : {
            instruction.imm = imm_i;
            if (funct3 == 0) {
                instruction.type = InstructionType::ADDI;
            }
            else if (funct3 == 7) {
                instruction.type = InstructionType::ANDI;
            }
            else if (funct3 == 6) {
                instruction.type = InstructionType::ORI;
            }
            else if (funct3 == 4) {
                instruction.type = InstructionType::XORI;
            }
            else if (funct3 == 1) {
                instruction.type = InstructionType::SLLI;
                instruction.imm = instruction.rs2;
            }
            else if (funct3 == 5) {
                if (funct7 == 0x20) {
                    instruction.type = InstructionType::SRAI;
                }
                else {
                    instruction.type = InstructionType::SRLI;
                }
                instruction.imm = instruction.rs2;
            }
            break;
        }
        case 0b0000011 : {
            instruction.imm = imm_i;
            if (funct3 == 0) {
                instruction.type = InstructionType::LB;
            }
            else if (funct3 == 4) {
                instruction.type = InstructionType::LBU;
            }
            else if (funct3 == 1) {
                instruction.type = InstructionType::LH;
            }
            else if (funct3 == 5) {
                instruction.type = InstructionType::LHU;
            }
            else if (funct3 == 2) {
                instruction.type = InstructionType::LW;
            }
            break;
        }
        case 0b0100011 : {
            instruction.imm = imm_s;
            if (funct3 == 0) {
                instruction.type = InstructionType::SB;
            }
            else if (funct3 == 1) {
                instruction.type = InstructionType::SH;
            }
            else if (funct3 == 2) {
                instruction.type = InstructionType::SW;
            }
            break;
        }
        case 0b1100011 : {
            instruction.imm = imm_b;
            if (funct3 == 0) {
                instruction.type = InstructionType::BEQ;
            }
            else if (funct3 == 5) {
                instruction.type = InstructionType::BGE;
            }
            else if (funct3 == 7) {
                instruction.type = InstructionType::BGEU;
            }
            else if (funct3 == 4) {
                instruction.type = InstructionType::BLT;
            }
            else if (funct3 == 6) {
                instruction.type = InstructionType::BLTU;
            }
            else if (funct3 == 1) {
                instruction.type = InstructionType::BNE;
            }
            break;
        }
        case 0b1101111 : {
            instruction.type = InstructionType::JAL;
            instruction.imm = imm_j;
            break;
        }
        case 0b1100111 : {
            instruction.type = InstructionType::JALR;
            instruction.imm = imm_i;
            break;
        }
        case 0b0010111 : {
            instruction.type = InstructionType::AUIPC;
            instruction.imm = imm_u;
            break;
        }
        case 0b0110111 : {
            instruction.type = InstructionType::LUI;
            instruction.imm = imm_u;
            break;
        }
        default:
            instruction.type = InstructionType::HALT;
            break;
    }
    if (instruction.type == InstructionType::SB || instruction.type == InstructionType::SH || instruction.type == InstructionType::SW ||
        instruction.type == InstructionType::BEQ || instruction.type == InstructionType::BGE || instruction.type == InstructionType::BGEU ||
        instruction.type == InstructionType::BLT || instruction.type == InstructionType::BLTU || instruction.type == InstructionType::BNE) {
        instruction.rd = 0;
    }
    return instruction;
}
