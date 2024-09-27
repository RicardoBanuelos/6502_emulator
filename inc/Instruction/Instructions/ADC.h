#include "../Instruction.h"

class ADC : public Instruction 
{
public:
    ADC(std::shared_ptr<ICPU> icpu, 
        std::function<uint16_t()> addressingFunction, 
        uint8_t cycles = 0);

    ~ADC();

    void run() override;
protected:
    void updateStatus() override;
};