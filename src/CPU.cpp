#include "../include/CPU.h"

#include <iostream>
#include <sstream>

namespace {
bool is_branch_type(InstructionType t) {
    return t == InstructionType::BEQ || t == InstructionType::BGE || t == InstructionType::BGEU ||
           t == InstructionType::BLT || t == InstructionType::BLTU || t == InstructionType::BNE;
}

bool is_store_type(InstructionType t) {
    return t == InstructionType::SB || t == InstructionType::SH || t == InstructionType::SW;
}

bool is_load_type(InstructionType t) {
    return t == InstructionType::LB || t == InstructionType::LBU || t == InstructionType::LH ||
           t == InstructionType::LHU || t == InstructionType::LW;
}

bool is_mem_type(InstructionType t) {
    return is_load_type(t) || is_store_type(t);
}

bool is_imm_arith(InstructionType t) {
    return t == InstructionType::ADDI || t == InstructionType::ANDI || t == InstructionType::ORI ||
           t == InstructionType::XORI || t == InstructionType::SLLI || t == InstructionType::SRLI ||
           t == InstructionType::SRAI || t == InstructionType::SLTI || t == InstructionType::SLTIU;
}
}  // namespace

void CPU::load_program() {
    std::string line;
    uint32_t cur_addr = 0;
    bool in_segment = false;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            in_segment = false;
            continue;
        }
        if (line[0] == '@') {
            cur_addr = std::stoul(line.substr(1), nullptr, 16);
            in_segment = true;
        }
        else if (in_segment) {
            std::istringstream iss(line);
            std::string str;
            while (iss >> str) {
                uint8_t byte = static_cast<uint8_t>(std::stoul(str, nullptr, 16));
                mem.load_byte(cur_addr++, byte);
            }
        }
    }
}

bool CPU::check_terminated() {
    if (halted && rob.is_empty() && !mem.is_busy()) {
        uint32_t res = reg.read_value(10) & 0xff;
        std::cout << std::dec << res << std::endl;
        return true;
    }
    return false;
}

void CPU::store_resolve() {
    for (int i = 0; i < 16; ++i) {
        const LSQEntry& entry = lsq.get_entry(i);  // cur
        if (entry.busy && entry.is_store && entry.ready && entry.qk == -1) {
            rob.next_entries[entry.rob].ready = true;  // next
        }
    }
}

bool CPU::commit() {
    // 上一周期检测到预测失败：本周期统一清空所有推测状态
    if (mispredicted) {
        rob.clear();
        rs.clear();
        lsq.clear();
        cdb.clear();
        mem.clear();
        reg.clear_rob();
        next_mispredicted = false;
        next_jalr = false;
        next_pc = redirect_pc;
        return false;
    }
    if (rob.is_empty()) {
        return false;
    }
    const ROBEntry& head = rob.get_head();  // cur
    if (!head.ready) {
        return false;
    }
    const Instruction& inst = head.instruction;

    // 写寄存器（next）
    if (inst.rd != 0) {
        reg.set_value(inst.rd, head.value);
        reg.commit_clear_tag(inst.rd, rob.head);  // 顺序无关地清除过期 rob tag
    }

    if (is_branch_type(inst.type)) {
        predict_total++;
        bool actual_taken = (head.target_pc != inst.pc + 4);
        bool predict_taken = head.jump;
        predictor.update(inst.pc, actual_taken);  // 写 predictor.next_bht
        if (actual_taken == predict_taken) {
            predict_correct++;
        }
        // 预测失败：本周期仅置位，下周期由 mispredicted 分支统一清空
        if (actual_taken != predict_taken) {
            next_mispredicted = true;
            next_redirect_pc = head.target_pc;
            next_pc = head.target_pc;
            return false;
        }
        // 预测正确：正常提交（落到末尾 commit_head）
    }
    else if (inst.type == InstructionType::JALR) {
        next_pc = head.target_pc;
        next_jalr = false;
    }
    else if (inst.type == InstructionType::HALT) {
        uint32_t res = reg.read_value(10) & 0xff;
        std::cout << std::dec << res << std::endl;
        return true;
    }

    // store 提交：发出写请求（next）；成功则弹出 LSQ 头并提交 ROB 头
    if (is_store_type(inst.type)) {
        uint32_t st_addr, st_data;
        int st_size;
        if (lsq.find_store(rob.head, st_addr, st_data, st_size)) {  // cur
            if (mem.issue_write(st_addr, st_data, st_size)) {       // next
                lsq.pop_head();                                     // next
                rob.commit_head();                                  // next
            }
        }
    }
    else {
        rob.commit_head();  // next
    }
    return false;
}

void CPU::cdb_listen() {
    if (!cdb.is_active()) {  // cur
        return;
    }
    int rob_id = cdb.get_rob();
    uint32_t val = cdb.get_value();
    uint32_t target = cdb.get_target_pc();
    rob.listen_cdb(rob_id, val, target);  // next
    rs.listen_cdb(rob_id, val);           // next
    lsq.listen_cdb(rob_id, val);           // next
}

void CPU::execute() {
    //   RS 就绪即可发射，享有 CDB 优先权；load 完成仅在无 RS 就绪时占用 CDB。
    int ready_rs = rs.find_ready();  // cur

    if (ready_rs != -1) {
        const RSEntry& entry = rs.get_entry(ready_rs);  // cur
        uint32_t res = ALU::execute(entry.op, entry.vj, entry.vk, entry.pc, entry.imm);
        uint32_t target_pc = entry.pc + 4;

        if (is_branch_type(entry.op)) {
            bool tmp = ALU::execute_branch(entry.op, entry.vj, entry.vk);
            target_pc = tmp ? (entry.pc + entry.imm) : (entry.pc + 4);
            res = tmp ? 1 : 0;
        }
        else if (entry.op == InstructionType::JAL) {
            target_pc = entry.pc + entry.imm;
        }
        else if (entry.op == InstructionType::JALR) {
            target_pc = (entry.vj + entry.imm) & ~1u;
        }
        cdb.broadcast(entry.dest, res, target_pc);  // next
        rs.release(ready_rs);                       // next
    }
}

void CPU::memory_access() {
    // 基于 cur ：仅当无 RS 就绪时，load 完成结果占用 CDB
    int mem_rob = -1;
    uint32_t mem_val = 0;
    bool mem_done = mem.check_read_done(mem_rob, mem_val);  // cur
    bool exec_ready = (rs.find_ready() != -1);               // cur

    if (mem_done && !exec_ready) {
        cdb.broadcast(mem_rob, mem_val, 0);  // next
        mem.finish_read();                    // next
    }
    mem.check_write_done();  // cur 读 + 写 mem 字节（store 写完成）

    // load 发出
    if (!mem.is_busy()) {  // cur
        int load_rob = -1;
        int ready_load = lsq.find_load(load_rob);  // cur
        if (ready_load != -1) {
            const LSQEntry& entry = lsq.get_entry(ready_load);  // cur
            int size = 4;
            if (entry.op == InstructionType::LB || entry.op == InstructionType::LBU) {
                size = 1;
            }
            else if (entry.op == InstructionType::LH || entry.op == InstructionType::LHU) {
                size = 2;
            }
            if (mem.issue_read(entry.addr, size, load_rob)) {  // next
                lsq.pop_entry(ready_load);                     // next
            }
        }
    }
}

void CPU::read_operand(uint32_t reg_id, uint32_t& v, int& q) {
    v = 0;
    q = -1;
    if (reg_id == 0) {
        return;
    }
    int r_dep = reg.read_rob(reg_id);  // cur
    if (r_dep == -1) {
        v = reg.read_value(reg_id);  // cur
    }
    else if (rob.entries[r_dep].ready) {  // cur
        v = rob.entries[r_dep].value;     // cur
    }
    else if (cdb.is_active() && cdb.get_rob() == r_dep) {  // cur
        v = cdb.get_value();                                // cur
    }
    else {
        q = r_dep;
    }
}

void CPU::issue() {
    if (rob.is_full() || rs.is_full() || lsq.is_full()) {
        return;
    }
    if (jalr || halted || mispredicted) {  // cur
        return;
    }
    uint32_t raw_inst = mem.read_instruction(pc);  // cur
    Instruction inst = decode(pc, raw_inst);

    if (inst.type == InstructionType::HALT) {
        next_halted = true;
        return;
    }

    uint32_t vj = 0, vk = 0;
    int qj = -1, qk = -1;
    read_operand(inst.rs1, vj, qj);
    read_operand(inst.rs2, vk, qk);

    int allocate_rob = rob.push(inst, inst.rd);  // next
    if (inst.rd != 0) {
        reg.set_rob(inst.rd, allocate_rob);  // next
    }

    if (is_mem_type(inst.type)) {
        bool is_st = is_store_type(inst.type);
        lsq.push(inst.type, is_st, vj, vk, qj, qk, inst.imm, allocate_rob);  // next
    }
    else {
        int rs_idx = rs.allocate();  // cur
        if (is_imm_arith(inst.type)) {
            vk = inst.imm;
            qk = -1;
        }
        rs.issue(rs_idx, inst.type, vj, vk, qj, qk, allocate_rob, pc, inst.imm);  // next
    }

    // 计算下一 PC（next_pc）
    if (is_branch_type(inst.type)) {
        bool taken = predictor.predict(pc);  // cur bht
        next_pc = taken ? (pc + inst.imm) : (pc + 4);
        rob.next_entries[allocate_rob].jump = taken;  // next
    }
    else if (inst.type == InstructionType::JAL) {
        next_pc = pc + inst.imm;
    }
    else if (inst.type == InstructionType::JALR) {
        next_jalr = true;  // 暂停取指直到 JALR 提交
    }
    else {
        next_pc = pc + 4;
    }
}

void CPU::update_all() {
    mem.update();
    reg.update();
    rob.update();
    rs.update();
    lsq.update();
    cdb.update();
    predictor.advance();

    // 前端状态 next -> cur
    jalr = next_jalr;
    halted = next_halted;
    mispredicted = next_mispredicted;
    redirect_pc = next_redirect_pc;
    pc = next_pc;
}

void CPU::print_stats() const {
    std::cerr << "cycles: " << cycle_cnt << '\n';
    std::cerr << "branch predictions: " << predict_correct << " / " << predict_total;
    if (predict_total > 0) {
        double acc = static_cast<double>(predict_correct) / static_cast<double>(predict_total) * 100.0;
        std::cerr << " (" << acc << "%)";
    }
    std::cerr << '\n';
}

void CPU::run() {
    load_program();
    while (true) {
        ++cycle_cnt;
        // 各 stage 顺序可任意交换
        if (check_terminated()) {
            break;
        }
        store_resolve();
        commit();
        cdb_listen();
        execute();
        memory_access();
        issue();
        update_all();
    }
    //print_stats();
}
