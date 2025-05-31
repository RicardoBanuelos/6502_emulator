#pragma once
#include <stdint.h>

struct AddressingData
{
    uint16_t address;
    uint16_t data;


    AddressingData(uint16_t address, uint16_t data) 
        :   address(address),
            data(data)
    {

    }
};