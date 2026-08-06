#ifndef RISC_V_CPU_CPU_H
#define RISC_V_CPU_CPU_H

#include <cstdint>
#include "ALU.h"
#include "CDB.h"
#include "Instruction.h"
#include "LSQ.h"
#include "Memory.h"
#include "Predictor.h"
#include "Reg.h"
#include "ROB.h"
#include "RS.h"

// 各 module（stage）均遵循「读 cur 旧状态 / 写 next 新状态」的时序纪律，
// 因此 run() 中各 stage 的调用顺序可以任意交换（满足 CR 随机打乱要求）。
// 唯一的跨周期依赖：分支预测失败采用「本周期置位 / 下周期清空」的两段式
// 处理，配合 cur/next 的 jalr / mispredicted / pc 状态，保证顺序无关。
class CPU {
    Memory mem;
    Register reg;
    ROB rob;
    RS rs;
    LSQ lsq;
    CDB cdb;
    BimodelPredictor predictor;

    // 前端时序状态（cur）
    uint32_t pc = 0;
    uint32_t next_pc = 0;
    bool jalr = false;          // JALR 在途，暂停取指
    bool halted = false;        // 已取到 HALT
    bool mispredicted = false;  // 上一周期检测到预测失败，本周期执行清空
    uint32_t redirect_pc = 0;

    // 前端时序状态（next），更新时拷贝到 cur
    bool next_jalr = false;
    bool next_halted = false;
    bool next_mispredicted = false;
    uint32_t next_redirect_pc = 0;

    // 统计
    uint64_t cycle_cnt = 0;
    uint64_t predict_total = 0;
    uint64_t predict_correct = 0;

    // 取指时为源寄存器读取操作数（读 cur：reg / rob / cdb）
    void read_operand(uint32_t reg_id, uint32_t& v, int& q);

public:
    // 读入机器码（@addr + 十六进制字节）到内存
    void load_program();

    // 检查是否应终止：halted 且 ROB/LSQ 排空且内存空闲，则输出结果返回 true
    bool check_terminated();

    // 1. 标记已就绪的 store 的 ROB 项为 ready（读 LSQ cur，写 ROB next）
    void store_resolve();

    // 2. 提交 ROB 头部（含分支预测失败检测、JALR、store 写发出、普通提交）
    //    返回 true 表示遇到 HALT 并已输出结果（当前实现中 HALT 不入 ROB，
    //    该返回值保留以兼容旧逻辑）
    bool commit();

    // 3. CDB 广播扇出（读 CDB cur，写 ROB/RS/LSQ next）
    void cdb_listen();

    // 4. RS 计算 + 发 CDB（基于 cur 的固定优先级仲裁：store/load 完成 > RS 就绪）
    void execute();

    // 5. 访存完成 + load 发出（基于 cur 仲裁；写 CDB/Memory/LSQ next）
    void memory_access();

    // 6. 取指 / 译码 / 分配 / 预测（读 cur：mem 指令、reg、rob、cdb、predictor）
    void issue();

    // 7. 周期末更新：各部件 next->cur，前端状态 next->cur，pc=next_pc
    void update_all();

    // 主循环：顺序可任意交换的各 stage 调用
    void run();

    // 输出统计信息（时钟周期数、分支预测准确率）到 stderr
    void print_stats() const;
};

#endif //RISC_V_CPU_CPU_H
