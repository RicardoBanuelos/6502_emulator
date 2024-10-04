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