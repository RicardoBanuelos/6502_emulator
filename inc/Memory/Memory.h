#include<stdint.h>

class Memory {
public:
    Memory();
    ~Memory();

    void initialize();
    uint8_t readByte(uint32_t address);
    uint16_t readWord(uint32_t address);
    void writeByte(uint32_t address);
    void writeWord(uint32_t address);
private:
    bool validateAddress(uint32_t address);

    static const uint32_t MAX_MEMORY = 1024 * 64;
    uint8_t data[MAX_MEMORY];
};