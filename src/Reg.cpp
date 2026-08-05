#include "../include/Reg.h"

void Register::update() {
    cur = next;
    cur.val[0] = 0;
    cur.rob[0] = -1;
    next.val[0] = 0;
    next.rob[0] = -1;
}

void Register::clear() {
    for (int i = 0; i < 32; ++i) {
        cur.val[i] = 0;
        cur.rob[i] = -1;
        next.val[i] = 0;
        next.rob[i] = -1;
    }
}

uint32_t Register::read_value(int reg_id) const {
    if (reg_id == 0) {
        return 0;
    }
    return cur.val[reg_id];
}

int Register::read_rob(uint32_t reg_id) const {
    if (reg_id == 0) {
        return -1;
    }
    return cur.rob[reg_id];
}

void Register::set_value(uint32_t reg_id, uint32_t value) {
    if (reg_id == 0) {
        return;
    }
    next.val[reg_id] = value;
}

void Register::set_rob(uint32_t reg_id, int rob_id) {
    if (reg_id == 0) {
        return;
    }
    next.rob[reg_id] = rob_id;
}





