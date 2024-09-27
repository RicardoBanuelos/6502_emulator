#include "../Instruction.h"

class ASL : public Instruction 
{
public:
    ASL(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~ASL();

    void run() override;
};