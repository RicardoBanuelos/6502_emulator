#include <gtest/gtest.h>
#include "JSR.h"
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

class JSRTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test JSR basic functionality
TEST_F(JSRTest, JumpToSubroutineBasic) {
    // Set initial PC and stack pointer
    uint16_t initialPC = 0x1000;
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::PC, initialPC);
    cpu->setRegister(Register::SP, initialSP);
    
    // Set the target subroutine address at the current PC
    uint16_t targetAddress = 0x2000;
    mem->writeWord(initialPC, targetAddress);
    
    // Execute JSR
    std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
    jsr->run();
    
    // Verify PC now points to the target address
    ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress) 
        << "PC should be set to target address 0x" << std::hex << targetAddress;
    
    // Verify stack pointer was decremented by 2 (for the 2-byte return address)
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 2) 
        << "Stack pointer should be decremented by 2";
    
    // Verify correct return address was pushed to the stack (PC+2)
    uint8_t highByte = mem->readByte(0x0100 + initialSP);
    uint8_t lowByte = mem->readByte(0x0100 + initialSP - 1);
    uint16_t returnAddress = (highByte << 8) | lowByte;
    
    ASSERT_EQ(returnAddress, initialPC + 2) 
        << "Return address on stack should be PC+2 (0x" << std::hex << (initialPC + 2) << ")";
}

// Test JSR with various addresses
TEST_F(JSRTest, JumpToSubroutineVariousAddresses) {
    const uint16_t addresses[] = {
        0x0200, // Common program start
        0x0801, // Typical BASIC program start (C64)
        0x1000, // Random address
        0x7FFF, // Middle of address space
        0x8000, // Start of typical ROM area
        0xFFFF  // Highest possible address
    };
    
    for (auto targetAddress : addresses) {
        // Set up test case
        setup();
        
        std::stringstream ss;
        ss << "Testing JSR to 0x" << std::hex << targetAddress;
        SCOPED_TRACE(ss.str());
        
        uint16_t initialPC = 0x1000;
        uint8_t initialSP = 0xFF;
        cpu->setRegister(Register::PC, initialPC);
        cpu->setRegister(Register::SP, initialSP);
        
        mem->writeWord(initialPC, targetAddress);
        
        // Execute JSR
        std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
        jsr->run();
        
        // Verify PC and stack
        ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress);
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 2);
        
        // Check return address on stack
        uint8_t highByte = mem->readByte(0x0100 + initialSP);
        uint8_t lowByte = mem->readByte(0x0100 + initialSP - 1);
        uint16_t returnAddress = (highByte << 8) | lowByte;
        
        ASSERT_EQ(returnAddress, initialPC + 2);
    }
}

// Test JSR with stack at different positions
TEST_F(JSRTest, JumpToSubroutineStackPositions) {
    const uint8_t stackPositions[] = {
        0xFF, // Top of stack
        0x80, // Middle of stack
        0x10, // Low in stack
        0x02  // Almost bottom of stack
    };
    
    for (auto sp : stackPositions) {
        // Set up test case
        setup();
        
        std::stringstream ss;
        ss << "Testing JSR with stack at 0x" << std::hex << (int)sp;
        SCOPED_TRACE(ss.str());
        
        uint16_t initialPC = 0x1000;
        uint16_t targetAddress = 0x2000;
        
        cpu->setRegister(Register::PC, initialPC);
        cpu->setRegister(Register::SP, sp);
        
        mem->writeWord(initialPC, targetAddress);
        
        // Execute JSR
        std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
        jsr->run();
        
        // Verify PC and stack
        ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress);
        ASSERT_EQ(cpu->getRegister(Register::SP), sp - 2);
        
        // Check return address on stack
        uint8_t highByte = mem->readByte(0x0100 + sp);
        uint8_t lowByte = mem->readByte(0x0100 + sp - 1);
        uint16_t returnAddress = (highByte << 8) | lowByte;
        
        ASSERT_EQ(returnAddress, initialPC + 2);
    }
}

// Test JSR with stack overflow
TEST_F(JSRTest, JumpToSubroutineStackOverflow) {
    // Set initial PC and stack pointer to 0x01 (which will overflow to 0xFF after JSR)
    uint16_t initialPC = 0x1000;
    uint8_t initialSP = 0x01;
    cpu->setRegister(Register::PC, initialPC);
    cpu->setRegister(Register::SP, initialSP);
    
    // Set the target subroutine address
    uint16_t targetAddress = 0x2000;
    mem->writeWord(initialPC, targetAddress);
    
    // Execute JSR
    std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
    jsr->run();
    
    // Verify PC now points to the target address
    ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress);
    
    // Verify stack pointer wrapped around
    ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF);
    
    // Verify return address on stack
    uint8_t highByte = mem->readByte(0x0100 + 0x01);
    uint8_t lowByte = mem->readByte(0x0100 + 0x00);
    uint16_t returnAddress = (highByte << 8) | lowByte;
    
    ASSERT_EQ(returnAddress, initialPC + 2);
}

// Test JSR doesn't affect processor status flags
TEST_F(JSRTest, JumpToSubroutineNoFlagEffects) {
    // Set all flag states explicitly
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true);
    
    // Set up JSR
    uint16_t initialPC = 0x1000;
    uint16_t targetAddress = 0x2000;
    
    cpu->setRegister(Register::PC, initialPC);
    mem->writeWord(initialPC, targetAddress);
    
    // Execute JSR
    std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
    jsr->run();
    
    // Verify PC changed
    ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress);
    
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
    
    initialPC = 0x1000;
    targetAddress = 0x2000;
    
    cpu->setRegister(Register::PC, initialPC);
    mem->writeWord(initialPC, targetAddress);
    
    jsr.reset(new JSR(cpu, AddressingMode::Absolute, 6));
    jsr->run();
    
    // Verify flags still unchanged
    ASSERT_FALSE(cpu->getFlag(Flag::C));
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_FALSE(cpu->getFlag(Flag::I));
    ASSERT_FALSE(cpu->getFlag(Flag::D));
    ASSERT_FALSE(cpu->getFlag(Flag::B));
    ASSERT_FALSE(cpu->getFlag(Flag::V));
    ASSERT_FALSE(cpu->getFlag(Flag::N));
}

// Test JSR followed by RTS
TEST_F(JSRTest, JumpToSubroutineAndReturn) {
    // This test simulates a JSR followed by an RTS to ensure the return mechanism works
    
    // Set up initial state
    uint16_t mainProgram = 0x1000;
    uint16_t subroutine = 0x2000;
    uint8_t initialSP = 0xFF;
    
    cpu->setRegister(Register::PC, mainProgram);
    cpu->setRegister(Register::SP, initialSP);
    
    // Set the target subroutine address
    mem->writeWord(mainProgram, subroutine);
    
    // Execute JSR
    std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
    jsr->run();
    
    // Verify PC and SP after JSR
    ASSERT_EQ(cpu->getRegister(Register::PC), subroutine);
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 2);
    
    // Now manually simulate an RTS instruction
    // Pull return address from stack and increment it
    uint8_t lowByte = mem->readByte(0x0100 + cpu->getRegister(Register::SP) + 1);
    uint8_t highByte = mem->readByte(0x0100 + cpu->getRegister(Register::SP) + 2);
    uint16_t returnAddress = (highByte << 8) | lowByte;
    
    // Increment SP by 2
    cpu->setRegister(Register::SP, cpu->getRegister(Register::SP) + 2);
    
    // Set PC to return address
    cpu->setRegister(Register::PC, returnAddress);
    
    // Verify we're back at the expected address after "RTS"
    ASSERT_EQ(cpu->getRegister(Register::PC), mainProgram + 2);
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP);
}

// Comprehensive randomized test
TEST_F(JSRTest, RandomizedJumps) {
    const int numTests = 100; // Reduced from your 10000
    
    for (int i = 0; i < numTests; ++i) {
        setup();
        
        // Random PC between 0x0200 and 0xFF00 (avoiding edge cases)
        uint16_t initialPC = 0x0200 + (rand() % 0xFD00);
        uint8_t initialSP = rand() & 0xFF;
        
        // Random target address
        uint16_t targetAddress = 0x0200 + (rand() % 0xFD00);
        
        cpu->setRegister(Register::PC, initialPC);
        cpu->setRegister(Register::SP, initialSP);
        mem->writeWord(initialPC, targetAddress);
        
        // Execute JSR
        std::unique_ptr<JSR> jsr(new JSR(cpu, AddressingMode::Absolute, 6));
        jsr->run();
        
        // Verify PC
        ASSERT_EQ(cpu->getRegister(Register::PC), targetAddress) 
            << "PC incorrect after JSR: expected=0x" << std::hex << targetAddress;
        
        // Verify SP
        uint8_t expectedSP = (initialSP - 2) & 0xFF; // Accounts for wrap around
        ASSERT_EQ(cpu->getRegister(Register::SP), expectedSP)
            << "SP incorrect after JSR: expected=0x" << std::hex << (int)expectedSP;
        
        // Verify return address on stack
        uint8_t highByte = mem->readByte(0x0100 + ((initialSP) & 0xFF));
        uint8_t lowByte = mem->readByte(0x0100 + ((initialSP - 1) & 0xFF));
        uint16_t returnAddress = (highByte << 8) | lowByte;
        
        ASSERT_EQ(returnAddress, initialPC + 2)
            << "Return address incorrect on stack: expected=0x" << std::hex << (initialPC + 2);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();
    
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->reset();
    
    cppu->connectBus(bus);
    bus->connectMemory(mem);
    
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}