#include <gtest/gtest.h>
#include "ASL.h"
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

// Test case parameters: value, expected_result, expected_C, expected_Z, expected_N
using ASLTestCase = std::tuple<uint8_t, uint8_t, bool, bool, bool>;

class ASLTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>> {
protected:
    void SetUp() override {
        setup();
    }
};

void checkFlags(uint8_t result, uint8_t expected, bool carry, bool zero, bool negative)
{
    ASSERT_EQ(result, expected);
    ASSERT_EQ(cpu->getFlag(Flag::C), carry);
    ASSERT_EQ(cpu->getFlag(Flag::Z), zero);
    ASSERT_EQ(cpu->getFlag(Flag::N), negative);
}

// Generate comprehensive test cases
std::vector<ASLTestCase> generateTestCases() {
    std::vector<ASLTestCase> cases;
    
    // Basic shifts
    cases.push_back(std::make_tuple(0x01, 0x02, false, false, false)); // Simple shift
    cases.push_back(std::make_tuple(0x40, 0x80, false, false, true));  // Shift resulting in negative
    cases.push_back(std::make_tuple(0x80, 0x00, true, true, false));   // Shift with carry out and zero result
    cases.push_back(std::make_tuple(0x00, 0x00, false, true, false));  // Shift zero stays zero
    
    // Carry flag tests
    cases.push_back(std::make_tuple(0x81, 0x02, true, false, false));  // Carry out with bit 7 set
    cases.push_back(std::make_tuple(0xAA, 0x54, true, false, false));  // Carry from patterns
    cases.push_back(std::make_tuple(0x7F, 0xFE, false, false, true));  // Shift with negative result
    
    // Flag combinations
    cases.push_back(std::make_tuple(0xFF, 0xFE, true, false, true));   // Carry and negative
    cases.push_back(std::make_tuple(0xC0, 0x80, true, false, true));   // Carry and negative
    cases.push_back(std::make_tuple(0x40, 0x80, false, false, true));  // Just negative
    
    return cases;
}

// Helper to set up memory for different addressing modes
void setupAddressing(AddressingMode mode, uint8_t value) {
    setPC(0x200);
    
    switch (mode) {
        case AddressingMode::Accumulator:
        case AddressingMode::Implied:
            setA(value);
            break;
            
        case AddressingMode::ZeroPage:
            mem->writeByte(0x200, 0x42);
            mem->writeByte(0x42, value);
            break;
            
        case AddressingMode::ZeroPageX:
            setX(0x05);
            mem->writeByte(0x200, 0x40);
            mem->writeByte(0x45, value);
            break;
            
        case AddressingMode::Absolute:
            mem->writeWord(0x200, 0x1234);
            mem->writeByte(0x1234, value);
            break;
            
        case AddressingMode::AbsoluteX:
            setX(0x01);
            mem->writeWord(0x200, 0x1234);
            mem->writeByte(0x1235, value);
            break;
    }
}

// Get result based on addressing mode
uint8_t getResult(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Accumulator:
        case AddressingMode::Implied:
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

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::Accumulator:
        case AddressingMode::Implied: return 2;
        case AddressingMode::ZeroPage: return 5;
        case AddressingMode::ZeroPageX: return 6;
        case AddressingMode::Absolute: return 6;
        case AddressingMode::AbsoluteX: return 7;
        default: return 2;
    }
}

TEST_P(ASLTest, ExhaustiveTest) {
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];
    
    auto [value, expected_result, expected_carry, expected_zero, expected_negative] = testCase;
    
    setup();
    setupAddressing(mode, value);
    
    std::unique_ptr<ASL> asl(new ASL(cpu, mode, getCycleCount(mode)));
    asl->run();
    
    std::string modeName;
    switch (mode) {
        case AddressingMode::Accumulator:
        case AddressingMode::Implied: modeName = "Accumulator"; break;
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::ZeroPageX: modeName = "ZeroPageX"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        case AddressingMode::AbsoluteX: modeName = "AbsoluteX"; break;
        default: modeName = "Unknown";
    }
    
    SCOPED_TRACE("Mode: " + modeName + 
                 ", Value: " + std::to_string(value) + 
                 ", Expected: " + std::to_string(expected_result));
    
    uint8_t result = getResult(mode);
    checkFlags(result, expected_result, expected_carry, expected_zero, expected_negative);
}

// Generate combinations of addressing modes and test cases
std::vector<AddressingMode> addressingModes = {
    AddressingMode::Accumulator, // ASL uses Accumulator mode, not Implied
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
    ASL,
    ASLTest,
    ::testing::ValuesIn(GenerateTestParams())
);

// Test zero page wrap-around behavior
TEST(ASL, ZeroPageWrapAround) {
    setup();
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeByte(0x7F, 0x40);
    
    std::unique_ptr<ASL> asl(new ASL(cpu, AddressingMode::ZeroPageX, 6));
    asl->run();
    
    ASSERT_EQ(mem->readByte(0x7F), 0x80);
    ASSERT_EQ(cpu->getFlag(Flag::N), true);
    ASSERT_EQ(cpu->getFlag(Flag::Z), false);
    ASSERT_EQ(cpu->getFlag(Flag::C), false);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}