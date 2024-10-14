#include <gtest/gtest.h>
#include "Instruction/Instructions/ADC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

// build\test\Instructions\ADC\adc_test.exe

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void ASSERT_ALL(uint16_t a, uint16_t fetched, uint16_t expected)
{
    // Overflow formula based on javidx9
    // Explanation here: https://github.com/OneLoneCoder/olcNES/blob/master/Part%232%20-%20CPU/olc6502.cpp
    bool overflow = (~(a ^ fetched) & (a ^ expected)) & 0x0080;
    ASSERT_EQ(cpu->getFlag(Flag::V), overflow);
    ASSERT_EQ(cpu->getFlag(Flag::C), expected > 255);
    ASSERT_EQ(cpu->getFlag(Flag::Z), (expected & 0x00FF) == 0);
    ASSERT_EQ(cpu->getFlag(Flag::N), static_cast<bool>(expected & 0x80));
    ASSERT_EQ(expected & 0x00FF, cpu->getRegister(Register::A));
}

TEST(instructions, adc_immediate)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(address);
        uint16_t expected = fetched
                        + a 
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::Immediate, 2));
        adc->run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, adc_zero_page)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(zeroPageAddress);
        uint16_t expected = fetched 
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::ZeroPage, 3));
        adc->run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, adc_zero_page_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t zeroPageAddress = cpu->readByte(address) 
                                + cpu->getRegister(Register::X);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(zeroPageAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::ZeroPageX, 4));
        adc->run();

        ASSERT_ALL(a, fetched, expected);
    }
}

TEST(instructions, adc_absolute)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(absoluteAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::Absolute, 4));
        adc->run();

        ASSERT_ALL(a, fetched, expected);   
    }
}

TEST(instructions, adc_absolute_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address)
                                + cpu->getRegister(Register::X);;

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(absoluteAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::AbsoluteX, 4));
        adc->run();

        ASSERT_ALL(a, fetched, expected);   
    }
}

TEST(instructions, adc_absolute_y)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t address = cpu->getRegister(Register::PC);
        uint16_t absoluteAddress = cpu->readWord(address)
                                + cpu->getRegister(Register::Y);;

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(absoluteAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::AbsoluteY, 4));
        adc->run();

        ASSERT_ALL(a, fetched, expected);   
    }
}

TEST(instructions, adc_indirect_x)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t baseAddress = cpu->readByte(PC) + cpu->getRegister(Register::X);
        uint16_t indirectAddress = cpu->readWord(baseAddress);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(indirectAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::IndirectX, 6));
        adc->run();

        ASSERT_ALL(a, fetched, expected);   
    }
}

TEST(instructions, adc_indirect_y)
{
    for(int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        cpu->randomizeRegisters();
        uint16_t PC = cpu->getRegister(Register::PC);
        uint16_t baseAddress = cpu->readByte(PC) + cpu->getRegister(Register::Y);
        uint16_t indirectAddress = cpu->readWord(baseAddress);

        uint16_t a = cpu->getRegister(Register::A);
        uint16_t fetched =  cpu->readByte(indirectAddress);
        uint16_t expected = fetched
                        + a
                        + cpu->getFlag(Flag::C);

        std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::IndirectY, 5));
        adc->run();

        ASSERT_ALL(a, fetched, expected);   
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