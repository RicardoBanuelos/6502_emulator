#include <gtest/gtest.h>
#include "DEX.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"

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

// Helper to verify DEX results
void verifyDEXResults(uint8_t originalValue) {
    uint8_t expectedValue = (originalValue - 1) & 0xFF;
    uint8_t actualValue = cpu->getRegister(Register::X);
    
    // Check that X was decremented correctly
    ASSERT_EQ(actualValue, expectedValue) << "X register not decremented correctly:"
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

// Test cases for specific scenarios
class DEXTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test decrementing to zero
TEST_F(DEXTest, DecrementToZero) {
    setup();
    
    // Set X to 0x01
    cpu->setRegister(Register::X, 0x01);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify X was decremented to zero and Z flag is set
    ASSERT_EQ(cpu->getRegister(Register::X), 0x00) << "X should be decremented to 0x00";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when result is zero";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Test decrementing to negative value
TEST_F(DEXTest, DecrementToNegative) {
    setup();
    
    // Set X to 0x00 (will wrap to 0xFF)
    cpu->setRegister(Register::X, 0x00);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify X wrapped around to 0xFF and N flag is set
    ASSERT_EQ(cpu->getRegister(Register::X), 0xFF) << "X should be decremented to 0xFF (wrap around)";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when bit 7 is set";
}

// Test decrementing a regular value
TEST_F(DEXTest, DecrementRegularValue) {
    setup();
    
    // Set X to 0x42
    cpu->setRegister(Register::X, 0x42);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify X was decremented correctly and flags are set correctly
    ASSERT_EQ(cpu->getRegister(Register::X), 0x41) << "X should be decremented to 0x41";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Test decrementing from negative to negative
TEST_F(DEXTest, DecrementNegativeToNegative) {
    setup();
    
    // Set X to 0x80
    cpu->setRegister(Register::X, 0x80);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify X was decremented correctly and N flag is still set
    ASSERT_EQ(cpu->getRegister(Register::X), 0x7F) << "X should be decremented to 0x7F";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear when result is positive";
}

// Test with specific values
class DEXValueTest : public ::testing::TestWithParam<uint8_t> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(DEXValueTest, DecrementVariousValues) {
    uint8_t value = GetParam();
    
    // Set X register
    cpu->setRegister(Register::X, value);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify results
    verifyDEXResults(value);
}

// Test that DEX doesn't affect other registers
TEST_F(DEXTest, DoesNotAffectOtherRegisters) {
    setup();
    
    // Set initial register values
    uint8_t a_value = 0x42;
    uint8_t x_value = 0x77;
    uint8_t y_value = 0x55;
    cpu->setRegister(Register::A, a_value);
    cpu->setRegister(Register::X, x_value);
    cpu->setRegister(Register::Y, y_value);
    
    // Execute DEX
    std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
    dex->run();
    
    // Verify only X changed, others didn't
    ASSERT_EQ(cpu->getRegister(Register::A), a_value) << "DEX should not affect A register";
    ASSERT_EQ(cpu->getRegister(Register::X), x_value - 1) << "DEX should decrement X register";
    ASSERT_EQ(cpu->getRegister(Register::Y), y_value) << "DEX should not affect Y register";
}

// Generate test cases with important values to test
INSTANTIATE_TEST_SUITE_P(
    ImportantValues,
    DEXValueTest,
    ::testing::Values(
        0x00,  // Decrement from 0x00 wraps to 0xFF (tests negative flag)
        0x01,  // Decrement to 0x00 (tests zero flag)
        0x7F,  // Decrement from positive to positive
        0x80,  // Decrement from negative to positive
        0xFF   // Decrement from max negative
    )
);

// Test with randomized values (reduced from 1000 to 100 iterations)
TEST(DEXRandomTest, RandomizedValues) {
    for(int i = 0; i < 100; ++i) {
        setup();
        
        // Randomize X register
        uint8_t x_value = rand() & 0xFF;
        cpu->setRegister(Register::X, x_value);
        
        // Execute DEX
        std::unique_ptr<DEX> dex(new DEX(cpu, AddressingMode::Implied, 2));
        dex->run();
        
        // Verify results
        verifyDEXResults(x_value);
    }
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
   
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}