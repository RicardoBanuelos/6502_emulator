#include <gtest/gtest.h>
#include "CPU/CPU.h"
#include "Instruction/Instructions.h"

static std::shared_ptr<CPU> cpu(new CPU());


template<typename T>
void ASSERT_TEMPLATE(AddressingMode mode, OpCode opcode)
{
    const T* ins = dynamic_cast<const T *>(cpu->getInstruction(opcode));
    ASSERT_EQ(true, ins && ins->mode() == mode);
}

TEST(cpu_opcodes, test_adc)
{
    ASSERT_TEMPLATE<ADC>(AddressingMode::Immediate, OpCode::OC_ADC_IMMEDIATE);
    ASSERT_TEMPLATE<ADC>(AddressingMode::ZeroPage, OpCode::OC_ADC_ZERO_PAGE);
    ASSERT_TEMPLATE<ADC>(AddressingMode::ZeroPageX, OpCode::OC_ADC_ZERO_PAGE_X);
    ASSERT_TEMPLATE<ADC>(AddressingMode::Absolute, OpCode::OC_ADC_ABSOLUTE);
    ASSERT_TEMPLATE<ADC>(AddressingMode::AbsoluteX, OpCode::OC_ADC_ABSOLUTE_X);
    ASSERT_TEMPLATE<ADC>(AddressingMode::AbsoluteY, OpCode::OC_ADC_ABSOLUTE_Y);
    ASSERT_TEMPLATE<ADC>(AddressingMode::IndirectX, OpCode::OC_ADC_INDIRECT_X);
    ASSERT_TEMPLATE<ADC>(AddressingMode::IndirectY, OpCode::OC_ADC_INDIRECT_Y);
}

TEST(cpu_opcodes, test_and)
{
    ASSERT_TEMPLATE<AND>(AddressingMode::Immediate, OpCode::OC_AND_IMMEDIATE);
    ASSERT_TEMPLATE<AND>(AddressingMode::ZeroPage, OpCode::OC_AND_ZERO_PAGE);
    ASSERT_TEMPLATE<AND>(AddressingMode::ZeroPageX, OpCode::OC_AND_ZERO_PAGE_X);
    ASSERT_TEMPLATE<AND>(AddressingMode::Absolute, OpCode::OC_AND_ABSOLUTE);
    ASSERT_TEMPLATE<AND>(AddressingMode::AbsoluteX, OpCode::OC_AND_ABSOLUTE_X);
    ASSERT_TEMPLATE<AND>(AddressingMode::AbsoluteY, OpCode::OC_AND_ABSOLUTE_Y);
    ASSERT_TEMPLATE<AND>(AddressingMode::IndirectX, OpCode::OC_AND_INDIRECT_X);
    ASSERT_TEMPLATE<AND>(AddressingMode::IndirectY, OpCode::OC_AND_INDIRECT_Y);
}

TEST(cpu_opcodes, test_asl)
{
    ASSERT_TEMPLATE<ASL>(AddressingMode::Accumulator, OpCode::OC_ASL_ACCUMULATOR);
    ASSERT_TEMPLATE<ASL>(AddressingMode::ZeroPage, OpCode::OC_ASL_ZERO_PAGE);
    ASSERT_TEMPLATE<ASL>(AddressingMode::ZeroPageX, OpCode::OC_ASL_ZERO_PAGE_X);
    ASSERT_TEMPLATE<ASL>(AddressingMode::Absolute, OpCode::OC_ASL_ABSOLUTE);
    ASSERT_TEMPLATE<ASL>(AddressingMode::AbsoluteX, OpCode::OC_ASL_ABSOLUTE_X);
}


TEST(cpu_opcodes, test_bcc)
{
    ASSERT_TEMPLATE<BCC>(AddressingMode::Relative, OpCode::OC_BCC_RELATIVE);
}

TEST(cpu_opcodes, test_beq)
{
    ASSERT_TEMPLATE<BEQ>(AddressingMode::Relative, OpCode::OC_BEQ_RELATIVE);
}

TEST(cpu_opcodes, test_bit)
{
    ASSERT_TEMPLATE<BIT>(AddressingMode::ZeroPage, OpCode::OC_BIT_ZERO_PAGE);
    ASSERT_TEMPLATE<BIT>(AddressingMode::Absolute, OpCode::OC_BIT_ABSOLUTE);    
}

TEST(cpu_opcodes, test_bmi)
{
    ASSERT_TEMPLATE<BMI>(AddressingMode::Relative, OpCode::OC_BMI_RELATIVE);
}

TEST(cpu_opcodes, test_bne)
{
    ASSERT_TEMPLATE<BNE>(AddressingMode::Relative, OpCode::OC_BNE_RELATIVE);
}

TEST(cpu_opcodes, test_jsr)
{
    ASSERT_TEMPLATE<JSR>(AddressingMode::Absolute, OpCode::OC_JSR_ABSOLUTE);
}

TEST(cpu_opcodes, test_lda)
{
    ASSERT_TEMPLATE<LDA>(AddressingMode::Immediate, OpCode::OC_LDA_IMMEDIATE);
    ASSERT_TEMPLATE<LDA>(AddressingMode::ZeroPage, OpCode::OC_LDA_ZERO_PAGE);
    ASSERT_TEMPLATE<LDA>(AddressingMode::ZeroPageX, OpCode::OC_LDA_ZERO_PAGE_X);
    ASSERT_TEMPLATE<LDA>(AddressingMode::Absolute, OpCode::OC_LDA_ABSOLUTE);
    ASSERT_TEMPLATE<LDA>(AddressingMode::AbsoluteX, OpCode::OC_LDA_ABSOLUTE_X);
    ASSERT_TEMPLATE<LDA>(AddressingMode::AbsoluteY, OpCode::OC_LDA_ABSOLUTE_Y);
    ASSERT_TEMPLATE<LDA>(AddressingMode::IndirectX, OpCode::OC_LDA_INDIRECT_X);
    ASSERT_TEMPLATE<LDA>(AddressingMode::IndirectY, OpCode::OC_LDA_INDIRECT_Y);
}

TEST(cpu_opcodes, test_ldx)
{
    ASSERT_TEMPLATE<LDX>(AddressingMode::Immediate, OpCode::OC_LDX_IMMEDIATE);
    ASSERT_TEMPLATE<LDX>(AddressingMode::ZeroPage, OpCode::OC_LDX_ZERO_PAGE);
    ASSERT_TEMPLATE<LDX>(AddressingMode::ZeroPageY, OpCode::OC_LDX_ZERO_PAGE_Y);
    ASSERT_TEMPLATE<LDX>(AddressingMode::Absolute, OpCode::OC_LDX_ABSOLUTE);
    ASSERT_TEMPLATE<LDX>(AddressingMode::AbsoluteY, OpCode::OC_LDX_ABSOLUTE_Y);
}

TEST(cpu_opcodes, test_ldy)
{
    ASSERT_TEMPLATE<LDY>(AddressingMode::Immediate, OpCode::OC_LDY_IMMEDIATE);
    ASSERT_TEMPLATE<LDY>(AddressingMode::ZeroPage, OpCode::OC_LDY_ZERO_PAGE);
    ASSERT_TEMPLATE<LDY>(AddressingMode::ZeroPageX, OpCode::OC_LDY_ZERO_PAGE_X);
    ASSERT_TEMPLATE<LDY>(AddressingMode::Absolute, OpCode::OC_LDY_ABSOLUTE);
    ASSERT_TEMPLATE<LDY>(AddressingMode::AbsoluteX, OpCode::OC_LDY_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_lsr)
{
    ASSERT_TEMPLATE<LSR>(AddressingMode::Accumulator, OpCode::OC_LSR_ACCUMULATOR);
    ASSERT_TEMPLATE<LSR>(AddressingMode::ZeroPage, OpCode::OC_LSR_ZERO_PAGE);
    ASSERT_TEMPLATE<LSR>(AddressingMode::ZeroPageX, OpCode::OC_LSR_ZERO_PAGE_X);
    ASSERT_TEMPLATE<LSR>(AddressingMode::Absolute, OpCode::OC_LSR_ABSOLUTE);
    ASSERT_TEMPLATE<LSR>(AddressingMode::AbsoluteX, OpCode::OC_LSR_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_nop)
{
    ASSERT_TEMPLATE<NOP>(AddressingMode::Implied, OpCode::OC_NOP_IMPLIED);
}

TEST(cpu_opcodes, test_ora)
{
    ASSERT_TEMPLATE<ORA>(AddressingMode::Immediate, OpCode::OC_ORA_IMMEDIATE);
    ASSERT_TEMPLATE<ORA>(AddressingMode::ZeroPage, OpCode::OC_ORA_ZERO_PAGE);
    ASSERT_TEMPLATE<ORA>(AddressingMode::ZeroPageX, OpCode::OC_ORA_ZERO_PAGE_X);
    ASSERT_TEMPLATE<ORA>(AddressingMode::ZeroPage, OpCode::OC_ORA_ZERO_PAGE);
    ASSERT_TEMPLATE<ORA>(AddressingMode::Absolute, OpCode::OC_ORA_ABSOLUTE);
    ASSERT_TEMPLATE<ORA>(AddressingMode::AbsoluteX, OpCode::OC_ORA_ABSOLUTE_X);
    ASSERT_TEMPLATE<ORA>(AddressingMode::AbsoluteY, OpCode::OC_ORA_ABSOLUTE_Y);
    ASSERT_TEMPLATE<ORA>(AddressingMode::IndirectX, OpCode::OC_ORA_INDIRECT_X);
    ASSERT_TEMPLATE<ORA>(AddressingMode::IndirectY, OpCode::OC_ORA_INDIRECT_Y);
}

TEST(cpu_opcodes, test_pha)
{
    ASSERT_TEMPLATE<PHA>(AddressingMode::Implied, OpCode::OC_PHA_IMPLIED);
}

TEST(cpu_opcodes, test_php)
{
    ASSERT_TEMPLATE<PHP>(AddressingMode::Implied, OpCode::OC_PHP_IMPLIED);
}

TEST(cpu_opcodes, test_pla)
{
    ASSERT_TEMPLATE<PLA>(AddressingMode::Implied, OpCode::OC_PLA_IMPLIED);
}

TEST(cpu_opcodes, test_plp)
{
    ASSERT_TEMPLATE<PLP>(AddressingMode::Implied, OpCode::OC_PLP_IMPLIED);
}

TEST(cpu_opcodes, test_rol)
{
    ASSERT_TEMPLATE<ROL>(AddressingMode::Accumulator, OpCode::OC_ROL_ACCUMULATOR);
    ASSERT_TEMPLATE<ROL>(AddressingMode::ZeroPage, OpCode::OC_ROL_ZERO_PAGE);
    ASSERT_TEMPLATE<ROL>(AddressingMode::ZeroPageX, OpCode::OC_ROL_ZERO_PAGE_X);
    ASSERT_TEMPLATE<ROL>(AddressingMode::Absolute, OpCode::OC_ROL_ABSOLUTE);
    ASSERT_TEMPLATE<ROL>(AddressingMode::AbsoluteX, OpCode::OC_ROL_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_ror)
{
    ASSERT_TEMPLATE<ROR>(AddressingMode::Accumulator, OpCode::OC_ROR_ACCUMULATOR);
    ASSERT_TEMPLATE<ROR>(AddressingMode::ZeroPage, OpCode::OC_ROR_ZERO_PAGE);
    ASSERT_TEMPLATE<ROR>(AddressingMode::ZeroPageX, OpCode::OC_ROR_ZERO_PAGE_X);
    ASSERT_TEMPLATE<ROR>(AddressingMode::Absolute, OpCode::OC_ROR_ABSOLUTE);
    ASSERT_TEMPLATE<ROR>(AddressingMode::AbsoluteX, OpCode::OC_ROR_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_rti)
{
    ASSERT_TEMPLATE<RTI>(AddressingMode::Implied, OpCode::OC_RTI_IMPLIED);
}

TEST(cpu_opcodes, test_rts)
{
    ASSERT_TEMPLATE<RTS>(AddressingMode::Implied, OpCode::OC_RTS_IMPLIED);
}

TEST(cpu_opcodes, test_sbc)
{
    ASSERT_TEMPLATE<SBC>(AddressingMode::Immediate, OpCode::OC_SBC_IMMEDIATE);
    ASSERT_TEMPLATE<SBC>(AddressingMode::ZeroPage, OpCode::OC_SBC_ZERO_PAGE);
    ASSERT_TEMPLATE<SBC>(AddressingMode::ZeroPageX, OpCode::OC_SBC_ZERO_PAGE_X);
    ASSERT_TEMPLATE<SBC>(AddressingMode::ZeroPage, OpCode::OC_SBC_ZERO_PAGE);
    ASSERT_TEMPLATE<SBC>(AddressingMode::Absolute, OpCode::OC_SBC_ABSOLUTE);
    ASSERT_TEMPLATE<SBC>(AddressingMode::AbsoluteX, OpCode::OC_SBC_ABSOLUTE_X);
    ASSERT_TEMPLATE<SBC>(AddressingMode::AbsoluteY, OpCode::OC_SBC_ABSOLUTE_Y);
    ASSERT_TEMPLATE<SBC>(AddressingMode::IndirectX, OpCode::OC_SBC_INDIRECT_X);
    ASSERT_TEMPLATE<SBC>(AddressingMode::IndirectY, OpCode::OC_SBC_INDIRECT_Y);
}

TEST(cpu_opcodes, test_sec)
{
    ASSERT_TEMPLATE<SEC>(AddressingMode::Implied, OpCode::OC_SEC_IMPLIED);
}

TEST(cpu_opcodes, test_sed)
{
    ASSERT_TEMPLATE<SED>(AddressingMode::Implied, OpCode::OC_SED_IMPLIED);
}

TEST(cpu_opcodes, test_sei)
{
    ASSERT_TEMPLATE<SEI>(AddressingMode::Implied, OpCode::OC_SEI_IMPLIED);
}

TEST(cpu_opcodes, test_sta)
{
    ASSERT_TEMPLATE<STA>(AddressingMode::ZeroPage, OpCode::OC_STA_ZERO_PAGE);
    ASSERT_TEMPLATE<STA>(AddressingMode::ZeroPageX, OpCode::OC_STA_ZERO_PAGE_X);
    ASSERT_TEMPLATE<STA>(AddressingMode::Absolute, OpCode::OC_STA_ABSOLUTE);
    ASSERT_TEMPLATE<STA>(AddressingMode::AbsoluteX, OpCode::OC_STA_ABSOLUTE_X);
    ASSERT_TEMPLATE<STA>(AddressingMode::AbsoluteY, OpCode::OC_STA_ABSOLUTE_Y);
    ASSERT_TEMPLATE<STA>(AddressingMode::IndirectX, OpCode::OC_STA_INDIRECT_X);
    ASSERT_TEMPLATE<STA>(AddressingMode::IndirectY, OpCode::OC_STA_INDIRECT_Y);
}

TEST(cpu_opcodes, test_stx)
{
    ASSERT_TEMPLATE<STX>(AddressingMode::ZeroPage, OpCode::OC_STX_ZERO_PAGE);
    ASSERT_TEMPLATE<STX>(AddressingMode::ZeroPageY, OpCode::OC_STX_ZERO_PAGE_Y);
    ASSERT_TEMPLATE<STX>(AddressingMode::Absolute, OpCode::OC_STX_ABSOLUTE);
}

TEST(cpu_opcodes, test_sty)
{
    ASSERT_TEMPLATE<STY>(AddressingMode::ZeroPage, OpCode::OC_STY_ZERO_PAGE);
    ASSERT_TEMPLATE<STY>(AddressingMode::ZeroPageX, OpCode::OC_STY_ZERO_PAGE_X);
    ASSERT_TEMPLATE<STY>(AddressingMode::Absolute, OpCode::OC_STY_ABSOLUTE);
}

TEST(cpu_opcodes, test_tax)
{
    ASSERT_TEMPLATE<TAX>(AddressingMode::Implied, OpCode::OC_TAX_IMPLIED);
}

TEST(cpu_opcodes, test_tay)
{
    ASSERT_TEMPLATE<TAY>(AddressingMode::Implied, OpCode::OC_TAY_IMPLIED);
}

TEST(cpu_opcodes, test_tsx)
{
    ASSERT_TEMPLATE<TSX>(AddressingMode::Implied, OpCode::OC_TSX_IMPLIED);
}

TEST(cpu_opcodes, test_txa)
{
    ASSERT_TEMPLATE<TXA>(AddressingMode::Implied, OpCode::OC_TXA_IMPLIED);
}

TEST(cpu_opcodes, test_txs)
{
    ASSERT_TEMPLATE<TXS>(AddressingMode::Implied, OpCode::OC_TXS_IMPLIED);
}

TEST(cpu_opcodes, test_tya)
{
    ASSERT_TEMPLATE<TYA>(AddressingMode::Implied, OpCode::OC_TYA_IMPLIED);
}
TEST(cpu_opcodes, test_bpl)
{
    ASSERT_TEMPLATE<BPL>(AddressingMode::Relative, OpCode::OC_BPL_RELATIVE);
}

TEST(cpu_opcodes, test_bkr)
{
    ASSERT_TEMPLATE<BRK>(AddressingMode::Implied, OpCode::OC_BRK_IMPLIED);
}

TEST(cpu_opcodes, test_bvc)
{
    ASSERT_TEMPLATE<BVC>(AddressingMode::Relative, OpCode::OC_BVC_RELATIVE);
}

TEST(cpu_opcodes, test_bvs)
{
    ASSERT_TEMPLATE<BVS>(AddressingMode::Relative, OpCode::OC_BVS_RELATIVE);
}

TEST(cpu_opcodes, test_clc)
{
    ASSERT_TEMPLATE<CLC>(AddressingMode::Implied, OpCode::OC_CLC_IMPLIED);
}

TEST(cpu_opcodes, test_cli)
{
    ASSERT_TEMPLATE<CLI>(AddressingMode::Implied, OpCode::OC_CLI_IMPLIED);
}

TEST(cpu_opcodes, test_clv)
{
    ASSERT_TEMPLATE<CLV>(AddressingMode::Implied, OpCode::OC_CLV_IMPLIED);
}

TEST(cpu_opcodes, test_cmp)
{
    ASSERT_TEMPLATE<CMP>(AddressingMode::Immediate, OpCode::OC_CMP_IMMEDIATE);
    ASSERT_TEMPLATE<CMP>(AddressingMode::ZeroPage, OpCode::OC_CMP_ZERO_PAGE);
    ASSERT_TEMPLATE<CMP>(AddressingMode::ZeroPageX, OpCode::OC_CMP_ZERO_PAGE_X);
    ASSERT_TEMPLATE<CMP>(AddressingMode::Absolute, OpCode::OC_CMP_ABSOLUTE);
    ASSERT_TEMPLATE<CMP>(AddressingMode::AbsoluteX, OpCode::OC_CMP_ABSOLUTE_X);
    ASSERT_TEMPLATE<CMP>(AddressingMode::AbsoluteY, OpCode::OC_CMP_ABSOLUTE_Y);
    ASSERT_TEMPLATE<CMP>(AddressingMode::IndirectX, OpCode::OC_CMP_INDIRECT_X);
    ASSERT_TEMPLATE<CMP>(AddressingMode::IndirectY, OpCode::OC_CMP_INDIRECT_Y);
}

TEST(cpu_opcodes, test_cpx)
{
    ASSERT_TEMPLATE<CPX>(AddressingMode::Immediate, OpCode::OC_CPX_IMMEDIATE);
    ASSERT_TEMPLATE<CPX>(AddressingMode::ZeroPage, OpCode::OC_CPX_ZERO_PAGE);
    ASSERT_TEMPLATE<CPX>(AddressingMode::Absolute, OpCode::OC_CPX_ABSOLUTE);
}


TEST(cpu_opcodes, test_cpy)
{
    ASSERT_TEMPLATE<CPY>(AddressingMode::Immediate, OpCode::OC_CPY_IMMEDIATE);
    ASSERT_TEMPLATE<CPY>(AddressingMode::ZeroPage, OpCode::OC_CPY_ZERO_PAGE);
    ASSERT_TEMPLATE<CPY>(AddressingMode::Absolute, OpCode::OC_CPY_ABSOLUTE);
}

TEST(cpu_opcodes, test_dec)
{
    ASSERT_TEMPLATE<DEC>(AddressingMode::ZeroPage, OpCode::OC_DEC_ZERO_PAGE);
    ASSERT_TEMPLATE<DEC>(AddressingMode::ZeroPageX, OpCode::OC_DEC_ZERO_PAGE_X);
    ASSERT_TEMPLATE<DEC>(AddressingMode::Absolute, OpCode::OC_DEC_ABSOLUTE);
    ASSERT_TEMPLATE<DEC>(AddressingMode::AbsoluteX, OpCode::OC_DEC_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_dex)
{
    ASSERT_TEMPLATE<DEX>(AddressingMode::Implied, OpCode::OC_DEX_IMPLIED);
}

TEST(cpu_opcodes, test_dey)
{
    ASSERT_TEMPLATE<DEY>(AddressingMode::Implied, OpCode::OC_DEY_IMPLIED);
}

TEST(cpu_opcodes, test_eor)
{
    ASSERT_TEMPLATE<EOR>(AddressingMode::Immediate, OpCode::OC_EOR_IMMEDIATE);
    ASSERT_TEMPLATE<EOR>(AddressingMode::ZeroPage, OpCode::OC_EOR_ZERO_PAGE);
    ASSERT_TEMPLATE<EOR>(AddressingMode::ZeroPageX, OpCode::OC_EOR_ZERO_PAGE_X);
    ASSERT_TEMPLATE<EOR>(AddressingMode::Absolute, OpCode::OC_EOR_ABSOLUTE);
    ASSERT_TEMPLATE<EOR>(AddressingMode::AbsoluteX, OpCode::OC_EOR_ABSOLUTE_X);
    ASSERT_TEMPLATE<EOR>(AddressingMode::AbsoluteY, OpCode::OC_EOR_ABSOLUTE_Y);
    ASSERT_TEMPLATE<EOR>(AddressingMode::IndirectX, OpCode::OC_EOR_INDIRECT_X);
    ASSERT_TEMPLATE<EOR>(AddressingMode::IndirectY, OpCode::OC_EOR_INDIRECT_Y);
}

TEST(cpu_opcodes, test_inc)
{
    ASSERT_TEMPLATE<INC>(AddressingMode::ZeroPage, OpCode::OC_INC_ZERO_PAGE);
    ASSERT_TEMPLATE<INC>(AddressingMode::ZeroPageX, OpCode::OC_INC_ZERO_PAGE_X);
    ASSERT_TEMPLATE<INC>(AddressingMode::Absolute, OpCode::OC_INC_ABSOLUTE);
    ASSERT_TEMPLATE<INC>(AddressingMode::AbsoluteX, OpCode::OC_INC_ABSOLUTE_X);
}

TEST(cpu_opcodes, test_inx)
{
    ASSERT_TEMPLATE<INX>(AddressingMode::Implied, OpCode::OC_INX_IMPLIED);
}

TEST(cpu_opcodes, test_iny)
{
    ASSERT_TEMPLATE<INY>(AddressingMode::Implied, OpCode::OC_INY_IMPLIED);
}

TEST(cpu_opcodes, test_jmp)
{
    ASSERT_TEMPLATE<JMP>(AddressingMode::Absolute, OpCode::OC_JMP_ABSOLUTE);
    ASSERT_TEMPLATE<JMP>(AddressingMode::Indirect, OpCode::OC_JMP_INDIRECT);
    
}


int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    cpu->init();
    

    return RUN_ALL_TESTS();
}