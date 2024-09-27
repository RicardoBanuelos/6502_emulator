#include "Instruction/Instruction.h"

class PHA : public Instruction
{
    
public:
    PHA(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode, 
        uint8_t cycles = 0);
    ~PHA();

    void run() override;
private:
};