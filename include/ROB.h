#ifndef RISC_V_CPU_ROB_H
#define RISC_V_CPU_ROB_H
#include "Instruction.h"

struct ROBEntry {
    Instruction instruction;
    uint32_t dest = 0;  //修改目标寄存器编号
    uint32_t value = 0;  //已计算但未提交的临时结果
    uint32_t target_pc = 0;  //跳转目标
    bool jump = false;  //是否跳转
    bool ready = false;  // 是否算完就绪
    bool busy = false;  //是否被占用

    ROBEntry() = default;
};

class ROB {
public:
    ROBEntry entries[32];
    int head = 0;
    int tail = 0;
    int cnt = 0;
    ROBEntry next_entries[32];
    int next_head = 0;
    int next_tail = 0;
    int next_cnt = 0;

    ROB() = default;
    void update();
    void clear();
    bool is_full() const;
    bool is_empty() const;
    int get_count() const;
    int push(Instruction instruction, uint32_t dest);
    const ROBEntry& get_head() const;
    ROBEntry& get_next_head();
    void commit_head();
    void listen_cdb(int rob_id, uint32_t value, uint32_t target_pc);
    const ROBEntry& get_entry(int rob_id) const;
    void flush(int rob_id);
};

#endif //RISC_V_CPU_ROB_H