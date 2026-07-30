#ifndef RISC_V_CPU_RS_H
#define RISC_V_CPU_RS_H
#include "Instruction.h"

struct RSEntry {
    InstructionType op = InstructionType::HALT;
    uint32_t vj = 0, vk = 0;   //源寄存器的值
    int qj = -1, qk = -1;   //源寄存器依赖的ROB编号
    int dest = -1;   // 目标ROB编号
    uint32_t pc = 0;  //指令自身pc
    int32_t imm = 0;  // 立即数
    bool busy = false; //保留站该槽位是否被占用
};

class RS {
public:
    RSEntry entries[32];
    RSEntry next_entries[32];

    RS() = default;
    void update();
    bool is_full() const;
    int allocate() const;
    void clear();
};
#endif //RISC_V_CPU_RS_H