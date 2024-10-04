#include <gtest/gtest.h>
#include "Instruction/Instructions/STA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, sta_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);

        STA ins(cpu, AddressingMode::ZeroPage, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp), cpu->getRegister(Register::A));
    }
}


TEST(instructions, sta_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);

        STA ins(cpu, AddressingMode::ZeroPageX, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp + cpu->getRegister(Register::X)), cpu->getRegister(Register::A));
    }
}

TEST(instructions, sta_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUp = cpu->readWord(currentAddr);

        STA ins(cpu, AddressingMode::Absolute, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp), cpu->getRegister(Register::A));
    }
}

TEST(instructions, sta_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUp = cpu->readWord(currentAddr);

        STA ins(cpu, AddressingMode::AbsoluteX, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp + cpu->getRegister(Register::X)), cpu->getRegister(Register::A));
    }
}

TEST(instructions, sta_test_absoluteY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUp = cpu->readWord(currentAddr);

        STA ins(cpu, AddressingMode::AbsoluteY, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp + cpu->getRegister(Register::Y)), cpu->getRegister(Register::A));
    }
}

TEST(instructions, sta_test_IndirectX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUp + cpu->getRegister(Register::X));

        STA ins(cpu, AddressingMode::IndirectX, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(indirect), cpu->getRegister(Register::A));
    }
}

TEST(instructions, sta_test_IndirectY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUp + cpu->getRegister(Register::Y));

        STA ins(cpu, AddressingMode::IndirectY, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(indirect), cpu->getRegister(Register::A));
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