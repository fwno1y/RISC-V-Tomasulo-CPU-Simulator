#ifndef RISC_V_CPU_CDB_H
#define RISC_V_CPU_CDB_H
#include <cstdint>

struct CDBEntry {
    bool active = false;  //当前CDB广播是否有效
    int rob = -1;         //CDB广播数据生产者ROB编号
    uint32_t value = 0;   //CDB广播数据
    uint32_t target_pc = 0;     //实际跳转地址（分支跳转）
};

class CDB {
    CDBEntry entries;
    CDBEntry next_entries;

public:
    CDB() = default;
    void update();
    void clear();
    void broadcast(int rob_id, uint32_t value, uint32_t target_pc); //写入next_entries
    bool is_active() const;
    int get_rob() const;
    uint32_t get_value() const;
    uint32_t get_target_pc() const;
    bool is_next_free() const;
};
#endif //RISC_V_CPU_CDB_H