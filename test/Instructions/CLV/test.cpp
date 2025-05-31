#include <gtest/gtest.h>
#include "CLV.h"
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

// Test clearing overflow flag when it's set
TEST(CLVTest, ClearSetOverflowFlag) {
    SCOPED_TRACE("Testing CLV instruction when overflow flag is set");
    setup();
    
    // Set overflow flag
    cpu->setFlag(Flag::V, true);
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "Setup failed: Overflow flag not set initially";
    
    // Execute CLV
    std::unique_ptr<CLV> clv(new CLV(cpu));
    clv->run();
    
    // Check if overflow flag is cleared
    ASSERT_FALSE(cpu->getFlag(Flag::V)) << "CLV failed: Overflow flag not cleared";
}

// Test clearing overflow flag when it's already clear
TEST(CLVTest, ClearClearedOverflowFlag) {
    SCOPED_TRACE("Testing CLV instruction when overflow flag is already clear");
    setup();
    
    // Clear overflow flag
    cpu->setFlag(Flag::V, false);
    ASSERT_FALSE(cpu->getFlag(Flag::V)) << "Setup failed: Overflow flag not clear initially";
    
    // Execute CLV
    std::unique_ptr<CLV> clv(new CLV(cpu));
    clv->run();
    
    // Check if overflow flag is still cleared
    ASSERT_FALSE(cpu->getFlag(Flag::V)) << "CLV failed: Overflow flag changed when already clear";
}

// Test that CLV doesn't affect other flags
TEST(CLVTest, DoesNotAffectOtherFlags) {
    SCOPED_TRACE("Testing CLV instruction doesn't affect other flags");
    setup();
    
    // Set all flags
    uint8_t original_ps = 0xFF;
    cpu->setRegister(Register::PS, original_ps);
    
    // Execute CLV
    std::unique_ptr<CLV> clv(new CLV(cpu));
    clv->run();
    
    // Check if only overflow flag is cleared, others remain set
    ASSERT_FALSE(cpu->getFlag(Flag::V)) << "CLV failed: Overflow flag not cleared";
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "CLV affected Carry flag";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "CLV affected Zero flag";
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "CLV affected Interrupt flag";
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "CLV affected Decimal flag";
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "CLV affected Break flag";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "CLV affected Negative flag";
}

// Test that CLV uses the correct number of cycles
TEST(CLVTest, CycleCount) {
    SCOPED_TRACE("Testing CLV instruction uses correct cycle count");
    setup();
    
    // CLV is always 2 cycles
    std::unique_ptr<CLV> clv(new CLV(cpu));
    
    // If the implementation tracks cycles, you could verify that here
    // This depends on how your CPU implementation handles cycle counting
}

// Test CLV in a typical arithmetic sequence
TEST(CLVTest, TypicalUsageAfterArithmetic) {
    SCOPED_TRACE("Testing CLV after arithmetic operation");
    setup();
    
    // Simulate an arithmetic operation that set overflow flag
    // For example, adding 0x7F + 0x01 would overflow from positive to negative
    cpu->setFlag(Flag::V, true);
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "Setup failed: Could not set overflow flag";
    
    // Execute CLV to clear overflow flag
    std::unique_ptr<CLV> clv(new CLV(cpu));
    clv->run();
    
    // Verify overflow flag is cleared
    ASSERT_FALSE(cpu->getFlag(Flag::V)) << "CLV failed to clear overflow flag";
    
    // In a real program, arithmetic operations would continue
    // without the previous overflow affecting logic
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