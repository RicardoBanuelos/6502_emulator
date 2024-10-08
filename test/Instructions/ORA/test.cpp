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
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;

        cpu->setRegister(Register::PC, currentAddr);

        uint8_t registerA = cpu->getRegister(Register::A);
        uint8_t expected = cpu->readByte(currentAddr) | cpu->getRegister(Register::A);

        ORA ins(cpu, AddressingMode::Immediate, 2);
        ins.run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
        
    }
}

TEST(instructions, ora_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::ZeroPage, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}


TEST(instructions, ora_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;

        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::ZeroPageX, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}



TEST(instructions, ora_test_zeroPageY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;

        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + cpu->getRegister(Register::Y)) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::ZeroPageY, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ora_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::Absolute, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ora_test_absoluteX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + cpu->getRegister(Register::X)) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::AbsoluteX, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ora_test_absoluteY)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUpAddr = cpu->readWord(currentAddr);
        uint8_t expected = cpu->readByte(lookUpAddr + cpu->getRegister(Register::Y)) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::AbsoluteY, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}

TEST(instructions, ora_test_indirect_X)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUpAddr + cpu->getRegister(Register::X));
        uint8_t expected = cpu->readByte(indirect) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::IndirectX, 2);
        lda.run();

        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & Flag::N) > 0);
    }
}


TEST(instructions, ora_test_indirect_Y)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);


        uint8_t lookUpAddr = cpu->readByte(currentAddr);
        uint16_t indirect = cpu->readWord(lookUpAddr + cpu->getRegister(Register::Y));
        uint8_t expected = cpu->readByte(indirect) | cpu->getRegister(Register::A);

        ORA lda(cpu, AddressingMode::IndirectY, 2);
        lda.run();

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