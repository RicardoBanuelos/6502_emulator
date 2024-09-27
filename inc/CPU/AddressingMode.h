#pragma once

enum AddressingMode
{
    Implied,
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
    IndirectY
};