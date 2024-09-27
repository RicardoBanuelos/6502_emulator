#include "../Instruction.h"

class AND : public Instruction {
public:
    AND(std::shared_ptr<ICPU> icpu, 
        std::function<uint16_t()> addressingFunction, 
        uint8_t cycles = 0);

    ~AND();

    void run() override;
};