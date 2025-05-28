#include <gtest/gtest.h>
#include "INY.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <tuple>
#include <sstream>

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

// Test case parameters: Y, expected_result, expected_Z, expected_N
using INYTestCase = std::tuple<uint8_t, uint8_t, bool, bool>;

// Helper to verify INY results
void verifyINYResults(uint8_t originalValue) {
    uint8_t expectedValue = (originalValue + 1) & 0xFF;
    uint8_t actualValue = cpu->getRegister(Register::Y);
    
    // Check that Y was incremented correctly
    ASSERT_EQ(actualValue, expectedValue) << "Y register not incremented correctly:"
                                         << " original=0x" << std::hex << (int)originalValue
                                         << ", expected=0x" << (int)expectedValue
                                         << ", actual=0x" << (int)actualValue;
    
    // Check zero flag (set if result is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), expectedValue == 0) 
        << "Zero flag incorrect: result=0x" << std::hex << (int)expectedValue 
        << ", Z flag should be " << (expectedValue == 0 ? "set" : "clear");
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (expectedValue & 0x80) != 0) 
        << "Negative flag incorrect: result=0x" << std::hex << (int)expectedValue
        << ", N flag should be " << ((expectedValue & 0x80) != 0 ? "set" : "clear");
}

// Comprehensive test with all possible Y values
TEST(INYExhaustive, AllYValues) {
    for (int y = 0; y <= 0xFF; ++y) {
        setup();
        
        std::stringstream ss;
        ss << "Testing Y=0x" << std::hex << y;
        SCOPED_TRACE(ss.str());
        
        cpu->setRegister(Register::Y, y);
        uint8_t expectedResult = (y + 1) & 0xFF;
        bool expectedZ = (expectedResult == 0);
        bool expectedN = (expectedResult & 0x80) != 0;
        
        std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
        iny->run();
        
        ASSERT_EQ(cpu->getRegister(Register::Y), expectedResult);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expectedZ);
        ASSERT_EQ(cpu->getFlag(Flag::N), expectedN);
    }
}

// Specific edge case tests
class INYTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test incrementing to zero (255 → 0)
TEST_F(INYTest, IncrementToZero) {
    cpu->setRegister(Register::Y, 0xFF);
    
    std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
    iny->run();
    
    ASSERT_EQ(cpu->getRegister(Register::Y), 0x00);
    ASSERT_TRUE(cpu->getFlag(Flag::Z));
    ASSERT_FALSE(cpu->getFlag(Flag::N));
}

// Test incrementing to negative (127 → 128)
TEST_F(INYTest, IncrementToNegative) {
    cpu->setRegister(Register::Y, 0x7F);
    
    std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
    iny->run();
    
    ASSERT_EQ(cpu->getRegister(Register::Y), 0x80);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
}

// Test that INY doesn't affect other registers
TEST_F(INYTest, DoesNotAffectOtherRegisters) {
    // Set initial register values
    uint8_t a_value = 0x42;
    uint8_t x_value = 0x77;
    uint8_t y_value = 0x55;
    cpu->setRegister(Register::A, a_value);
    cpu->setRegister(Register::X, x_value);
    cpu->setRegister(Register::Y, y_value);
    
    // Execute INY
    std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
    iny->run();
    
    // Verify only Y changed, others didn't
    ASSERT_EQ(cpu->getRegister(Register::A), a_value);
    ASSERT_EQ(cpu->getRegister(Register::X), x_value);
    ASSERT_EQ(cpu->getRegister(Register::Y), y_value + 1);
}

// Test that INY only affects Y, Z, and N flags
TEST_F(INYTest, OnlyAffectsZNFlags) {
    // Set all flags beforehand
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false);  // Will change
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false);  // Will change
    
    // Set Y to 0x7F, which will increment to 0x80 (negative)
    cpu->setRegister(Register::Y, 0x7F);
    
    // Execute INY
    std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
    iny->run();
    
    // Check Z and N flags were affected
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
    
    // Check other flags weren't affected
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
}

// Test with specific values
class INYValueTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t, bool, bool>> {
protected:
    void SetUp() override {
        setup();
    }
};

// Test with specific values in a parameterized test
TEST_P(INYValueTest, TestSpecificValues) {
    auto [initialY, expectedY, expectedZ, expectedN] = GetParam();
    
    // Set Y register
    cpu->setRegister(Register::Y, initialY);
    
    // Execute INY
    std::unique_ptr<INY> iny(new INY(cpu, AddressingMode::Implied, 2));
    iny->run();
    
    // Verify results
    std::stringstream ss;
    ss << "Y=0x" << std::hex << (int)initialY << " → 0x" << (int)expectedY;
    SCOPED_TRACE(ss.str());
    
    ASSERT_EQ(cpu->getRegister(Register::Y), expectedY);
    ASSERT_EQ(cpu->getFlag(Flag::Z), expectedZ);
    ASSERT_EQ(cpu->getFlag(Flag::N), expectedN);
}

// Specific test cases
INSTANTIATE_TEST_SUITE_P(
    SpecificCases,
    INYValueTest,
    ::testing::Values(
        // Y, expected Y, expected Z flag, expected N flag
        std::make_tuple(0x00, 0x01, false, false),  // 0 → 1
        std::make_tuple(0xFF, 0x00, true, false),   // 255 → 0 (overflow, Z=1)
        std::make_tuple(0x7F, 0x80, false, true),   // 127 → 128 (sign change, N=1)
        std::make_tuple(0xFE, 0xFF, false, true),   // 254 → 255 (N=1)
        std::make_tuple(0x80, 0x81, false, true),   // 128 → 129 (staying negative)
        std::make_tuple(0xAA, 0xAB, false, true),   // Typical negative number
        std::make_tuple(0x55, 0x56, false, false)   // Typical positive number
    )
);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();

    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();

    cppu->connectBus(bus);
    bus->connectMemory(mem);
   
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}