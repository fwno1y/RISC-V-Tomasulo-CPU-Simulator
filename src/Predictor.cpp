#include "../include/Predictor.h"

StaticPredictor::StaticPredictor() = default;

bool StaticPredictor::predict(uint32_t pc) const {
    return false;
}

void StaticPredictor::update(uint32_t pc, bool jump) {}

BimodelPredictor::BimodelPredictor() = default;

bool BimodelPredictor::predict(uint32_t pc) const {
    int idx = (pc >> 2) & 0xfff;
    return bht[idx] >= 2;
}

void BimodelPredictor::update(uint32_t pc, bool jump) {
    int idx = (pc >> 2) & 0xfff;
    if (jump) {
        if (bht[idx] < 3) {
            bht[idx]++;
        }
        else {
            if (bht[idx] > 0) {
                bht[idx]--;
            }
        }
    }
}





