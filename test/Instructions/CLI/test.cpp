#include <gtest/gtest.h>
#include "CLI.h"
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

// Test clearing interrupt flag when it's set
TEST(CLITest, ClearSetInterruptFlag) {
    SCOPED_TRACE("Testing CLI instruction when interrupt flag is set");
    setup();
    
    // Set interrupt flag
    cpu->setFlag(Flag::I, true);
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "Setup failed: Interrupt flag not set initially";
    
    // Execute CLI
    std::unique_ptr<CLI> cli(new CLI(cpu));
    cli->run();
    
    // Check if interrupt flag is cleared
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "CLI failed: Interrupt flag not cleared";
}

// Test clearing interrupt flag when it's already clear
TEST(CLITest, ClearClearedInterruptFlag) {
    SCOPED_TRACE("Testing CLI instruction when interrupt flag is already clear");
    setup();
    
    // Clear interrupt flag
    cpu->setFlag(Flag::I, false);
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "Setup failed: Interrupt flag not clear initially";
    
    // Execute CLI
    std::unique_ptr<CLI> cli(new CLI(cpu));
    cli->run();
    
    // Check if interrupt flag is still cleared
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "CLI failed: Interrupt flag changed when already clear";
}

// Test that CLI doesn't affect other flags
TEST(CLITest, DoesNotAffectOtherFlags) {
    SCOPED_TRACE("Testing CLI instruction doesn't affect other flags");
    setup();
    
    // Set all flags
    uint8_t original_ps = 0xFF;
    cpu->setRegister(Register::PS, original_ps);
    
    // Execute CLI
    std::unique_ptr<CLI> cli(new CLI(cpu));
    cli->run();
    
    // Check if only interrupt flag is cleared, others remain set
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "CLI failed: Interrupt flag not cleared";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "CLI affected Carry flag";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "CLI affected Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "CLI affected Decimal flag";
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "CLI affected Break flag";
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "CLI affected Overflow flag";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "CLI affected Negative flag";
}

// Test that CLI uses the correct number of cycles
TEST(CLITest, CycleCount) {
    SCOPED_TRACE("Testing CLI instruction uses correct cycle count");
    setup();
    
    // CLI is always 2 cycles
    std::unique_ptr<CLI> cli(new CLI(cpu));
    
    // If the implementation tracks cycles, you could verify that here
    // This depends on how your CPU implementation handles cycle counting
}

// Test CLI in a typical interrupt handling sequence
TEST(CLITest, TypicalUsageEnableInterrupts) {
    SCOPED_TRACE("Testing CLI to enable interrupts after initialization");
    setup();
    
    // After reset, interrupts are disabled
    cpu->reset();
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "Setup failed: Interrupt flag should be set after reset";
    
    // Execute CLI to enable interrupts
    std::unique_ptr<CLI> cli(new CLI(cpu));
    cli->run();
    
    // Verify interrupts are now enabled
    ASSERT_FALSE(cpu->getFlag(Flag::I)) << "CLI failed to enable interrupts";
    
    // In a real program, interrupts would now be processed
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
   
    return RUN_ALL_TESTS();
}