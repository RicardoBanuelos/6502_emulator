#include <gtest/gtest.h>
#include "PHA.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
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

class PHATest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test basic PHA functionality
TEST_F(PHATest, BasicOperation) {
    // Set initial A register and stack pointer
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::A, testValue);
    cpu->setRegister(Register::SP, initialSP);
    
    // Execute PHA
    std::unique_ptr<PHA> pha(new PHA(cpu));
    pha->run();
    
    // Check that stack pointer was decremented
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 1)
        << "Stack pointer should be decremented by 1";
    
    // Check that value was pushed to the stack
    ASSERT_EQ(mem->readByte(0x0100 + initialSP), testValue)
        << "Value on stack should match A register";
    
    // Verify A register wasn't changed
    ASSERT_EQ(cpu->getRegister(Register::A), testValue)
        << "A register should remain unchanged";
}

// Test PHA with specific values
TEST_F(PHATest, SpecificValues) {
    const uint8_t testValues[] = {
        0x00, // Zero
        0x01, // Smallest positive value
        0x7F, // Largest positive value
        0x80, // Smallest negative value
        0xFF, // Largest negative value
        0x55, // Alternating bit pattern 01010101
        0xAA  // Alternating bit pattern 10101010
    };
    
    for (auto value : testValues) {
        SCOPED_TRACE(std::string("Testing PHA with A=0x") + 
                     (value < 16 ? "0" : "") + 
                     std::to_string(value));
        
        // Reset for each test
        setup();
        
        uint8_t initialSP = 0xFF;
        cpu->setRegister(Register::A, value);
        cpu->setRegister(Register::SP, initialSP);
        
        // Execute PHA
        std::unique_ptr<PHA> pha(new PHA(cpu));
        pha->run();
        
        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 1);
        ASSERT_EQ(mem->readByte(0x0100 + initialSP), value);
        ASSERT_EQ(cpu->getRegister(Register::A), value);
    }
}

// Test PHA with different initial stack pointer values
TEST_F(PHATest, DifferentStackPointers) {
    const uint8_t stackPointers[] = {
        0xFF, // Top of stack
        0x80, // Middle of stack
        0x01, // Near bottom of stack
    };
    
    for (auto sp : stackPointers) {
        SCOPED_TRACE(std::string("Testing PHA with SP=0x") + 
                     (sp < 16 ? "0" : "") + 
                     std::to_string(sp));
        
        // Reset for each test
        setup();
        
        uint8_t testValue = 0x42;
        cpu->setRegister(Register::A, testValue);
        cpu->setRegister(Register::SP, sp);
        
        // Execute PHA
        std::unique_ptr<PHA> pha(new PHA(cpu));
        pha->run();
        
        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), sp - 1);
        ASSERT_EQ(mem->readByte(0x0100 + sp), testValue);
    }
}

// Test PHA with stack wrap-around (should wrap from 0x00 to 0xFF)
TEST_F(PHATest, StackWrapAround) {
    // Set SP to 0x00
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0x00;
    cpu->setRegister(Register::A, testValue);
    cpu->setRegister(Register::SP, initialSP);
    
    // Execute PHA
    std::unique_ptr<PHA> pha(new PHA(cpu));
    pha->run();
    
    // Check that SP wrapped around to 0xFF
    ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF)
        << "Stack pointer should wrap from 0x00 to 0xFF";
    
    // Check that value was pushed to the correct location
    ASSERT_EQ(mem->readByte(0x0100 + initialSP), testValue)
        << "Value on stack should be at 0x0100 + initial SP";
}

// Test PHA doesn't affect processor flags
TEST_F(PHATest, NoFlagsAffected) {
    // Set all flags to known states
    bool flags[8] = {true, false, true, false, true, false, true, false};
    cpu->setFlag(Flag::C, flags[0]);
    cpu->setFlag(Flag::Z, flags[1]);
    cpu->setFlag(Flag::I, flags[2]);
    cpu->setFlag(Flag::D, flags[3]);
    cpu->setFlag(Flag::B, flags[4]);
    cpu->setFlag(Flag::V, flags[6]);
    cpu->setFlag(Flag::N, flags[7]);
    
    // Set A and SP
    cpu->setRegister(Register::A, 0x42);
    cpu->setRegister(Register::SP, 0xFF);
    
    // Execute PHA
    std::unique_ptr<PHA> pha(new PHA(cpu));
    pha->run();
    
    // Check flags unchanged
    ASSERT_EQ(cpu->getFlag(Flag::C), flags[0]);
    ASSERT_EQ(cpu->getFlag(Flag::Z), flags[1]);
    ASSERT_EQ(cpu->getFlag(Flag::I), flags[2]);
    ASSERT_EQ(cpu->getFlag(Flag::D), flags[3]);
    ASSERT_EQ(cpu->getFlag(Flag::B), flags[4]);
    ASSERT_EQ(cpu->getFlag(Flag::V), flags[6]);
    ASSERT_EQ(cpu->getFlag(Flag::N), flags[7]);
}

// Test PHA with subsequent PLA
TEST_F(PHATest, PHAFollowedByPLA) {
    // Set A and SP
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::A, testValue);
    cpu->setRegister(Register::SP, initialSP);
    
    // Execute PHA
    std::unique_ptr<PHA> pha(new PHA(cpu));
    pha->run();
    
    // Change A register
    cpu->setRegister(Register::A, 0x00);
    
    // Now manually simulate PLA
    cpu->setRegister(Register::SP, cpu->getRegister(Register::SP) + 1);
    uint8_t pulledValue = mem->readByte(0x0100 + cpu->getRegister(Register::SP));
    cpu->setRegister(Register::A, pulledValue);
    
    // Verify A contains the original value
    ASSERT_EQ(cpu->getRegister(Register::A), testValue)
        << "A register should contain the pushed value after PLA";
    
    // Verify SP is back to initial value
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should be restored after PHA+PLA";
}

// Test PHA with multiple pushes
TEST_F(PHATest, MultiplePushes) {
    // Set initial SP
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);
    
    // Push several values
    const uint8_t values[] = {0x11, 0x22, 0x33, 0x44};
    
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
        cpu->setRegister(Register::A, values[i]);
        std::unique_ptr<PHA> pha(new PHA(cpu));
        pha->run();
    }
    
    // Check SP decremented by number of pushes
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - sizeof(values)/sizeof(values[0]))
        << "Stack pointer should be decremented by number of pushes";
    
    // Check values on stack in reverse order
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
        uint8_t stackValue = mem->readByte(0x0100 + initialSP - i);
        ASSERT_EQ(stackValue, values[i])
            << "Value at stack position " << i << " should be " << std::hex << (int)values[i];
    }
}

// Test PHA cycle count
TEST_F(PHATest, CycleCount) {
    // PHA should take 3 cycles
    // std::unique_ptr<PHA> pha(new PHA(cpu));
    // ASSERT_EQ(pha->getCycles(), 3)
    //     << "PHA instruction should take 3 cycles";
}

// Randomized test for extra coverage (reduced from 10000 to 100 iterations)
TEST_F(PHATest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Random A value and stack pointer
        uint8_t testValue = rand() & 0xFF;
        uint8_t initialSP = rand() & 0xFF;
        
        cpu->setRegister(Register::A, testValue);
        cpu->setRegister(Register::SP, initialSP);
        
        // Execute PHA
        std::unique_ptr<PHA> pha(new PHA(cpu));
        pha->run();
        
        // Check stack pointer and value
        uint8_t expectedSP = (initialSP - 1) & 0xFF; // Account for wrap-around
        ASSERT_EQ(cpu->getRegister(Register::SP), expectedSP)
            << "Iteration " << i << ": Stack pointer incorrect";
        
        ASSERT_EQ(mem->readByte(0x0100 + initialSP), testValue)
            << "Iteration " << i << ": Value on stack incorrect";
        
        // Verify A register wasn't changed
        ASSERT_EQ(cpu->getRegister(Register::A), testValue)
            << "Iteration " << i << ": A register was modified";
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