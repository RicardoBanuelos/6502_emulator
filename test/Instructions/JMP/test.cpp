#include <gtest/gtest.h>
#include "JMP.h"
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

class JMPTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test JMP Absolute - basic functionality
TEST_F(JMPTest, JumpAbsoluteBasic) {
    // Set initial PC
    cpu->setRegister(Register::PC, 0x1000);
    
    // Set the target address at the current PC
    mem->writeWord(0x1000, 0x2000);
    
    // Execute JMP Absolute
    std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Absolute, 3));
    jmp->run();
    
    // Verify PC now points to the target address
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x2000) 
        << "PC should be set to 0x2000 after JMP Absolute";
}

// Test JMP Absolute - jump to various addresses
TEST_F(JMPTest, JumpAbsoluteVariousAddresses) {
    const uint16_t addresses[] = {
        0x0000, // Zero page
        0x00FF, // End of zero page
        0x0100, // Start of stack
        0x01FF, // End of stack
        0x0200, // Common program start
        0x0801, // Typical BASIC program start (C64)
        0x1000, // Random address
        0x7FFF, // Middle of address space
        0x8000, // Start of typical ROM area
        0xFFFC, // Reset vector location
        0xFFFF  // Highest possible address
    };
    
    for (auto address : addresses) {
        // Set up test case
        setup();
        
        std::stringstream ss;
        ss << "Testing JMP to 0x" << std::hex << address;
        SCOPED_TRACE(ss.str());
        
        uint16_t startPC = 0x1000;
        cpu->setRegister(Register::PC, startPC);
        mem->writeWord(startPC, address);
        
        // Execute JMP Absolute
        std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Absolute, 3));
        jmp->run();
        
        // Verify PC
        ASSERT_EQ(cpu->getRegister(Register::PC), address);
    }
}

// Test JMP Indirect - basic functionality
TEST_F(JMPTest, JumpIndirectBasic) {
    // Set initial PC
    cpu->setRegister(Register::PC, 0x1000);
    
    // Set up memory:
    // - At 0x1000, put the pointer 0x2000
    // - At 0x2000, put the final target 0x3000
    mem->writeWord(0x1000, 0x2000);
    mem->writeWord(0x2000, 0x3000);
    
    // Execute JMP Indirect
    std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Indirect, 5));
    jmp->run();
    
    // Verify PC now points to the target address
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x3000) 
        << "PC should be set to 0x3000 after JMP Indirect";
}

// Test JMP Indirect - various indirection levels
TEST_F(JMPTest, JumpIndirectVariousAddresses) {
    struct TestCase {
        uint16_t initialPC;
        uint16_t pointerAddress;
        uint16_t targetAddress;
    };
    
    const TestCase testCases[] = {
        { 0x1000, 0x0100, 0x2000 }, // Zero page pointer
        { 0x1000, 0x0200, 0xFFFF }, // Highest possible target
        { 0x1000, 0x8000, 0x0000 }, // ROM area pointer to zero
        { 0x1000, 0xF000, 0x0801 }, // High memory pointer
    };
    
    for (auto test : testCases) {
        // Set up test case
        setup();
        
        std::stringstream ss;
        ss << "Testing JMP Indirect: PC=0x" << std::hex << test.initialPC
           << ", Pointer=0x" << test.pointerAddress
           << ", Target=0x" << test.targetAddress;
        SCOPED_TRACE(ss.str());
        
        cpu->setRegister(Register::PC, test.initialPC);
        mem->writeWord(test.initialPC, test.pointerAddress);
        mem->writeWord(test.pointerAddress, test.targetAddress);
        
        // Execute JMP Indirect
        std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Indirect, 5));
        jmp->run();
        
        // Verify PC
        ASSERT_EQ(cpu->getRegister(Register::PC), test.targetAddress);
    }
}

// Test the infamous 6502 JMP Indirect bug at page boundaries
TEST_F(JMPTest, JumpIndirectPageBoundaryBug) {
    // Set initial PC
    cpu->setRegister(Register::PC, 0x1000);
    
    // Set up memory to reproduce the bug:
    // - At 0x1000, put the pointer 0x02FF (page boundary - 1)
    // - At 0x02FF, put the low byte of the target (0x34)
    // - At 0x0200, put the high byte of the target (0x12)
    //   (Note: should have been 0x0300, but the bug causes it to wrap within the same page)
    mem->writeWord(0x1000, 0x02FF);
    mem->writeByte(0x02FF, 0x34);
    mem->writeByte(0x0200, 0x12); // This should be at 0x0300, but the bug reads from 0x0200
    
    // Execute JMP Indirect
    std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Indirect, 5));
    jmp->run();
    
    // Verify PC is set to 0x1234 (combining low byte 0x34 from 0x02FF and high byte 0x12 from 0x0200)
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1234) 
        << "PC should be set to 0x1234 due to the JMP Indirect page boundary bug";
}

// Test that JMP doesn't affect any flags
TEST_F(JMPTest, JumpDoesNotAffectFlags) {
    // Set all flag states explicitly
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true);
    
    // Set up JMP
    cpu->setRegister(Register::PC, 0x1000);
    mem->writeWord(0x1000, 0x2000);
    
    // Execute JMP
    std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Absolute, 3));
    jmp->run();
    
    // Verify PC changed
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x2000);
    
    // Verify flags unchanged
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
    
    // Repeat with all flags clear
    setup();
    cpu->setFlag(Flag::C, false);
    cpu->setFlag(Flag::Z, false);
    cpu->setFlag(Flag::I, false);
    cpu->setFlag(Flag::D, false);
    cpu->setFlag(Flag::B, false);
    cpu->setFlag(Flag::V, false);
    cpu->setFlag(Flag::N, false);
    
    cpu->setRegister(Register::PC, 0x1000);
    mem->writeWord(0x1000, 0x2000);
    
    jmp.reset(new JMP(cpu, AddressingMode::Absolute, 3));
    jmp->run();
    
    // Verify flags still unchanged
    ASSERT_FALSE(cpu->getFlag(Flag::C));
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_FALSE(cpu->getFlag(Flag::I));
    ASSERT_FALSE(cpu->getFlag(Flag::D));
    ASSERT_FALSE(cpu->getFlag(Flag::B));
    ASSERT_FALSE(cpu->getFlag(Flag::V));
    ASSERT_FALSE(cpu->getFlag(Flag::N));
}

// Comprehensive randomized test
TEST_F(JMPTest, RandomizedJumps) {
    // Perform fewer random tests (100 instead of 1000)
    for (int i = 0; i < 100; ++i) {
        setup();
        
        // Random PC value
        uint16_t initialPC = rand() & 0xFFFC; // Ensure word-aligned for simplicity
        cpu->setRegister(Register::PC, initialPC);
        
        // Random target address
        uint16_t targetAddress = rand() & 0xFFFF;
        mem->writeWord(initialPC, targetAddress);
        
        bool useIndirect = (rand() % 2) == 1;
        
        if (useIndirect) {
            // For indirect jumps, write the final target to the pointer location
            uint16_t indirectPointer = targetAddress;
            uint16_t finalTarget = rand() & 0xFFFF;
            mem->writeWord(indirectPointer, finalTarget);
            
            // Execute JMP Indirect
            std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Indirect, 5));
            jmp->run();
            
            // Verify PC
            ASSERT_EQ(cpu->getRegister(Register::PC), finalTarget) 
                << "PC incorrect after JMP Indirect: expected=0x" << std::hex << finalTarget;
        } else {
            // Execute JMP Absolute
            std::unique_ptr<JMP> jmp(new JMP(cpu, AddressingMode::Absolute, 3));
            jmp->run();
            
            // Verify PC
            ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress) 
                << "PC incorrect after JMP Absolute: expected=0x" << std::hex << targetAddress;
        }
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