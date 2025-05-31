#pragma once
#include <stdint.h>

struct AddressingData
{
    uint16_t address;
    uint16_t data;
    uint8_t extraCycles;


    AddressingData(uint16_t address, uint16_t data, uint8_t extraCycles = 0) 
        :   address(address),
            data(data),
            extraCycles(0)
    {

    }
};