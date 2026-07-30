#ifndef RISC_V_CPU_CDB_H
#define RISC_V_CPU_CDB_H
#include <cstdint>

struct CDBEntry {
    bool active = false;  //当前CDB广播是否有效
    int rob = -1;         //CDB广播数据生产者ROB编号
    uint32_t value = 0;   //CDB广播数据
    uint32_t target_pc = 0;     //实际跳转地址
};

class CDB {
    CDBEntry entries;
    CDBEntry next_entries;

    CDB() = default;
    void update();
    void clear();
};
#endif //RISC_V_CPU_CDB_H