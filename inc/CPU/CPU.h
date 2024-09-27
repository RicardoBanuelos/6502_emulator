#include "Registers.h"
#include "CPU/ICPU.h"

class CPU : public ICPU
{
public:
    CPU();
    ~CPU();

    void reset();

private:
    Registers mRegisters;
    uint8_t fetchByte();
    uint16_t fetchWord();
    void writeByte(uint8_t data);
    void writeWord(uint16_t data);
    void execute();
};