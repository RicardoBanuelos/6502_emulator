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


int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    cpu->init();
    

    return RUN_ALL_TESTS();
}