#include "../include/ROB.h"

void ROB::update() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = next_entries[i];
    }
    head = next_head;
    tail = next_tail;
    cnt = next_cnt;
}

void ROB::clear() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = ROBEntry();
        next_entries[i] = ROBEntry();
    }
    head = tail = cnt = 0;
    next_head = next_tail = next_cnt = 0;
}

bool ROB::is_full() const {
    return cnt >= 32;
}

bool ROB::is_empty() const {
    return cnt == 0;
}

int ROB::get_count() const {
    return cnt;
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

const ROBEntry &ROB::get_head() const {
    return entries[head];
}

ROBEntry &ROB::get_next_head() {
    return next_entries[next_head];
}

void ROB::commit_head() {
    next_entries[next_head].busy = false;
    next_head = (next_head + 1) % 32;
    next_cnt--;
}

void ROB::listen_cdb(int rob_id, uint32_t value, uint32_t target_pc) {
    if (rob_id >= 0 && rob_id < 32 && next_entries[rob_id].busy) {
        next_entries[rob_id].value = value;
        next_entries[rob_id].target_pc = target_pc;
        next_entries[rob_id].ready = true;
    }
}

const ROBEntry &ROB::get_entry(int rob_id) const {
    return entries[rob_id];
}

void ROB::flush(int rob_id) {
    int idx = (rob_id + 1) % 32;
    while (idx != next_tail) {
        next_entries[idx] = ROBEntry();
        idx = (idx + 1) % 32;
    }
    next_tail = (rob_id + 1) % 32;
    next_cnt = (rob_id - next_head + 33) % 32 + 1;
}




