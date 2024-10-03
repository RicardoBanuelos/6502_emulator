#include <gtest/gtest.h>
#include "Instruction/Instructions/LDA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, lda_test_immediate)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t expected = cpu->readByte(currentAddr);

        LDA lda(cpu, AddressingMode::Immediate, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
        
    }
}

TEST(instructions, lda_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr);

        LDA lda(cpu, AddressingMode::ZeroPage, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }

}

TEST(instructions, lda_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentXvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, currentXvalue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentXvalue);

        LDA lda(cpu, AddressingMode::ZeroPageX, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}



TEST(instructions, lda_test_zeroPageY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentYvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::Y, currentYvalue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentYvalue);

        LDA lda(cpu, AddressingMode::ZeroPageY, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, lda_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr);

        LDA lda(cpu, AddressingMode::Absolute, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, lda_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentXvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, currentXvalue);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentXvalue);

        LDA lda(cpu, AddressingMode::AbsoluteOffsetX, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, lda_test_absoluteY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentYvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::Y, currentYvalue);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentYvalue);

        LDA lda(cpu, AddressingMode::AbsoluteOffsetY, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, lda_test_indirect_x)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentXvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, currentXvalue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUpAddr + currentXvalue);
        uint8_t expected = cpu->readByte(indirect);

        LDA lda(cpu, AddressingMode::IndirectX, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, lda_test_indirect_Y)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentYvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::Y, currentYvalue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUpAddr + currentYvalue);
        uint8_t expected = cpu->readByte(indirect);

        LDA lda(cpu, AddressingMode::IndirectY, 2);
        lda.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
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