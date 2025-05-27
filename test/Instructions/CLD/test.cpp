#include <gtest/gtest.h>
#include "CLD.h"
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

// Test clearing decimal flag when it's set
TEST(CLDTest, ClearSetDecimalFlag) {
    SCOPED_TRACE("Testing CLD instruction when decimal flag is set");
    setup();
    
    // Set decimal flag
    cpu->setFlag(Flag::D, true);
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "Setup failed: Decimal flag not set initially";
    
    // Execute CLD
    std::unique_ptr<CLD> cld(new CLD(cpu));
    cld->run();
    
    // Check if decimal flag is cleared
    ASSERT_FALSE(cpu->getFlag(Flag::D)) << "CLD failed: Decimal flag not cleared";
}

// Test clearing decimal flag when it's already clear
TEST(CLDTest, ClearClearedDecimalFlag) {
    SCOPED_TRACE("Testing CLD instruction when decimal flag is already clear");
    setup();
    
    // Clear decimal flag
    cpu->setFlag(Flag::D, false);
    ASSERT_FALSE(cpu->getFlag(Flag::D)) << "Setup failed: Decimal flag not clear initially";
    
    // Execute CLD
    std::unique_ptr<CLD> cld(new CLD(cpu));
    cld->run();
    
    // Check if decimal flag is still cleared
    ASSERT_FALSE(cpu->getFlag(Flag::D)) << "CLD failed: Decimal flag changed when already clear";
}

// Test that CLD doesn't affect other flags
TEST(CLDTest, DoesNotAffectOtherFlags) {
    SCOPED_TRACE("Testing CLD instruction doesn't affect other flags");
    setup();
    
    // Set all flags
    uint8_t original_ps = 0xFF;
    cpu->setRegister(Register::PS, original_ps);
    
    // Execute CLD
    std::unique_ptr<CLD> cld(new CLD(cpu));
    cld->run();
    
    // Check if only decimal flag is cleared, others remain set
    ASSERT_FALSE(cpu->getFlag(Flag::D)) << "CLD failed: Decimal flag not cleared";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "CLD affected Carry flag";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "CLD affected Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "CLD affected Interrupt flag";
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "CLD affected Break flag";
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "CLD affected Overflow flag";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "CLD affected Negative flag";
}

// Test that CLD uses the correct number of cycles
TEST(CLDTest, CycleCount) {
    SCOPED_TRACE("Testing CLD instruction uses correct cycle count");
    setup();
    
    // CLD is always 2 cycles
    std::unique_ptr<CLD> cld(new CLD(cpu));
    
    // If the implementation tracks cycles, you could verify that here
    // This depends on how your CPU implementation handles cycle counting
    // For example: cpu->getCycles() before and after, or checking internally
}

// Test CLD in binary mode conversion
TEST(CLDTest, TypicalUsageSwitchToBinary) {
    SCOPED_TRACE("Testing CLD to switch from decimal to binary mode");
    setup();
    
    // Set decimal mode
    cpu->setFlag(Flag::D, true);
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "Setup failed: Could not set decimal mode";
    
    // Execute CLD to switch to binary mode
    std::unique_ptr<CLD> cld(new CLD(cpu));
    cld->run();
    
    // Verify binary mode is active
    ASSERT_FALSE(cpu->getFlag(Flag::D)) << "CLD failed to switch to binary mode";
    
    // In a real program, arithmetic operations would now use binary mode
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