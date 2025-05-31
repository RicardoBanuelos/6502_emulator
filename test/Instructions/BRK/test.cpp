#include <gtest/gtest.h>
#include "BRK.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <tuple>

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

void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
void setSP(uint8_t value) { cpu->setRegister(Register::SP, value); }
void setPS(uint8_t value) { cpu->setRegister(Register::PS, value); }

// Helper function to verify stack contents after BRK
void verifyBRKStackContents(uint16_t expected_pc, uint8_t expected_flags) {
    
    // Check pushed status register (should have B flag and bit 5 set)
    uint8_t stored_ps = cpu->popByte();
    ASSERT_TRUE(stored_ps & Flag::B) << "B flag not set in pushed status";
    ASSERT_TRUE(stored_ps & 0x20) << "Bit 5 not set in pushed status";

    // PC value is pushed high byte first, then low byte
    uint16_t stored_pc = cpu->popWord();
    
    // Check pushed PC value (should be PC+2)
    ASSERT_EQ(stored_pc, expected_pc + 2) << "PC value on stack is incorrect";
    
    
    // Verify other flags match expected values
    uint8_t expected_with_extras = expected_flags | Flag::B | 0x20;
    ASSERT_EQ(stored_ps, expected_with_extras) << "Status register on stack doesn't match expected";
}

TEST(BRKTest, BasicInterruptSequence) {
    SCOPED_TRACE("Testing standard BRK interrupt sequence");
    setup();
    
    // Set up initial CPU state
    uint16_t initial_pc = 0x1234;
    setPC(initial_pc);
    uint8_t initial_flags = 0x00;
    setPS(initial_flags);
    setSP(0xFF);  // Full stack
    
    // Set interrupt vector
    uint16_t irq_vector = 0xABCD;
    mem->writeWord(0xFFFE, irq_vector);
    
    // Execute BRK
    cpu->setFlag(Flag::I, false);  // Ensure I flag is clear
    std::unique_ptr<BRK> brk(new BRK(cpu));
    brk->run();
    
    // Verify PC now points to the interrupt handler
    ASSERT_EQ(cpu->getRegister(Register::PC), irq_vector) << "PC not set to interrupt vector";
    
    // Verify stack pointer decremented by 3 bytes (PC high, PC low, PS)
    ASSERT_EQ(cpu->getRegister(Register::SP), 0xFC) << "Stack pointer incorrect after BRK";
    
    // Verify I flag is set after BRK
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "Interrupt disable flag not set after BRK";
    
    // Check stack contents
    verifyBRKStackContents(initial_pc, initial_flags);
}

TEST(BRKTest, PreservesProcessorFlags) {
    SCOPED_TRACE("Testing that processor flags are preserved in stack");
    setup();
    
    // Set up initial CPU state with various flags set
    setPC(0x2000);
    
    // Clear all flags first to ensure a known state
    setPS(0);
    
    // Set specific flags individually 
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true);
    
    // Get the actual status register value after setting flags
    uint8_t actual_flags = cpu->getRegister(Register::PS);
    
    // Set interrupt vector
    mem->writeWord(0xFFFE, 0x5000);
    
    // Execute BRK
    std::unique_ptr<BRK> brk(new BRK(cpu));
    brk->run();


    // Check status register on stack
    uint8_t stored_ps = cpu->popByte();
    
    // Pop PC
    cpu->popWord();
    
    
    // Print values for debugging
    std::cout << "Original status register: 0x" << std::hex << (int)actual_flags << std::endl;
    std::cout << "Status pushed to stack: 0x" << std::hex << (int)stored_ps << std::endl;
    
    // Expected: original flags + B flag + bit 5
    uint8_t expected = actual_flags | Flag::B | 0x20;
    ASSERT_EQ(stored_ps, expected) << "Status register on stack doesn't match expected value";
    
    // Also verify that individual flags were preserved
    ASSERT_TRUE(stored_ps & Flag::C) << "C flag not preserved";
    ASSERT_TRUE(stored_ps & Flag::Z) << "Z flag not preserved";
    ASSERT_TRUE(stored_ps & Flag::D) << "D flag not preserved";
    ASSERT_TRUE(stored_ps & Flag::V) << "V flag not preserved";
    ASSERT_TRUE(stored_ps & Flag::N) << "N flag not preserved";
    ASSERT_TRUE(stored_ps & Flag::B) << "B flag not set in stack";
    ASSERT_TRUE(stored_ps & 0x20) << "Bit 5 not set in stack";
}

TEST(BRKTest, StackWraparound) {
    SCOPED_TRACE("Testing stack pointer wraparound");
    setup();
    
    // Set stack pointer to wrap around
    setSP(1);  // Will wrap to 0xFE after pushing 3 bytes
    setPC(0x3000);
    
    // Set interrupt vector
    mem->writeWord(0xFFFE, 0x4000);
    
    // Execute BRK
    std::unique_ptr<BRK> brk(new BRK(cpu));
    brk->run();
    
    // Verify stack pointer wrapped correctly
    ASSERT_EQ(cpu->getRegister(Register::SP), 0xFE) << "Stack pointer didn't wrap correctly";
    
    // Verify PC set to interrupt vector
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x4000) << "PC not set to interrupt vector";
}

TEST(BRKTest, IgnoresInterruptDisableFlag) {
    SCOPED_TRACE("Testing BRK works even when I flag is set");
    setup();
    
    // Set I flag before BRK
    setPC(0x1000);
    cpu->setFlag(Flag::I, true);
    
    // Set interrupt vector
    mem->writeWord(0xFFFE, 0x2000);
    
    // Execute BRK
    std::unique_ptr<BRK> brk(new BRK(cpu));
    brk->run();
    
    // BRK should still work despite I flag
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x2000) << "BRK didn't execute with I flag set";
    
    // And I flag should still be set after BRK
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "I flag not set after BRK";
}

TEST(BRKTest, MultipleVectors) {
    // Test with different IRQ vector values
    std::vector<uint16_t> vectors = {0x1234, 0xABCD, 0xFFFF, 0x0000};
    
    for (uint16_t vector : vectors) {
        SCOPED_TRACE("Testing interrupt vector: 0x" + std::to_string(vector));
        
        setup();
        setPC(0x1000);
        mem->writeWord(0xFFFE, vector);
        
        std::unique_ptr<BRK> brk(new BRK(cpu));
        brk->run();
        
        ASSERT_EQ(cpu->getRegister(Register::PC), vector) << "PC not set to correct interrupt vector";
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}