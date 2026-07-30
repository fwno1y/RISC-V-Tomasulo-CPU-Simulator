#ifndef RISC_V_CPU_PREDICTOR_H
#define RISC_V_CPU_PREDICTOR_H

#include <cstdint>
#include <vector>
class Predictor {
    std::vector<uint8_t> bht;
public:
    Predictor() : bht(4096, 1) {}
    bool predict(uint32_t pc) const;
    void update(uint32_t pc, bool jump);
};

#endif //RISC_V_CPU_PREDICTOR_H