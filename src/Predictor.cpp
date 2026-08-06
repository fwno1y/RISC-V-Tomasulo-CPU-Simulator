#include "../include/Predictor.h"

StaticPredictor::StaticPredictor() = default;

bool StaticPredictor::predict(uint32_t pc) const {
    return false;
}

void StaticPredictor::update(uint32_t pc, bool jump) {}

BimodelPredictor::BimodelPredictor() = default;

bool BimodelPredictor::predict(uint32_t pc) const {
    int idx = (pc >> 2) & 0x3ff;
    return bht[idx] >= 2;
}

void BimodelPredictor::update(uint32_t pc, bool jump) {
    int idx = (pc >> 2) & 0x3ff;
    if (jump) {
        if (next_bht[idx] < 3) {
            next_bht[idx]++;
        }
    }
    else {
        if (next_bht[idx] > 0) {
            next_bht[idx]--;
        }
    }
}

void BimodelPredictor::advance() {
    for (int i = 0; i < 1024; ++i) {
        bht[i] = next_bht[i];
    }
}
