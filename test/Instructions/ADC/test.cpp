#include <gtest/gtest.h>
#include "Instruction/Instructions/ADC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(instructions, adc_immediate)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);

    uint8_t expected = cpu->readByte(address) 
                     + cpu->getRegister(Register::A) 
                     + cpu->getFlag(Flag::C);

    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::Immediate, 2));
    adc->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, adc_zero_page)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t zeroPageAddress = cpu->readByte(address);

    uint8_t expected = cpu->readByte(zeroPageAddress) 
                     + cpu->getRegister(Register::A) 
                     + cpu->getFlag(Flag::C);

    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::ZeroPage, 3));
    adc->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, adc_zero_page_x)
{
    cpu->reset();
    uint16_t address = cpu->getRegister(Register::PC);
    uint16_t zeroPageAddress = cpu->readByte(address) 
                             + cpu->getRegister(Register::X);

    uint8_t expected = cpu->readByte(zeroPageAddress) 
                     + cpu->getRegister(Register::A) 
                     + cpu->getFlag(Flag::C);

    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::ZeroPage, 4));
    adc->run();

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