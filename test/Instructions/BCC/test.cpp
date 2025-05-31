#include <gtest/gtest.h>
#include "BCC.h"
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
void setC(bool value) { cpu->setFlag(Flag::C, value); }

// Test case parameters: carry_flag, offset, extra_cycle
using BCCTestCase = std::tuple<bool, int8_t, bool>;

class BCCTest : public ::testing::TestWithParam<BCCTestCase> {
protected:
    void SetUp() override {
        setup();
    }
};

// Generate comprehensive test cases for branch instruction
std::vector<BCCTestCase> generateTestCases() {
    std::vector<BCCTestCase> cases;
    
    // Test branching (when carry clear)
    cases.push_back(std::make_tuple(false, 10, false));    // Forward branch, no page cross
    cases.push_back(std::make_tuple(false, -10, false));   // Backward branch, no page cross
    cases.push_back(std::make_tuple(false, 127, true));    // Maximum forward branch
    cases.push_back(std::make_tuple(false, -128, true));   // Maximum backward branch
    
    // Test no branching (when carry set)
    cases.push_back(std::make_tuple(true, 10, false));     // No branch, carry set
    cases.push_back(std::make_tuple(true, -10, false));    // No branch, carry set
    
    // Test page crossing (adds 1 cycle)
    cases.push_back(std::make_tuple(false, 5, true));      // Cross page boundary forward
    cases.push_back(std::make_tuple(false, -5, true));     // Cross page boundary backward
    
    return cases;
}

TEST_P(BCCTest, BranchTest) {
    auto [carry_flag, offset, page_cross] = GetParam();
    
    setup();
    
    // Set up starting position
    uint16_t start_pc = page_cross ? 0x00FD : 0x1000;  // Position for page cross testing
    setPC(start_pc);
    setC(carry_flag);
    
    // Set up memory with the branch offset
    mem->writeByte(start_pc, offset);
    
    // Calculate expected PC
    uint16_t expected_pc;
    if (carry_flag) {
        // If carry set, branch not taken, only increment PC by 1
        expected_pc = start_pc + 1;
    } else {
        // If carry clear, branch taken
        expected_pc = start_pc + 1 + offset;
    }
    
    // Determine expected cycle count
    uint8_t cycles = 2;  // Base cycles
    if (!carry_flag) {
        cycles++;  // +1 if branch taken
        if (page_cross) {
            cycles++;  // +1 if page boundary crossed
        }
    }
    
    // Run the instruction
    std::unique_ptr<BCC> bcc(new BCC(cpu, AddressingMode::Relative, cycles));
    bcc->run();
    
    // Check if PC is as expected
    ASSERT_EQ(cpu->getRegister(Register::PC), expected_pc);
}

// Test specifically for page boundary crossing calculations
TEST(BCCTest, PageBoundaryCrossing) {
    // Test forward page crossing
    setup();
    setPC(0x10FF);  // End of page
    setC(false);    // Ensure branch is taken
    mem->writeByte(0x10FF, 5);  // Branch forward 5 bytes
    
    std::unique_ptr<BCC> bcc1(new BCC(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bcc1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1105); // 0x10FF + 1 + 5 = 0x1105
    
    // Test backward page crossing
    setup();
    setPC(0x1000);  // Start of page
    setC(false);    // Ensure branch is taken
    mem->writeByte(0x1000, -5);  // Branch backward 5 bytes
    
    std::unique_ptr<BCC> bcc2(new BCC(cpu, AddressingMode::Relative, 4)); // 4 cycles with page cross
    bcc2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0FFC); // 0x1000 + 1 - 5 = 0x0FFC
}

// Test for no branch when carry is set
TEST(BCCTest, NoBranchWhenCarrySet) {
    setup();
    setPC(0x1000);
    setC(true);     // Set carry flag, should not branch
    mem->writeByte(0x1000, 10);  // This offset should be ignored
    
    std::unique_ptr<BCC> bcc(new BCC(cpu, AddressingMode::Relative, 2)); // Only 2 cycles when not branching
    bcc->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1001); // Only increment by 1 to skip offset
}

// Test edge cases for branch offsets
TEST(BCCTest, BranchOffsetEdgeCases) {
    // Test maximum positive offset
    setup();
    setPC(0x1000);
    setC(false);    // Ensure branch is taken
    mem->writeByte(0x1000, 127);  // Maximum positive offset
    
    std::unique_ptr<BCC> bcc1(new BCC(cpu, AddressingMode::Relative, 3));
    bcc1->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x1080); // 0x1000 + 1 + 127 = 0x1080
    
    // Test maximum negative offset
    setup();
    setPC(0x1000);
    setC(false);    // Ensure branch is taken
    mem->writeByte(0x1000, -128);  // Maximum negative offset (0x80)
    
    std::unique_ptr<BCC> bcc2(new BCC(cpu, AddressingMode::Relative, 3));
    bcc2->run();
    
    ASSERT_EQ(cpu->getRegister(Register::PC), 0x0F81); // 0x1000 + 1 - 128 = 0x0F81
}

INSTANTIATE_TEST_SUITE_P(
    BCC,
    BCCTest,
    ::testing::ValuesIn(generateTestCases())
);

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}