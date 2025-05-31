#include <gtest/gtest.h>
#include "ROL.h"
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
void setX(uint8_t value) { cpu->setRegister(Register::X, value); }
void setY(uint8_t value) { cpu->setRegister(Register::Y, value); }
void setC(bool value) { cpu->setFlag(Flag::C, value); }
void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }

// Test case parameters: input value, carry_in, expected_result, expected_C, expected_Z, expected_N
using ROLTestCase = std::tuple<uint8_t, bool, uint8_t, bool, bool, bool>;

class ROLTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>> {
protected:
    void SetUp() override {
        setup();
    }
};

void checkResults(uint8_t expected_result, bool expected_carry, bool expected_zero, bool expected_negative, uint8_t actual_result)
{
    // Check the result
    ASSERT_EQ(actual_result, expected_result) << "Result value incorrect";
    
    // Check flags
    ASSERT_EQ(cpu->getFlag(Flag::C), expected_carry) << "Carry flag incorrect";
    ASSERT_EQ(cpu->getFlag(Flag::Z), expected_zero) << "Zero flag incorrect";
    ASSERT_EQ(cpu->getFlag(Flag::N), expected_negative) << "Negative flag incorrect";
}

// Comprehensive test data covering all flag combinations
std::vector<ROLTestCase> generateTestCases() {
    std::vector<ROLTestCase> cases;
    
    // Normal rotation without flags
    cases.push_back(std::make_tuple(0x01, false, 0x02, false, false, false)); // 00000001 -> 00000010, C=0
    
    // Test with carry in
    cases.push_back(std::make_tuple(0x01, true, 0x03, false, false, false));  // 00000001 + C -> 00000011, C=0
    
    // Carry out
    cases.push_back(std::make_tuple(0x80, false, 0x00, true, true, false));   // 10000000 -> 00000000, C=1, Z=1
    cases.push_back(std::make_tuple(0x80, true, 0x01, true, false, false));   // 10000000 + C -> 00000001, C=1
    
    // Zero result
    cases.push_back(std::make_tuple(0x00, false, 0x00, false, true, false));  // 00000000 -> 00000000, Z=1
    
    // Negative result
    cases.push_back(std::make_tuple(0x40, false, 0x80, false, false, true));  // 01000000 -> 10000000, N=1
    cases.push_back(std::make_tuple(0x40, true, 0x81, false, false, true));   // 01000000 + C -> 10000001, N=1
    
    // All bits set
    cases.push_back(std::make_tuple(0xFF, false, 0xFE, true, false, true));   // 11111111 -> 11111110, C=1, N=1
    cases.push_back(std::make_tuple(0xFF, true, 0xFF, true, false, true));    // 11111111 + C -> 11111111, C=1, N=1
    
    // Alternating bits
    cases.push_back(std::make_tuple(0x55, false, 0xAA, false, false, true));  // 01010101 -> 10101010, N=1
    cases.push_back(std::make_tuple(0xAA, false, 0x54, true, false, false));  // 10101010 -> 01010100, C=1
    
    return cases;
}

// Helper to set up memory for different addressing modes
void setupAddressing(AddressingMode mode, uint8_t operand) {
    setPC(0x200);
    
    switch (mode) {
        case AddressingMode::Accumulator:
            setA(operand);
            break;
            
        case AddressingMode::ZeroPage:
            mem->writeByte(0x200, 0x42);
            mem->writeByte(0x42, operand);
            break;
            
        case AddressingMode::ZeroPageX:
            setX(0x05);
            mem->writeByte(0x200, 0x40);
            mem->writeByte(0x45, operand);
            break;
            
        case AddressingMode::Absolute:
            mem->writeWord(0x200, 0x1234);
            mem->writeByte(0x1234, operand);
            break;
            
        case AddressingMode::AbsoluteX:
            setX(0x01);
            mem->writeWord(0x200, 0x1234);
            mem->writeByte(0x1235, operand);
            break;
    }
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Accumulator: return 2;
        case AddressingMode::ZeroPage: return 5;
        case AddressingMode::ZeroPageX: return 6;
        case AddressingMode::Absolute: return 6;
        case AddressingMode::AbsoluteX: return 7;
        default: return 2;
    }
}

// Get result after ROL for different addressing modes
uint8_t getResult(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Accumulator:
            return cpu->getRegister(Register::A);
            
        case AddressingMode::ZeroPage:
            return mem->readByte(0x42);
            
        case AddressingMode::ZeroPageX:
            return mem->readByte(0x45);
            
        case AddressingMode::Absolute:
            return mem->readByte(0x1234);
            
        case AddressingMode::AbsoluteX:
            return mem->readByte(0x1235);
            
        default:
            return 0;
    }
}

TEST_P(ROLTest, ExhaustiveTest) {
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];
    
    auto [value, carry_in, expected_result, expected_carry, expected_zero, expected_negative] = testCase;
    
    setup();
    setC(carry_in);
    setupAddressing(mode, value);
    
    std::unique_ptr<ROL> rol(new ROL(cpu, mode, getCycleCount(mode)));
    rol->run();
    
    std::string modeName;
    switch (mode) {
        case AddressingMode::Accumulator: modeName = "Accumulator"; break;
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::ZeroPageX: modeName = "ZeroPageX"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        case AddressingMode::AbsoluteX: modeName = "AbsoluteX"; break;
        default: modeName = "Unknown";
    }
    
    SCOPED_TRACE("Mode: " + modeName + 
                 ", Value: " + std::to_string(value) + 
                 ", C in: " + std::to_string(carry_in));
    
    uint8_t result = getResult(mode);
    checkResults(expected_result, expected_carry, expected_zero, expected_negative, result);
}

// Generate combinations of addressing modes and test cases
std::vector<AddressingMode> addressingModes = {
    AddressingMode::Accumulator,
    AddressingMode::ZeroPage,
    AddressingMode::ZeroPageX,
    AddressingMode::Absolute,
    AddressingMode::AbsoluteX
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
    ROL,
    ROLTest,
    ::testing::ValuesIn(GenerateTestParams())
);

// Test zero page wrap-around behavior
TEST(ROL, ZeroPageWrapAround) {
    setup();
    setX(0xFF);
    setC(false);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeByte(0x7F, 0x40);   // Will rotate to 0x80
    
    std::unique_ptr<ROL> rol(new ROL(cpu, AddressingMode::ZeroPageX, 6));
    rol->run();
    
    ASSERT_EQ(mem->readByte(0x7F), 0x80);
    ASSERT_EQ(cpu->getFlag(Flag::C), false);
    ASSERT_EQ(cpu->getFlag(Flag::Z), false);
    ASSERT_EQ(cpu->getFlag(Flag::N), true);
}

// Test carry flag behavior across multiple rotations
TEST(ROL, MultiRotation) {
    setup();
    
    // Start with 0x81 in accumulator (10000001)
    setA(0x81);
    setC(false);
    
    // First rotation: 10000001 -> 00000010, C=1
    std::unique_ptr<ROL> rol1(new ROL(cpu, AddressingMode::Accumulator, 2));
    rol1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x02);
    ASSERT_EQ(cpu->getFlag(Flag::C), true);
    
    // Second rotation: 00000010 -> 00000101, C=0 (carry in from previous operation)
    std::unique_ptr<ROL> rol2(new ROL(cpu, AddressingMode::Accumulator, 2));
    rol2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x05);
    ASSERT_EQ(cpu->getFlag(Flag::C), false);
}

// Edge case: All zeros with carry in
TEST(ROL, AllZerosWithCarry) {
    setup();
    setA(0x00);
    setC(true);
    
    std::unique_ptr<ROL> rol(new ROL(cpu, AddressingMode::Accumulator, 2));
    rol->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x01);
    ASSERT_EQ(cpu->getFlag(Flag::C), false);
    ASSERT_EQ(cpu->getFlag(Flag::Z), false);
    ASSERT_EQ(cpu->getFlag(Flag::N), false);
}

// Test ROL memory operations properly read-modify-write
TEST(ROL, ReadModifyWrite) {
    setup();
    
    // Setup memory with 0x01
    setPC(0x200);
    mem->writeWord(0x200, 0x1234);
    mem->writeByte(0x1234, 0x01);
    setC(true);
    
    // Execute ROL
    std::unique_ptr<ROL> rol(new ROL(cpu, AddressingMode::Absolute, 6));
    rol->run();
    
    // Check memory was updated correctly (0x01 -> 0x03 with carry in)
    ASSERT_EQ(mem->readByte(0x1234), 0x03);
    ASSERT_EQ(cpu->getFlag(Flag::C), false);
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