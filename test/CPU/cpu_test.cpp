#include <gtest/gtest.h>
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"


static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());


TEST(cpu_test, registersTest0)
{
    uint8_t expected = 125;
    Register test = Register::A;

    cpu->setRegister(test, expected);
    ASSERT_EQ(expected, cpu->getRegister(test));
}

TEST(cpu_test, registersTest1)
{
    uint8_t expected = 147;
    Register test = Register::PC;

    cpu->setRegister(test, expected);
    ASSERT_EQ(expected, cpu->getRegister(test));
}

TEST(cpu_test, registersTest2)
{
    uint8_t expected = 12;
    Register test = Register::X;

    cpu->setRegister(test, expected);
    ASSERT_EQ(expected, cpu->getRegister(test));
}


TEST(cpu_test, registersTest3)
{
    uint8_t expected = 78;
    Register test = Register::Y;

    cpu->setRegister(test, expected);
    ASSERT_EQ(expected, cpu->getRegister(test));
}

TEST(cpu_test, statusRegisterCarry)
{
    Flag test = Flag::C;
    uint8_t expected = 1;

    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));

    expected = 0;
    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));

}


TEST(cpu_test, statusRegisterOverflow)
{
    Flag test = Flag::V;
    uint8_t expected = 1;

    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));

    expected = 0;
    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));
    
}

TEST(cpu_test, statusRegisterNegative)
{
    Flag test = Flag::N;
    uint8_t expected = 1;

    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));

    expected = 0;
    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));
}

TEST(cpu_test, statusRegisterDecimal)
{
    Flag test = Flag::D;
    uint8_t expected = 1;

    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));

    expected = 0;
    cpu->setFlag(test, expected);
    ASSERT_EQ(expected, cpu->getFlag(test));
}

TEST(stack_test, stackTest0)
{
    uint8_t expected = 100;
    cpu->pushByte(expected);

    ASSERT_EQ(expected, cpu->popByte());
}

TEST(stack_test, stackTest1)
{
    cpu->pushByte(10);
    cpu->pushByte(20);
    cpu->pushByte(30);
    cpu->pushByte(40);
    

    ASSERT_EQ(40, cpu->popByte());
    ASSERT_EQ(30, cpu->popByte());
    ASSERT_EQ(20, cpu->popByte());
    ASSERT_EQ(10, cpu->popByte());
}


TEST(stack_test, stackTest2)
{
    uint16_t expected = 4587;
    cpu->pushWord(expected);

    ASSERT_EQ(expected, cpu->popWord());
}


TEST(stack_test, stackTest3)
{
    cpu->pushWord(1);
    cpu->pushWord(1000);
    cpu->pushWord(200);
    cpu->pushWord(3000);
    

    ASSERT_EQ(3000, cpu->popWord());
    ASSERT_EQ(200, cpu->popWord());
    ASSERT_EQ(1000, cpu->popWord());
    ASSERT_EQ(1, cpu->popWord());
}



int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();

    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();

    cppu->connectBus(bus);
    bus->connectMemory(mem);
    

    return RUN_ALL_TESTS();
}