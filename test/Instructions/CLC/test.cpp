#include <gtest/gtest.h>
#include "CLC.h"
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

// Test clearing carry flag when it's set
TEST(CLCTest, ClearSetCarryFlag) {
    SCOPED_TRACE("Testing CLC instruction when carry flag is set");
    setup();
    
    // Set carry flag
    cpu->setFlag(Flag::C, true);
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Setup failed: Carry flag not set initially";
    
    // Execute CLC
    std::unique_ptr<CLC> clc(new CLC(cpu));
    clc->run();
    
    // Check if carry flag is cleared
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "CLC failed: Carry flag not cleared";
}

// Test clearing carry flag when it's already clear
TEST(CLCTest, ClearClearedCarryFlag) {
    SCOPED_TRACE("Testing CLC instruction when carry flag is already clear");
    setup();
    
    // Clear carry flag
    cpu->setFlag(Flag::C, false);
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "Setup failed: Carry flag not clear initially";
    
    // Execute CLC
    std::unique_ptr<CLC> clc(new CLC(cpu));
    clc->run();
    
    // Check if carry flag is still cleared
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "CLC failed: Carry flag changed when already clear";
}

// Test that CLC doesn't affect other flags
TEST(CLCTest, DoesNotAffectOtherFlags) {
    SCOPED_TRACE("Testing CLC instruction doesn't affect other flags");
    setup();
    
    // Set all flags
    uint8_t original_ps = 0xFF;
    cpu->setRegister(Register::PS, original_ps);
    
    // Execute CLC
    std::unique_ptr<CLC> clc(new CLC(cpu));
    clc->run();
    
    // Check if only carry flag is cleared, others remain set
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "CLC failed: Carry flag not cleared";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "CLC affected Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "CLC affected Interrupt flag";
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "CLC affected Decimal flag";
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "CLC affected Break flag";
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "CLC affected Overflow flag";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "CLC affected Negative flag";
}

// Test that CLC uses the correct number of cycles
TEST(CLCTest, CycleCount) {
    SCOPED_TRACE("Testing CLC instruction uses correct cycle count");
    setup();
    
    // CLC is always 2 cycles
    std::unique_ptr<CLC> clc(new CLC(cpu));
    
    // If the implementation tracks cycles, you could verify that here
    // This depends on how your CPU implementation handles cycle counting
    // For example: cpu->getCycles() before and after, or checking internally
    
    // No assertions in this test unless your CPU tracks cycles
}

// Test CLC in a typical ADC sequence
TEST(CLCTest, TypicalUsageWithADC) {
    SCOPED_TRACE("Testing CLC in typical ADC sequence");
    setup();
    
    // Set accumulator and memory for addition
    cpu->setRegister(Register::A, 0x30);
    mem->writeByte(0x0200, 0x42);
    
    // Set carry flag (as if from previous operation)
    cpu->setFlag(Flag::C, true);
    
    // Execute CLC to clear carry before ADC
    std::unique_ptr<CLC> clc(new CLC(cpu));
    clc->run();
    
    // Verify carry is cleared before ADC would execute
    ASSERT_FALSE(cpu->getFlag(Flag::C)) << "CLC failed to clear carry before ADC";
    
    // In a real program, ADC would execute here
    // We're just checking the state is set up correctly for it
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}