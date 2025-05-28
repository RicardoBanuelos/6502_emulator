#include <gtest/gtest.h>
#include "TYA.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <string>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 1000; // Reduced from 10000 for faster tests

class TYATest : public ::testing::Test {
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
TEST_F(TYATest, BasicFunctionality) {
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("Y = 0x" + std::string(testValue < 16 ? "0" : "") + 
                     std::to_string(testValue));
        
        // Setup initial state - Y has test value, A is different
        setY(testValue);
        setA(~testValue);  // Different value to ensure it changes
        
        // Capture initial flags
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Execute TYA
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        // Verify A now equals Y
        ASSERT_EQ(cpu->getRegister(Register::A), testValue) << "A should equal Y after TYA";
        
        // Calculate expected flags
        bool expectZ = (testValue == 0);
        bool expectN = (testValue & 0x80) != 0;
        
        // Verify flags
        initialFlags.verifyPreservedExcept(cpu, expectZ, expectN);
    }
}

// Test that X register remains unchanged
TEST_F(TYATest, PreserveXRegister) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        
        // Setup with random values
        SetUp();
        uint8_t y = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        
        setY(y);
        setX(x);
        
        // Run TYA
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        // Check X is unchanged
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "X register should not be affected by TYA";
    }
}

// Test that Y register remains unchanged
TEST_F(TYATest, PreserveYRegister) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        
        // Setup with random values
        SetUp();
        uint8_t y = rand() & 0xFF;
        
        setY(y);
        
        // Run TYA
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        // Check Y is unchanged
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "Y register should not be modified by TYA";
    }
}

// Test Zero flag behavior
TEST_F(TYATest, ZeroFlagBehavior) {
    // Test when Y is zero
    {
        SCOPED_TRACE("Y = 0x00");
        SetUp();
        
        setY(0x00);
        setA(0x01);  // Non-zero
        setFlag(Flag::Z, false);  // Clear zero flag initially
        
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when Y=0";
    }
    
    // Test when Y is non-zero
    {
        SCOPED_TRACE("Y = 0x01");
        SetUp();
        
        setY(0x01);
        setA(0x00);  // Zero
        setFlag(Flag::Z, true);  // Set zero flag initially
        
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be cleared when Y≠0";
    }
}

// Test Negative flag behavior
TEST_F(TYATest, NegativeFlagBehavior) {
    // Test when Y has bit 7 set (negative)
    {
        SCOPED_TRACE("Y = 0x80");
        SetUp();
        
        setY(0x80);
        setA(0x01);  // Positive
        setFlag(Flag::N, false);  // Clear negative flag initially
        
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when Y has bit 7 set";
    }
    
    // Test when Y has bit 7 clear (positive)
    {
        SCOPED_TRACE("Y = 0x7F");
        SetUp();
        
        setY(0x7F);
        setA(0x80);  // Negative
        setFlag(Flag::N, true);  // Set negative flag initially
        
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be cleared when Y has bit 7 clear";
    }
}

// Test that TYA preserves other flags (C, I, D, B, V)
TEST_F(TYATest, PreserveOtherFlags) {
    std::vector<Flag> flagsToTest = {
        Flag::C, Flag::I, Flag::D, Flag::B, Flag::V
    };
    
    for (Flag flag : flagsToTest) {
        // Test with flag initially set
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially set");
            SetUp();
            
            setY(0x42);
            setFlag(flag, true);
            
            std::unique_ptr<TYA> tya(new TYA(cpu));
            tya->run();
            
            ASSERT_TRUE(cpu->getFlag(flag)) << "Flag should remain set after TYA";
        }
        
        // Test with flag initially clear
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially clear");
            SetUp();
            
            setY(0x42);
            setFlag(flag, false);
            
            std::unique_ptr<TYA> tya(new TYA(cpu));
            tya->run();
            
            ASSERT_FALSE(cpu->getFlag(flag)) << "Flag should remain clear after TYA";
        }
    }
}



// Test randomized behavior (comprehensive coverage)
TEST_F(TYATest, RandomizedBehavior) {
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        SetUp();
        
        // Randomize registers and flags
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        
        // Save initial state
        uint8_t initialY = cpu->getRegister(Register::Y);
        uint8_t initialX = cpu->getRegister(Register::X);
        
        // Capture initial flags
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Run TYA
        std::unique_ptr<TYA> tya(new TYA(cpu));
        tya->run();
        
        // Verify A equals Y
        ASSERT_EQ(cpu->getRegister(Register::A), initialY) << "A should equal Y after TYA";
        
        // Verify Y is unchanged
        ASSERT_EQ(cpu->getRegister(Register::Y), initialY) << "Y should be unchanged";
        
        // Verify X is unchanged
        ASSERT_EQ(cpu->getRegister(Register::X), initialX) << "X should be unchanged";
        
        // Calculate expected flags
        bool expectZ = (initialY == 0);
        bool expectN = (initialY & 0x80) != 0;
        
        // Verify all flags
        ASSERT_EQ(cpu->getFlag(Flag::C), initialFlags.C) << "Carry flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::Z), expectZ) << "Zero flag should reflect A value";
        ASSERT_EQ(cpu->getFlag(Flag::I), initialFlags.I) << "Interrupt flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::D), initialFlags.D) << "Decimal flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::B), initialFlags.B) << "Break flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::V), initialFlags.V) << "Overflow flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::N), expectN) << "Negative flag should reflect A value";
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