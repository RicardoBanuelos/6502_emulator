#include <gtest/gtest.h>
#include "PLA.h"
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

class PLATest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Helper to verify PLA results
void verifyPLAResults(uint8_t expectedValue) {
    // Check A register contains the pulled value
    ASSERT_EQ(cpu->getRegister(Register::A), expectedValue)
        << "A register should contain pulled value: expected=0x" << std::hex << (int)expectedValue;
    
    // Check zero flag (set if value is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), expectedValue == 0)
        << "Zero flag incorrect: value=0x" << std::hex << (int)expectedValue
        << ", Z flag should be " << (expectedValue == 0 ? "set" : "clear");
    
    // Check negative flag (set if bit 7 of value is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (expectedValue & 0x80) != 0)
        << "Negative flag incorrect: value=0x" << std::hex << (int)expectedValue
        << ", N flag should be " << ((expectedValue & 0x80) != 0 ? "set" : "clear");
}

// Test basic PLA functionality
TEST_F(PLATest, BasicOperation) {
    // Set initial stack pointer and push a test value
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);
    
    // Push value to the stack
    // Note: Stack grows downward, so SP will be decremented after push
    mem->writeByte(0x0100 + initialSP, testValue);
    cpu->setRegister(Register::SP, initialSP - 1);
    
    // Execute PLA
    std::unique_ptr<PLA> pla(new PLA(cpu, 4)); 
    pla->run();
    
    // Check that stack pointer was incremented back to initial value
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should be incremented after pull";
    
    // Verify A register and flags
    verifyPLAResults(testValue);
}

// Test PLA with specific values that trigger flag behavior
TEST_F(PLATest, SpecificValues) {
    const uint8_t testValues[] = {
        0x00, // Zero - tests zero flag
        0x01, // Smallest positive value
        0x7F, // Largest positive value
        0x80, // Smallest negative value
        0xFF, // Largest negative value
        0x55, // Alternating bit pattern 01010101
        0xAA  // Alternating bit pattern 10101010
    };
    
    for (auto value : testValues) {
        SCOPED_TRACE(std::string("Testing PLA with value=0x") + 
                    (value < 16 ? "0" : "") + 
                    std::to_string(value));
        
        // Reset for each test
        setup();
        
        // Setup initial A register with a different value
        cpu->setRegister(Register::A, ~value); // Set to opposite bits
        
        // Setup stack with test value
        uint8_t initialSP = 0xFD; // We'll pull from 0xFE
        cpu->setRegister(Register::SP, initialSP);
        mem->writeByte(0x0100 + initialSP + 1, value);
        
        // Execute PLA
        std::unique_ptr<PLA> pla(new PLA(cpu, 4));
        pla->run();
        
        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP + 1)
            << "Stack pointer should be incremented after pull";
        
        // Verify A register and flags
        verifyPLAResults(value);
    }
}

// Test PLA after PHA to verify correct stack operation
TEST_F(PLATest, PHAfollowedByPLA) {
    // Setup
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);
    cpu->setRegister(Register::A, testValue);
    
    // Simulate PHA - decrements SP and stores A at the old SP location
    cpu->setRegister(Register::SP, initialSP - 1);
    mem->writeByte(0x0100 + initialSP, testValue);
    
    // Change A register to ensure PLA is actually working
    cpu->setRegister(Register::A, 0x00);
    
    // Execute PLA
    std::unique_ptr<PLA> pla(new PLA(cpu, 4));
    pla->run();
    
    // Stack pointer should be back to initial value
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should return to original value after PHA+PLA";
    
    // A register should have the original value
    ASSERT_EQ(cpu->getRegister(Register::A), testValue)
        << "A register should contain the pushed value after PLA";
}

// Test PLA with stack wrap-around (SP = 0x00)
TEST_F(PLATest, StackWrapAround) {
    // Setup
    uint8_t testValue = 0x42;
    uint8_t initialSP = 0x00;
    cpu->setRegister(Register::SP, initialSP);
    mem->writeByte(0x0100 + 0x00 + 1, testValue); // In 6502, this would wrap to 0x01FF
    
    // Execute PLA
    std::unique_ptr<PLA> pla(new PLA(cpu, 4));
    pla->run();
    
    // Stack pointer should now be 0x01
    ASSERT_EQ(cpu->getRegister(Register::SP), 0x01)
        << "Stack pointer should increment from 0x00 to 0x01";
    
    // A register should have the pulled value
    ASSERT_EQ(cpu->getRegister(Register::A), testValue)
        << "A register should contain the pulled value";
}

// Test multiple pull operations
TEST_F(PLATest, MultiplePulls) {
    // Setup
    const uint8_t values[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t initialSP = 0xFF - sizeof(values); // Point below our values
    
    // Setup stack with values (the stack grows downward)
    // Last index (0x44) at highest address, first index (0x11) at lowest
    for (size_t i = 0; i < sizeof(values); ++i) {
        mem->writeByte(0x0100 + initialSP + 1 + i, values[i]);
    }
    
    cpu->setRegister(Register::SP, initialSP);
    
    // Pull values one by one and verify (FIFO - First In, First Out)
    for (size_t i = 0; i < sizeof(values); ++i) {
        // Execute PLA
        std::unique_ptr<PLA> pla(new PLA(cpu, 4));
        pla->run();
        
        // Verify pulled value
        ASSERT_EQ(cpu->getRegister(Register::A), values[i])
            << "Value pulled at step " << i << " should be " << std::hex << (int)values[i];
        
        // Verify SP incremented
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP + i + 1)
            << "Stack pointer incorrect after pull " << i;
    }
}

// Test that PLA affects only A, Z, and N flags
TEST_F(PLATest, OnlyAffectsAZNFlags) {
    // Set all flags to known states
    bool flags[8] = {true, false, true, false, true, false, true, false};
    cpu->setFlag(Flag::C, flags[0]);
    cpu->setFlag(Flag::Z, flags[1]); // Will be changed
    cpu->setFlag(Flag::I, flags[2]);
    cpu->setFlag(Flag::D, flags[3]);
    cpu->setFlag(Flag::B, flags[4]);
    cpu->setFlag(Flag::V, flags[6]);
    cpu->setFlag(Flag::N, flags[7]); // Will be changed
    
    // Setup stack with a value that will affect Z and N flags
    uint8_t testValue = 0x80; // Negative, not zero
    uint8_t initialSP = 0xFE;
    cpu->setRegister(Register::SP, initialSP);
    mem->writeByte(0x0100 + initialSP + 1, testValue);
    
    // Execute PLA
    std::unique_ptr<PLA> pla(new PLA(cpu, 4));
    pla->run();
    
    // Check affected register and flags
    ASSERT_EQ(cpu->getRegister(Register::A), testValue);
    ASSERT_FALSE(cpu->getFlag(Flag::Z)); // Should be clear (value not zero)
    ASSERT_TRUE(cpu->getFlag(Flag::N));  // Should be set (bit 7 set)
    
    // Check unaffected flags
    ASSERT_EQ(cpu->getFlag(Flag::C), flags[0]);
    ASSERT_EQ(cpu->getFlag(Flag::I), flags[2]);
    ASSERT_EQ(cpu->getFlag(Flag::D), flags[3]);
    ASSERT_EQ(cpu->getFlag(Flag::B), flags[4]);
    ASSERT_EQ(cpu->getFlag(Flag::V), flags[6]);
}



// Randomized test for extra coverage
TEST_F(PLATest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Random test value
        uint8_t testValue = rand() & 0xFF;
        uint8_t initialSP = 0xFE;
        
        // Setup stack
        cpu->setRegister(Register::SP, initialSP);
        mem->writeByte(0x0100 + initialSP + 1, testValue);
        
        // Execute PLA
        std::unique_ptr<PLA> pla(new PLA(cpu, 4));
        pla->run();
        
        // Verify results
        ASSERT_EQ(cpu->getRegister(Register::A), testValue)
            << "Iteration " << i << ": A register incorrect";
        
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP + 1)
            << "Iteration " << i << ": Stack pointer incorrect";
        
        // Check flags
        ASSERT_EQ(cpu->getFlag(Flag::Z), testValue == 0)
            << "Iteration " << i << ": Zero flag incorrect";
        
        ASSERT_EQ(cpu->getFlag(Flag::N), (testValue & 0x80) != 0)
            << "Iteration " << i << ": Negative flag incorrect";
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