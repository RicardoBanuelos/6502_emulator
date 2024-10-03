#include <gtest/gtest.h>
#include "Instruction/Instructions/JMP.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, jmp_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        
        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(PC);
        uint16_t expected = cpu->readByte(absoluteAddress);

        std::unique_ptr<JMP> instruction(new JMP(cpu, AddressingMode::Absolute, 3));
        instruction->run();

        ASSERT_EQ(cpu->getRegister(Register::PC), expected);
    }
}

TEST(instructions, jmp_indirect)
{
    for(int i = 0; i < 1000; ++i)
    {    
        cpu->reset();

        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t indirectAddress = cpu->readWord(PC);
        uint16_t expected = cpu->readWord(indirectAddress);
        
        std::unique_ptr<JMP> instruction(new JMP(cpu, AddressingMode::Indirect, 5));
        instruction->run();

        ASSERT_EQ(cpu->getRegister(Register::PC), expected);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();

    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();

    cppu->connectBus(bus);
    bus->connectMemory(mem);

    return RUN_ALL_TESTS();
}