#include <gtest/gtest.h>
#include <math.h>
#include "Instructions.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "test_paths.h"

static std::shared_ptr<CPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());


static void RunProgram()
{
    Instruction *current = cpu->fetchInstruction();
    ASSERT_NE(current, nullptr);
    while (current->name() != "BRK")
    {
        current->run();
        current = cpu->fetchInstruction();
        ASSERT_EQ(current != nullptr, true);
    }
}

/*
; Example data:
; $10 = 4   ; Multiplicand
; $11 = 3   ; Multiplier
; After execution, $12 = 12
*/
TEST(math_programs, multiply_test)
{
    for (int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        ASSERT_EQ(true, mem->loadBinary(MULTIPLY_TEST_PROGRAM));

        cpu->setRegister(Register::PC, 0x0000);
        uint8_t multiplicand = rand() % 256;
        uint8_t multiplier = rand() % 256;
        mem->writeByte(0x0030, multiplicand);
        mem->writeByte(0x0031, multiplier);
    
        
        RunProgram();

        uint8_t result = mem->readByte(0x0032);
        uint8_t expected = multiplicand * multiplier;
        ASSERT_EQ(result, expected) << "Expected: " << (expected) 
                                    << ", got: " << (result)
                                    << ", multiplicand: " << (multiplicand)
                                    << ", multiplier: " << (multiplier);
    }
}


TEST(math_programs, bitwise_and_test)
{
    for (int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        ASSERT_EQ(true, mem->loadBinary(BITWISE_AND_TEST_PROGRAM));
        cpu->setRegister(Register::PC, 0x0000);
        uint8_t operand1 = rand() % 256;
        uint8_t operand2 = rand() % 256;
        mem->writeByte(0x0030, operand1);
        mem->writeByte(0x0031, operand2);
        mem->writeByte(0x0032, 0); // Clear the result location
        // Run the program
        RunProgram();


        uint8_t result = mem->readByte(0x0032);
        uint8_t expected = operand1 & operand2;
        ASSERT_EQ(result, expected) << "Expected: " << (expected) 
                                    << ", got: " << (result)
                                    << ", operand1: " << (operand1)
                                    << ", operand2: " << (operand2);
    }
}

TEST(math_programs, bitwise_xor)
{
    for (int i = 0; i < 1000; ++i)
    {
        cpu->reset();
        ASSERT_EQ(true, mem->loadBinary(BITWISE_XOR_TEST_PROGRAM));
        cpu->setRegister(Register::PC, 0x0000);
        uint8_t operand1 = rand() % 256;
        uint8_t operand2 = rand() % 256;
        mem->writeByte(0x0030, operand1);
        mem->writeByte(0x0031, operand2);
        mem->writeByte(0x0032, 0); // Clear the result location
        // Run the program
        RunProgram();
        uint8_t result = mem->readByte(0x0032);
        uint8_t expected = (operand1 ^ operand2) ^ 0xFF;
        ASSERT_EQ(result, expected) << "Expected: " << (expected) 
                                    << ", got: " << (result)
                                    << ", operand1: " << (operand1)
                                    << ", operand2: " << (operand2);

    }
}

int main(int argc, char **argv)
{
    srand(time(nullptr));
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();

    cpu->init();

    cpu->connectBus(bus);
    bus->connectMemory(mem);

    return RUN_ALL_TESTS();
}