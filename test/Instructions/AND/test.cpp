#include <gtest/gtest.h>
#include "AND.h"
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
void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }

// Test case parameters: A, operand, expected_result, expected_Z, expected_N
using ANDTestCase = std::tuple<uint8_t, uint8_t, uint8_t, bool, bool>;

class ANDTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>> {
protected:
    void SetUp() override {
        setup();
    }
};

void checkFlags(uint8_t result, bool zero, bool negative)
{
    ASSERT_EQ(cpu->getRegister(Register::A), result);
    ASSERT_EQ(cpu->getFlag(Flag::Z), zero);
    ASSERT_EQ(cpu->getFlag(Flag::N), negative);
}

// Generate comprehensive test cases covering all flag combinations
std::vector<ANDTestCase> generateTestCases() {
    std::vector<ANDTestCase> cases;
    
    // Basic AND operations
    cases.push_back(std::make_tuple(0xFF, 0xFF, 0xFF, false, true));  // All bits set
    cases.push_back(std::make_tuple(0x00, 0xFF, 0x00, true, false));  // Zero result
    cases.push_back(std::make_tuple(0xFF, 0x00, 0x00, true, false));  // Zero result
    cases.push_back(std::make_tuple(0x0F, 0xF0, 0x00, true, false));  // No overlapping bits
    
    // Testing specific bits
    cases.push_back(std::make_tuple(0x55, 0xAA, 0x00, true, false));  // Alternating bits
    cases.push_back(std::make_tuple(0x55, 0xFF, 0x55, false, false)); // Keep every other bit
    cases.push_back(std::make_tuple(0xAA, 0xFF, 0xAA, false, true));  // Keep every other bit, negative
    
    // Testing flags
    cases.push_back(std::make_tuple(0x80, 0x80, 0x80, false, true));  // Negative flag
    cases.push_back(std::make_tuple(0x80, 0x7F, 0x00, true, false));  // Zero flag
    
    // Edge cases
    cases.push_back(std::make_tuple(0x01, 0x01, 0x01, false, false)); // Smallest positive result
    cases.push_back(std::make_tuple(0x7F, 0xFF, 0x7F, false, false)); // Largest positive result
    cases.push_back(std::make_tuple(0x80, 0xFF, 0x80, false, true));  // Smallest negative result
    cases.push_back(std::make_tuple(0xFF, 0xFF, 0xFF, false, true));  // All bits set
    
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

TEST_P(ANDTest, ExhaustiveTest) {
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];
    
    auto [accumulator, operand, expected_result, expected_zero, expected_negative] = testCase;
    
    setup();
    setA(accumulator);
    setupAddressing(mode, operand);
    
    std::unique_ptr<AND> and_inst(new AND(cpu, mode, getCycleCount(mode)));
    and_inst->run();
    
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
                 ", Op: " + std::to_string(operand));
    
    checkFlags(expected_result, expected_zero, expected_negative);
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
    AND,
    ANDTest,
    ::testing::ValuesIn(GenerateTestParams())
);

// Test zero page wrap-around behavior
TEST(AND, ZeroPageWrapAround) {
    setup();
    setA(0xFF);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeByte(0x7F, 0xF0);
    
    std::unique_ptr<AND> and_inst(new AND(cpu, AddressingMode::ZeroPageX, 4));
    and_inst->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0xF0);
}

// Test page crossing with additional cycle
TEST(AND, PageCrossing) {
    // Test AbsoluteX page crossing
    setup();
    setA(0xFF);
    setX(0xFF);
    setPC(0x200);
    mem->writeWord(0x200, 0x1200);  // Base address: 0x1200 + 0xFF = 0x12FF (crosses to 0x1300)
    mem->writeByte(0x12FF, 0xF0);
    
    std::unique_ptr<AND> and_inst(new AND(cpu, AddressingMode::AbsoluteX, 5)); // 5 cycles with page crossing
    and_inst->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0xF0);
}

// Test IndirectX zero-page wrap
TEST(AND, IndirectXZeroPageWrap) {
    setup();
    setA(0xFF);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F in zero page
    mem->writeWord(0x7F, 0x1234);  // Write target address at wrapped location
    mem->writeByte(0x1234, 0xF0);  // Store operand at target address
    
    std::unique_ptr<AND> and_inst(new AND(cpu, AddressingMode::IndirectX, 6));
    and_inst->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0xF0);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}