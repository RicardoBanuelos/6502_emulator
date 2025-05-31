#include <gtest/gtest.h>
#include "CPY.h"
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

// Helper to verify CPY results
void verifyCPYResults(uint8_t Y, uint8_t M) {
    uint8_t result = Y - M;
    
    // Check carry flag (set if Y >= M)
    ASSERT_EQ(cpu->getFlag(Flag::C), Y >= M) << "Carry flag incorrect: Y=" << (int)Y << ", M=" << (int)M;
    
    // Check zero flag (set if Y == M)
    ASSERT_EQ(cpu->getFlag(Flag::Z), Y == M) << "Zero flag incorrect: Y=" << (int)Y << ", M=" << (int)M;
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (result & 0x80) != 0) << "Negative flag incorrect: Y=" << (int)Y << ", M=" << (int)M;
}

// Test cases for specific scenarios
class CPYSpecificTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test special cases for CPY operations
TEST_F(CPYSpecificTest, CompareWithEqualValues) {
    setup();
    
    // Set Y to 0x42
    uint8_t value = 0x42;
    cpu->setRegister(Register::Y, value);
    
    // Set memory at PC to same value
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, value);
    
    // Execute CPY
    std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
    cpy->run();
    
    // Y == M: Should set Zero and Carry flags, clear Negative
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Equal values should set Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Equal values should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Equal values should clear Negative flag";
}

TEST_F(CPYSpecificTest, CompareYGreaterThanM) {
    setup();
    
    // Y > M
    uint8_t y_value = 0xFF;
    uint8_t m_value = 0x01;
    
    cpu->setRegister(Register::Y, y_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPY
    std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
    cpy->run();
    
    // Y > M: Should set Carry, may set Negative, clear Zero
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Y > M should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Y > M should clear Zero flag";
    // Y=0xFF, M=0x01, Result=0xFE (negative)
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Y=0xFF - M=0x01 should set Negative flag";
}

TEST_F(CPYSpecificTest, CompareYLessThanM) {
    setup();
    
    // Y < M
    uint8_t y_value = 0x01;
    uint8_t m_value = 0x80;
    
    cpu->setRegister(Register::Y, y_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPY
    std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
    cpy->run();
    
    // Y < M: Should clear Carry and Zero, may set Negative
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "Y < M should clear Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Y < M should clear Zero flag";
    // Y=0x01, M=0x80, Result=0x81 (negative)
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Y=0x01 - M=0x80 should set Negative flag";
}

// Parametrized tests for different addressing modes
class CPYAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
    
    // Helper to setup memory for different addressing modes
    void setupAddressingMode(AddressingMode mode, uint8_t value) {
        uint16_t pc = cpu->getRegister(Register::PC);
        
        switch(mode) {
            case AddressingMode::Immediate:
                mem->writeByte(pc, value);
                break;
                
            case AddressingMode::ZeroPage:
                mem->writeByte(pc, 0x42);    // Zero page address
                mem->writeByte(0x42, value); // Value at zero page address
                break;
                
            case AddressingMode::Absolute:
                mem->writeWord(pc, 0x1234);    // Absolute address
                mem->writeByte(0x1234, value); // Value at absolute address
                break;
                
            default:
                FAIL() << "Unsupported addressing mode for CPY";
        }
    }
};

// Test cases for different addressing modes
TEST_P(CPYAddressingModeTest, CompareAcrossAddressingModes) {
    auto [mode, value] = GetParam();
    
    // Set Y register
    cpu->setRegister(Register::Y, 0x42);
    
    // Set up memory for this addressing mode
    setupAddressingMode(mode, value);
    
    // Get cycles based on addressing mode
    uint8_t cycles;
    switch(mode) {
        case AddressingMode::Immediate: cycles = 2; break;
        case AddressingMode::ZeroPage:  cycles = 3; break;
        case AddressingMode::Absolute:  cycles = 4; break;
        default: FAIL() << "Unsupported addressing mode"; cycles = 0;
    }
    
    // Execute CPY with the specified addressing mode
    std::unique_ptr<CPY> cpy(new CPY(cpu, mode, cycles));
    cpy->run();
    
    // Verify results
    verifyCPYResults(0x42, value);
}

// Test combinations of Y and M values
class CPYValueTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(CPYValueTest, CompareVariousValues) {
    auto [y_value, m_value] = GetParam();
    
    // Set Y register
    cpu->setRegister(Register::Y, y_value);
    
    // Set memory at PC to the value to compare
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPY with immediate addressing
    std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
    cpy->run();
    
    // Verify results
    verifyCPYResults(y_value, m_value);
}

// Special test case: verify CPY does not affect other registers
TEST_F(CPYSpecificTest, DoesNotAffectOtherRegisters) {
    setup();
    
    // Set initial register values
    uint8_t a_value = 0x42;
    uint8_t x_value = 0x77;
    uint8_t y_value = 0x55;
    cpu->setRegister(Register::A, a_value);
    cpu->setRegister(Register::X, x_value);
    cpu->setRegister(Register::Y, y_value);
    
    // Execute CPY
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, 0x33); // Some value to compare with
    
    std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
    cpy->run();
    
    // Verify registers are unchanged
    ASSERT_EQ(cpu->getRegister(Register::A), a_value) << "CPY should not affect A register";
    ASSERT_EQ(cpu->getRegister(Register::X), x_value) << "CPY should not affect X register";
    ASSERT_EQ(cpu->getRegister(Register::Y), y_value) << "CPY should not modify Y register";
}

// Generate test cases for addressing modes
INSTANTIATE_TEST_SUITE_P(
    AddressingModes,
    CPYAddressingModeTest,
    ::testing::Combine(
        ::testing::Values(
            AddressingMode::Immediate,
            AddressingMode::ZeroPage,
            AddressingMode::Absolute
        ),
        ::testing::Values(0x00, 0x42, 0xFF) // Values to compare with
    )
);

// Generate test cases for different Y and M combinations
INSTANTIATE_TEST_SUITE_P(
    ValueCombinations,
    CPYValueTest,
    ::testing::Combine(
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF), // Y values
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF)  // M values
    )
);

// Test with randomized values
TEST(CPYRandomTest, RandomizedValues) {
    for(int i = 0; i < 100; ++i) {
        setup();
        
        // Randomize registers
        uint8_t y_value = rand() & 0xFF;
        uint8_t m_value = rand() & 0xFF;
        
        cpu->setRegister(Register::Y, y_value);
        uint16_t pc = cpu->getRegister(Register::PC);
        mem->writeByte(pc, m_value);
        
        // Execute CPY with immediate addressing
        std::unique_ptr<CPY> cpy(new CPY(cpu, AddressingMode::Immediate, 2));
        cpy->run();
        
        // Verify results
        verifyCPYResults(y_value, m_value);
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