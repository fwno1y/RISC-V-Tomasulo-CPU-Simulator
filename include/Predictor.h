#ifndef RISC_V_CPU_PREDICTOR_H
#define RISC_V_CPU_PREDICTOR_H

#include <cstdint>

class Predictor {
public:
    virtual  ~Predictor() = default;
    virtual bool predict(uint32_t pc) const = 0;
    virtual void update(uint32_t pc, bool jump) = 0;
};

class StaticPredictor : Predictor {
public:
    StaticPredictor();
    bool predict(uint32_t pc) const override;
    void update(uint32_t pc, bool jump) override;
};

class BimodelPredictor : Predictor {
public:
    BimodelPredictor();
    bool predict(uint32_t pc) const override;
    void update(uint32_t pc, bool jump) override;
};

class GlobalPredictor : Predictor {
public:
    int log[1024] = {};
    uint32_t history = 0;

    GlobalPredictor();
    bool predict(uint32_t pc) const override;
    void update(uint32_t pc, bool jump) override;
};
#endif //RISC_V_CPU_PREDICTOR_H