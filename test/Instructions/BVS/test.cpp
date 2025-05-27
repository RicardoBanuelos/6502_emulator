#include <gtest/gtest.h>
#include "BVS.h"
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
void setV(bool value) { cpu->setFlag(Flag::V, value); }

// Test case parameters: overflow_flag, offset, extra_cycle
using BVSTestCase = std::tuple<bool, int8_t, bool>;

class BVSTest : public ::testing::TestWithParam<BVSTestCase> {
protected:
    void SetUp() override {
        setup();
    }
};

// Generate comprehensive test cases for branch instruction
std::vector<BVSTestCase> generateTestCases() {
    std::vector<BVSTestCase> cases;
    
    // Test branching (when overflow flag set)
    cases.push_back(std::make_tuple(true, 10, false));    // Forward branch, no page cross
    cases.push_back(std::make_tuple(true, -10, false));   // Backward branch, no page cross
    cases.push_back(std::make_tuple(true, 127, true));    // Maximum forward branch
    cases.push_back(std::make_tuple(true, -128, true));   // Maximum backward branch
    
    // Test no branching (when overflow flag clear)
    cases.push_back(std::make_tuple(false, 10, false));   // No branch, overflow flag clear
    cases.push_back(std::make_tuple(false, -10, false));  // No branch, overflow flag clear
    
    // Test page crossing (adds 1 cycle)
    cases.push_back(std::make_tuple(true, 5, true));      // Cross page boundary forward
    cases.push_back(std::make_tuple(true, -5, true));     // Cross page boundary backward
    
    return cases;
}

TEST_P(BVSTest, BranchTest) {
    auto [overflow_flag, offset, page_cross] = GetParam();
    
    setup();
    
    // Set up starting position
    uint16_t start_pc = page_cross ? 0x00FD : 0x1000;  // Position for page cross testing
    setPC(start_pc);
    setV(overflow_flag);
    
    // Set up memory with the branch offset
    mem->writeByte(start_pc, offset);
    
    // Calculate expected PC
    uint16_t expected_pc;
    if (!overflow_flag) {
        // If V flag clear, branch not taken, only increment PC by 1
        expected_pc = start_pc + 1;
    } else {
        // If V flag set, branch taken
        expected_pc = start_pc + 1 + offset;
    }
    
    // Determine expected cycle count
    uint8_t cycles = 2;  // Base cycles
    if (overflow_flag) {
        cycles++;  // +1 if branch taken
        if (page_cross) {
            cycles++;  // +1 if page boundary crossed
        }
    }
    
    // Run the instruction
    std::unique_ptr<BVS> bvs(new BVS(cpu, AddressingMode::Relative, cycles));
    bvs->run();
    
    // Check if PC is as expected
    ASSERT_EQ(cpu->getRegister(Register::PC), expected_pc);
}

// Test specifically for page boundary crossing calculations
TEST(BVSTest, PageBoundaryCrossing) {
    // Test forward page crossing
    setup();
    setPC(0x10FF);  // End of page
    setV(true);     // Ensure branch is taken
    mem->writeByte(0x10FF, 5);  // Branch forward 5 bytes
    
    std::unique_ptr<BVS> bvs1(new BVS(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bvs1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1105); // 0x10FF + 1 + 5 = 0x1105
    
    // Test backward page crossing
    setup();
    setPC(0x1000);  // Start of page
    setV(true);     // Ensure branch is taken
    mem->writeByte(0x1000, -5);  // Branch backward 5 bytes
    
    std::unique_ptr<BVS> bvs2(new BVS(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bvs2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0FFC); // 0x1000 + 1 - 5 = 0x0FFC
}

// Test for no branch when overflow flag is clear
TEST(BVSTest, NoBranchWhenOverflowFlagClear) {
    setup();
    setPC(0x1000);
    setV(false);    // Clear overflow flag, should not branch
    mem->writeByte(0x1000, 10);  // This offset should be ignored
    
    std::unique_ptr<BVS> bvs(new BVS(cpu, AddressingMode::Relative, 2)); // Only 2 cycles when not branching
    bvs->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1001); // Only increment by 1 to skip offset
}

// Test edge cases for branch offsets
TEST(BVSTest, BranchOffsetEdgeCases) {
    // Test maximum positive offset
    setup();
    setPC(0x1000);
    setV(true);     // Ensure branch is taken
    mem->writeByte(0x1000, 127);  // Maximum positive offset
    
    std::unique_ptr<BVS> bvs1(new BVS(cpu, AddressingMode::Relative, 3));
    bvs1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1080); // 0x1000 + 1 + 127 = 0x1080
    
    // Test maximum negative offset
    setup();
    setPC(0x1000);
    setV(true);     // Ensure branch is taken
    mem->writeByte(0x1000, -128);  // Maximum negative offset (0x80)
    
    std::unique_ptr<BVS> bvs2(new BVS(cpu, AddressingMode::Relative, 3));
    bvs2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0F81); // 0x1000 + 1 - 128 = 0x0F81
}

// Test with other status flags set to ensure they don't affect BVS behavior
TEST(BVSTest, OtherFlagsIndependence) {
    // Set all other flags, V flag set - should branch
    setup();
    setPC(0x1000);
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::N, true);
    setV(true);  // This is the only flag that matters for BVS
    
    mem->writeByte(0x1000, 10);
    
    std::unique_ptr<BVS> bvs1(new BVS(cpu, AddressingMode::Relative, 3));
    bvs1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x100B); // Should branch
    
    // Set all other flags, V flag clear - should not branch
    setup();
    setPC(0x1000);
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::N, true);
    setV(false);  // This is the only flag that matters for BVS
    
    mem->writeByte(0x1000, 10);
    
    std::unique_ptr<BVS> bvs2(new BVS(cpu, AddressingMode::Relative, 2));
    bvs2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1001); // Should not branch
}

// Test for a common arithmetic operation followed by BVS pattern
TEST(BVSTest, CommonArithmeticPattern) {
    setup();
    setPC(0x1000);
    
    // First simulate an arithmetic operation that sets overflow flag
    setV(true);  // As if an addition or subtraction caused overflow
    
    mem->writeByte(0x1000, 20);  // Branch offset
    
    std::unique_ptr<BVS> bvs(new BVS(cpu, AddressingMode::Relative, 3));
    bvs->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1015); // Should branch
}

INSTANTIATE_TEST_SUITE_P(
    BVS,
    BVSTest,
    ::testing::ValuesIn(generateTestCases())
);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}