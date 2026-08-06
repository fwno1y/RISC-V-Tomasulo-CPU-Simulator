#ifndef RISC_V_CPU_PREDICTOR_H
#define RISC_V_CPU_PREDICTOR_H

#include <cstdint>

class Predictor {
public:
    virtual ~Predictor() = default;
    virtual bool predict(uint32_t pc) const = 0;   // 读 cur
    virtual void update(uint32_t pc, bool jump) = 0;  // 写 next
    virtual void advance() {}                        // next -> cur
};

class StaticPredictor : public Predictor {
public:
    StaticPredictor();
    bool predict(uint32_t pc) const override;
    void update(uint32_t pc, bool jump) override;
};

class BimodelPredictor : public Predictor {
public:
    int bht[1024] = {};          // cur
    int next_bht[1024] = {};     // next

    BimodelPredictor();
    bool predict(uint32_t pc) const override;       // 读 bht (cur)
    void update(uint32_t pc, bool jump) override;   // 写 next_bht
    void advance() override;                        // next_bht -> bht
};

class GlobalPredictor : public Predictor {
public:
    int log[1024] = {};
    uint32_t history = 0;

    GlobalPredictor();
    bool predict(uint32_t pc) const override;
    void update(uint32_t pc, bool jump) override;
};
#endif //RISC_V_CPU_PREDICTOR_H
