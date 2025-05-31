#include <gtest/gtest.h>
#include "TSX.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

class TSXTest : public ::testing::Test {
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
    void setSP(uint8_t value) { cpu->setRegister(Register::SP, value); }
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
    
    // Common test values covering important stack pointer cases
    std::vector<uint8_t> testValues = {
        0x00,       // Stack pointer at bottom of stack (unusual in real code)
        0x01,       // Near bottom of stack
        0x7F,       // Middle value
        0x80,       // Will set negative flag
        0xFD,       // Common initial value (stack at 0x01FD)
        0xFF,       // Top of stack (after reset)
    };
};

// Test basic functionality with specific values
TEST_F(TSXTest, BasicFunctionality) {
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("SP = 0x" + std::string(testValue < 16 ? "0" : "") + 
                     std::to_string(testValue));
        
        // Setup initial state - SP has test value, X is different
        setSP(testValue);
        setX(~testValue);  // Different value to ensure it changes
        
        // Capture initial flags
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Execute TSX
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        // Verify X now equals SP
        ASSERT_EQ(cpu->getRegister(Register::X), testValue) << "X should equal SP after TSX";
        
        // Calculate expected flags
        bool expectZ = (testValue == 0);
        bool expectN = (testValue & 0x80) != 0;
        
        // Verify flags
        initialFlags.verifyPreservedExcept(cpu, expectZ, expectN);
    }
}

// Test that other registers remain unchanged
TEST_F(TSXTest, PreserveOtherRegisters) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        
        // Setup with random values
        SetUp();
        uint8_t sp = rand() & 0xFF;
        uint8_t a = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        setSP(sp);
        setA(a);
        setY(y);
        
        // Run TSX
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        // Check other registers are unchanged
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "A register should not be affected by TSX";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "Y register should not be affected by TSX";
        ASSERT_EQ(cpu->getRegister(Register::SP), sp) << "SP should not be changed by TSX";
    }
}

// Test Zero flag behavior
TEST_F(TSXTest, ZeroFlagBehavior) {
    // Test when SP is zero
    {
        SCOPED_TRACE("SP = 0x00");
        SetUp();
        
        setSP(0x00);
        setX(0x01);  // Non-zero
        setFlag(Flag::Z, false);  // Clear zero flag initially
        
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when SP=0";
    }
    
    // Test when SP is non-zero
    {
        SCOPED_TRACE("SP = 0x01");
        SetUp();
        
        setSP(0x01);
        setX(0x00);  // Zero
        setFlag(Flag::Z, true);  // Set zero flag initially
        
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be cleared when SP≠0";
    }
}

// Test Negative flag behavior
TEST_F(TSXTest, NegativeFlagBehavior) {
    // Test when SP has bit 7 set (negative)
    {
        SCOPED_TRACE("SP = 0x80");
        SetUp();
        
        setSP(0x80);
        setX(0x01);  // Positive
        setFlag(Flag::N, false);  // Clear negative flag initially
        
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when SP has bit 7 set";
    }
    
    // Test when SP has bit 7 clear (positive)
    {
        SCOPED_TRACE("SP = 0x7F");
        SetUp();
        
        setSP(0x7F);
        setX(0x80);  // Negative
        setFlag(Flag::N, true);  // Set negative flag initially
        
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be cleared when SP has bit 7 clear";
    }
}

// Test that TSX preserves other flags (C, I, D, B, V)
TEST_F(TSXTest, PreserveOtherFlags) {
    std::vector<Flag> flagsToTest = {
        Flag::C, Flag::I, Flag::D, Flag::B, Flag::V
    };
    
    for (Flag flag : flagsToTest) {
        // Test with flag initially set
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially set");
            SetUp();
            
            setSP(0x42);
            setFlag(flag, true);
            
            std::unique_ptr<TSX> tsx(new TSX(cpu));
            tsx->run();
            
            ASSERT_TRUE(cpu->getFlag(flag)) << "Flag should remain set after TSX";
        }
        
        // Test with flag initially clear
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially clear");
            SetUp();
            
            setSP(0x42);
            setFlag(flag, false);
            
            std::unique_ptr<TSX> tsx(new TSX(cpu));
            tsx->run();
            
            ASSERT_FALSE(cpu->getFlag(flag)) << "Flag should remain clear after TSX";
        }
    }
}


// Test with stack manipulations
TEST_F(TSXTest, StackManipulation) {
    // Test TSX after push operations
    {
        SCOPED_TRACE("TSX after push operations");
        SetUp();
        
        // Initialize SP to stack top
        setSP(0xFF);
        
        // Simulate some push operations (decrement SP)
        uint8_t newSP = 0xFF - 4; // After pushing 4 bytes
        setSP(newSP);
        
        // Run TSX
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        // X should equal the new SP value
        ASSERT_EQ(cpu->getRegister(Register::X), newSP) << "X should equal SP after pushes";
    }
    
    // Test TSX after pop operations
    {
        SCOPED_TRACE("TSX after pop operations");
        SetUp();
        
        // Initialize SP to somewhere in the middle of the stack
        uint8_t initialSP = 0xF0;
        setSP(initialSP);
        
        // Simulate some pop operations (increment SP)
        uint8_t newSP = initialSP + 3; // After popping 3 bytes
        setSP(newSP);
        
        // Run TSX
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        // X should equal the new SP value
        ASSERT_EQ(cpu->getRegister(Register::X), newSP) << "X should equal SP after pops";
    }
}

// Test randomized behavior (comprehensive coverage)
TEST_F(TSXTest, RandomizedBehavior) {
    const int ITERATIONS = 1000; // Reduced from 10000 for faster tests
    
    for (int i = 0; i < ITERATIONS; i++) {
        SetUp();
        
        // Randomize registers and flags
        uint8_t sp = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        uint8_t a = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        setSP(sp);
        setX(x);
        setA(a);
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
        
        // Run TSX
        std::unique_ptr<TSX> tsx(new TSX(cpu));
        tsx->run();
        
        // Verify X equals SP
        ASSERT_EQ(cpu->getRegister(Register::X), sp) << "X should equal SP after TSX";
        
        // Verify other registers are unchanged
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "A should be unchanged";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "Y should be unchanged";
        ASSERT_EQ(cpu->getRegister(Register::SP), sp) << "SP should be unchanged";
        
        // Calculate expected flags
        bool expectZ = (sp == 0);
        bool expectN = (sp & 0x80) != 0;
        
        // Verify all flags
        ASSERT_EQ(cpu->getFlag(Flag::C), initialC) << "Carry flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::Z), expectZ) << "Zero flag should reflect X value";
        ASSERT_EQ(cpu->getFlag(Flag::I), initialI) << "Interrupt flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::D), initialD) << "Decimal flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::B), initialB) << "Break flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::V), initialV) << "Overflow flag should be preserved";
        ASSERT_EQ(cpu->getFlag(Flag::N), expectN) << "Negative flag should reflect X value";
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