#include <gtest/gtest.h>
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "ADC.h"
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

// Test case parameters: A, operand, carry_in, expected_result, expected_C, expected_Z, expected_N, expected_V
using ADCTestCase = std::tuple<uint8_t, uint8_t, bool, uint8_t, bool, bool, bool, bool>;

class ADCTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>> {
protected:
    void SetUp() override {
        setup();
    }
};

void checkFlags(uint8_t result, bool carry, bool zero, bool negative, bool overflow)
{
    ASSERT_EQ(cpu->getRegister(Register::A), result);
    ASSERT_EQ(cpu->getFlag(Flag::C), carry);
    ASSERT_EQ(cpu->getFlag(Flag::Z), zero);
    ASSERT_EQ(cpu->getFlag(Flag::N), negative);
    ASSERT_EQ(cpu->getFlag(Flag::V), overflow);
}

// Comprehensive test data covering all flag combinations
std::vector<ADCTestCase> generateTestCases() {
    std::vector<ADCTestCase> cases;
    
    // Normal addition without flags
    cases.push_back(std::make_tuple(0x10, 0x20, false, 0x30, false, false, false, false));
    
    // Test with carry in
    cases.push_back(std::make_tuple(0x10, 0x20, true, 0x31, false, false, false, false));
    
    // Carry out
    cases.push_back(std::make_tuple(0xF0, 0x20, false, 0x10, true, false, false, false));
    cases.push_back(std::make_tuple(0xFF, 0x01, false, 0x00, true, true, false, false));
    
    // Zero result
    cases.push_back(std::make_tuple(0x00, 0x00, false, 0x00, false, true, false, false));
    cases.push_back(std::make_tuple(0xFF, 0x01, true, 0x01, true, false, false, false));
    
    // Negative result
    cases.push_back(std::make_tuple(0x80, 0x01, false, 0x81, false, false, true, false));
    cases.push_back(std::make_tuple(0xC0, 0x20, false, 0xE0, false, false, true, false));
    
    // Overflow cases
    // Pos + Pos = Neg (overflow)
    cases.push_back(std::make_tuple(0x50, 0x50, false, 0xA0, false, false, true, true));
    // Neg + Neg = Pos (overflow)
    cases.push_back(std::make_tuple(0x90, 0x90, false, 0x20, true, false, false, true));
    
    // Overflow with carry in
    cases.push_back(std::make_tuple(0x50, 0x4F, true, 0xA0, false, false, true, true));
    
    // All flags set
    cases.push_back(std::make_tuple(0x7F, 0x01, false, 0x80, false, false, true, true));
    
    // Boundary values
    cases.push_back(std::make_tuple(0x01, 0xFF, false, 0x00, true, true, false, false));
    cases.push_back(std::make_tuple(0xFF, 0xFF, false, 0xFE, true, false, true, false));
    cases.push_back(std::make_tuple(0xFF, 0xFF, true, 0xFF, true, false, true, false));
    
    // Special case: 0x80 + 0x80 + 0 = 0x00 with carry and no overflow
    cases.push_back(std::make_tuple(0x80, 0x80, false, 0x00, true, true, false, true));
    
    return cases;
}

// Helper to set up memory for different addressing modes
void setupAddressing(AddressingMode mode, uint8_t operand) {
    setPC(0x200);
    
    switch (mode) {
        case AddressingMode::Immediate:
            mem->writeByte(0x200, operand);
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
            
        case AddressingMode::AbsoluteY:
            setY(0x02);
            mem->writeWord(0x200, 0x1234);
            mem->writeByte(0x1236, operand);
            break;
            
        case AddressingMode::IndirectX:
            setX(0x04);
            mem->writeByte(0x200, 0x20);
            mem->writeWord(0x24, 0x1234);
            mem->writeByte(0x1234, operand);
            break;
            
        case AddressingMode::IndirectY:
            setY(0x03);
            mem->writeByte(0x200, 0x20);
            mem->writeWord(0x20, 0x1234);
            mem->writeByte(0x1237, operand);
            break;
    }
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Immediate: return 2;
        case AddressingMode::ZeroPage: return 3;
        case AddressingMode::ZeroPageX: return 4;
        case AddressingMode::Absolute: return 4;
        case AddressingMode::AbsoluteX: return 4; // +1 if page boundary crossed
        case AddressingMode::AbsoluteY: return 4; // +1 if page boundary crossed
        case AddressingMode::IndirectX: return 6;
        case AddressingMode::IndirectY: return 5; // +1 if page boundary crossed
        default: return 2;
    }
}

TEST_P(ADCTest, ExhaustiveTest) {
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];
    
    auto [accumulator, operand, carry_in, expected_result, 
          expected_carry, expected_zero, expected_negative, expected_overflow] = testCase;
    
    setup();
    setA(accumulator);
    setC(carry_in);
    setupAddressing(mode, operand);
    
    std::unique_ptr<ADC> adc(new ADC(cpu, mode, getCycleCount(mode)));
    adc->run();
    
    std::string modeName;
    switch (mode) {
        case AddressingMode::Immediate: modeName = "Immediate"; break;
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::ZeroPageX: modeName = "ZeroPageX"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        case AddressingMode::AbsoluteX: modeName = "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: modeName = "AbsoluteY"; break;
        case AddressingMode::IndirectX: modeName = "IndirectX"; break;
        case AddressingMode::IndirectY: modeName = "IndirectY"; break;
        default: modeName = "Unknown";
    }
    
    SCOPED_TRACE("Mode: " + modeName + 
                 ", A: " + std::to_string(accumulator) + 
                 ", Op: " + std::to_string(operand) + 
                 ", C: " + std::to_string(carry_in));
    
    checkFlags(expected_result, expected_carry, expected_zero, expected_negative, expected_overflow);
}

// Generate combinations of addressing modes and test cases
std::vector<AddressingMode> addressingModes = {
    AddressingMode::Immediate,
    AddressingMode::ZeroPage,
    AddressingMode::ZeroPageX,
    AddressingMode::Absolute,
    AddressingMode::AbsoluteX,
    AddressingMode::AbsoluteY,
    AddressingMode::IndirectX,
    AddressingMode::IndirectY
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
    ADC,
    ADCTest,
    ::testing::ValuesIn(GenerateTestParams())
);

// Page crossing tests for modes that have different cycle counts
TEST(ADC, PageCrossing) {
    // Test AbsoluteX page crossing
    setup();
    setA(0x10);
    setX(0xFF);
    setPC(0x200);
    mem->writeWord(0x200, 0x1200);  // Base address: 0x1200 + 0xFF = 0x12FF (crosses to 0x1300)
    mem->writeByte(0x12FF, 0x20);
    
    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::AbsoluteX, 5)); // 5 cycles with page crossing
    adc->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x30);
    
    // Test AbsoluteY page crossing
    setup();
    setA(0x10);
    setY(0xFF);
    setPC(0x200);
    mem->writeWord(0x200, 0x1200);
    mem->writeByte(0x12FF, 0x20);
    
    std::unique_ptr<ADC> adc2(new ADC(cpu, AddressingMode::AbsoluteY, 5)); // 5 cycles with page crossing
    adc2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x30);
    
    // Test IndirectY page crossing
    setup();
    setA(0x10);
    setY(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);
    mem->writeWord(0x80, 0x1200);
    mem->writeByte(0x12FF, 0x20);
    
    std::unique_ptr<ADC> adc3(new ADC(cpu, AddressingMode::IndirectY, 6)); // 6 cycles with page crossing
    adc3->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x30);
}

// Test zero page wrap-around behavior
TEST(ADC, ZeroPageWrapAround) {
    setup();
    setA(0x10);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeByte(0x7F, 0x20);
    
    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::ZeroPageX, 4));
    adc->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x30);
}

// Special 6502 bug: JMP indirect has an issue with page boundaries
TEST(ADC, IndirectXWrapAround) {
    setup();
    setA(0x10);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F in zero page
    mem->writeWord(0x7F, 0x1234);  // Write target address at wrapped location
    mem->writeByte(0x1234, 0x20);  // Store operand at target address
    
    std::unique_ptr<ADC> adc(new ADC(cpu, AddressingMode::IndirectX, 6));
    adc->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x30);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}