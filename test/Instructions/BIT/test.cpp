/*
Structured Test Cases: Uses defined test cases that verify specific behaviors of BIT

Fixed ASSERT_ALL Function: The original function had a bug in the Z flag check (missing parentheses)

Better Coverage of Flag Testing:

Z flag: Tested with various bit patterns that should/shouldn't produce zero
V flag: Explicitly tested that it copies bit 6 from memory
N flag: Explicitly tested that it copies bit 7 from memory
Verify A Register Unchanged: Specifically checks that BIT doesn't modify the accumulator

Clearer Test Structure: Separated tests for different behaviors

Better Diagnostics: Added SCOPED_TRACE for easier debugging

Consistent Setup: Uses the same setup approach as our other instruction tests


*/

#include <gtest/gtest.h>
#include "BIT.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <tuple>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

void setup()
{
    mem->initialize();
    mem->randomize();
    cpu->reset();
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->connectBus(bus);
    bus->connectMemory(mem);
}

void setA(uint8_t value) { cpu->setRegister(Register::A, value); }
void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }

// Test case parameters: A, M, expected_Z, expected_V, expected_N
using BITTestCase = std::tuple<uint8_t, uint8_t, bool, bool, bool>;

class BITTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>> {
protected:
    void SetUp() override {
        setup();
    }
};

void checkFlags(bool z, bool v, bool n)
{
    ASSERT_EQ(cpu->getFlag(Flag::Z), z);
    ASSERT_EQ(cpu->getFlag(Flag::V), v);
    ASSERT_EQ(cpu->getFlag(Flag::N), n);
}

// Generate comprehensive test cases
std::vector<BITTestCase> generateTestCases() {
    std::vector<BITTestCase> cases;
    
    // Test Z flag (set when A & M == 0)
    cases.push_back(std::make_tuple(0x00, 0xFF, true, true, true));    // A=0 always gives Z=1
    cases.push_back(std::make_tuple(0xFF, 0x00, true, false, false));  // M=0 always gives Z=1
    cases.push_back(std::make_tuple(0x0F, 0xF0, true, true, true));    // No overlapping bits
    cases.push_back(std::make_tuple(0xFF, 0xFF, false, true, true));   // All bits match, Z=0
    
    // Test V flag (copies bit 6 of M)
    cases.push_back(std::make_tuple(0xFF, 0x40, false, true, false));  // Bit 6 set, V=1
    cases.push_back(std::make_tuple(0xFF, 0xBF, false, false, true));  // Bit 6 clear, V=0
    
    // Test N flag (copies bit 7 of M)
    cases.push_back(std::make_tuple(0xFF, 0x80, false, false, true));  // Bit 7 set, N=1
    cases.push_back(std::make_tuple(0xFF, 0x7F, false, true, false));  // Bit 7 clear, N=0
    
    // Combined flag tests
    cases.push_back(std::make_tuple(0x00, 0x00, true, false, false));  // Z=1, V=0, N=0
    cases.push_back(std::make_tuple(0x00, 0xC0, true, true, true));    // Z=1, V=1, N=1
    cases.push_back(std::make_tuple(0x55, 0xAA, true, false, true));    // Z=1, V=1, N=1 (alternating bits)
    cases.push_back(std::make_tuple(0xFF, 0xC0, false, true, true));   // Z=0, V=1, N=1
    
    return cases;
}

// Helper to set up memory for different addressing modes
void setupAddressing(AddressingMode mode, uint8_t value) {
    setPC(0x200);
    
    switch (mode) {
        case AddressingMode::ZeroPage:
            mem->writeByte(0x200, 0x42); // Zero page address
            mem->writeByte(0x42, value); // Value at zero page
            break;
            
        case AddressingMode::Absolute:
            mem->writeWord(0x200, 0x1234); // Absolute address
            mem->writeByte(0x1234, value); // Value at absolute address
            break;
    }
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::ZeroPage: return 3;
        case AddressingMode::Absolute: return 4;
        default: return 2;
    }
}

TEST_P(BITTest, ExhaustiveTest) {
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];
    
    auto [accumulator, memory_value, expected_zero, expected_overflow, expected_negative] = testCase;
    
    setup();
    setA(accumulator);
    setupAddressing(mode, memory_value);
    
    std::unique_ptr<BIT> bit(new BIT(cpu, mode, getCycleCount(mode)));
    bit->run();
    
    std::string modeName;
    switch (mode) {
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        default: modeName = "Unknown";
    }
    
    SCOPED_TRACE("Mode: " + modeName + 
                 ", A: " + std::to_string(accumulator) + 
                 ", M: " + std::to_string(memory_value));
    
    // Verify A register is unchanged
    ASSERT_EQ(cpu->getRegister(Register::A), accumulator);
    
    // Check flags
    checkFlags(expected_zero, expected_overflow, expected_negative);
}

// Test specific bit patterns for clearer verification
TEST(BITTest, BitFlagsBehavior) {
    // Test that bit 7 of M sets N flag
    setup();
    setA(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x42);  // Zero page address
    mem->writeByte(0x42, 0x80);   // Value with bit 7 set
    
    std::unique_ptr<BIT> bit1(new BIT(cpu, AddressingMode::ZeroPage, 3));
    bit1->run();
    
    ASSERT_EQ(cpu->getFlag(Flag::N), true);
    
    // Test that bit 6 of M sets V flag
    setup();
    setA(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x42);  // Zero page address
    mem->writeByte(0x42, 0x40);   // Value with bit 6 set
    
    std::unique_ptr<BIT> bit2(new BIT(cpu, AddressingMode::ZeroPage, 3));
    bit2->run();
    
    ASSERT_EQ(cpu->getFlag(Flag::V), true);
    
    // Test that Z flag is set based on A & M
    setup();
    setA(0x0F);  // Low nibble set
    setPC(0x200);
    mem->writeByte(0x200, 0x42);  // Zero page address
    mem->writeByte(0x42, 0xF0);   // High nibble set - result will be zero
    
    std::unique_ptr<BIT> bit3(new BIT(cpu, AddressingMode::ZeroPage, 3));
    bit3->run();
    
    ASSERT_EQ(cpu->getFlag(Flag::Z), true);
}

// Generate combinations of addressing modes and test cases
std::vector<AddressingMode> addressingModes = {
    AddressingMode::ZeroPage,
    AddressingMode::Absolute
};

std::vector<std::tuple<AddressingMode, int>> GenerateTestParams() {
    std::vector<std::tuple<AddressingMode, int>> params;
    auto testCases = generateTestCases();
    
    for (auto mode : addressingModes) {
        for (int i = 0; i < testCases.size(); ++i) {
            params.push_back(std::make_tuple(mode, i));
        }
    }
    
    return params;
}

INSTANTIATE_TEST_SUITE_P(
    BIT,
    BITTest,
    ::testing::ValuesIn(GenerateTestParams())
);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}