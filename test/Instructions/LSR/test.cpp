#include <gtest/gtest.h>
#include "LSR.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <tuple>
#include <sstream>

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

// Helper to verify LSR results for accumulator mode
void verifyLSRAccumulator(uint8_t original, uint8_t expected) {
    // Check result in A register
    ASSERT_EQ(cpu->getRegister(Register::A), expected)
        << "A register should contain: expected=0x" << std::hex << (int)expected;
    
    // Check carry flag (set if bit 0 of original was 1)
    ASSERT_EQ(cpu->getFlag(Flag::C), (original & 0x01) != 0)
        << "Carry flag incorrect: original=0x" << std::hex << (int)original
        << ", C flag should be " << ((original & 0x01) != 0 ? "set" : "clear");
    
    // Check zero flag (set if result is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0)
        << "Zero flag incorrect: result=0x" << std::hex << (int)expected
        << ", Z flag should be " << (expected == 0 ? "set" : "clear");
    
    // Check negative flag (should always be 0 for LSR)
    ASSERT_FALSE(cpu->getFlag(Flag::N))
        << "Negative flag should always be clear after LSR";
}

// Helper to verify LSR results for memory modes
void verifyLSRMemory(uint16_t address, uint8_t original, uint8_t expected) {
    // Check result in memory
    ASSERT_EQ(mem->readByte(address), expected)
        << "Memory at 0x" << std::hex << address << " should contain: expected=0x" << (int)expected;
    
    // Check carry flag (set if bit 0 of original was 1)
    ASSERT_EQ(cpu->getFlag(Flag::C), (original & 0x01) != 0)
        << "Carry flag incorrect: original=0x" << std::hex << (int)original
        << ", C flag should be " << ((original & 0x01) != 0 ? "set" : "clear");
    
    // Check zero flag (set if result is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), expected == 0)
        << "Zero flag incorrect: result=0x" << std::hex << (int)expected
        << ", Z flag should be " << (expected == 0 ? "set" : "clear");
    
    // Check negative flag (should always be 0 for LSR)
    ASSERT_FALSE(cpu->getFlag(Flag::N))
        << "Negative flag should always be clear after LSR";
}

// Helper to setup addressing mode and return effective address
uint16_t setupAddressingMode(AddressingMode mode, uint8_t testValue) {
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    uint16_t effectiveAddress = 0;
    
    switch(mode) {
        case AddressingMode::Accumulator:
            cpu->setRegister(Register::A, testValue);
            return 0; // For accumulator mode, address isn't relevant
            
        case AddressingMode::ZeroPage:
            effectiveAddress = 0x0042;
            mem->writeByte(pc, 0x42); // ZeroPage address
            mem->writeByte(effectiveAddress, testValue);
            break;
            
        case AddressingMode::ZeroPageX:
            cpu->setRegister(Register::X, 0x10);
            effectiveAddress = 0x0052; // 0x42 + 0x10 = 0x52
            mem->writeByte(pc, 0x42); // ZeroPage address
            mem->writeByte(effectiveAddress, testValue);
            break;
            
        case AddressingMode::Absolute:
            effectiveAddress = 0x1234;
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, testValue);
            break;
            
        case AddressingMode::AbsoluteX:
            cpu->setRegister(Register::X, 0x10);
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, testValue);
            break;
            
        default: {
            ADD_FAILURE() << "Unsupported addressing mode for LSR";
            return 0;
        }
    }
    
    return effectiveAddress;
}

// Helper to get cycle count for different addressing modes
int getCycleCount(AddressingMode mode) {
    switch(mode) {
        case AddressingMode::Accumulator: return 2;
        case AddressingMode::ZeroPage: return 5;
        case AddressingMode::ZeroPageX: return 6;
        case AddressingMode::Absolute: return 6;
        case AddressingMode::AbsoluteX: return 7;
        default: return 2;
    }
}

// Important test values for LSR
const uint8_t testValues[] = {
    0x00, // Zero - tests what happens with zero input
    0x01, // Lowest bit set - tests carry flag
    0x02, // Second bit set - simple shift
    0x7F, // 01111111 - test shifting of multi-bit value
    0x80, // 10000000 - highest bit set only
    0x55, // 01010101 - alternating pattern
    0xAA, // 10101010 - alternating pattern
    0xFF  // All bits set - checks result and carry
};

// Exhaustive test for LSR with accumulator mode
TEST(LSRExhaustive, AccumulatorMode) {
    // Try all possible values for accumulator
    for (int value = 0; value <= 0xFF; ++value) {
        setup();
        
        uint8_t original = static_cast<uint8_t>(value);
        uint8_t expected = original >> 1;
        
        // Set up and execute
        cpu->setRegister(Register::A, original);
        std::unique_ptr<LSR> lsr(new LSR(cpu, AddressingMode::Accumulator, 2));
        lsr->run();
        
        // Verify results
        verifyLSRAccumulator(original, expected);
    }
}

// Test LSR with memory addressing modes using selected key test values
class LSRAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(LSRAddressingModeTest, ShiftRightOperation) {
    auto [mode, testValue] = GetParam();
    
    std::stringstream ss;
    ss << "Testing LSR with addressing mode: ";
    switch(mode) {
        case AddressingMode::Accumulator: ss << "Accumulator"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        default: ss << "Unknown";
    }
    ss << ", value: 0x" << std::hex << (int)testValue;
    SCOPED_TRACE(ss.str());
    
    uint8_t expected = testValue >> 1;
    uint16_t effectiveAddress = setupAddressingMode(mode, testValue);
    
    // Execute LSR with the appropriate addressing mode
    std::unique_ptr<LSR> lsr(new LSR(cpu, mode, getCycleCount(mode)));
    lsr->run();
    
    // Verify results
    if (mode == AddressingMode::Accumulator) {
        verifyLSRAccumulator(testValue, expected);
    } else {
        verifyLSRMemory(effectiveAddress, testValue, expected);
    }
}

// Generate combinations of addressing modes and test values
std::vector<std::tuple<AddressingMode, uint8_t>> generateTestCases() {
    std::vector<std::tuple<AddressingMode, uint8_t>> cases;
    
    // Addressing modes to test
    const AddressingMode modes[] = {
        AddressingMode::Accumulator,
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageX,
        AddressingMode::Absolute,
        AddressingMode::AbsoluteX
    };
    
    for (auto mode : modes) {
        for (auto value : testValues) {
            cases.push_back(std::make_tuple(mode, value));
        }
    }
    
    return cases;
}

// Generate test name
std::string GetTestName(const testing::TestParamInfo<std::tuple<AddressingMode, uint8_t>>& info) {
    auto [mode, value] = info.param;
    std::stringstream ss;
    
    switch(mode) {
        case AddressingMode::Accumulator: ss << "Accumulator"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        default: ss << "Unknown";
    }
    
    ss << "_Value_" << std::hex << (int)value;
    return ss.str();
}

INSTANTIATE_TEST_SUITE_P(
    LSR,
    LSRAddressingModeTest,
    ::testing::ValuesIn(generateTestCases()),
    GetTestName
);

// Test zero page wrap-around behavior for ZeroPageX
TEST(LSRTest, ZeroPageWrapAround) {
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::X, 0xFF);
    mem->writeByte(pc, 0x80); // Zero page address 0x80
    uint8_t original = 0x42;
    mem->writeByte(0x7F, original); // Target value at (0x80 + 0xFF) & 0xFF = 0x7F
    
    std::unique_ptr<LSR> lsr(new LSR(cpu, AddressingMode::ZeroPageX, 6));
    lsr->run();
    
    verifyLSRMemory(0x7F, original, original >> 1);
}

// Test that LSR only affects the value, C, Z, N flags and not other flags
TEST(LSRTest, OnlyAffectsValueAndCZNFlags) {
    setup();
    
    // Set all flags beforehand
    cpu->setFlag(Flag::C, false); // Will be changed
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true); // Will be changed (should be clear after LSR)
    
    // Set A register with a value that will have carry and result in a non-zero value
    uint8_t testValue = 0x05; // 00000101
    cpu->setRegister(Register::A, testValue);
    
    // Execute LSR on accumulator
    std::unique_ptr<LSR> lsr(new LSR(cpu, AddressingMode::Accumulator, 2));
    lsr->run();
    
    // Check result
    ASSERT_EQ(cpu->getRegister(Register::A), 0x02); // 00000010
    
    // Check affected flags
    ASSERT_TRUE(cpu->getFlag(Flag::C)); // Bit 0 was 1
    ASSERT_FALSE(cpu->getFlag(Flag::Z)); // Result is not zero
    ASSERT_FALSE(cpu->getFlag(Flag::N)); // Bit 7 is always 0 after LSR
    
    // Check unaffected flags
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
    
    // Now test with a value that will result in zero
    setup();
    cpu->setFlag(Flag::C, false); // Will be changed
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::N, true);  // Will be changed
    
    testValue = 0x01; // 00000001 -> 00000000 after shift
    cpu->setRegister(Register::A, testValue);
    
    lsr.reset(new LSR(cpu, AddressingMode::Accumulator, 2));
    lsr->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x00);
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::Z));
    ASSERT_FALSE(cpu->getFlag(Flag::N));
}

// Randomized test for additional coverage (reduced iterations)
TEST(LSRTest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Randomly choose an addressing mode
        int modeInt = rand() % 5;
        AddressingMode mode;
        
        switch(modeInt) {
            case 0: mode = AddressingMode::Accumulator; break;
            case 1: mode = AddressingMode::ZeroPage; break;
            case 2: mode = AddressingMode::ZeroPageX; break;
            case 3: mode = AddressingMode::Absolute; break;
            default: mode = AddressingMode::AbsoluteX; break;
        }
        
        // Random test value
        uint8_t testValue = rand() & 0xFF;
        uint8_t expected = testValue >> 1;
        
        // Setup memory and execute LSR
        uint16_t effectiveAddress = setupAddressingMode(mode, testValue);
        
        std::unique_ptr<LSR> lsr(new LSR(cpu, mode, getCycleCount(mode)));
        lsr->run();
        
        // Verify results
        if (mode == AddressingMode::Accumulator) {
            verifyLSRAccumulator(testValue, expected);
        } else {
            verifyLSRMemory(effectiveAddress, testValue, expected);
        }
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    
    mem->initialize();
    mem->randomize();
    
    CPU *cppu = static_cast<CPU*>(cpu.get());
    cppu->init();
    cppu->reset();
    
    cppu->connectBus(bus);
    bus->connectMemory(mem);
    
    return RUN_ALL_TESTS();
}