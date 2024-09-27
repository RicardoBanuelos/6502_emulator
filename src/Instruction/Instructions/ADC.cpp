#include "Instruction/Instructions/ADC.h"
#include "ADC.h"

ADC::ADC(std::shared_ptr<ICPU> icpu, std::function<uint16_t()> addressingFunction, uint8_t cycles)
    :   Instruction("ADC", icpu, addressingFunction, cycles)
{
}

ADC::~ADC()
{
}

void ADC::run()
{
}

void ADC::updateStatus()
{
}
