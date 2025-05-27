#include <gtest/gtest.h>
#include "CMP.h"
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

// Helper to verify CMP results
void verifyCMPResults(uint8_t A, uint8_t M) {
    uint8_t result = A - M;
    
    // Check carry flag (set if A >= M)
    ASSERT_EQ(cpu->getFlag(Flag::C), A >= M) << "Carry flag incorrect: A=" << (int)A << ", M=" << (int)M;
    
    // Check zero flag (set if A == M)
    ASSERT_EQ(cpu->getFlag(Flag::Z), A == M) << "Zero flag incorrect: A=" << (int)A << ", M=" << (int)M;
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (result & 0x80) != 0) << "Negative flag incorrect: A=" << (int)A << ", M=" << (int)M;
}

// Test cases for specific scenarios
class CMPSpecificTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test special cases for CMP operations
TEST_F(CMPSpecificTest, CompareWithEqualValues) {
    setup();
    
    // Set A to 0x42
    uint8_t value = 0x42;
    cpu->setRegister(Register::A, value);
    
    // Set memory at PC to same value
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, value);
    
    // Execute CMP
    std::unique_ptr<CMP> cmp(new CMP(cpu, AddressingMode::Immediate, 2));
    cmp->run();
    
    // A == M: Should set Zero and Carry flags, clear Negative
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Equal values should set Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Equal values should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Equal values should clear Negative flag";
}

TEST_F(CMPSpecificTest, CompareAGreaterThanM) {
    setup();
    
    // A > M
    uint8_t a_value = 0x80;
    uint8_t m_value = 0x01;
    
    cpu->setRegister(Register::A, a_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CMP
    std::unique_ptr<CMP> cmp(new CMP(cpu, AddressingMode::Immediate, 2));
    cmp->run();
    
    // A > M: Should set Carry, may set Negative, clear Zero
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "A > M should set Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "A > M should clear Zero flag";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "A=0x80 - M=0x01 should set Negative flag";
}

TEST_F(CMPSpecificTest, CompareALessThanM) {
    setup();
    
    // A < M
    uint8_t a_value = 0x01;
    uint8_t m_value = 0x80;
    
    cpu->setRegister(Register::A, a_value);
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CMP
    std::unique_ptr<CMP> cmp(new CMP(cpu, AddressingMode::Immediate, 2));
    cmp->run();
    
    // A < M: Should clear Carry and Zero, may set Negative
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "A < M should clear Carry flag";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "A < M should clear Zero flag";
    // A=0x01, M=0x80, Result=0x81 (negative)
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "A=0x01 - M=0x80 should set Negative flag";
}

// Parametrized tests for different addressing modes
struct CMPCase {
    uint8_t a_value;
    uint8_t m_value;
    std::string description;
};

class CMPAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t>> {
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
                
            case AddressingMode::ZeroPageX:
                cpu->setRegister(Register::X, 0x10);
                mem->writeByte(pc, 0x42);         // Zero page base address
                mem->writeByte(0x42 + 0x10, value); // Value at (base + X)
                break;
                
            case AddressingMode::Absolute:
                mem->writeWord(pc, 0x1234);    // Absolute address
                mem->writeByte(0x1234, value); // Value at absolute address
                break;
                
            case AddressingMode::AbsoluteX:
                cpu->setRegister(Register::X, 0x10);
                mem->writeWord(pc, 0x1234);         // Absolute base address
                mem->writeByte(0x1234 + 0x10, value); // Value at (base + X)
                break;
                
            case AddressingMode::AbsoluteY:
                cpu->setRegister(Register::Y, 0x10);
                mem->writeWord(pc, 0x1234);         // Absolute base address
                mem->writeByte(0x1234 + 0x10, value); // Value at (base + Y)
                break;
                
            case AddressingMode::IndirectX:
                cpu->setRegister(Register::X, 0x10);
                mem->writeByte(pc, 0x42);           // Zero page base address
                mem->writeWord(0x42 + 0x10, 0x5678); // Address at (base + X)
                mem->writeByte(0x5678, value);      // Value at indirect address
                break;
                
            case AddressingMode::IndirectY:
                cpu->setRegister(Register::Y, 0x10);
                mem->writeByte(pc, 0x42);           // Zero page address
                mem->writeWord(0x42, 0x5678);       // Base address at zero page
                mem->writeByte(0x5678 + 0x10, value); // Value at (indirect + Y)
                break;
                
            default:
                FAIL() << "Unsupported addressing mode";
        }
    }
};

// Test cases for different addressing modes
TEST_P(CMPAddressingModeTest, CompareAcrossAddressingModes) {
    auto [mode, value] = GetParam();
    
    // Set accumulator
    cpu->setRegister(Register::A, 0x42);
    
    // Set up memory for this addressing mode
    setupAddressingMode(mode, value);
    
    // Get cycles based on addressing mode
    uint8_t cycles;
    switch(mode) {
        case AddressingMode::Immediate: cycles = 2; break;
        case AddressingMode::ZeroPage:  cycles = 3; break;
        case AddressingMode::ZeroPageX: cycles = 4; break;
        case AddressingMode::Absolute:  cycles = 4; break;
        case AddressingMode::AbsoluteX: cycles = 4; break;
        case AddressingMode::AbsoluteY: cycles = 4; break;
        case AddressingMode::IndirectX: cycles = 6; break;
        case AddressingMode::IndirectY: cycles = 5; break;
        default: FAIL() << "Unsupported addressing mode"; cycles = 0;
    }
    
    // Execute CMP with the specified addressing mode
    std::unique_ptr<CMP> cmp(new CMP(cpu, mode, cycles));
    cmp->run();
    
    // Verify results
    verifyCMPResults(0x42, value);
}

// Test combinations of A and M values
class CMPValueTest : public ::testing::TestWithParam<std::tuple<uint8_t, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(CMPValueTest, CompareVariousValues) {
    auto [a_value, m_value] = GetParam();
    
    // Set accumulator
    cpu->setRegister(Register::A, a_value);
    
    // Set memory at PC to the value to compare
    uint16_t pc = cpu->getRegister(Register::PC);
    mem->writeByte(pc, m_value);
    
    // Execute CMP with immediate addressing
    std::unique_ptr<CMP> cmp(new CMP(cpu, AddressingMode::Immediate, 2));
    cmp->run();
    
    // Verify results
    verifyCMPResults(a_value, m_value);
}

// Generate test cases for addressing modes
INSTANTIATE_TEST_SUITE_P(
    AddressingModes,
    CMPAddressingModeTest,
    ::testing::Combine(
        ::testing::Values(
            AddressingMode::Immediate,
            AddressingMode::ZeroPage,
            AddressingMode::ZeroPageX,
            AddressingMode::Absolute,
            AddressingMode::AbsoluteX,
            AddressingMode::AbsoluteY,
            AddressingMode::IndirectX,
            AddressingMode::IndirectY
        ),
        ::testing::Values(0x00, 0x42, 0xFF) // Values to compare with
    )
);

// Generate test cases for different A and M combinations
INSTANTIATE_TEST_SUITE_P(
    ValueCombinations,
    CMPValueTest,
    ::testing::Combine(
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF), // A values
        ::testing::Values(0x00, 0x01, 0x7F, 0x80, 0xFF)  // M values
    )
);

// Test with randomized values
TEST(CMPRandomTest, RandomizedValues) {
    for(int i = 0; i < 100; ++i) {
        setup();
        
        // Randomize registers
        uint8_t a_value = rand() & 0xFF;
        uint8_t m_value = rand() & 0xFF;
        
        cpu->setRegister(Register::A, a_value);
        uint16_t pc = cpu->getRegister(Register::PC);
        mem->writeByte(pc, m_value);
        
        // Execute CMP with immediate addressing
        std::unique_ptr<CMP> cmp(new CMP(cpu, AddressingMode::Immediate, 2));
        cmp->run();
        
        // Verify results
        verifyCMPResults(a_value, m_value);
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