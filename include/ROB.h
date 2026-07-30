#ifndef RISC_V_CPU_ROB_H
#define RISC_V_CPU_ROB_H
#include "Instruction.h"

struct ROBEntry {
    Instruction instruction;
    uint32_t dest = 0;
    uint32_t value = 0;
    bool ready = false;
    bool busy = false;

    ROBEntry() = default;
};

class ROB {
    ROBEntry entries[32];
    int head = 0;
    int tail = 0;
    int cnt = 0;
    ROBEntry next_entries[32];
    int next_head = 0;
    int next_tail = 0;
    int next_cnt = 0;

    void update();
    bool is_full() const;
    bool is_empty() const;
    int push(Instruction instruction, uint32_t dest);
};

#endif //RISC_V_CPU_ROB_H