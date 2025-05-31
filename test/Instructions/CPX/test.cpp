#include <gtest/gtest.h>
#include "CPX.h"
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

// Helper to verify CPX results
void verifyCPXResults(uint8_t X, uint8_t M) {
    uint8_t result = X - M;
    
    // Check carry flag (set if X >= M)
    ASSERT_EQ(cpu->getFlag(Flag::C), X >= M) << "Carry flag incorrect: X=" << (int)X << ", M=" << (int)M;
    
    // Check zero flag (set if X == M)
    ASSERT_EQ(cpu->getFlag(Flag::Z), X == M) << "Zero flag incorrect: X=" << (int)X << ", M=" << (int)M;
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (result & 0x80) != 0) << "Negative flag incorrect: X=" << (int)X << ", M=" << (int)M;
}

// Test cases for specific scenarios
class CPXSpecificTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test special cases for CPX operations
TEST_F(CPXSpecificTest, CompareWithEqualValues) {
    setup();
    
    // Set X to 0x42
    uint8_t value = 0x42;
    cpu->setRegister(Register::X, value);
    
    // Set memory at PC to same value
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, value);
    
    // Execute CPX
    std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
    cpx->run();
    
    // X == M: Should set Zero and Carry flags, clear Negative
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Equal values should set Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Equal values should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Equal values should clear Negative flag";
}

TEST_F(CPXSpecificTest, CompareXGreaterThanM) {
    setup();
    
    // X > M
    uint8_t x_value = 0xFF;
    uint8_t m_value = 0x01;
    
    cpu->setRegister(Register::X, x_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPX
    std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
    cpx->run();
    
    // X > M: Should set Carry, may set Negative, clear Zero
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "X > M should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "X > M should clear Zero flag";
    // X=0xFF, M=0x01, Result=0xFE (negative)
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "X=0xFF - M=0x01 should set Negative flag";
}

TEST_F(CPXSpecificTest, CompareXLessThanM) {
    setup();
    
    // X < M
    uint8_t x_value = 0x01;
    uint8_t m_value = 0x80;
    
    cpu->setRegister(Register::X, x_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPX
    std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
    cpx->run();
    
    // X < M: Should clear Carry and Zero, may set Negative
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "X < M should clear Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "X < M should clear Zero flag";
    // X=0x01, M=0x80, Result=0x81 (negative)
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "X=0x01 - M=0x80 should set Negative flag";
}

// Parametrized tests for different addressing modes
struct CPXCase {
    uint8_t x_value;
    uint8_t m_value;
    std::string description;
};

class CPXAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t>> {
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
                FAIL() << "Unsupported addressing mode for CPX";
        }
    }
};

// Test cases for different addressing modes
TEST_P(CPXAddressingModeTest, CompareAcrossAddressingModes) {
    auto [mode, value] = GetParam();
    
    // Set X register
    cpu->setRegister(Register::X, 0x42);
    
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
    
    // Execute CPX with the specified addressing mode
    std::unique_ptr<CPX> cpx(new CPX(cpu, mode, cycles));
    cpx->run();
    
    // Verify results
    verifyCPXResults(0x42, value);
}

// Test combinations of X and M values
class CPXValueTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(CPXValueTest, CompareVariousValues) {
    auto [x_value, m_value] = GetParam();
    
    // Set X register
    cpu->setRegister(Register::X, x_value);
    
    // Set memory at PC to the value to compare
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CPX with immediate addressing
    std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
    cpx->run();
    
    // Verify results
    verifyCPXResults(x_value, m_value);
}

// Special test case: verify CPX does not affect accumulator
TEST_F(CPXSpecificTest, DoesNotAffectAccumulator) {
    setup();
    
    // Set initial register values
    uint8_t a_value = 0x42;
    uint8_t x_value = 0x77;
    cpu->setRegister(Register::A, a_value);
    cpu->setRegister(Register::X, x_value);
    
    // Execute CPX
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, 0x33); // Some value to compare with
    
    std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
    cpx->run();
    
    // Verify A register is unchanged
    ASSERT_EQ(cpu->getRegister(Register::A), a_value) << "CPX should not affect A register";
    
    // Verify X register is unchanged
    ASSERT_EQ(cpu->getRegister(Register::X), x_value) << "CPX should not modify X register";
}

// Generate test cases for addressing modes
INSTANTIATE_TEST_SUITE_P(
    AddressingModes,
    CPXAddressingModeTest,
    ::testing::Combine(
        ::testing::Values(
            AddressingMode::Immediate,
            AddressingMode::ZeroPage,
            AddressingMode::Absolute
        ),
        ::testing::Values(0x00, 0x42, 0xFF) // Values to compare with
    )
);

// Generate test cases for different X and M combinations
INSTANTIATE_TEST_SUITE_P(
    ValueCombinations,
    CPXValueTest,
    ::testing::Combine(
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF), // X values
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF)  // M values
    )
);

// Test with randomized values (similar to original test)
TEST(CPXRandomTest, RandomizedValues) {
    for(int i = 0; i < 100; ++i) {
        setup();
        
        // Randomize registers
        uint8_t x_value = rand() & 0xFF;
        uint8_t m_value = rand() & 0xFF;
        
        cpu->setRegister(Register::X, x_value);
        uint16_t pc = cpu->getRegister(Register::PC);
        mem->writeByte(pc, m_value);
        
        // Execute CPX with immediate addressing
        std::unique_ptr<CPX> cpx(new CPX(cpu, AddressingMode::Immediate, 2));
        cpx->run();
        
        // Verify results
        verifyCPXResults(x_value, m_value);
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