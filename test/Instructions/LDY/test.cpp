#include <gtest/gtest.h>
#include "Instruction/Instructions/LDY.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 20000;


TEST(instructions, ldy_test_immediate)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t expected = cpu->readByte(currentAddr);

        LDY ins(cpu, AddressingMode::Immediate, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::Y), expected);

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ldy_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr);

        LDY ins(cpu, AddressingMode::ZeroPage, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::Y), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }

}


TEST(instructions, ldy_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentXvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, currentXvalue);
        
        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentXvalue);

        LDY ins(cpu, AddressingMode::ZeroPageX, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::Y), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ldy_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr);

        LDY ins(cpu, AddressingMode::Absolute, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::Y), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}



TEST(instructions, ldy_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        uint16_t currentAddr = rand() % UINT16_MAX;
        uint8_t currentXvalue = rand() % UINT8_MAX;

        cpu->setRegister(Register::PC, currentAddr);
        cpu->setRegister(Register::X, currentXvalue);
        
        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + currentXvalue);

        LDY ins(cpu, AddressingMode::AbsoluteOffsetX, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::Y), expected);
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