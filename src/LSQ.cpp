#include "../include/LSQ.h"

void LSQ::update() {
    for (int i = 0; i < 16; ++i) {
        entries[i] = next_entries[i];
    }
    head = next_head;
    tail = next_tail;
    cnt = next_cnt;
}

bool LSQ::is_full() const {
    return cnt >= 16;
}

bool LSQ::is_empty() const {
    return cnt == 0;
}

void LSQ::clear() {
    for (int i = 0; i < 16; ++i) {
        entries[i] = LSQEntry();
        next_entries[i] = LSQEntry();
    }
    head = tail = cnt = 0;
    next_head = next_tail = next_cnt = 0;
}

int LSQ::push(InstructionType op, bool is_store, uint32_t vj, uint32_t vk, int qj, int qk, int32_t imm, int rob) {
    if (is_full()) {
        return -1;
    }
    int idx = next_tail;
    next_entries[idx].busy = true;
    next_entries[idx].is_store = is_store;
    next_entries[idx].op = op;
    next_entries[idx].vj = vj;
    next_entries[idx].vk = vk;
    next_entries[idx].qj = qj;
    next_entries[idx].qk = qk;
    next_entries[idx].imm = imm;
    next_entries[idx].rob = rob;
    next_entries[idx].ready = false;
    next_tail = (next_tail + 1) % 32;
    next_cnt++;
    return idx;
}

