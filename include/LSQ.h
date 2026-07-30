#ifndef RISC_V_CPU_LSQ_H
#define RISC_V_CPU_LSQ_H
#include "Instruction.h"

struct LSQEntry {
    InstructionType op = InstructionType::HALT;
    bool is_store = false;
    uint32_t vj = 0, vk = 0; //rs1读出的基地址，rs2读出的待存数据
    int qj = -1, qk = -1;  //二者的rob编码
    uint32_t addr = 0; //vj + imm
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

    LSQ() = default;
    void update();
    bool is_full() const;
    bool is_empty() const;
    int push(InstructionType op, bool is_store, uint32_t vj, uint32_t vk, int qj, int qk, int32_t imm, int rob);
    void clear();
};
#endif //RISC_V_CPU_LSQ_H