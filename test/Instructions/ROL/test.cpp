#include <gtest/gtest.h>
#include "Instruction/Instructions/ROL.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, rol_test_accumulator)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();


        uint8_t carry = cpu->getRegister(Register::A) & (1 << 7);
        uint8_t expected = (cpu->getRegister(Register::A) << 1) | (cpu->getRegister(Register::A) >> 7);

        ROL ins(cpu, AddressingMode::Accumulator, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->getRegister(Register::A));
        ASSERT_EQ(carry > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
        
        
    }
}

TEST(instructions, rol_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = (cpu->readByte(lookUpAddr) << 1) | (cpu->readByte(lookUpAddr) >> 7);
        uint8_t carry = cpu->readByte(lookUpAddr) & (1 << 7);

        ROL ins(cpu, AddressingMode::ZeroPage, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr));
        ASSERT_EQ(carry > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
    }
}

TEST(instructions, rol_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = (cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) << 1) | (cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) >> 7);
        uint8_t carry = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) & (1 << 7);

        ROL ins(cpu, AddressingMode::ZeroPageX, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)));
        ASSERT_EQ(carry > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
    }
}

TEST(instructions, rol_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = (cpu->readByte(lookUpAddr) << 1) | (cpu->readByte(lookUpAddr) >> 7);
        uint8_t carry = cpu->readByte(lookUpAddr) & (1 << 7);

        ROL ins(cpu, AddressingMode::Absolute, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr));
        ASSERT_EQ(carry > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
    }
}

TEST(instructions, rol_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = (cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) << 1) | (cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) >> 7);
        uint8_t carry = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) & (1 << 7);

        ROL ins(cpu, AddressingMode::AbsoluteX, 2);
        ins.run();

        ASSERT_EQ(expected, cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)));
        ASSERT_EQ(carry > 0, cpu->getFlag(Flag::C));
        ASSERT_EQ((expected & Flag::N) > 0, cpu->getFlag(Flag::N));
        ASSERT_EQ(expected == 0, cpu->getFlag(Flag::Z));
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