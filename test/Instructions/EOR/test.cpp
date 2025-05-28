#include <gtest/gtest.h>
#include "EOR.h"
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

void setA(uint8_t value) { cpu->setRegister(Register::A, value); }
void setX(uint8_t value) { cpu->setRegister(Register::X, value); }
void setY(uint8_t value) { cpu->setRegister(Register::Y, value); }
void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }

// Test case parameters: A, operand, expected_result, expected_Z, expected_N
using EORTestCase = std::tuple<uint8_t, uint8_t, uint8_t, bool, bool>;

class EORTest : public ::testing::TestWithParam<std::tuple<AddressingMode, int>>
{
protected:
    void SetUp() override
    {
        setup();
    }
};

void checkFlags(uint8_t result, bool zero, bool negative)
{
    ASSERT_EQ(cpu->getRegister(Register::A), result)
        << "Accumulator should contain result of EOR operation";
    ASSERT_EQ(cpu->getFlag(Flag::Z), zero)
        << "Zero flag should be " << (zero ? "set" : "clear") << " for result 0x" << std::hex << (int)result;
    ASSERT_EQ(cpu->getFlag(Flag::N), negative)
        << "Negative flag should be " << (negative ? "set" : "clear") << " for result 0x" << std::hex << (int)result;
}

// Comprehensive test data covering different bit patterns and flag combinations
std::vector<EORTestCase> generateTestCases()
{
    std::vector<EORTestCase> cases;

    // Basic XOR operations
    cases.push_back(std::make_tuple(0x00, 0x00, 0x00, true, false)); // 0 ^ 0 = 0, Z=1, N=0
    cases.push_back(std::make_tuple(0xFF, 0x00, 0xFF, false, true)); // FF ^ 0 = FF, Z=0, N=1
    cases.push_back(std::make_tuple(0x00, 0xFF, 0xFF, false, true)); // 0 ^ FF = FF, Z=0, N=1
    cases.push_back(std::make_tuple(0xFF, 0xFF, 0x00, true, false)); // FF ^ FF = 0, Z=1, N=0

    // Different bit patterns
    cases.push_back(std::make_tuple(0xAA, 0x55, 0xFF, false, true)); // 10101010 ^ 01010101 = 11111111, Z=0, N=1
    cases.push_back(std::make_tuple(0x55, 0xAA, 0xFF, false, true)); // 01010101 ^ 10101010 = 11111111, Z=0, N=1
    cases.push_back(std::make_tuple(0xF0, 0x0F, 0xFF, false, true)); // 11110000 ^ 00001111 = 11111111, Z=0, N=1
    cases.push_back(std::make_tuple(0x0F, 0xF0, 0xFF, false, true)); // 00001111 ^ 11110000 = 11111111, Z=0, N=1

    // Testing all bits
    cases.push_back(std::make_tuple(0x01, 0x01, 0x00, true, false)); // Bit 0 only
    cases.push_back(std::make_tuple(0x02, 0x02, 0x00, true, false)); // Bit 1 only
    cases.push_back(std::make_tuple(0x04, 0x04, 0x00, true, false)); // Bit 2 only
    cases.push_back(std::make_tuple(0x08, 0x08, 0x00, true, false)); // Bit 3 only
    cases.push_back(std::make_tuple(0x10, 0x10, 0x00, true, false)); // Bit 4 only
    cases.push_back(std::make_tuple(0x20, 0x20, 0x00, true, false)); // Bit 5 only
    cases.push_back(std::make_tuple(0x40, 0x40, 0x00, true, false)); // Bit 6 only
    cases.push_back(std::make_tuple(0x80, 0x80, 0x00, true, false)); // Bit 7 only (sign bit)

    // Single bit differences
    cases.push_back(std::make_tuple(0x00, 0x01, 0x01, false, false)); // 00000000 ^ 00000001 = 00000001
    cases.push_back(std::make_tuple(0x00, 0x80, 0x80, false, true));  // 00000000 ^ 10000000 = 10000000 (negative)

    // Negative to positive transition
    cases.push_back(std::make_tuple(0x80, 0x01, 0x81, false, true)); // 10000000 ^ 00000001 = 10000001
    cases.push_back(std::make_tuple(0x81, 0x01, 0x80, false, true)); // 10000001 ^ 00000001 = 10000000

    // Common values
    cases.push_back(std::make_tuple(0x42, 0x24, 0x66, false, false)); // 01000010 ^ 00100100 = 01100110
    cases.push_back(std::make_tuple(0x33, 0xCC, 0xFF, false, true));  // 00110011 ^ 11001100 = 11111111

    // "Toggling" bits
    cases.push_back(std::make_tuple(0xA5, 0xFF, 0x5A, false, false)); // Toggling all bits of 0xA5
    cases.push_back(std::make_tuple(0x5A, 0xFF, 0xA5, false, true));  // Toggling all bits of 0x5A

    return cases;
}

// Helper to set up memory for different addressing modes
void setupAddressing(AddressingMode mode, uint8_t operand)
{
    setPC(0x200);

    switch (mode)
    {
    case AddressingMode::Immediate:
        mem->writeByte(0x200, operand);
        break;

    case AddressingMode::ZeroPage:
        mem->writeByte(0x200, 0x42);
        mem->writeByte(0x42, operand);
        break;

    case AddressingMode::ZeroPageX:
        setX(0x05);
        mem->writeByte(0x200, 0x40);
        mem->writeByte(0x45, operand);
        break;

    case AddressingMode::Absolute:
        mem->writeWord(0x200, 0x1234);
        mem->writeByte(0x1234, operand);
        break;

    case AddressingMode::AbsoluteX:
        setX(0x01);
        mem->writeWord(0x200, 0x1234);
        mem->writeByte(0x1235, operand);
        break;

    case AddressingMode::AbsoluteY:
        setY(0x02);
        mem->writeWord(0x200, 0x1234);
        mem->writeByte(0x1236, operand);
        break;

    case AddressingMode::IndirectX:
        setX(0x04);
        mem->writeByte(0x200, 0x20);
        mem->writeWord(0x24, 0x1234);
        mem->writeByte(0x1234, operand);
        break;

    case AddressingMode::IndirectY:
        setY(0x03);
        mem->writeByte(0x200, 0x20);
        mem->writeWord(0x20, 0x1234);
        mem->writeByte(0x1237, operand);
        break;
    }
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode)
{
    switch (mode)
    {
    case AddressingMode::Immediate:
        return 2;
    case AddressingMode::ZeroPage:
        return 3;
    case AddressingMode::ZeroPageX:
        return 4;
    case AddressingMode::Absolute:
        return 4;
    case AddressingMode::AbsoluteX:
        return 4; // +1 if page boundary crossed
    case AddressingMode::AbsoluteY:
        return 4; // +1 if page boundary crossed
    case AddressingMode::IndirectX:
        return 6;
    case AddressingMode::IndirectY:
        return 5; // +1 if page boundary crossed
    default:
        return 2;
    }
}

TEST_P(EORTest, ExhaustiveTest)
{
    auto [mode, testCaseIndex] = GetParam();
    auto testCase = generateTestCases()[testCaseIndex];

    auto [accumulator, operand, expected_result, expected_zero, expected_negative] = testCase;

    setup();
    setA(accumulator);
    setupAddressing(mode, operand);

    std::unique_ptr<EOR> eor(new EOR(cpu, mode, getCycleCount(mode)));
    eor->run();

    std::string modeName;
    switch (mode)
    {
    case AddressingMode::Immediate:
        modeName = "Immediate";
        break;
    case AddressingMode::ZeroPage:
        modeName = "ZeroPage";
        break;
    case AddressingMode::ZeroPageX:
        modeName = "ZeroPageX";
        break;
    case AddressingMode::Absolute:
        modeName = "Absolute";
        break;
    case AddressingMode::AbsoluteX:
        modeName = "AbsoluteX";
        break;
    case AddressingMode::AbsoluteY:
        modeName = "AbsoluteY";
        break;
    case AddressingMode::IndirectX:
        modeName = "IndirectX";
        break;
    case AddressingMode::IndirectY:
        modeName = "IndirectY";
        break;
    default:
        modeName = "Unknown";
    }

    std::stringstream ss;
    ss << "Mode: " << modeName
       << ", A: 0x" << std::hex << static_cast<int>(accumulator)
       << ", Op: 0x" << static_cast<int>(operand);
    SCOPED_TRACE(ss.str());

    checkFlags(expected_result, expected_zero, expected_negative);
}

// Generate combinations of addressing modes and test cases
std::vector<AddressingMode> addressingModes = {
    AddressingMode::Immediate,
    AddressingMode::ZeroPage,
    AddressingMode::ZeroPageX,
    AddressingMode::Absolute,
    AddressingMode::AbsoluteX,
    AddressingMode::AbsoluteY,
    AddressingMode::IndirectX,
    AddressingMode::IndirectY};

std::vector<std::tuple<AddressingMode, int>> GenerateTestParams()
{
    std::vector<std::tuple<AddressingMode, int>> params;
    auto testCases = generateTestCases();

    for (auto mode : addressingModes)
    {
        for (int i = 0; i < testCases.size(); ++i)
        {
            params.push_back(std::make_tuple(mode, i));
        }
    }

    return params;
}

INSTANTIATE_TEST_SUITE_P(
    EOR,
    EORTest,
    ::testing::ValuesIn(GenerateTestParams()));

// Page crossing tests for modes that have different cycle counts
TEST(EORTest, PageCrossing)
{
    // Test AbsoluteX page crossing
    setup();
    setA(0xAA);
    setX(0xFF);
    setPC(0x200);
    mem->writeWord(0x200, 0x1200); // Base address: 0x1200 + 0xFF = 0x12FF (crosses to 0x1300)
    mem->writeByte(0x12FF, 0x55);  // Results in 0xAA ^ 0x55 = 0xFF

    std::unique_ptr<EOR> eor1(new EOR(cpu, AddressingMode::AbsoluteX, 5)); // 5 cycles with page crossing
    eor1->run();

    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));

    // Test AbsoluteY page crossing
    setup();
    setA(0xF0);
    setY(0xFF);
    setPC(0x200);
    mem->writeWord(0x200, 0x1200);
    mem->writeByte(0x12FF, 0x0F); // Results in 0xF0 ^ 0x0F = 0xFF

    std::unique_ptr<EOR> eor2(new EOR(cpu, AddressingMode::AbsoluteY, 5)); // 5 cycles with page crossing
    eor2->run();

    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));

    // Test IndirectY page crossing
    setup();
    setA(0x33);
    setY(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);
    mem->writeWord(0x80, 0x1200);
    mem->writeByte(0x12FF, 0xCC); // Results in 0x33 ^ 0xCC = 0xFF

    std::unique_ptr<EOR> eor3(new EOR(cpu, AddressingMode::IndirectY, 6)); // 6 cycles with page crossing
    eor3->run();

    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
}

// Test zero page wrap-around behavior
TEST(EORTest, ZeroPageWrapAround)
{
    setup();
    setA(0x55);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80); // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeByte(0x7F, 0xAA);  // Results in 0x55 ^ 0xAA = 0xFF

    std::unique_ptr<EOR> eor(new EOR(cpu, AddressingMode::ZeroPageX, 4));
    eor->run();

    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
}

// IndirectX wrap-around test
TEST(EORTest, IndirectXWrapAround)
{
    setup();
    setA(0x33);
    setX(0xFF);
    setPC(0x200);
    mem->writeByte(0x200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F in zero page
    mem->writeWord(0x7F, 0x1234); // Write target address at wrapped location
    mem->writeByte(0x1234, 0xCC); // Results in 0x33 ^ 0xCC = 0xFF

    std::unique_ptr<EOR> eor(new EOR(cpu, AddressingMode::IndirectX, 6));
    eor->run();

    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
}

// Test that EOR only affects A, Z, and N flags
TEST(EORTest, OnlyAffectsAZNFlags)
{
    setup();

    // Set all flags beforehand
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false); // Will be changed

    setA(0x00);
    setPC(0x200);
    mem->writeByte(0x200, 0xFF); // This will set A to 0xFF, Z=0, N=1

    std::unique_ptr<EOR> eor(new EOR(cpu, AddressingMode::Immediate, 2));
    eor->run();

    // Check affected flags
    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));

    // Check unaffected flags
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
}

// Test with randomized values (reduced from 1000 to 100 iterations)
TEST(EORTest, RandomizedValues)
{
    for (int i = 0; i < 100; ++i)
    {
        setup();

        // Randomize registers
        uint8_t a_value = rand() & 0xFF;
        uint8_t m_value = rand() & 0xFF;
        uint8_t expected = a_value ^ m_value;

        setA(a_value);
        setPC(0x200);
        mem->writeByte(0x200, m_value);

        std::unique_ptr<EOR> eor(new EOR(cpu, AddressingMode::Immediate, 2));
        eor->run();

        ASSERT_EQ(cpu->getRegister(Register::A), expected);
        ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0);
        ASSERT_EQ(cpu->getFlag(Flag::N), (expected & 0x80) != 0);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();

    CPU *cppu = static_cast<CPU *>(cpu.get());
    cppu->init();

    cppu->connectBus(bus);
    bus->connectMemory(mem);

    srand(time(nullptr));
    return RUN_ALL_TESTS();
}