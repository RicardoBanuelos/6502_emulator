#include <gtest/gtest.h>
#include "Instruction/Instructions/ORA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, ora_test_immediate)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t accumulator = rand() % UINT8_MAX;

        cpu->setRegister(Register::A, accumulator);
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t expected = cpu->readByte(currentAddr);

        ORA ins(cpu, AddressingMode::Immediate, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
        
    }
}

TEST(instructions, lda_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }

}

TEST(instructions, lda_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}



TEST(instructions, lda_test_zeroPageY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}

TEST(instructions, lda_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}

TEST(instructions, lda_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}

TEST(instructions, lda_test_absoluteY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}

TEST(instructions, lda_test_indirect_x)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}

TEST(instructions, lda_test_indirect_Y)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {

    }
}


int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));

    mem->initialize();
    mem->randomize();
    
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->reset();
    

    cppu->connectBus(bus);
    bus->connectMemory(mem);
    
   
    return RUN_ALL_TESTS();
}