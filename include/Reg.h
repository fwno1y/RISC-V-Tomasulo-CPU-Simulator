#ifndef RISC_V_CPU_REG_H
#define RISC_V_CPU_REG_H
#include <cstdint>

struct RegState {
    uint32_t val[32] = {0};
    int rob[32]{};

    RegState() {
        for (int i = 0; i < 32; ++i) {
            rob[i] = -1;
        }
    }
};

class Register {
    RegState cur;
    RegState next;

public:
    void update();
    void clear();
    void clear_rob();
    //读寄存器
    uint32_t read_value(int reg_id) const;
    int read_rob(uint32_t reg_id) const;
    //写寄存器
    void set_value(uint32_t reg_id, uint32_t value);
    void set_rob(uint32_t reg_id, int rob_id);

};

#endif //RISC_V_CPU_REG_H