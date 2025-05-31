#include "Memory.h"
#include <random>
#include <fstream>
#include <iostream>
#include <iomanip>

Memory::Memory(){}
Memory::~Memory(){}

void Memory::initialize()
{
    for(uint32_t i = 0; i < MAX_MEMORY; ++i)
    {
        mData[i] = 0;
    }
}

void Memory::randomize()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, UINT8_MAX);

    for(int i = 0; i < MAX_MEMORY; ++i)
    {
        mData[i] = dis(gen);
    }
}

uint8_t Memory::readByte(uint32_t address) const
{
    address = wrapAddress(address);
    return mData[address];
}

uint16_t Memory::readWord(uint32_t address) const
{
    uint16_t tmp;
    uint8_t low = readByte(address);
    uint8_t high = readByte(address + 1);

    tmp = high;
    tmp <<= 8;
    tmp |= low;

    return tmp;
}

void Memory::writeByte(uint32_t address, uint8_t byte)
{
    address = wrapAddress(address);
    mData[address] = byte;
}

void Memory::writeWord(uint32_t address, uint16_t word)
{
    uint16_t lowByteAddr = wrapAddress(address);
    uint16_t highByteAddr = wrapAddress(address + 1);

    writeByte(lowByteAddr, static_cast<uint8_t>(word & 0xFF)); // Low byte
    writeByte(highByteAddr, static_cast<uint8_t>((word >> 8) & 0xFF)); // High byte
}

void Memory::dumpMemory(uint32_t startAddress, uint32_t endAddress) const
{
    startAddress = wrapAddress(startAddress);
    endAddress = wrapAddress(endAddress);

    const int BYTES_PER_ROW = 16;
    const int NUM_ROWS = (endAddress - startAddress + BYTES_PER_ROW) / BYTES_PER_ROW;
    
    // Print header row with column numbers
    std::cout << "       ";
    for (int i = 0; i < BYTES_PER_ROW; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << i << " ";
    }
    std::cout << "  |  ASCII" << std::endl;
    
    // Print divider
    std::cout << "-------";
    for (int i = 0; i < BYTES_PER_ROW; i++) {
        std::cout << "---";
    }
    std::cout << "-----------" << std::endl;
    
    // Print memory content row by row
    for (int row = 0; row < NUM_ROWS; row++) {
        uint32_t rowAddr = startAddress + (row * BYTES_PER_ROW);
        
        // Print address at start of row
        std::cout << std::hex << std::setw(4) << std::setfill('0') << rowAddr << " | ";
        
        // Print hex values
        std::string asciiRow;
        for (int i = 0; i < BYTES_PER_ROW; i++) {
            uint32_t addr = rowAddr + i;
            addr = wrapAddress(addr);
            if (addr <= endAddress) {
                uint8_t value = mData[addr];
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value) << " ";
                
                // Collect ASCII representation
                if (value >= 32 && value <= 126) { // Printable ASCII
                    asciiRow += static_cast<char>(value);
                } else {
                    asciiRow += '.'; // Non-printable
                }
            } else {
                std::cout << "   ";
                asciiRow += ' ';
            }
        }
        
        // Print ASCII representation
        std::cout << " | " << asciiRow << std::endl;
        
        // Stop if we've reached the end address
        if (rowAddr + BYTES_PER_ROW > endAddress)
            break;
    }
    
    std::cout << std::dec; // Reset to decimal output
}

bool Memory::loadBinary(const std::string &path, uint16_t startAddress)
{
    std::ifstream binaryFile(path, std::ios::in | std::ios::binary);
    if(!binaryFile.is_open())
        return false;

    startAddress = wrapAddress(startAddress); // Ensure start address is valid
    
    char buffer;
    uint16_t currentAddr = startAddress;
    while(binaryFile.read(&buffer, sizeof(char)))
    {
        mData[currentAddr] = static_cast<uint8_t>(buffer);
        currentAddr = wrapAddress(currentAddr + 1); // Wrap around if needed
        
        // Optional: detect if we've wrapped back to the start address
        if(currentAddr == startAddress) {
            break; // We've filled the entire memory
        }
    }

    return true;
}

uint16_t Memory::wrapAddress(uint32_t address) const
{
    return address % MAX_MEMORY;
}
