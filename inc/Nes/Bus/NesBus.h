#pragma once 

#include "Bus.h"



class IPPU;

class NesBus : public Bus
{
    
public:
    NesBus();
    ~NesBus();


    virtual void connectPPU(std::shared_ptr<IPPU> ppu) = 0;
    virtual uint8_t readPPURegister(uint16_t address) = 0;
    virtual void writePPURegister(uint16_t address, uint8_t data) = 0;
private:

};
