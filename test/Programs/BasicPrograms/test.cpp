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

static std::vector<uint16_t> getPrimeNumbers(uint16_t n) {
    if (n <= 0) {
        return {};
    }
    
    std::vector<uint16_t> primes;
    primes.reserve(n);
    
    // 2 is the first prime number
    primes.push_back(2);
    
    // Start checking from 3 and only check odd numbers
    int num = 3;
    while (primes.size() < n) {
        bool isPrime = true;
        
        // We only need to check divisibility up to sqrt(num)
        int sqrtNum = std::sqrt(num);
        
        // Check if num is divisible by any previously found prime
        for (const int& prime : primes) {
            if (prime > sqrtNum) {
                break; // No need to check beyond sqrt(num)
            }
            
            if (num % prime == 0) {
                isPrime = false;
                break;
            }
        }
        
        if (isPrime) {
            primes.push_back(num);
        }
        
        num += 2; // Check only odd numbers
    }
    
    return primes;
}

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

TEST(programs, basic_program_test_0)
{

    for (int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        ASSERT_EQ(true, mem->loadBinary(BASIC_PROGRAM_0_PATH));

        cpu->reset();

        cpu->setRegister(Register::PC, 0);

        RunProgram();

        ASSERT_EQ(cpu->getRegister(Register::A), 0x8F);
        ASSERT_EQ(cpu->getRegister(Register::X), 0X0A);
        ASSERT_EQ(cpu->getRegister(Register::Y), 0x18);
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF);

        ASSERT_EQ(cpu->getFlag(Flag::N), true);
        ASSERT_EQ(cpu->getFlag(Flag::V), false);
        ASSERT_EQ(cpu->getFlag(Flag::Z), false);
        ASSERT_EQ(cpu->getFlag(Flag::C), false);
    }
}

TEST(programs, basic_program_test_1)
{
    cpu->setFlag(Flag::B, true);
    for (int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        ASSERT_EQ(true, mem->loadBinary(BASIC_PROGRAM_1_PATH));

        cpu->reset();

        cpu->setRegister(Register::PC, 0);

        RunProgram();

        ASSERT_EQ(cpu->getRegister(Register::A), 0x8F);
        ASSERT_EQ(cpu->getRegister(Register::X), 0X0A);
        ASSERT_EQ(cpu->getRegister(Register::Y), 0X18);
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF);

        ASSERT_EQ(cpu->getFlag(Flag::N), true);
        ASSERT_EQ(cpu->getFlag(Flag::V), false);
        ASSERT_EQ(cpu->getFlag(Flag::Z), false);
        ASSERT_EQ(cpu->getFlag(Flag::C), false);
    }
}

TEST(programs, basic_program_test_2)
{

    mem->randomize();
    ASSERT_EQ(true, mem->loadBinary(BASIC_PROGRAM_2_PATH));

    cpu->reset();
    cpu->setRegister(Register::PC, 0);

    RunProgram();

    ASSERT_EQ(mem->readByte(0x0200), 0x42);
    ASSERT_EQ(mem->readByte(0x0201), 0x99);
}

TEST(programs, basic_program_test_3)
{
    const uint16_t START_ADDRESS = 0x0600;
    mem->randomize();
    ASSERT_EQ(true, mem->loadBinary(BASIC_PROGRAM_3_PATH, START_ADDRESS));

    cpu->reset();
    cpu->setRegister(Register::PC, START_ADDRESS);

    RunProgram();

    ASSERT_EQ(mem->readByte(0x0200), 0x0F);
    ASSERT_EQ(mem->readByte(0x0201), 0x0D);
}

TEST(programs, basic_math_program_test_0)
{
    const uint16_t START_ADDRESS = 0x0600;
    
    mem->randomize();
    ASSERT_EQ(true, mem->loadBinary(BASIC_MATH_PROGRAM_0_PATH, START_ADDRESS));

    cpu->reset();
    cpu->setRegister(Register::PC, START_ADDRESS);

    RunProgram();

    ASSERT_EQ(mem->readByte(0x0200), 0x18);
    ASSERT_EQ(mem->readByte(0x0201), 0x04);

}


TEST(programs, basic_program_square_root)
{

    // 00F0: number to find square root low byte
    // 00F1: number to find square root high byte
    // 00F2: remainder low byte
    // 00F3: remainder high byte
    // 00F6: square root (answer)

    uint16_t INPUT_LOW_ADDR = 0x00F0;
    uint16_t OUTPUT_ADDR = 0x00F6;

    for (int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        ASSERT_EQ(true, mem->loadBinary(BASIC_PROGRAM_SQUARE_ROOT, 0x8000));

        cpu->reset();
        cpu->setRegister(Register::PC, 0x8000);

        // write input
        mem->writeWord(INPUT_LOW_ADDR, rand() % UINT16_MAX);

        Instruction *current = cpu->fetchInstruction();
        ASSERT_NE(current, nullptr);

        uint8_t expected = sqrt(mem->readWord(INPUT_LOW_ADDR));
        uint8_t remainder = 1;

        while (current->name() != "RTS")
        {
            current->run();
            current = cpu->fetchInstruction();
            ASSERT_EQ(current != nullptr, true);
        }

        ASSERT_EQ(expected, mem->readByte(OUTPUT_ADDR));
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