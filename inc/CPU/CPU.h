#include "Registers.h"

class CPU 
{
public:
    CPU();
    ~CPU();

    void reset();

private:
    Registers mRegisters;
    uint8_t fetchByte();
    uint16_t fetchWord();
    void execute();
};