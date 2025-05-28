#include <gtest/gtest.h>
#include "PLP.h"
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

class PLPTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Helper to verify PLP results
void verifyPLPResults(uint8_t expectedPS) {
    // Important: The B flag (bit 4) and unused flag (bit 5) are special cases in the status register
    // B flag should not be affected by PLP
    // Bit 5 is always set in the status register
    
    // Create expected value with hardware-specific behavior (B flag not affected, bit 5 always set)
    uint8_t mask = 0xEF; // 1110 1111 - Mask out B flag (bit 4)
    uint8_t originalB = cpu->getRegister(Register::PS) & 0x10; // Save original B flag
    uint8_t adjustedExpected = (expectedPS & mask) | originalB | 0x20; // Add original B and set bit 5
    
    // Check processor status matches expected (with adjustments)
    ASSERT_EQ(cpu->getRegister(Register::PS), adjustedExpected)
        << "PS should contain pulled value (with B flag preserved): expected=0x" 
        << std::hex << (int)adjustedExpected;
    
    // Check individual flags (except B flag)
    ASSERT_EQ(cpu->getFlag(Flag::C), (expectedPS & 0x01) != 0)
        << "C flag incorrect: should be " << ((expectedPS & 0x01) != 0 ? "set" : "clear");
    
    ASSERT_EQ(cpu->getFlag(Flag::Z), (expectedPS & 0x02) != 0)
        << "Z flag incorrect: should be " << ((expectedPS & 0x02) != 0 ? "set" : "clear");
    
    ASSERT_EQ(cpu->getFlag(Flag::I), (expectedPS & 0x04) != 0)
        << "I flag incorrect: should be " << ((expectedPS & 0x04) != 0 ? "set" : "clear");
    
    ASSERT_EQ(cpu->getFlag(Flag::D), (expectedPS & 0x08) != 0)
        << "D flag incorrect: should be " << ((expectedPS & 0x08) != 0 ? "set" : "clear");
    
    // B flag is not affected by PLP
    
    ASSERT_EQ(cpu->getFlag(Flag::V), (expectedPS & 0x40) != 0)
        << "V flag incorrect: should be " << ((expectedPS & 0x40) != 0 ? "set" : "clear");
    
    ASSERT_EQ(cpu->getFlag(Flag::N), (expectedPS & 0x80) != 0)
        << "N flag incorrect: should be " << ((expectedPS & 0x80) != 0 ? "set" : "clear");
}

// Test basic PLP functionality
TEST_F(PLPTest, BasicOperation) {
    // Set initial stack pointer and push a test status value
    uint8_t testPS = 0xA5; // 10100101: N=1, V=0, B=1, D=0, I=1, Z=0, C=1
    uint8_t initialSP = 0xFF;
    
    // Remember original B flag value
    uint8_t originalBFlag = cpu->getFlag(Flag::B);
    
    // Push status to stack
    cpu->setRegister(Register::SP, initialSP);
    mem->writeByte(0x0100 + initialSP, testPS);
    cpu->setRegister(Register::SP, initialSP - 1);
    
    // Change flags to make sure PLP actually changes them
    cpu->setFlag(Flag::C, !(testPS & 0x01));
    cpu->setFlag(Flag::Z, !(testPS & 0x02));
    cpu->setFlag(Flag::I, !(testPS & 0x04));
    cpu->setFlag(Flag::D, !(testPS & 0x08));
    cpu->setFlag(Flag::V, !(testPS & 0x40));
    cpu->setFlag(Flag::N, !(testPS & 0x80));
    
    // Execute PLP
    std::unique_ptr<PLP> plp(new PLP(cpu, 4));
    plp->run();
    
    // Check that stack pointer was incremented
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should be incremented after pull";
    
    // Verify processor status and flags
    verifyPLPResults(testPS);
    
    // Check B flag wasn't changed
    ASSERT_EQ(cpu->getFlag(Flag::B), originalBFlag)
        << "B flag should not be affected by PLP";
}

// Test PLP with specific meaningful status combinations
TEST_F(PLPTest, SpecificStatusValues) {
    const uint8_t testValues[] = {
        0x00, // All flags clear
        0xFF, // All flags set
        0xA5, // 10100101 (alternating)
        0x5A, // 01011010 (alternating)
        0x30, // Just B flag and bit 5 set
        0x01, // Just carry flag
        0x02, // Just zero flag
        0x80  // Just negative flag
    };
    
    for (auto value : testValues) {
        SCOPED_TRACE(std::string("Testing PLP with PS=0x") + 
                    (value < 16 ? "0" : "") + 
                    std::to_string(value));
        
        // Reset for each test
        setup();
        
        // Remember original B flag value
        uint8_t originalBFlag = cpu->getFlag(Flag::B);
        
        // Setup stack with test value
        uint8_t initialSP = 0xFD; // We'll pull from 0xFE
        cpu->setRegister(Register::SP, initialSP);
        mem->writeByte(0x0100 + initialSP + 1, value);
        
        // Execute PLP
        std::unique_ptr<PLP> plp(new PLP(cpu, 4));
        plp->run();
        
        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP + 1)
            << "Stack pointer should be incremented after pull";
        
        // Verify processor status and flags
        verifyPLPResults(value);
        
        // Check B flag wasn't changed
        ASSERT_EQ(cpu->getFlag(Flag::B), originalBFlag)
            << "B flag should not be affected by PLP";
    }
}

// Test PHP followed by PLP to verify correct stack operation
TEST_F(PLPTest, PHPfollowedByPLP) {
    // Setup initial flags to a known pattern
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, false);
    cpu->setFlag(Flag::B, false); // Will be set in pushed value, but not affected by pull
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false);
    
    uint8_t initialPS = cpu->getRegister(Register::PS);
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);
    
    // Remember original B flag value
    bool originalBFlag = cpu->getFlag(Flag::B);
    
    // Simulate PHP - push PS to stack with B and bit 5 set
    uint8_t pushedPS = initialPS | 0x30; // Set B flag and bit 5 for push
    mem->writeByte(0x0100 + initialSP, pushedPS);
    cpu->setRegister(Register::SP, initialSP - 1);
    
    // Change flags to verify PLP will restore them
    cpu->setFlag(Flag::C, false);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, false);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::V, false);
    cpu->setFlag(Flag::N, true);
    
    // Execute PLP
    std::unique_ptr<PLP> plp(new PLP(cpu, 4));
    plp->run();
    
    // Verify stack pointer
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should be incremented after pull";
    
    // Verify flags are restored, but B flag retains original value
    ASSERT_EQ(cpu->getFlag(Flag::C), true);
    ASSERT_EQ(cpu->getFlag(Flag::Z), false);
    ASSERT_EQ(cpu->getFlag(Flag::I), true);
    ASSERT_EQ(cpu->getFlag(Flag::D), false);
    ASSERT_EQ(cpu->getFlag(Flag::B), originalBFlag); // B flag unchanged
    ASSERT_EQ(cpu->getFlag(Flag::V), true);
    ASSERT_EQ(cpu->getFlag(Flag::N), false);
}

// Test PLP with stack wrap-around (SP = 0x00)
TEST_F(PLPTest, StackWrapAround) {
    // Setup
    uint8_t testPS = 0x55; // 01010101
    uint8_t initialSP = 0x00;
    cpu->setRegister(Register::SP, initialSP);
    mem->writeByte(0x0100 + initialSP + 1, testPS); // In 6502, this would wrap to 0x01FF
    
    // Remember original B flag value
    uint8_t originalBFlag = cpu->getFlag(Flag::B);
    
    // Execute PLP
    std::unique_ptr<PLP> plp(new PLP(cpu, 4));
    plp->run();
    
    // Stack pointer should now be 0x01
    ASSERT_EQ(cpu->getRegister(Register::SP), 0x01)
        << "Stack pointer should increment from 0x00 to 0x01";
    
    // Verify PS was updated correctly (except B flag)
    verifyPLPResults(testPS);
}

// Test that PLP ignores the B flag in the pulled value
TEST_F(PLPTest, IgnoresBFlag) {
    // Set initial B flag
    bool initialBFlag = true;
    cpu->setFlag(Flag::B, initialBFlag);
    
    // Pull a value with B flag in the opposite state
    uint8_t testPS = cpu->getRegister(Register::PS) & ~0x10; // Clear B flag in test value
    if (initialBFlag == false) {
        testPS |= 0x10; // Set B flag in test value if it was initially clear
    }
    
    // Push to stack
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);
    mem->writeByte(0x0100 + initialSP, testPS);
    cpu->setRegister(Register::SP, initialSP - 1);
    
    // Execute PLP
    std::unique_ptr<PLP> plp(new PLP(cpu, 4));
    plp->run();
    
    // Verify B flag wasn't changed
    ASSERT_EQ(cpu->getFlag(Flag::B), initialBFlag)
        << "B flag should not be affected by PLP";
}


// Randomized test for extra coverage
TEST_F(PLPTest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Random test value (status)
        uint8_t testPS = rand() & 0xFF;
        uint8_t initialSP = 0xFE;
        
        // Remember original B flag
        bool originalBFlag = cpu->getFlag(Flag::B);
        
        // Setup stack
        cpu->setRegister(Register::SP, initialSP);
        mem->writeByte(0x0100 + initialSP + 1, testPS);
        
        // Execute PLP
        std::unique_ptr<PLP> plp(new PLP(cpu, 4));
        plp->run();
        
        // Stack pointer should be incremented
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP + 1)
            << "Iteration " << i << ": Stack pointer incorrect";
        
        // Verify processor status
        uint8_t mask = 0xEF; // 1110 1111 - Mask out B flag (bit 4)
        uint8_t adjustedExpected = (testPS & mask) | (originalBFlag ? 0x10 : 0x00) | 0x20;
        
        ASSERT_EQ(cpu->getRegister(Register::PS) & mask, adjustedExpected & mask)
            << "Iteration " << i << ": Processor status incorrect";
        
        // B flag should not be affected
        ASSERT_EQ(cpu->getFlag(Flag::B), originalBFlag)
            << "Iteration " << i << ": B flag was incorrectly modified";
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