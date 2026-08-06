#ifndef RISC_V_CPU_REG_H
#define RISC_V_CPU_REG_H
#include <cstdint>

struct RegState {
    uint32_t val[32] = {0};
    int rob[32]{};

    RegState() {
        for (int i = 0; i < 32; ++i) {
            rob[i] = -1;
        }
    }
};

class Register {
    RegState cur;
    RegState next;

    // 顺序无关化：issue 的 set_rob 与 commit 的清 tag 都可能写同一 rd 的 rob 域。
    // 用两个掩码分别记录「本周期 issue 写过」「本周期 commit 要求清」，update 时合并，
    // 规则：issue 写过 > commit 清 > 沿用旧值。这样二者执行顺序可任意交换。
    bool issue_wrote[32] = {};
    bool commit_clear[32] = {};

public:
    void update();
    void clear();
    void clear_rob();
    //读寄存器
    uint32_t read_value(int reg_id) const;
    int read_rob(uint32_t reg_id) const;
    //写寄存器
    void set_value(uint32_t reg_id, uint32_t value);
    void set_rob(uint32_t reg_id, int rob_id);
    //提交时清除过期的 rob tag（仅当 cur 仍指向该 rob 头时；不与 issue 的写冲突）
    void commit_clear_tag(uint32_t reg_id, int rob_head);
};

#endif //RISC_V_CPU_REG_H
