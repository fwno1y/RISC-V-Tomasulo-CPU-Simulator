#ifndef RISC_V_CPU_LSQ_H
#define RISC_V_CPU_LSQ_H
#include "Instruction.h"

struct LSQEntry {
    InstructionType op = InstructionType::HALT;
    bool is_store = false;  //区分读写标志
    uint32_t vj = 0, vk = 0; //rs1读出的基地址，rs2读出的待存数据
    int qj = -1, qk = -1;  //二者的rob编码
    uint32_t addr = 0; //vj + imm，最终访存地址
    int32_t imm = 0;
    bool ready = false; //地址是否计算完毕
    int rob = -1;  //指令的目标rob编号
    bool busy = false;
};

class LSQ {
    LSQEntry entries[16];
    int head = 0;
    int tail = 0;
    int cnt = 0;
    LSQEntry next_entries[16];
    int next_head = 0;
    int next_tail = 0;
    int next_cnt = 0;

public:
    LSQ() = default;
    void update();
    void clear();
    bool is_full() const;
    bool is_empty() const;
    int push(InstructionType op, bool is_store, uint32_t vj, uint32_t vk, int qj, int qk, int32_t imm, int rob);
    void listen_cdb(int rob_id, uint32_t value);
    //寻找当前队列可安全发射的load指令
    int find_load(int& rob_out) const;
    //队列头部的store确认，用于提交最老的store指令
    bool find_store(int rob_id, uint32_t& addr, uint32_t& data, int& size) const;
    void pop_head();
    void pop_entry(int idx);
    const LSQEntry& get_entry(int idx) const;
};
#endif //RISC_V_CPU_LSQ_H