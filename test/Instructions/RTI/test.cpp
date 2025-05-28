#include <gtest/gtest.h>
#include "RTI.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"

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

class RTITest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Helper to set up the stack for RTI
void setupRTIStack(uint8_t ps, uint16_t pc, uint8_t initialSP = 0xFF) {
    // Set stack pointer
    cpu->setRegister(Register::SP, initialSP);
    
    // Push PC and PS on the stack (as if from an interrupt)
    // PC high byte, PC low byte, then PS
    cpu->pushWord(pc);
    cpu->pushByte(ps);
}

// Verify RTI restored the processor state correctly
void verifyRTIResults(uint8_t expectedPS, uint16_t expectedPC, uint8_t expectedSP) {
    // On the 6502, RTI restores all bits, including B and bit 5,
    // unlike PLP which preserves the B flag
    
    // Check processor status (PS)
    ASSERT_EQ(cpu->getRegister(Register::PS), expectedPS)
        << "PS should be restored to: 0x" << std::hex << (int)expectedPS;
    
    // Check program counter (PC)
    ASSERT_EQ(cpu->getRegister(Register::PC), expectedPC)
        << "PC should be restored to: 0x" << std::hex << expectedPC;
    
    // Check stack pointer (SP)
    ASSERT_EQ(cpu->getRegister(Register::SP), expectedSP)
        << "SP should be incremented by 3: 0x" << std::hex << (int)expectedSP;
}

// Test basic RTI functionality
TEST_F(RTITest, BasicOperation) {
    // Set up expected values
    uint8_t psExpected = 0xA5;  // 10100101: N=1, V=0, B=1, D=0, I=1, Z=0, C=1
    uint16_t pcExpected = 0x1234;
    uint8_t initialSP = 0xFA;  // Leave space on stack
    
    // Setup stack with test values
    setupRTIStack(psExpected, pcExpected, initialSP);
    
    // Change the current PS and PC to different values
    cpu->setRegister(Register::PS, ~psExpected);  // Set to opposite value
    cpu->setRegister(Register::PC, 0xFFFF);       // Set to different value
    
    // Execute RTI
    std::unique_ptr<RTI> rti(new RTI(cpu));  // RTI takes 6 cycles
    rti->run();
    
    // Verify the processor state was restored correctly
    verifyRTIResults(psExpected, pcExpected, initialSP);
}

// Test RTI with B flag and bit 5 in processor status
TEST_F(RTITest, BFlagAndBit5Handling) {
    // The B flag (bit 4) and bit 5 in PS should be restored exactly as they
    // appear on the stack, unlike PLP which ignores B and sets bit 5
    
    // Test case 1: B flag and bit 5 both clear
    uint8_t ps1 = 0xCF;  // 11001111 (B and bit 5 clear)
    uint16_t pc1 = 0xABCD;
    uint8_t initialSP1 = 0xF5;
    
    setupRTIStack(ps1, pc1, initialSP1);
    std::unique_ptr<RTI> rti1(new RTI(cpu));
    rti1->run();
    
    // B and bit 5 should be as they were on the stack
    verifyRTIResults(ps1, pc1, initialSP1);
    ASSERT_FALSE(cpu->getFlag(Flag::B)) << "B flag should be clear";
    
    // Test case 2: B flag and bit 5 both set
    setup();
    uint8_t ps2 = 0xFF;  // 11111111 (all bits set)
    uint16_t pc2 = 0x1234;
    uint8_t initialSP2 = 0xF8;
    
    setupRTIStack(ps2, pc2, initialSP2);
    std::unique_ptr<RTI> rti2(new RTI(cpu));
    rti2->run();
    
    // B and bit 5 should be as they were on the stack
    verifyRTIResults(ps2, pc2, initialSP2);
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "B flag should be set";
}

// Test RTI with stack pointer wrap-around
TEST_F(RTITest, StackWrapAround) {
    // Test RTI with stack at positions that would cause wrap-around
    uint8_t psExpected = 0x55;  // 01010101
    uint16_t pcExpected = 0x4321;
    uint8_t initialSP = 0x02;  // Only 3 bytes above bottom of stack
    
    setupRTIStack(psExpected, pcExpected, initialSP);
    
    // Execute RTI
    std::unique_ptr<RTI> rti(new RTI(cpu));
    rti->run();
    
    // Verify results - stack pointer should wrap around
    verifyRTIResults(psExpected, pcExpected, initialSP);
}

// Test RTI with all processor status bits
TEST_F(RTITest, AllFlagCombinations) {
    const uint8_t flagCombinations[] = {
        0x00,  // All flags clear
        0xFF,  // All flags set
        0xA5,  // 10100101 (alternating)
        0x5A,  // 01011010 (alternating)
        0x30,  // Just B flag and bit 5
        0x01,  // Just carry flag
        0x02,  // Just zero flag
        0x80   // Just negative flag
    };
    
    for (auto ps : flagCombinations) {
        SCOPED_TRACE(std::string("Testing RTI with PS=0x") + 
                    (ps < 16 ? "0" : "") + 
                    std::to_string(ps));
        
        setup();
        uint16_t pcExpected = 0x1000 + ps;  // Different PC for each case
        uint8_t initialSP = 0xFA;
        
        setupRTIStack(ps, pcExpected, initialSP);
        
        // Execute RTI
        std::unique_ptr<RTI> rti(new RTI(cpu));
        rti->run();
        
        // Verify processor status and PC
        verifyRTIResults(ps, pcExpected, initialSP);
        
        // Check each individual flag
        ASSERT_EQ(cpu->getFlag(Flag::C), (ps & 0x01) != 0) << "C flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::Z), (ps & 0x02) != 0) << "Z flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::I), (ps & 0x04) != 0) << "I flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::D), (ps & 0x08) != 0) << "D flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::B), (ps & 0x10) != 0) << "B flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::V), (ps & 0x40) != 0) << "V flag incorrect";
        ASSERT_EQ(cpu->getFlag(Flag::N), (ps & 0x80) != 0) << "N flag incorrect";
    }
}

// Test RTI with different PC values including edge cases
TEST_F(RTITest, PCEdgeCases) {
    const uint16_t pcValues[] = {
        0x0000,  // Bottom of memory
        0xFFFF,  // Top of memory
        0x0100,  // Start of stack page
        0x01FF,  // End of stack page
        0x8000,  // Middle of memory
        0xC000   // Common ROM location
    };
    
    for (auto pc : pcValues) {
        SCOPED_TRACE(std::string("Testing RTI with PC=0x") + 
                    (pc < 0x1000 ? "0" : "") +
                    (pc < 0x0100 ? "0" : "") +
                    (pc < 0x0010 ? "0" : "") +
                    std::to_string(pc));
        
        setup();
        uint8_t psExpected = 0x45;  // Some arbitrary status
        uint8_t initialSP = 0xFA;
        
        setupRTIStack(psExpected, pc, initialSP);
        
        // Execute RTI
        std::unique_ptr<RTI> rti(new RTI(cpu));
        rti->run();
        
        // Verify PC was restored correctly
        verifyRTIResults(psExpected, pc, initialSP);
    }
}


// Test simulating a complete interrupt sequence (BRK followed by RTI)
TEST_F(RTITest, CompleteInterruptSequence) {
    // Set up the initial state
    setup();
    uint16_t originalPC = 0x1000;
    uint8_t originalPS = 0x45;  // 01000101
    uint8_t originalSP = 0xFF;
    
    cpu->setRegister(Register::PC, originalPC);
    cpu->setRegister(Register::PS, originalPS);
    cpu->setRegister(Register::SP, originalSP);
    
    // Set up interrupt vector
    uint16_t interruptHandler = 0x2000;
    mem->writeWord(0xFFFE, interruptHandler);  // IRQ/BRK vector
    
    // Simulate BRK instruction
    // Push PC+1 and PS (with B flag set) to stack
    cpu->pushWord(originalPC + 1);  // PC+1 for BRK
    cpu->pushByte(originalPS | 0x10);  // Set B flag in pushed PS
    
    // Set processor state as if interrupt occurred
    cpu->setRegister(Register::PC, interruptHandler);
    cpu->setFlag(Flag::I, true);  // Interrupts disabled during handler
    
    // Now execute RTI
    std::unique_ptr<RTI> rti(new RTI(cpu));
    rti->run();
    
    // Verify we're back to the original state (PC+1)
    ASSERT_EQ(cpu->getRegister(Register::PC), originalPC + 1)
        << "PC should be restored to original+1 (as pushed by BRK)";
    
    ASSERT_EQ(cpu->getRegister(Register::PS), originalPS | 0x10)
        << "PS should be restored to original with B flag set (as pushed by BRK)";
    
    ASSERT_EQ(cpu->getRegister(Register::SP), originalSP)
        << "SP should be back to original value";
}

// Randomized test for extra coverage
TEST_F(RTITest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Random PS and PC values
        uint8_t psExpected = rand() & 0xFF;
        uint16_t pcExpected = rand() & 0xFFFF;
        uint8_t initialSP = 0xFA - (i % 10);  // Vary the stack pointer
        
        setupRTIStack(psExpected, pcExpected, initialSP);
        
        // Execute RTI
        std::unique_ptr<RTI> rti(new RTI(cpu));
        rti->run();
        
        // Verify results
        verifyRTIResults(psExpected, pcExpected, initialSP);
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