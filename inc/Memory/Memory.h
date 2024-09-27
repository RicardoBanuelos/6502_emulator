#include<stdint.h>

class Memory {
public:
    Memory();
    ~Memory();

    void initialize();
    uint8_t readByte(uint32_t address) const;
    uint16_t readWord(uint32_t address) const;
    void writeByte(uint32_t address, uint8_t byte);
    void writeWord(uint32_t address, uint16_t word);
private:
    bool validateAddress(uint32_t address) const;

    static const uint32_t MAX_MEMORY = 1024 * 64;
    uint8_t mData[MAX_MEMORY];
};