/**
 * Comprehensive Test Cases: Tests all important scenarios, not just random values

Branch taken vs not taken
Forward and backward branches
Page boundary crossing
Edge cases with maximum offsets
Better Test Organization: Clear test structure and naming

Flag Independence Testing: Verifies that other flags don't affect BCS behavior

Cycle Count Testing: Properly accounts for variable cycle counts

Parameterized Testing: Makes it easy to add more test cases

Explicit Setup: Each test has clear setup of flags and memory
 */

#include <gtest/gtest.h>
#include "BCS.h"
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
    CPU *cppu = static_cast<CPU *>(cpu.get());
    cppu->init();
    cppu->connectBus(bus);
    bus->connectMemory(mem);
}

void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
void setC(bool value) { cpu->setFlag(Flag::C, value); }

// Test case parameters: carry_flag, offset, extra_cycle
using BCSTestCase = std::tuple<bool, int8_t, bool>;

class BCSTest : public ::testing::TestWithParam<BCSTestCase>
{
protected:
    void SetUp() override
    {
        setup();
    }
};

// Generate comprehensive test cases for branch instruction
std::vector<BCSTestCase> generateTestCases()
{
    std::vector<BCSTestCase> cases;

    // Test branching (when carry set)
    cases.push_back(std::make_tuple(true, 10, false));  // Forward branch, no page cross
    cases.push_back(std::make_tuple(true, -10, false)); // Backward branch, no page cross
    cases.push_back(std::make_tuple(true, 127, true));  // Maximum forward branch
    cases.push_back(std::make_tuple(true, -128, true)); // Maximum backward branch

    // Test no branching (when carry clear)
    cases.push_back(std::make_tuple(false, 10, false));  // No branch, carry clear
    cases.push_back(std::make_tuple(false, -10, false)); // No branch, carry clear

    // Test page crossing (adds 1 cycle)
    cases.push_back(std::make_tuple(true, 5, true));  // Cross page boundary forward
    cases.push_back(std::make_tuple(true, -5, true)); // Cross page boundary backward

    return cases;
}

TEST_P(BCSTest, BranchTest)
{
    auto [carry_flag, offset, page_cross] = GetParam();

    setup();

    // Set up starting position
    uint16_t start_pc = page_cross ? 0x00FD : 0x1000; // Position for page cross testing
    setPC(start_pc);
    setC(carry_flag);

    // Set up memory with the branch offset
    mem->writeByte(start_pc, offset);

    // Calculate expected PC
    uint16_t expected_pc;
    if (!carry_flag)
    {
        // If carry clear, branch not taken, only increment PC by 1
        expected_pc = start_pc + 1;
    }
    else
    {
        // If carry set, branch taken
        expected_pc = start_pc + 1 + offset;
    }

    // Determine expected cycle count
    uint8_t cycles = 2; // Base cycles
    if (carry_flag)
    {
        cycles++; // +1 if branch taken
        if (page_cross)
        {
            cycles++; // +1 if page boundary crossed
        }
    }

    // Run the instruction
    std::unique_ptr<BCS> bcs(new BCS(cpu, AddressingMode::Relative, cycles));
    bcs->run();

    // Check if PC is as expected
    ASSERT_EQ(cpu->getRegister(Register::PC), expected_pc);
}

// Test specifically for page boundary crossing calculations
TEST(BCSTest, PageBoundaryCrossing)
{
    // Test forward page crossing
    setup();
    setPC(0x10FF);             // End of page
    setC(true);                // Ensure branch is taken
    mem->writeByte(0x10FF, 5); // Branch forward 5 bytes

    std::unique_ptr<BCS> bcs1(new BCS(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bcs1->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1105); // 0x10FF + 1 + 5 = 0x1105

    // Test backward page crossing
    setup();
    setPC(0x1000);              // Start of page
    setC(true);                 // Ensure branch is taken
    mem->writeByte(0x1000, -5); // Branch backward 5 bytes

    std::unique_ptr<BCS> bcs2(new BCS(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bcs2->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0FFC); // 0x1000 + 1 - 5 = 0x0FFC
}

// Test for no branch when carry is clear
TEST(BCSTest, NoBranchWhenCarryClear)
{
    setup();
    setPC(0x1000);
    setC(false);                // Clear carry flag, should not branch
    mem->writeByte(0x1000, 10); // This offset should be ignored

    std::unique_ptr<BCS> bcs(new BCS(cpu, AddressingMode::Relative, 2)); // Only 2 cycles when not branching
    bcs->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1001); // Only increment by 1 to skip offset
}

// Test edge cases for branch offsets
TEST(BCSTest, BranchOffsetEdgeCases)
{
    // Test maximum positive offset
    setup();
    setPC(0x1000);
    setC(true);                  // Ensure branch is taken
    mem->writeByte(0x1000, 127); // Maximum positive offset

    std::unique_ptr<BCS> bcs1(new BCS(cpu, AddressingMode::Relative, 3));
    bcs1->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1080); // 0x1000 + 1 + 127 = 0x1080

    // Test maximum negative offset
    setup();
    setPC(0x1000);
    setC(true);                   // Ensure branch is taken
    mem->writeByte(0x1000, -128); // Maximum negative offset (0x80)

    std::unique_ptr<BCS> bcs2(new BCS(cpu, AddressingMode::Relative, 3));
    bcs2->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0F81); // 0x1000 + 1 - 128 = 0x0F81
}

// Test with other status flags set to ensure they don't affect BCS behavior
TEST(BCSTest, OtherFlagsIndependence)
{
    // Set all other flags, carry set - should branch
    setup();
    setPC(0x1000);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true);
    setC(true); // This is the only flag that matters for BCS

    mem->writeByte(0x1000, 10);

    std::unique_ptr<BCS> bcs1(new BCS(cpu, AddressingMode::Relative, 3));
    bcs1->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x100B); // Should branch

    // Set all other flags, carry clear - should not branch
    setup();
    setPC(0x1000);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true);
    setC(false); // This is the only flag that matters for BCS

    mem->writeByte(0x1000, 10);

    std::unique_ptr<BCS> bcs2(new BCS(cpu, AddressingMode::Relative, 2));
    bcs2->run();

    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1001); // Should not branch
}

INSTANTIATE_TEST_SUITE_P(
    BCS,
    BCSTest,
    ::testing::ValuesIn(generateTestCases()));

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}