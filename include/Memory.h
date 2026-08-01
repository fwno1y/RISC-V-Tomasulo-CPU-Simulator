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

public:
    Memory();
    void update();
    void clear();

    //取指
    uint32_t read_instruction(uint32_t pc) const;
    void load_byte(uint32_t addr, uint8_t byte);
    void issue_read(uint32_t addr, int size, int rob, int late = 3);
    void issue_write(uint32_t addr, uint32_t data, int size, int late = 3);
    bool check_read_done(int& rob, uint32_t& data);
    bool check_write_done();
    bool is_busy() const;
};


#endif //RISC_V_CPU_MEMORY_H