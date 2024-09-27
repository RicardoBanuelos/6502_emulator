#pragma once
#include<stdint.h>

union StatusRegister {
    uint8_t byte;
    uint8_t C : 1;
    uint8_t Z : 1;
    uint8_t I : 1;
    uint8_t D : 1;
    uint8_t B : 1;
    uint8_t V : 1;
    uint8_t N : 1;
};

struct Registers{
    uint16_t PC;
    uint8_t SP;

    uint8_t A;
    uint8_t X;
    uint8_t Y;

    StatusRegister status;
};