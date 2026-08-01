#include "../include/ROB.h"

void ROB::update() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = next_entries[i];
    }
    head = next_head;
    tail = next_tail;
    cnt = next_cnt;
}

bool ROB::is_full() const {
    return cnt >= 32;
}

bool ROB::is_empty() const {
    return cnt == 0;
}


int ROB::push(Instruction instruction, uint32_t dest) {
    if (is_full()) {
        return -1;
    }
    int idx = next_tail;
    next_entries[idx].busy = true;
    next_entries[idx].instruction = instruction;
    next_entries[idx].dest = dest;
    next_entries[idx].value = 0;
    next_entries[idx].ready = false;
    next_tail = (next_tail + 1) % 32;
    next_cnt++;
    return idx;
}

void ROB::clear() {

}
