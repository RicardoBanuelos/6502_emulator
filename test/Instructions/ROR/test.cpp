#include <gtest/gtest.h>
#include "Instruction/Instructions/ROR.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 1000;

void ASSERT_ALL(uint16_t result, uint8_t expected)
{
    
    ASSERT_EQ(result & 0x00FF, expected);
    ASSERT_EQ((result & 0x01) > 0, cpu->getFlag(Flag::C));
    ASSERT_EQ((result & Flag::N) > 0, cpu->getFlag(Flag::N));
    ASSERT_EQ((result & 0x00FF) == 0, cpu->getFlag(Flag::Z));
}


TEST(instructions, ror_test_accumulator)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        
        cpu->setFlag(Flag::C, rand() % 2);
        uint16_t fetched = cpu->getRegister(Register::A);
        uint16_t result = static_cast<uint16_t>(cpu->getFlag(Flag::C) << 7) | (fetched >> 1);

        ROR ins(cpu, AddressingMode::Accumulator, 2);
        ins.run();

        ASSERT_ALL(result, cpu->getRegister(Register::A));
        
        
    }
}

TEST(instructions, ror_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        cpu->setFlag(Flag::C, rand() % 2);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t fetched = cpu->readByte(lookUpAddr);
        uint16_t result = static_cast<uint16_t>(cpu->getFlag(Flag::C) << 7) | (fetched >> 1);

        ROR ins(cpu, AddressingMode::ZeroPage, 2);
        ins.run();


        ASSERT_ALL(result, cpu->readByte(lookUpAddr));
    }
}

TEST(instructions, ror_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t fetched = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X));
        uint16_t result = static_cast<uint16_t>(cpu->getFlag(Flag::C) << 7) | (fetched >> 1);

        ROR ins(cpu, AddressingMode::ZeroPageX, 2);
        ins.run();

        ASSERT_ALL(result, cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)));
    }
}

TEST(instructions, ror_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint16_t fetched = cpu->readByte(lookUpAddr);

        uint16_t result = static_cast<uint16_t>(cpu->getFlag(Flag::C) << 7) | (fetched >> 1);

        ROR ins(cpu, AddressingMode::Absolute, 2);
        ins.run();

        ASSERT_ALL(result, cpu->readByte(lookUpAddr));  
    }
}

TEST(instructions, ror_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint16_t fetched = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X));

        uint16_t result = static_cast<uint16_t>(cpu->getFlag(Flag::C) << 7) | (fetched >> 1);

        ROR ins(cpu, AddressingMode::AbsoluteX, 2);
        ins.run();

        ASSERT_ALL(result, cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)));  
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