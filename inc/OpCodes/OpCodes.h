#pragma once

// OC_OPCODE_ADDRESSING
enum OpCode
{
    // ADC
    OC_ADC_IMMEDIATE = 0x69,
    OC_ADC_ZERO_PAGE = 0x65,
    OC_ADC_ZERO_PAGE_X = 0x75,
    OC_ADC_ABSOLUTE = 0x6D,
    OC_ADC_ABSOLUTE_X = 0x7D,
    OC_ADC_ABSOLUTE_Y = 0x79,
    OC_ADC_INDIRECT_X = 0x61,
    OC_ADC_INDIRECT_Y = 0x71,
    // AND - Logical AND
    OC_AND_IMMEDIATE = 0x29,
    OC_AND_ZERO_PAGE = 0x25,
    OC_AND_ZERO_PAGE_X = 0x35,
    OC_AND_ABSOLUTE = 0x2D,
    OC_AND_ABSOLUTE_X = 0x3D,
    OC_AND_ABSOLUTE_Y = 0x39,
    OC_AND_INDIRECT_X = 0x21,
    OC_AND_INDIRECT_Y = 0x31,
    // ASL - Arithmetic Shift Left
    OC_ASL_ACCUMULATOR = 0x0A,
    OC_ASL_ZERO_PAGE = 0x06,
    OC_ASL_ZERO_PAGE_X = 0x16,
    OC_ASL_ABSOLUTE = 0x0E,
    OC_ASL_ABSOLUTE_X = 0x1E,
    // BCC - Branch if Carry Clear
    OC_BCC_RELATIVE = 0x90,
    // BCS - Branch if Carry Set
    OC_BCS_RELATIVE = 0xB0,
    // BEQ - Branch if Equal
    OC_BEQ_RELATIVE = 0xF0,
    // BIT - Bit Test
    OC_BIT_ZERO_PAGE = 0x24,
    OC_BIT_ABSOLUTE = 0x2C,
    // BMI - Branch if Minus
    OC_BMI_RELATIVE = 0x30,
    // BNE - Branch if Not Equal
    OC_BNE_RELATIVE = 0xD0,
    // BPL - Branch if Positive
    OC_BPL_RELATIVE = 0x10,
    // BRK - Force Interrupt
    OC_BRK_RELATIVE = 0x00,
    // BVC - Branch if Overflow Clear
    OC_BVC_RELATIVE = 0x50,
    // BVS - Branch if Overflow Set
    OC_BVS_RELATIVE = 0x70,
    // CLC - Clear Carry Flag
    OC_CLC_IMPLIED = 0x18,
    // CLD - Clear Decimal Mode
    OC_CLD_IMPLIED = 0xD8,
    // CLI - Clear Interrupt Disable
    OC_CLI_IMPLIED = 0x58,
    // CLV - Clear Overflow Flag
    OC_CLV_IMPLIED = 0xB8,
    // CMP - Compare
    OC_CMP_IMMEDIATE = 0xC9,
    OC_CMP_ZERO_PAGE = 0xC5,
    OC_CMP_ZERO_PAGE_X = 0xD5,
    OC_CMP_ABSOLUTE = 0xCD,
    OC_CMP_ABSOLUTE_X = 0xDD,
    OC_CMP_ABSOLUTE_Y = 0xD9,
    OC_CMP_INDIRECT_X = 0xC1,
    OC_CMP_INDIRECT_Y = 0xD1,
    // CMC - Compare X Register
    OC_CMC_IMMEDIATE = 0xE0,
    OC_CMC_ZERO_PAGE = 0xE4,
    OC_CMC_ABSOLUTE = 0xEC,
    // CPY - Compare Y Register
    OC_CPY_IMMEDIATE = 0xC0,
    OC_CPY_ZERO_PAGE = 0xC4,
    OC_CPY_ABSOLUTE = 0xCC,
    // DEC - Decrement Memory
    OC_DEC_ZERO_PAGE = 0xC6,
    OC_DEC_ZERO_PAGE_X = 0xD6,
    OC_DEC_ABSOLUTE = 0xCE,
    OC_DEC_ABSOLUTE_X = 0xDE,
    // DEX - Decrement X Register
    OC_DEX_IMPLIED = 0xCA,
    // DEY - Decrement Y Register
    OC_DEY_IMPLIED = 0x88,
    // EOR - Exclusive OR
    OC_EOR_IMMEDIATE = 0x49,
    OC_EOR_ZERO_PAGE = 0x45,
    OC_EOR_ZERO_PAGE_X = 0x55,
    OC_EOR_ABSOLUTE = 0x4D,
    OC_EOR_ABSOLUTE_X = 0x5D,
    OC_EOR_ABSOLUTE_Y = 0x59,
    OC_EOR_INDIRECT_X = 0x41,
    OC_EOR_INDIRECT_Y = 0x51,
    // INC - Increment Memory
    OC_INC_ZERO_PAGE = 0xE6,
    OC_INC_ZERO_PAGE_X = 0xF6,
    OC_INC_ABSOLUTE = 0xEE,
    OC_INC_ABSOLUTE_X = 0xFE,
    // INX - Increment X Register
    OC_INX_IMPLIED = 0xE8,
    // INY - Increment Y Register
    OC_INY_IMPLIED = 0xC8,
    // JMP - Jump
    OC_JMP_ABSOLUTE = 0x4C,
    OC_JMP_INDIRECT = 0x6C,
    // JSR - Jump to Subroutine
    OC_JSR_ABSOLUTE = 0x20,
    // LDA - Load Accumulator
    OC_LDA_IMMEDIATE = 0xA9,
    OC_LDA_ZERO_PAGE = 0xA5,
    OC_LDA_ZERO_PAGE_X = 0xB5,
    OC_LDA_ABSOLUTE = 0xAD,
    OC_LDA_ABSOLUTE_X = 0xBD,
    OC_LDA_ABSOLUTE_Y = 0xB9,
    OC_LDA_INDIRECT_X = 0xA1,
    OC_LDA_INDIRECT_Y = 0xB1,
    // LDX - Load X Register
    OC_LDX_IMMEDIATE = 0xA2,
    OC_LDX_ZERO_PAGE = 0xA6,
    OC_LDX_ZERO_PAGE_Y = 0xB6,
    OC_LDX_ABSOLUTE = 0xAE,
    OC_LDX_ABSOLUTE_Y = 0xBE,
    // LDY - Load Y Register
    OC_LDY_IMMEDIATE = 0xA0,
    OC_LDY_ZERO_PAGE = 0xA4,
    OC_LDY_ZERO_PAGE_X = 0xB4,
    OC_LDY_ABSOLUTE = 0xAC,
    OC_LDY_ABSOLUTE_X = 0xBC,
    // LSR - Logical Shift Right
    OC_LSR_ACCUMULATOR = 0x4A,
    OC_LSR_ZERO_PAGE = 0x46,
    OC_LSR_ZERO_PAGE_X = 0x56,
    OC_LSR_ABSOLUTE = 0x4E,
    OC_LSR_ABSOLUTE_X = 0x5E,
    // NOP - No Operation
    OC_NOP_IMPLIED = 0xEA,
    // ORA - Logical Inclusive OR
    OC_ORA_IMMEDIATE = 0x09,
    OC_ORA_ZERO_PAGE = 0x05,
    OC_ORA_ZERO_PAGE_X = 0x15,
    OC_ORA_ABSOLUTE = 0x0D,
    OC_ORA_ABSOLUTE_X = 0x1D,
    OC_ORA_ABSOLUTE_Y = 0x19,
    OC_ORA_INDIRECT_X = 0x01,
    OC_ORA_INDIRECT_Y = 0x11,
    // PHA - Push Accumulator
    OC_PHA_IMPLIED = 0X48,
    // PHP - Push Processor Status
    OC_PHP_IMPLIED = 0X08,
    // PLA - Pull Accumulator
    OC_PLA_IMPLIED = 0X68,
    // PLP - Pull Processor Status
    OC_PLP_IMPLIED = 0x28,
    // ROL - Rotate Left
    OC_ROL_ACCUMULATOR = 0X2A,
    OC_ROL_ZERO_PAGE = 0X26,
    OC_ROL_ZERO_PAGE_X = 0X36,
    OC_ROL_ABSOLUTE = 0X2E,
    OC_ROL_ABSOLUTE_X = 0X3E,
    // ROR - Rotate Right
    OC_ROR_ACCUMULATOR = 0X6A,
    OC_ROR_ZERO_PAGE = 0X66,
    OC_ROR_ZERO_PAGE_X = 0X76,
    OC_ROR_ABSOLUTE = 0X6E,
    OC_ROR_ABSOLUTE_X = 0X7E,
    // RTI - Return from Interrupt
    OC_RTI_IMPLIED = 0X40,
    // RTS - Return from Subroutine
    OC_RTS_IMPLIED = 0X60,
    // SBC - Subtract with Carry
    OC_SBC_IMMEIATE = 0XE9,
    OC_SBC_ZERO_PAGE = 0XE5,
    OC_SBC_ZERO_PAGE_X = 0XF5,
    OC_SBC_ABSOLUTE = 0XED,
    OC_SBC_ABSOLUTE_X = 0XFD,
    OC_SBC_ABSOLUTE_Y = 0XF9,
    OC_SBC_INDIRECT_X = 0XE1,
    OC_SBC_INDIRECT_Y = 0XF1,
    // SEC - Set Carry Flag
    OC_SEC_IMPLIED = 0X38,
    // SED - Set Decimal Flag
    OC_SED_IMPLIED = 0XF8,
    // SEI - Set Interrupt Disable
    OC_SEI_IMPLIED = 0X78,
    // STA - Store Accumulator
    OC_STA_ZERO_PAGE = 0X85,
    OC_STA_ZERO_PAGE_X = 0X95,
    OC_STA_ABSOLUTE = 0X8D,
    OC_STA_ABSOLUTE_X = 0X9D,
    OC_STA_ABSOLUTE_Y = 0X99,
    OC_STA_INDIRECT_X = 0X81,
    OC_STA_INDIRECT_Y = 0X91,
    // STX - Store X Register
    OC_STX_ZERO_PAGE = 0X86,
    OC_STX_ZERO_PAGE_X = 0X96,
    OC_STX_ABSOLUTE = 0X8E,
    // STY - Store Y Register
    OC_STY_ZERO_PAGE = 0X84,
    OC_STY_ZERO_PAGE_X = 0X94,
    OC_STY_ABSOLUTE = 0X8C,
    // TAX - Transfer Accumulator to X
    OC_TAX_IMPLIED = 0xAA,
    // TAY - Transfer Accumulator to Y
    OC_TAY_IMPLIED = 0XA8,
    // TSX - Transfer Stack Pointer to X
    OC_TSX_IMPLIED = 0XBA,
    // TXA - Transfer X to Accumulator
    OC_TXA_IMPLIED = 0X8A,
    // TXS - Transfer X to Stack Pointer
    OC_TXS_IMPLIED = 0X9A,
    // TYA - Transfer Y to Accumulator
    OC_TYA_IMPLIED = 0X98
};