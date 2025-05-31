#include <gtest/gtest.h>
#include "TXS.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <string>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());
static const int MAX_ITERATIONS = 1000; // Reduced from 10000 for faster tests

class TXSTest : public ::testing::Test {
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
        
        void verifyUnchanged(std::shared_ptr<ICPU> cpu) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag was modified";
        }
    };
    
    // Common test values covering important stack pointer cases
    std::vector<uint8_t> testValues = {
        0x00,       // Stack pointer at bottom of stack (unusual in real code)
        0x01,       // Near bottom of stack
        0x7F,       // Middle value
        0x80,       // High value (negative bit set)
        0xFD,       // Common initial value (stack at 0x01FD)
        0xFF        // Top of stack (after reset)
    };
};

// Test basic functionality with specific values
TEST_F(TXSTest, BasicFunctionality) {
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("X = 0x" + std::string(testValue < 16 ? "0" : "") + 
                     std::to_string(testValue));
        
        // Setup initial state - X has test value, SP is different
        setX(testValue);
        setSP(~testValue);  // Different value to ensure it changes
        
        // Capture initial flags
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Execute TXS
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        // Verify SP now equals X
        ASSERT_EQ(cpu->getRegister(Register::SP), testValue) << "SP should equal X after TXS";
        
        // Verify flags remain unchanged
        initialFlags.verifyUnchanged(cpu);
    }
}

// Test that register values remain unchanged (except SP)
TEST_F(TXSTest, PreserveOtherRegisters) {
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Iteration " + std::to_string(i));
        
        // Setup with random values
        SetUp();
        uint8_t x = rand() & 0xFF;
        uint8_t a = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        setX(x);
        setA(a);
        setY(y);
        
        // Run TXS
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        // Check other registers are unchanged
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "A register should not be affected by TXS";
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "X register should not be affected by TXS";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "Y register should not be affected by TXS";
    }
}

// Test that TXS preserves all processor status flags
TEST_F(TXSTest, PreserveFlags) {
    std::vector<Flag> flagsToTest = {
        Flag::C, Flag::Z, Flag::I, Flag::D, Flag::B, Flag::V, Flag::N
    };
    
    for (Flag flag : flagsToTest) {
        // Test with flag initially set
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially set");
            SetUp();
            
            setX(0x42);
            setFlag(flag, true);
            
            std::unique_ptr<TXS> txs(new TXS(cpu));
            txs->run();
            
            ASSERT_TRUE(cpu->getFlag(flag)) << "Flag should remain set after TXS";
        }
        
        // Test with flag initially clear
        {
            SCOPED_TRACE("Flag " + std::to_string(static_cast<int>(flag)) + " initially clear");
            SetUp();
            
            setX(0x42);
            setFlag(flag, false);
            
            std::unique_ptr<TXS> txs(new TXS(cpu));
            txs->run();
            
            ASSERT_FALSE(cpu->getFlag(flag)) << "Flag should remain clear after TXS";
        }
    }
}



// Test with stack manipulations
TEST_F(TXSTest, StackManipulation) {
    // Test with X value at stack top
    {
        SCOPED_TRACE("Setting SP to top of stack (0xFF)");
        SetUp();
        
        setX(0xFF);
        setSP(0x42); // Some arbitrary value
        
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF) << "SP should be set to top of stack";
    }
    
    // Test with X value at stack bottom
    {
        SCOPED_TRACE("Setting SP to bottom of stack (0x00)");
        SetUp();
        
        setX(0x00);
        setSP(0x42); // Some arbitrary value
        
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        ASSERT_EQ(cpu->getRegister(Register::SP), 0x00) << "SP should be set to bottom of stack";
    }
    
    // Test with typical stack value
    {
        SCOPED_TRACE("Setting SP to typical value (0xFD)");
        SetUp();
        
        setX(0xFD);
        setSP(0x42); // Some arbitrary value
        
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFD) << "SP should be set to typical stack value";
    }
}

// Test that unlike other transfer instructions, TXS doesn't affect flags
TEST_F(TXSTest, NoFlagAffect) {
    // Test with zero value
    {
        SCOPED_TRACE("X = 0x00 (should not set Z flag)");
        SetUp();
        
        setX(0x00);
        setFlag(Flag::Z, false); // Clear Z flag initially
        
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Z flag should not be affected by TXS";
    }
    
    // Test with negative value
    {
        SCOPED_TRACE("X = 0x80 (should not set N flag)");
        SetUp();
        
        setX(0x80);
        setFlag(Flag::N, false); // Clear N flag initially
        
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        ASSERT_FALSE(cpu->getFlag(Flag::N)) << "N flag should not be affected by TXS";
    }
}

// Original style randomized test (comprehensive coverage)
TEST_F(TXSTest, RandomizedBehavior) {
    for(int i = 0; i < MAX_ITERATIONS; ++i) {
        // Setup with random values
        SetUp();
        cpu->randomizeRegisters();
        cpu->randomizeFlags();
        
        // Capture initial state for verification
        uint8_t initialX = cpu->getRegister(Register::X);
        uint8_t initialA = cpu->getRegister(Register::A);
        uint8_t initialY = cpu->getRegister(Register::Y);
        
        // Capture initial flags for verification
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Execute TXS
        std::unique_ptr<TXS> txs(new TXS(cpu));
        txs->run();
        
        // Verify SP equals X
        ASSERT_EQ(cpu->getRegister(Register::SP), initialX) << "SP should equal X after TXS";
        
        // Verify other registers unchanged
        ASSERT_EQ(cpu->getRegister(Register::X), initialX) << "X should be unchanged";
        ASSERT_EQ(cpu->getRegister(Register::A), initialA) << "A should be unchanged";
        ASSERT_EQ(cpu->getRegister(Register::Y), initialY) << "Y should be unchanged";
        
        // Verify all flags unchanged
        initialFlags.verifyUnchanged(cpu);
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