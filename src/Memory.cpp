#include "../include/Memory.h"

Memory::Memory() {
    for (int i = 0; i < 0x200000; ++i) {
        mem[i] = 0;
    }
}

void Memory::update() {
    cur = next;
    if (cur.active && cur.cycles_left > 0) {
        next.cycles_left = cur.cycles_left - 1;
    }
}

void Memory::clear() {
    cur = Request();
    next = Request();
}

uint32_t Memory::read_instruction(uint32_t pc) const {
    uint32_t inst = 0;
    inst |= mem[pc];
    inst |= static_cast<uint32_t> (mem[pc + 1]) << 8;
    inst |= static_cast<uint32_t> (mem[pc + 2]) << 16;
    inst |= static_cast<uint32_t> (mem[pc + 3]) << 24;
    return inst;
}

void Memory::load_byte(uint32_t addr, uint8_t byte) {
    if (addr < 0x200000) {
        mem[addr] = byte;
    }
}

bool Memory::issue_read(uint32_t addr, int size, int rob, int late) {
    if (cur.active || next.active) {
        return false;
    }
    next.active = true;
    next.is_write = false;
    next.addr = addr;
    next.size = size;
    next.rob = rob;
    next.cycles_left = late;
    return true;
}

bool Memory::issue_write(uint32_t addr, uint32_t data, int size, int late) {
    if (cur.active || next.active) {
        return false;
    }
    next.active = true;
    next.is_write = true;
    next.addr = addr;
    next.size = size;
    next.data = data;
    next.cycles_left = late;
    return true;
}

bool Memory::check_read_done(int &out_rob, uint32_t &out_data) const {
    if (!cur.active || cur.is_write || cur.cycles_left > 0) {
        return false;
    }
    out_rob = cur.rob;
    out_data = 0;
    for (int i = 0; i < cur.size; ++i) {
        out_data |= static_cast<uint32_t> (mem[cur.addr + i]) << (i * 8);
    }
    return true;
}

void Memory::finish_read() {
    next.active = false;
}

bool Memory::check_write_done() {
    if (!cur.active || !cur.is_write || cur.cycles_left > 0) {
        return false;
    }
    for (int i = 0; i < cur.size; ++i) {
        mem[cur.addr + i] = (cur.data >> (i * 8)) & 0xff;
    }
    next.active = false;
    return true;
}


bool Memory::is_busy() const {
    return cur.active;
}










