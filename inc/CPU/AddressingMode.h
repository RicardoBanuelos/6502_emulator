#pragma once

enum AddressingMode : int
{
    Implied = 0,
    Accumulator,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteOffsetX,
    AbsoluteOffsetY,
    Indirect,
    IndirectX,
    IndirectY,
    NA
};