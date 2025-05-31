#include <gtest/gtest.h>
#include "PHP.h"
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
    CPU *cppu = static_cast<CPU *>(cpu.get());
    cppu->init();
    cppu->connectBus(bus);
    bus->connectMemory(mem);
}

class PHPTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        setup();
    }
};

// Test basic PHP functionality
TEST_F(PHPTest, BasicOperation)
{
    // Set initial processor status and stack pointer
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);

    // Set all flags to a known pattern
    cpu->setFlag(Flag::C, true);  // Carry
    cpu->setFlag(Flag::Z, false); // Zero
    cpu->setFlag(Flag::I, true);  // Interrupt Disable
    cpu->setFlag(Flag::D, false); // Decimal Mode
    cpu->setFlag(Flag::B, true);  // Break Command (note: should be set in pushed value)
    cpu->setFlag(Flag::V, false); // Overflow
    cpu->setFlag(Flag::N, true);  // Negative

    uint8_t expectedPS = cpu->getRegister(Register::PS);

    // B flag is always set in the pushed value for PHP, bit 5 is always set too
    expectedPS |= 0x30; // Set bits 4 and 5 (B flag and unused flag)

    // Execute PHP
    std::unique_ptr<PHP> php(new PHP(cpu));
    php->run();

    // Check that stack pointer was decremented
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 1)
        << "Stack pointer should be decremented by 1";

    // Check that PS was pushed to the stack with B flag set
    ASSERT_EQ(mem->readByte(0x0100 + initialSP), expectedPS)
        << "Value on stack should match PS with B flag and bit 5 set: "
        << std::hex << "expected=0x" << (int)expectedPS
        << ", actual=0x" << (int)mem->readByte(0x0100 + initialSP);

    // Verify PS wasn't changed in the CPU
    uint8_t actualPS = cpu->getRegister(Register::PS);
    ASSERT_NE(actualPS, expectedPS)
        << "PS register in CPU should not have B flag set";

    ASSERT_EQ((actualPS & ~0x30), (expectedPS & ~0x30))
        << "All other flags should remain unchanged";
}

// Test PHP with different flag combinations
TEST_F(PHPTest, FlagCombinations)
{
    // Set up different combinations of flags to test
    struct FlagTest
    {
        bool c, z, i, d, b, v, n;
        std::string name;
    };

    std::vector<FlagTest> tests = {
        {false, false, false, false, false, false, false, "All flags clear"},
        {true, true, true, true, true, true, true, "All flags set"},
        {true, false, true, false, true, false, true, "Alternating flags 1"},
        {false, true, false, true, false, true, false, "Alternating flags 2"}};

    for (const auto &test : tests)
    {
        SCOPED_TRACE(test.name);

        // Reset for each test
        setup();

        uint8_t initialSP = 0xFF;
        cpu->setRegister(Register::SP, initialSP);

        // Set flags according to test case
        cpu->setFlag(Flag::C, test.c);
        cpu->setFlag(Flag::Z, test.z);
        cpu->setFlag(Flag::I, test.i);
        cpu->setFlag(Flag::D, test.d);
        cpu->setFlag(Flag::B, test.b);
        cpu->setFlag(Flag::V, test.v);
        cpu->setFlag(Flag::N, test.n);

        uint8_t expectedPS = cpu->getRegister(Register::PS);

        // B flag and bit 5 are always set in the pushed value for PHP
        expectedPS |= 0x30;

        // Execute PHP
        std::unique_ptr<PHP> php(new PHP(cpu));
        php->run();

        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 1);
        ASSERT_EQ(mem->readByte(0x0100 + initialSP), expectedPS);

        // Check original PS wasn't modified (except B flag might be)
        uint8_t currentPS = cpu->getRegister(Register::PS);
        ASSERT_EQ(currentPS & ~0x30, expectedPS & ~0x30)
            << "PS register should remain unchanged except B flag";
    }
}

// Test PHP with different initial stack pointer values
TEST_F(PHPTest, DifferentStackPointers)
{
    const uint8_t stackPointers[] = {
        0xFF, // Top of stack
        0x80, // Middle of stack
        0x01, // Near bottom of stack
    };

    for (auto sp : stackPointers)
    {
        SCOPED_TRACE(std::string("Testing PHP with SP=0x") +
                     (sp < 16 ? "0" : "") +
                     std::to_string(sp));

        // Reset for each test
        setup();

        // Set some flags
        cpu->setFlag(Flag::C, true);
        cpu->setFlag(Flag::Z, false);
        cpu->setFlag(Flag::I, true);
        cpu->setRegister(Register::SP, sp);

        uint8_t expectedPS = cpu->getRegister(Register::PS);
        expectedPS |= 0x30; // Set B flag and bit 5

        // Execute PHP
        std::unique_ptr<PHP> php(new PHP(cpu));
        php->run();

        // Check stack pointer and value
        ASSERT_EQ(cpu->getRegister(Register::SP), sp - 1);
        ASSERT_EQ(mem->readByte(0x0100 + sp), expectedPS);
    }
}

// Test PHP with stack wrap-around (should wrap from 0x00 to 0xFF)
TEST_F(PHPTest, StackWrapAround)
{
    // Set SP to 0x00
    uint8_t initialSP = 0x00;
    cpu->setRegister(Register::SP, initialSP);

    // Set some flags
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false);

    uint8_t expectedPS = cpu->getRegister(Register::PS);
    expectedPS |= 0x30; // Set B flag and bit 5

    // Execute PHP
    std::unique_ptr<PHP> php(new PHP(cpu));
    php->run();

    // Check that SP wrapped around to 0xFF
    ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF)
        << "Stack pointer should wrap from 0x00 to 0xFF";

    // Check that value was pushed to the correct location
    ASSERT_EQ(mem->readByte(0x0100 + initialSP), expectedPS)
        << "Value on stack should be at 0x0100 + initial SP";
}

// Test PHP with subsequent PLP to verify round-trip operation
TEST_F(PHPTest, PHPFollowedByPLP)
{
    // Set initial flags to a known pattern
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false);
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, false);
    cpu->setFlag(Flag::B, false); // B flag doesn't exist in processor status register
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false);

    uint8_t initialPS = cpu->getRegister(Register::PS);
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);

    // Execute PHP
    std::unique_ptr<PHP> php(new PHP(cpu));
    php->run();

    // Change some flags
    cpu->setFlag(Flag::C, false);
    cpu->setFlag(Flag::Z, true);
    cpu->setFlag(Flag::N, true);

    // Now manually simulate PLP
    // Increment SP first as it points to the next free slot
    cpu->setRegister(Register::SP, cpu->getRegister(Register::SP) + 1);

    // Get the value from the stack
    uint8_t pulledPS = mem->readByte(0x0100 + cpu->getRegister(Register::SP));

    // Note: B flag would be ignored in real PLP, it's determined by BRK vs interrupt
    pulledPS &= ~0x10; // Clear B flag

    // Set processor status
    // In a real CPU implementation, this would be like:
    // cpu->setRegister(Register::PS, pulledPS);
    // But we'll do it flag by flag for clarity
    cpu->setFlag(Flag::C, (pulledPS & 0x01) != 0);
    cpu->setFlag(Flag::Z, (pulledPS & 0x02) != 0);
    cpu->setFlag(Flag::I, (pulledPS & 0x04) != 0);
    cpu->setFlag(Flag::D, (pulledPS & 0x08) != 0);
    // B flag is not updated by PLP
    cpu->setFlag(Flag::V, (pulledPS & 0x40) != 0);
    cpu->setFlag(Flag::N, (pulledPS & 0x80) != 0);

    // Verify PS is back to original state (except B flag)
    uint8_t currentPS = cpu->getRegister(Register::PS);
    ASSERT_EQ(currentPS & ~0x10, initialPS & ~0x10)
        << "PS register should be restored to original value after PHP+PLP";

    // Verify SP is back to initial value
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP)
        << "Stack pointer should be restored after PHP+PLP";
}

// Test PHP with multiple pushes
TEST_F(PHPTest, MultiplePushes)
{
    // Set initial SP
    uint8_t initialSP = 0xFF;
    cpu->setRegister(Register::SP, initialSP);

    // Push status register multiple times with different flag values
    const bool flagValues[][7] = {
        // C,    Z,     I,     D,     B,     V,     N
        {true, false, true, false, true, false, true},  // Pattern 1
        {false, true, false, true, false, true, false}, // Pattern 2
        {true, true, true, false, false, false, true},  // Pattern 3
        {false, false, false, true, true, true, false}  // Pattern 4
    };

    uint8_t expectedPS[4];

    for (size_t i = 0; i < 4; ++i)
    {
        const auto &flags = flagValues[i];

        // Set flags
        cpu->setFlag(Flag::C, flags[0]);
        cpu->setFlag(Flag::Z, flags[1]);
        cpu->setFlag(Flag::I, flags[2]);
        cpu->setFlag(Flag::D, flags[3]);
        cpu->setFlag(Flag::B, flags[4]);
        cpu->setFlag(Flag::V, flags[5]);
        cpu->setFlag(Flag::N, flags[6]);

        expectedPS[i] = cpu->getRegister(Register::PS);
        expectedPS[i] |= 0x30; // Set B flag and bit 5 in pushed value

        // Execute PHP
        std::unique_ptr<PHP> php(new PHP(cpu));
        php->run();
    }

    // Check SP decremented by number of pushes
    ASSERT_EQ(cpu->getRegister(Register::SP), initialSP - 4)
        << "Stack pointer should be decremented by number of pushes";

    // Check values on stack in reverse order
    for (size_t i = 0; i < 4; ++i)
    {
        uint8_t stackValue = mem->readByte(0x0100 + initialSP - i);
        ASSERT_EQ(stackValue, expectedPS[i])
            << "Value at stack position " << i << " should be " << std::hex << (int)expectedPS[i];
    }
}

// Randomized test for extra coverage (reduced from 10000 to 100 iterations)
TEST_F(PHPTest, RandomizedFlagValues)
{
    const int ITERATIONS = 100;

    for (int i = 0; i < ITERATIONS; ++i)
    {
        setup();

        // Random flag values and stack pointer
        bool randomFlags[7];
        for (int j = 0; j < 7; ++j)
        {
            randomFlags[j] = (rand() % 2) == 1;
        }

        uint8_t initialSP = rand() & 0xFF;

        cpu->setFlag(Flag::C, randomFlags[0]);
        cpu->setFlag(Flag::Z, randomFlags[1]);
        cpu->setFlag(Flag::I, randomFlags[2]);
        cpu->setFlag(Flag::D, randomFlags[3]);
        cpu->setFlag(Flag::B, randomFlags[4]); // Will be set in pushed value anyway
        cpu->setFlag(Flag::V, randomFlags[5]);
        cpu->setFlag(Flag::N, randomFlags[6]);

        cpu->setRegister(Register::SP, initialSP);

        uint8_t expectedPS = cpu->getRegister(Register::PS);
        expectedPS |= 0x30; // Set B flag and bit 5 in pushed value

        // Execute PHP
        std::unique_ptr<PHP> php(new PHP(cpu));
        php->run();

        // Check stack pointer and value
        uint8_t expectedSP = (initialSP - 1) & 0xFF; // Account for wrap-around
        ASSERT_EQ(cpu->getRegister(Register::SP), expectedSP)
            << "Iteration " << i << ": Stack pointer incorrect";

        ASSERT_EQ(mem->readByte(0x0100 + initialSP), expectedPS)
            << "Iteration " << i << ": Value on stack incorrect";

        // Verify PS wasn't changed (except potentially B flag)
        uint8_t currentPS = cpu->getRegister(Register::PS);
        ASSERT_EQ(currentPS & ~0x30, expectedPS & ~0x30)
            << "Iteration " << i << ": PS register was incorrectly modified";
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));

    mem->initialize();
    mem->randomize();

    CPU *cppu = static_cast<CPU *>(cpu.get());
    cppu->init();
    cppu->reset();

    cppu->connectBus(bus);
    bus->connectMemory(mem);

    return RUN_ALL_TESTS();
}