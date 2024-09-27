#pragma once
#include <memory>
#include "CPU/ICPU.h"


class AddressingMode
{
    
public:
    enum Mode {
        Implied,
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Relative,
        Absolute,
        AbsoluteOffsetX,
        AbsoluteYOffset,
        Indirect,
        IndirectX,
        IndiectY
    };
    AddressingMode(std::shared_ptr<ICPU> icpu);
    ~AddressingMode();

    virtual uint8_t Implied();
    virtual uint8_t Immediate();
    virtual uint8_t ZeroPage();

private:
    std::shared_ptr<ICPU> mIcpu;
};

/*


uint8_t Instruction::Implied()
{

    return 0;
}
uint8_t Instruction::Immediate()
{
    return mIcpu->fetchByte();
}

uint8_t Instruction::ZeroPage()
{
    uint8_t address = mIcpu->fetchByte() & 0x00FF;
    return address;
}

uint16_t Instruction::absolute()
{
    return mIcpu->fetchWord();
}


uint16_t Instruction::Indirect()
{
    return mIcpu->fetchWord();
}
uint8_t Instruction::PreIndexed()
{

}
 */

