#ifndef RISC_V_CPU_MEMORY_H
#define RISC_V_CPU_MEMORY_H
#include <cstdint>
class Memory {
    uint8_t mem[0x200000]{};
    struct Request {
        bool active = false;
        bool is_write = false;
        uint32_t addr = 0;
        uint32_t data = 0;
        int size = 0;
        int rob = -1;
        int cycles_left = 0;
    };
    Request cur;
    Request next;

    Memory();
    void update();
    void clear();

    uint32_t read_instruction(uint32_t pc) const;
    void write(uint32_t addr, uint32_t val);
    bool is_busy() const;
    void issue_read(uint32_t addr, int size, int rob, int late = 3);
    void issue_write(uint32_t addr, uint32_t data, int size, int late = 3);
    bool check_read_done(int& out_rob, uint32_t& out_data);
    bool check_write_done();
};


#endif //RISC_V_CPU_MEMORY_H