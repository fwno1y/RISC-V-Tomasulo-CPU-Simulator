#include "../include/RS.h"

void RS::update() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = next_entries[i];
    }
}

void RS::clear() {
    for (int i = 0; i < 32; ++i) {
        entries[i] = RSEntry();
        next_entries[i] = RSEntry();
    }
}

bool RS::is_full() const {
    for (int i = 0; i < 32; ++i) {
        if (!entries[i].busy) {
            return false;
        }
    }
    return true;
}

int RS::allocate() const {
    for (int i = 0; i < 32; ++i) {
        if (!entries[i].busy) {
            return i;
        }
    }
    return -1;
}

void RS::issue(int rs_id, InstructionType op, uint32_t vj, uint32_t vk, int qj, int qk, int dest, uint32_t pc, int32_t imm) {
    next_entries[rs_id].op = op;
    next_entries[rs_id].vj = vj;
    next_entries[rs_id].vk = vk;
    next_entries[rs_id].qj = qj;
    next_entries[rs_id].qk = qk;
    next_entries[rs_id].dest = dest;
    next_entries[rs_id].pc = pc;
    next_entries[rs_id].imm = imm;
    next_entries[rs_id].busy = true;
}

void RS::listen_cdb(int rob_id, uint32_t value) {
    for (int i = 0; i < 32; ++i) {
        if (!next_entries[i].busy) {
            continue;
        }
        if (next_entries[i].qj == rob_id) {
            next_entries[i].vj = value;
            next_entries[i].qj = -1;
        }
        if (next_entries[i].qk == rob_id) {
            next_entries[i].vk = value;
            next_entries[i].qk = -1;
        }
    }
}

int RS::find_ready() const {
    for (int i = 0; i < 32; ++i) {
        if (entries[i].busy && entries[i].qj == -1 && entries[i].qk == -1) {
            return i;
        }
    }
    return -1;
}

void RS::release(int rs_id) {
    next_entries[rs_id] = RSEntry();
}

const RSEntry &RS::get_entry(int rs_id) const {
    return entries[rs_id];
}





