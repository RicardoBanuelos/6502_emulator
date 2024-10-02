#include <gtest/gtest.h>
#include "Instruction/Instructions/AND.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, and_immediate)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);

    uint8_t expected = cpu->readByte(address) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::Immediate, 2));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_zero_page)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t zeroPageAddress = cpu->readByte(address);

    uint8_t expected = cpu->readByte(zeroPageAddress) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::ZeroPage, 3));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_zero_page_x)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t zeroPageAddress = cpu->readByte(address) 
                             + cpu->getRegister(Register::X);

    uint8_t expected = cpu->readByte(zeroPageAddress) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::ZeroPageX, 4));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_absolute)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t absoluteAddress = cpu->readWord(address);

    uint8_t expected = cpu->readByte(absoluteAddress) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::Absolute, 4));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_absolute_x)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t absoluteAddress = cpu->readWord(address)
                             + cpu->getRegister(Register::X);;

    uint8_t expected = cpu->readByte(absoluteAddress) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::AbsoluteOffsetX, 4));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_absolute_y)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t absoluteAddress = cpu->readWord(address)
                             + cpu->getRegister(Register::Y);;

    uint8_t expected = cpu->readByte(absoluteAddress) 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::AbsoluteOffsetY, 4));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_indirect_x)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t indirectAddress = cpu->readWord(address)
                             + cpu->getRegister(Register::X);;

    uint8_t expected = cpu->readByte(indirectAddress) & 0x00FF 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::IndirectX, 6));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, and_indirect_y)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t indirectAddress = cpu->readWord(address)
                             + cpu->getRegister(Register::Y);;

    uint8_t expected = cpu->readByte(indirectAddress) & 0x00FF 
                     & cpu->getRegister(Register::A);

    std::unique_ptr<AND> instruction(new AND(cpu, AddressingMode::IndirectY, 5));
    instruction->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
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