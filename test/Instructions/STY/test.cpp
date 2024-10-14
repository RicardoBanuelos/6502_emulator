#include <gtest/gtest.h>
#include "Instruction/Instructions/STY.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 10000;


TEST(instructions, sty_test_zeroPage)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);

        STY ins(cpu, AddressingMode::ZeroPage, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp), cpu->getRegister(Register::Y));
    }
}


TEST(instructions, sty_test_zeroPageX)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint8_t lookUp = cpu->readByte(currentAddr);

        STY ins(cpu, AddressingMode::ZeroPageX, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp + cpu->getRegister(Register::X)), cpu->getRegister(Register::Y));
    }
}



TEST(instructions, sty_test_absolute)
{
    for(int i = 0; i < MAX_ITERATIONS; ++i)
    {
        cpu->randomizeRegisters();
        cpu->randomizeFlags();

        uint16_t currentAddr = rand() % UINT16_MAX;
        cpu->setRegister(Register::PC, currentAddr);

        uint16_t lookUp = cpu->readWord(currentAddr);

        STY ins(cpu, AddressingMode::Absolute, 3);
        ins.run();


        ASSERT_EQ(cpu->readByte(lookUp), cpu->getRegister(Register::Y));
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