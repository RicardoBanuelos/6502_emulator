#pragma once
#include<stdint.h>


enum Flag {
    C = 1 << 0,
    Z = 1 << 1,
    I = 1 << 2,
    D = 1 << 3,
    B = 1 << 4,
    V = 1 << 6,
    N = 1 << 7,
};

union StatusRegister {
    uint8_t byte;
    uint8_t C : 1;
    uint8_t Z : 1;
    uint8_t I : 1;
    uint8_t D : 1;
    uint8_t B : 1;
    uint8_t X : 1;
    uint8_t V : 1;
    uint8_t N : 1;

    void setFlag(Flag flag, bool value){
        if(value)
        {
            byte |= flag;
            return;
        }
        
        byte &= ~flag;
    }
    
};

struct Registers {
    uint16_t PC;
    uint8_t SP;

    uint8_t A;
    uint8_t X;
    uint8_t Y;

    StatusRegister status;
};