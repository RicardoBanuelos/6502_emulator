#include <gtest/gtest.h>
#include "Instruction/Instructions/LSR.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 20000;

TEST(instructions, lsr_test_accumulator)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint8_t aValue = rand() % UINT8_MAX;
        uint8_t expected = aValue >> 1;
        
        cpu->setRegister(Register::A, aValue);

        LSR ins(cpu, AddressingMode::Accumulator, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->getRegister(Register::A));
        ASSERT_EQ((aValue & 0x01) > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
    }
}

TEST(instructions, lsr_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t original = cpu->readByte(lookUpAddr);
        uint8_t expected = original >> 1;

        LSR ins(cpu, AddressingMode::ZeroPage, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr));
        ASSERT_EQ((original & 0x01) > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
    }

}

TEST(instructions, lsr_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t xValue = rand() % UINT8_MAX;
        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, xValue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t original = cpu->readByte(lookUpAddr + xValue);
        uint8_t expected = original >> 1;

        LSR ins(cpu, AddressingMode::ZeroPageX, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr + xValue));
        ASSERT_EQ((original & 0x01) > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
    }
}


TEST(instructions, lsr_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t original = cpu->readByte(lookUpAddr);
        uint8_t expected = original >> 1;

        LSR ins(cpu, AddressingMode::Absolute, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr));
        ASSERT_EQ((original & 0x01) > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
    }
}

TEST(instructions, lsr_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t xValue = rand() % UINT8_MAX;
        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, xValue);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t original = cpu->readByte(lookUpAddr + xValue);
        uint8_t expected = original >> 1;

        LSR ins(cpu, AddressingMode::AbsoluteOffsetX, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr + xValue));
        ASSERT_EQ((original & 0x01) > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
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