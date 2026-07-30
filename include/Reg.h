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
};

#endif //RISC_V_CPU_REG_H