#include <iostream>
#include <sstream>
#include "../include/ALU.h"
#include "../include/CDB.h"
#include "../include/Instruction.h"
#include "../include/LSQ.h"
#include "../include/Memory.h"
#include "../include/Predictor.h"
#include "../include/Reg.h"
#include "../include/ROB.h"
#include "../include/RS.h"

int main() {
    Memory mem;
    Register reg;
    ROB rob;
    RS rs;
    LSQ lsq;
    CDB cdb;
    BimodelPredictor predictor;
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
    uint32_t pc = 0;
    uint32_t next_pc = 0;
    int cycle_cnt = 0;
    bool jalr = false;
    bool halted = false;
    while (true) {
        cycle_cnt++;
        if (halted && rob.is_empty() && !mem.is_busy()) {
            uint32_t res = reg.read_value(10) & 0xff;
            std::cout << std::dec << res << std::endl;
            break;
        }
        bool mispredicted = false;
        for (int i = 0; i < 16; ++i) {
            const LSQEntry& entry = lsq.get_entry(i);
            if (entry.busy && entry.is_store) {
                if (entry.ready && entry.qk == -1) {
                    rob.next_entries[entry.rob].ready = true;
                }
            }
        }
        if (!rob.is_empty()) {
            const ROBEntry& head = rob.get_head();
            if (head.ready) {
                const Instruction& inst = head.instruction;
                if (inst.rd != 0) {
                    reg.set_value(inst.rd, head.value);
                    if (reg.read_rob(inst.rd) == rob.head) {
                        reg.set_rob(inst.rd, -1);
                    }
                }
                bool is_branch = (inst.type == InstructionType::BEQ || inst.type == InstructionType::BGE || inst.type == InstructionType::BGEU || inst.type == InstructionType::BLT || inst.type == InstructionType::BLTU || inst.type == InstructionType::BNE);
                if (is_branch) {
                    bool actual_taken = (head.target_pc != inst.pc + 4);
                    bool predict_taken = head.jump;
                    predictor.update(inst.pc, actual_taken);
                    //预测失败
                    if (actual_taken != predict_taken) {
                        uint32_t redirect_pc = head.target_pc;
                        rob.clear();
                        rs.clear();
                        lsq.clear();
                        cdb.clear();
                        next_pc = redirect_pc;
                        jalr = false;
                        reg.clear_rob();
                        mispredicted = true;
                    }
                }
                else if (inst.type == InstructionType::JALR) {
                    next_pc = head.target_pc;
                    jalr = false;
                }
                else if (inst.type == InstructionType::HALT) {
                    uint32_t res = reg.read_value(10) & 0xff;
                    std::cout << std::dec << res << std::endl;
                    break;
                }
                if (!mispredicted) {
                    bool is_store = (inst.type == InstructionType::SB || inst.type == InstructionType::SH || inst.type == InstructionType::SW);
                    if (is_store) {
                        uint32_t st_addr, st_data;
                        int st_size;
                        if (lsq.find_store(rob.head, st_addr, st_data, st_size)) {
                            if (mem.issue_write(st_addr, st_data, st_size)) {
                                lsq.pop_head();
                                rob.commit_head();
                            }
                        }
                    }
                    if (!is_store) {
                        rob.commit_head();
                    }
                }
            }
        }

        if (cdb.is_active()) {
            int rob_id = cdb.get_rob();
            uint32_t val = cdb.get_value();
            uint32_t target = cdb.get_target_pc();
            rob.listen_cdb(rob_id, val, target);
            rs.listen_cdb(rob_id, val);
            lsq.listen_cdb(rob_id, val);
        }

        int ready_rs_idx = rs.find_ready();
        if (ready_rs_idx != -1 && cdb.is_next_free()) {
            const RSEntry& entry = rs.get_entry(ready_rs_idx);
            uint32_t res = ALU::execute(entry.op, entry.vj, entry.vk, entry.pc, entry.imm);
            uint32_t target_pc = entry.pc + 4;

            bool is_b = (entry.op == InstructionType::BEQ || entry.op == InstructionType::BGE || entry.op == InstructionType::BGEU || entry.op == InstructionType::BLT || entry.op == InstructionType::BLTU || entry.op == InstructionType::BNE);
            if (is_b) {
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
            cdb.broadcast(entry.dest, res, target_pc);
            rs.release(ready_rs_idx);
        }

        int mem_rob = -1;
        uint32_t mem_val = 0;
        if (cdb.is_next_free() && mem.check_read_done(mem_rob, mem_val)) {
            cdb.broadcast(mem_rob, mem_val, 0);
            mem.finish_read();
        }
        mem.check_write_done();

        if (!mem.is_busy()) {
            int load_rob = -1;
            int ready_load_idx = lsq.find_load(load_rob);
            if (ready_load_idx != -1) {
                const LSQEntry& entry = lsq.get_entry(ready_load_idx);
                int size = 4;
                if (entry.op == InstructionType::LB || entry.op == InstructionType::LBU) {
                    size = 1;
                }
                else if (entry.op == InstructionType::LH || entry.op == InstructionType::LHU) {
                    size = 2;
                }
                if (mem.issue_read(entry.addr, size, load_rob)) {
                    lsq.pop_entry(ready_load_idx);
                }
            }
        }

        if (!rob.is_full() && !rs.is_full() && !lsq.is_full() && !jalr && !halted && !mispredicted) {
            uint32_t raw_inst = mem.read_instruction(pc);
            Instruction inst = decode(pc, raw_inst);
            if (inst.type == InstructionType::HALT) {
                halted = true;
            }
            else {
                uint32_t vj = 0, vk = 0;
                int qj = -1, qk = -1;
                if (inst.rs1 != 0) {
                    int r_dep = reg.read_rob(inst.rs1);
                    if (r_dep == -1) {
                        vj = reg.read_value(inst.rs1);
                    }
                    else if (rob.entries[r_dep].ready) {
                        vj = rob.entries[r_dep].value;
                    }
                    else if (cdb.is_active() && cdb.get_rob() == r_dep) {
                        vj = cdb.get_value();
                    }
                    else {
                        qj = r_dep;
                    }
                }
                if (inst.rs2 != 0) {
                    int r_dep = reg.read_rob(inst.rs2);
                    if (r_dep == -1) {
                        vk = reg.read_value(inst.rs2);
                    }
                    else if (rob.entries[r_dep].ready) {
                        vk = rob.entries[r_dep].value;
                    }
                    else if (cdb.is_active() && cdb.get_rob() == r_dep) {
                        vk = cdb.get_value();
                    }
                    else {
                        qk = r_dep;
                    }
                }
                int allocate_rob = rob.push(inst, inst.rd);
                if (inst.rd != 0) {
                    reg.set_rob(inst.rd, allocate_rob);
                }
                bool is_mem = (inst.type == InstructionType::LB || inst.type == InstructionType::LBU || inst.type == InstructionType::LH || inst.type == InstructionType::LHU || inst.type == InstructionType::LW || inst.type == InstructionType::SB || inst.type == InstructionType::SH || inst.type == InstructionType::SW);
                if (is_mem) {
                    bool is_st = (inst.type == InstructionType::SB || inst.type == InstructionType::SH || inst.type == InstructionType::SW);
                    lsq.push(inst.type, is_st, vj, vk, qj, qk, inst.imm, allocate_rob);
                }
                else {
                    int rs_idx = rs.allocate();
                    if (inst.type == InstructionType::ADDI || inst.type == InstructionType::ANDI ||inst.type == InstructionType::ORI || inst.type == InstructionType::XORI || inst.type == InstructionType::SLLI || inst.type == InstructionType::SRLI ||inst.type == InstructionType::SRAI || inst.type == InstructionType::SLTI ||inst.type == InstructionType::SLTIU) {
                        vk = inst.imm;
                        qk = -1;
                    }
                    rs.issue(rs_idx, inst.type, vj, vk, qj, qk, allocate_rob, pc, inst.imm);
                }

                bool is_branch = (inst.type == InstructionType::BEQ || inst.type == InstructionType::BNE ||inst.type == InstructionType::BLT || inst.type == InstructionType::BLTU ||inst.type == InstructionType::BGE || inst.type == InstructionType::BGEU);
                if (is_branch) {
                    bool taken = predictor.predict(pc);
                    next_pc = taken ? (pc + inst.imm) : (pc + 4);
                    rob.next_entries[allocate_rob].jump = taken;
                }
                else if (inst.type == InstructionType::JAL) {
                    next_pc = pc + inst.imm;
                }
                else if (inst.type == InstructionType::JALR) {
                    jalr = true;
                }
                else {
                    next_pc = pc + 4;
                }
            }
        }

        mem.update();
        reg.update();
        rob.update();
        rs.update();
        lsq.update();
        cdb.update();

        pc = next_pc;
    }
    return 0;
}