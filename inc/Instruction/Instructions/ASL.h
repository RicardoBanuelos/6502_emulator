#include "../Instruction.h"

class ASL : public Instruction 
{
public:
    ASL(std::shared_ptr<ICPU> icpu, 
        std::function<uint16_t()> addressingFunction, 
        uint8_t cycles = 0);

    ~ASL();

    void run() override;
};