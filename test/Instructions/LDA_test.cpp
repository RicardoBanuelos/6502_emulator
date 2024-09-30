#include <gtest/gtest.h>
#include "Instruction/Instructions/LDA.h"
#include "CPU/CPU.h"



TEST(lda_test, ImmediateTest)
{


    std::shared_ptr<ICPU> cpu;
    cpu.reset(new CPU());
    static_cast<CPU*>(cpu.get())->reset();


    cpu->registers().PC = 0x256;
    cpu->writeByte(0x257, 0x87);


    LDA ins(cpu, AddressingMode::Immediate, 2);
    ins.run();

    ASSERT_EQ(cpu->registers().A, 0x87);    
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

   
    return RUN_ALL_TESTS();
}