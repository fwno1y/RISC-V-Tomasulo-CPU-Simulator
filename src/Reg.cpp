#include "../include/Reg.h"

void Register::update() {
    cur = next;
    cur.val[0] = 0;
    cur.rob[0] = -1;
    next.val[0] = 0;
    next.rob[0] = -1;
}

void Register::clear() {
    for (int i = 0; i < 32; ++i) {
        cur.rob[i] = -1;
        next.rob[i] = -1;
    }
}

