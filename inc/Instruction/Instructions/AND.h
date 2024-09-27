#include "../Instruction.h"

class AND : public Instruction {
public:
    AND(std::shared_ptr<ICPU> icpu, 
        AddressingMode addressingMode,
        uint8_t cycles = 0);

    ~AND();

    void run() override;
};