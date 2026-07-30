#ifndef RISC_V_CPU_CDB_H
#define RISC_V_CPU_CDB_H
#include <cstdint>

struct CDBEntry {
    bool active = false;
    int rob = -1;
    uint32_t value = 0;
    uint32_t target_pc = 0;
};

class CDB {
    CDBEntry entries;
    CDBEntry next_entries;

    void update();
};
#endif //RISC_V_CPU_CDB_H