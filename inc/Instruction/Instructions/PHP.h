#pragma once 

#include "Instruction.h"

class PHP : public Instruction
{
    
public:
    PHP(std::shared_ptr<ICPU> icpu,  
        uint8_t cycles = 0);
    ~PHP();


    void run() override;
private:
};

