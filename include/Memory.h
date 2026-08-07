#ifndef RISC_V_CPU_MEMORY_H
#define RISC_V_CPU_MEMORY_H
#include <cstdint>
class Memory {
    uint8_t mem[0x200000]{};
    struct Request {
        bool active = false;
        bool is_write = false;  //读写标志
        uint32_t addr = 0;  //地址
        uint32_t data = 0;  //数据
        int size = 0;  //读写字节宽度
        int rob = -1;  //对应rob编号
        int cycles_left = 0;  //事务剩余周期
    };
    Request cur;
    Request next;

public:
    Memory();
    void update();
    void clear();

    //无延迟从内存直接取指
    uint32_t read_instruction(uint32_t pc) const;
    void load_byte(uint32_t addr, uint8_t byte);
    //向内存发起延迟读
    bool issue_read(uint32_t addr, int size, int rob, int late = 3);
    //向内存发起延迟写
    bool issue_write(uint32_t addr, uint32_t data, int size, int late = 3);
    //检测读完成，读取计时完成读出字节
    bool check_read_done(int& rob, uint32_t& data) const;
    void finish_read();
    //检测写完成，真正对mem修改
    bool check_write_done();
    bool is_busy() const;
};


#endif //RISC_V_CPU_MEMORY_H