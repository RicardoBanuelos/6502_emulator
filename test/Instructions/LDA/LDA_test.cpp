#include <gtest/gtest.h>
#include "Instruction/Instructions/LDA.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());



TEST(instructions, lda_test_immediate)
{
    uint16_t baseAddr = 1250;
    uint8_t expected = 89;
    cpu->setRegister(Register::PC, baseAddr);

    mem->writeByte(baseAddr, expected);
    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::Immediate, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, lda_test_zeroPage)
{
    uint16_t baseAddr = 1250;
    uint8_t addrToRead = 89;
    uint8_t expected = 120;

    cpu->setRegister(Register::PC, baseAddr);

    mem->writeByte(baseAddr, addrToRead);
    mem->writeByte(addrToRead, expected);
    

    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::ZeroPage, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, lda_test_zeroPageX)
{
    uint16_t baseAddr = 1250;
    uint8_t addrToRead = 81;
    uint8_t xOffset = 9;
    
    uint8_t expected = 120;
    
    cpu->setRegister(Register::PC, baseAddr);
    cpu->setRegister(Register::X, xOffset);
    

    mem->writeByte(baseAddr, addrToRead);
    mem->writeByte(addrToRead + xOffset, expected);


    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::ZeroPageX, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}



TEST(instructions, lda_test_zeroPageY)
{
    uint16_t baseAddr = 1250;
    uint8_t addrToRead = 81;
    uint8_t xOffset = 9;
    
    uint8_t expected = 120;
    
    cpu->setRegister(Register::PC, baseAddr);
    cpu->setRegister(Register::Y, xOffset);
    

    mem->writeByte(baseAddr, addrToRead);
    mem->writeByte(addrToRead + xOffset, expected);


    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::ZeroPageY, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, lda_test_absolute)
{
    uint16_t baseAddr = 1250;
    uint16_t addrToLook = 3254;
    uint8_t expected = 125;
    
    cpu->setRegister(Register::PC, baseAddr);

    cpu->writeWord(baseAddr, addrToLook);
    cpu->writeByte(addrToLook, expected);

     std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::Absolute, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, lda_test_absoluteX)
{
    uint16_t baseAddr = 1250;
    uint8_t addrOffset = 12;
    uint16_t addrToLook = 3254;
    uint8_t expected = 125;
    
    cpu->setRegister(Register::PC, baseAddr);
    cpu->setRegister(Register::X, addrOffset);

    cpu->writeWord(baseAddr, addrToLook);
    cpu->writeByte(addrToLook + addrOffset, expected);

    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::AbsoluteOffsetX, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
}

TEST(instructions, lda_test_absoluteY)
{
    uint16_t baseAddr = 3321;
    uint8_t addrOffset = 45;
    uint16_t addrToLook = 3333;
    uint8_t expected = 111;
    
    cpu->setRegister(Register::PC, baseAddr);
    cpu->setRegister(Register::Y, addrOffset);

    cpu->writeWord(baseAddr, addrToLook);
    cpu->writeByte(addrToLook + addrOffset, expected);

    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::AbsoluteOffsetY, 2));
    lda->run();

    ASSERT_EQ(expected, cpu->getRegister(Register::A));
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