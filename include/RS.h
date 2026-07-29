#ifndef RISC_V_CPU_RS_H
#define RISC_V_CPU_RS_H
#include "Instruction.h"

struct RSEntry {
    InstructionType op = InstructionType::HALT;
    uint32_t vj = 0, vk = 0;
    int qj = -1, qk = -1;
    int dest = -1;
    uint32_t pc = 0;
    int32_t imm = 0;
    bool busy = false;
};

class RS {
    RSEntry entries[32];
    RSEntry next_entries[32];

    void update();
    bool is_full() const;
    int allocate() const;
};
#endif //RISC_V_CPU_RS_H