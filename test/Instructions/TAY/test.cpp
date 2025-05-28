#include <gtest/gtest.h>
#include "TAY.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <string>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

class TAYTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test environment
        mem->initialize();
        mem->randomize();
        cpu->reset();
        
        CPU *cppu = static_cast<CPU*>(cpu.get());
        cppu->init();
        cppu->connectBus(bus);
        bus->connectMemory(mem);
    }
    
    // Helper methods
    void setA(uint8_t value) { cpu->setRegister(Register::A, value); }
    void setX(uint8_t value) { cpu->setRegister(Register::X, value); }
    void setY(uint8_t value) { cpu->setRegister(Register::Y, value); }
    void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
    
    void setFlag(Flag flag, bool value) { cpu->setFlag(flag, value); }
    
    // Capture flags before operation
    struct FlagState {
        bool C, Z, I, D, B, V, N;
        
        void captureFrom(std::shared_ptr<ICPU> cpu) {
            C = cpu->getFlag(Flag::C);
            Z = cpu->getFlag(Flag::Z);
            I = cpu->getFlag(Flag::I);
            D = cpu->getFlag(Flag::D);
            B = cpu->getFlag(Flag::B);
            V = cpu->getFlag(Flag::V);
            N = cpu->getFlag(Flag::N);
        }
        
        void verifyPreservedExcept(std::shared_ptr<ICPU> cpu, bool expectZ, bool expectN) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::Z), expectZ) << "Zero flag incorrect state";
            ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::N), expectN) << "Negative flag incorrect state";
        }
    };
    
    // Common test values covering important cases
    std::vector<uint8_t> testValues = {
        0x00,       // Zero - should set Zero flag
        0x01,       // Smallest positive
        0x7F,       // Largest positive (7-bit)
        0x80,       // Smallest negative (should set Negative flag)
        0xFF,       // Largest value/negative (should set Negative flag)
        0x55,       // 01010101
        0xAA        // 10101010 (should set Negative flag)
    };
};

// Test basic functionality with specific values
TEST_F(TAYTest, BasicFunctionality) {
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("A = 0x" + std::string(testValue < 16 ? "0" : "") + 
                     std::to_string(testValue));
        
        // Setup initial state - A has test value, Y is different
        setA(testValue);
        setY(~testValue);  // Different value to ensure it changes
        
        // Capture initial flags
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Execute TAY
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        // Verify Y now equals A
        ASSERT_EQ(cpu->getRegister(Register::Y), testValue) << "Y should equal A after TAY";
        
        // Calculate expected flags
        bool expectZ = (testValue == 0);
        bool expectN = (testValue & 0x80) != 0;
        
        // Verify flags
        initialFlags.verifyPreservedExcept(cpu, expectZ, expectN);
    }
}

// Test that X register remains unchanged
TEST_F(TAYTest, PreserveXRegister) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        
        // Setup with random values
        SetUp();
        uint8_t a = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        
        setA(a);
        setX(x);
        
        // Run TAY
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        // Check X is unchanged
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "X register should not be affected by TAY";
    }
}

// Test Zero flag behavior
TEST_F(TAYTest, ZeroFlagBehavior) {
    // Test when A is zero
    {
        SCOPED_TRACE("A = 0x00");
        SetUp();
        
        setA(0x00);
        setY(0x01);  // Non-zero
        setFlag(Flag::Z, false);  // Clear zero flag initially
        
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when A=0";
    }
    
    // Test when A is non-zero
    {
        SCOPED_TRACE("A = 0x01");
        SetUp();
        
        setA(0x01);
        setY(0x00);  // Zero
        setFlag(Flag::Z, true);  // Set zero flag initially
        
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be cleared when A≠0";
    }
}

// Test Negative flag behavior
TEST_F(TAYTest, NegativeFlagBehavior) {
    // Test when A has bit 7 set (negative)
    {
        SCOPED_TRACE("A = 0x80");
        SetUp();
        
        setA(0x80);
        setY(0x01);  // Positive
        setFlag(Flag::N, false);  // Clear negative flag initially
        
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when A has bit 7 set";
    }
    
    // Test when A has bit 7 clear (positive)
    {
        SCOPED_TRACE("A = 0x7F");
        SetUp();
        
        setA(0x7F);
        setY(0x80);  // Negative
        setFlag(Flag::N, true);  // Set negative flag initially
        
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be cleared when A has bit 7 clear";
    }
}

// Test that TAY preserves other flags (C, I, D, B, V)
TEST_F(TAYTest, PreserveOtherFlags) {
    std::vector<Flag> flagsToTest = {
        Flag::C, Flag::I, Flag::D, Flag::B, Flag::V
    };
    
    for (Flag flag : flagsToTest) {
        // Test with flag initially set
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially set");
            SetUp();
            
            setA(0x42);
            setFlag(flag, true);
            
            std::unique_ptr<TAY> tay(new TAY(cpu));
            tay->run();
            
            ASSERT_TRUE(cpu->getFlag(flag)) << "Flag should remain set after TAY";
        }
        
        // Test with flag initially clear
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially clear");
            SetUp();
            
            setA(0x42);
            setFlag(flag, false);
            
            std::unique_ptr<TAY> tay(new TAY(cpu));
            tay->run();
            
            ASSERT_FALSE(cpu->getFlag(flag)) << "Flag should remain clear after TAY";
        }
    }
}



// Test randomized behavior (comprehensive coverage)
TEST_F(TAYTest, RandomizedBehavior) {
    const int ITERATIONS = 1000; // Reduced from 10000 for faster tests
    
    for (int i = 0; i < ITERATIONS; i++) {
        SetUp();
        
        // Randomize registers and flags
        uint8_t a = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        setA(a);
        setX(x);
        setY(y);
        
        // Randomize flags
        bool initialC = rand() & 1;
        bool initialI = rand() & 1;
        bool initialD = rand() & 1;
        bool initialB = rand() & 1;
        bool initialV = rand() & 1;
        
        setFlag(Flag::C, initialC);
        setFlag(Flag::I, initialI);
        setFlag(Flag::D, initialD);
        setFlag(Flag::B, initialB);
        setFlag(Flag::V, initialV);
        
        // Run TAY
        std::unique_ptr<TAY> tay(new TAY(cpu));
        tay->run();
        
        // Verify Y equals A
        ASSERT_EQ(cpu->getRegister(Register::Y), a) << "Y should equal A after TAY";
        
        // Verify X is unchanged
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "X should be unchanged";
        
        // Calculate expected flags
        bool expectZ = (a == 0);
        bool expectN = (a & 0x80) != 0;
        
        // Verify all flags
        ASSERT_EQ(cpu->getFlag(Flag::C), initialC) << "Carry flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::Z), expectZ) << "Zero flag should reflect Y value";
        ASSERT_EQ(cpu->getFlag(Flag::I), initialI) << "Interrupt flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::D), initialD) << "Decimal flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::B), initialB) << "Break flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::V), initialV) << "Overflow flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::N), expectN) << "Negative flag should reflect Y value";
    }
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