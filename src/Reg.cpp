#include "../include/Reg.h"

void Register::update() {
    cur.val[0] = 0;
    cur.rob[0] = -1;
    next.val[0] = 0;
    next.rob[0] = -1;
    for (int i = 1; i < 32; ++i) {
        cur.val[i] = next.val[i];
        if (issue_wrote[i]) {
            cur.rob[i] = next.rob[i];          // issue 的写优先
        }
        else if (commit_clear[i]) {
            cur.rob[i] = -1;                  // commit 清除过期 tag
        }
        else {
            cur.rob[i] = next.rob[i];         // 沿用（next 沿用自上一周期）
        }
        issue_wrote[i] = false;
        commit_clear[i] = false;
        next.val[i] = cur.val[i];
        next.rob[i] = cur.rob[i];
    }
}

void Register::clear() {
    for (int i = 0; i < 32; ++i) {
        cur.val[i] = 0;
        cur.rob[i] = -1;
        next.val[i] = 0;
        next.rob[i] = -1;
        issue_wrote[i] = false;
        commit_clear[i] = false;
    }
}

void Register::clear_rob() {
    for (int i = 0; i < 32; ++i) {
        cur.rob[i] = -1;
        next.rob[i] = -1;
        issue_wrote[i] = false;
        commit_clear[i] = false;
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
    issue_wrote[reg_id] = true;
}

void Register::commit_clear_tag(uint32_t reg_id, int rob_head) {
    if (reg_id == 0) {
        return;
    }
    if (cur.rob[reg_id] == rob_head) {
        commit_clear[reg_id] = true;
    }
}
