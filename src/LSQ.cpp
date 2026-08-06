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
    next_entries[idx].ready = (qj == -1);
    if (next_entries[idx].ready) {
       next_entries[idx].addr = vj + imm;
    }
    next_tail = (next_tail + 1) % 16;
    next_cnt++;
    return idx;
}

void LSQ::listen_cdb(int rob_id, uint32_t value) {
    for (int i = 0; i < 16; ++i) {
        if (!next_entries[i].busy) {
            continue;
        }
        if (next_entries[i].qj == rob_id) {
            next_entries[i].vj = value;
            next_entries[i].qj = -1;
            next_entries[i].ready = true;
            next_entries[i].addr = value + next_entries[i].imm;
        }
        if (next_entries[i].qk == rob_id) {
            next_entries[i].vk = value;
            next_entries[i].qk = -1;
        }
    }
}

int LSQ::find_load(int &rob_out) const {
    for (int i = 0; i < cnt; ++i) {
        int idx = (head + i) % 16;
        if (entries[idx].busy && !entries[idx].is_store && entries[idx].ready) {
            bool flag = false;
            for (int j = 0; j < i; ++j) {
                int pre_idx = (head + j) % 16;
                if (entries[pre_idx].busy && entries[pre_idx].is_store) {
                    if (!entries[pre_idx].ready || entries[pre_idx].addr == entries[idx].addr) {
                        flag = true;
                        break;
                    }
                }
            }
            if (!flag) {
                rob_out = entries[idx].rob;
                return idx;
            }
        }
    }
    return -1;
}

bool LSQ::find_store(int rob_id, uint32_t &addr, uint32_t &data, int &size) const {
    if (is_empty()) {
        return false;
    }
    const LSQEntry& head_entry = entries[head];
    if (head_entry.busy && head_entry.is_store && head_entry.rob == rob_id && head_entry.ready && head_entry.qk == -1) {
        addr = head_entry.addr;
        data = head_entry.vk;
        if (head_entry.op == InstructionType::SB) {
            size = 1;
        }
        else if (head_entry.op == InstructionType::SH) {
            size = 2;
        }
        else {
            size = 4;
        }
        return true;
    }
    return false;
}

void LSQ::pop_head() {
    pop_entry(head);
}


void LSQ::pop_entry(int idx) {
    next_entries[idx] = LSQEntry();
    while (next_cnt > 0 && !next_entries[next_head].busy) {
        next_head = (next_head + 1) % 16;
        next_cnt--;
    }
}

const LSQEntry &LSQ::get_entry(int idx) const {
    return entries[idx];
}






