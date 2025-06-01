#pragma once

#include <stdint.h>
#include <string>
#include <vector>

class Memory
{
public:
    Memory(const uint32_t maxMemory = 64 * 1024);
    ~Memory();

    void initialize();
    void randomize();
    uint8_t readByte(uint32_t address) const;
    uint16_t readWord(uint32_t address) const;
    void writeByte(uint32_t address, uint8_t byte);
    void writeWord(uint32_t address, uint16_t word);
    void dumpMemory(uint32_t startAddress, uint32_t endAddress) const;
    bool loadBinary(const std::string &path, uint16_t startAddress = 0);
    uint32_t getMaxMemory() const;
private:
    uint16_t wrapAddress(uint32_t address) const;
    std::vector<uint8_t> mData;
};