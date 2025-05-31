#include <gtest/gtest.h>
#include "ORA.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <sstream>
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

// Helper to verify ORA results
void verifyORAResults(uint8_t accumulator, uint8_t operand) {
    uint8_t result = accumulator | operand;
    
    // Check A register
    ASSERT_EQ(cpu->getRegister(Register::A), result)
        << "A register should contain: expected=0x" << std::hex << (int)result
        << ", was A=0x" << (int)accumulator << " | operand=0x" << (int)operand;
    
    // Check zero flag (set if result is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), result == 0)
        << "Zero flag incorrect: result=0x" << std::hex << (int)result
        << ", Z flag should be " << (result == 0 ? "set" : "clear");
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (result & 0x80) != 0)
        << "Negative flag incorrect: result=0x" << std::hex << (int)result
        << ", N flag should be " << ((result & 0x80) != 0 ? "set" : "clear");
}

// Helper to setup addressing mode and return effective address
uint16_t setupAddressingMode(AddressingMode mode, uint8_t operand, uint8_t accumulator) {
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, accumulator);
    uint16_t effectiveAddress = 0;
    
    switch(mode) {
        case AddressingMode::Immediate:
            mem->writeByte(pc, operand);
            return pc;
            
        case AddressingMode::ZeroPage:
            effectiveAddress = 0x0042;
            mem->writeByte(pc, 0x42); // ZeroPage address
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::ZeroPageX:
            cpu->setRegister(Register::X, 0x10);
            effectiveAddress = 0x0052; // 0x42 + 0x10 = 0x52
            mem->writeByte(pc, 0x42); // ZeroPage address
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::Absolute:
            effectiveAddress = 0x1234;
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::AbsoluteX:
            cpu->setRegister(Register::X, 0x10);
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::AbsoluteY:
            cpu->setRegister(Register::Y, 0x10);
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::IndirectX:
            cpu->setRegister(Register::X, 0x10);
            mem->writeByte(pc, 0x42); // Zero page address
            mem->writeWord(0x52, 0x1234); // 0x42 + 0x10 = 0x52, contains pointer
            effectiveAddress = 0x1234;
            mem->writeByte(effectiveAddress, operand);
            break;
            
        case AddressingMode::IndirectY:
            cpu->setRegister(Register::Y, 0x10);
            mem->writeByte(pc, 0x42); // Zero page address
            mem->writeWord(0x42, 0x1234); // Contains base pointer
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeByte(effectiveAddress, operand);
            break;
            
        default: {
            ADD_FAILURE() << "Unsupported addressing mode for ORA";
            return 0;
        }
    }
    
    return effectiveAddress;
}

// Helper to get cycle count for different addressing modes
int getCycleCount(AddressingMode mode) {
    switch(mode) {
        case AddressingMode::Immediate: return 2;
        case AddressingMode::ZeroPage: return 3;
        case AddressingMode::ZeroPageX: return 4;
        case AddressingMode::Absolute: return 4;
        case AddressingMode::AbsoluteX: return 4; // +1 if page boundary crossed
        case AddressingMode::AbsoluteY: return 4; // +1 if page boundary crossed
        case AddressingMode::IndirectX: return 6;
        case AddressingMode::IndirectY: return 5; // +1 if page boundary crossed
        default: return 2;
    }
}

// Important test values
const uint8_t testAccumulators[] = {
    0x00, // Zero
    0x01, // Single bit set
    0x7F, // Many bits set, positive
    0x80, // Single bit set, negative
    0xFF, // All bits set
    0x55, // Alternating bits 01010101
    0xAA  // Alternating bits 10101010
};

const uint8_t testOperands[] = {
    0x00, // Zero
    0x01, // Single bit set
    0x7F, // Many bits set, positive
    0x80, // Single bit set, negative
    0xFF, // All bits set
    0x55, // Alternating bits 01010101
    0xAA  // Alternating bits 10101010
};

// Test case for ORA using different addressing modes
class ORAAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

// Generate test name
std::string GetTestName(const testing::TestParamInfo<std::tuple<AddressingMode, uint8_t, uint8_t>>& info) {
    auto [mode, accumulator, operand] = info.param;
    std::stringstream ss;
    
    switch(mode) {
        case AddressingMode::Immediate: ss << "Immediate"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: ss << "AbsoluteY"; break;
        case AddressingMode::IndirectX: ss << "IndirectX"; break;
        case AddressingMode::IndirectY: ss << "IndirectY"; break;
        default: ss << "Unknown";
    }
    
    ss << "_A" << std::hex << (int)accumulator << "_Op" << (int)operand;
    return ss.str();
}

// Systematic test for ORA operation with parameterized test
TEST_P(ORAAddressingModeTest, BitwiseOR) {
    auto [mode, accumulator, operand] = GetParam();
    
    std::stringstream ss;
    ss << "Testing ORA with addressing mode: ";
    switch(mode) {
        case AddressingMode::Immediate: ss << "Immediate"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: ss << "AbsoluteY"; break;
        case AddressingMode::IndirectX: ss << "IndirectX"; break;
        case AddressingMode::IndirectY: ss << "IndirectY"; break;
        default: ss << "Unknown";
    }
    ss << ", A=0x" << std::hex << (int)accumulator << ", operand=0x" << (int)operand;
    SCOPED_TRACE(ss.str());
    
    // Setup memory and addressing mode
    setupAddressingMode(mode, operand, accumulator);
    
    // Execute ORA with the appropriate addressing mode
    std::unique_ptr<ORA> ora(new ORA(cpu, mode, getCycleCount(mode)));
    ora->run();
    
    // Verify results
    verifyORAResults(accumulator, operand);
}

// Generate combinations of addressing modes and test values
std::vector<std::tuple<AddressingMode, uint8_t, uint8_t>> generateTestCases() {
    std::vector<std::tuple<AddressingMode, uint8_t, uint8_t>> cases;
    
    // Addressing modes to test
    const AddressingMode modes[] = {
        AddressingMode::Immediate,
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageX,
        AddressingMode::Absolute,
        AddressingMode::AbsoluteX,
        AddressingMode::AbsoluteY,
        AddressingMode::IndirectX,
        AddressingMode::IndirectY
    };
    
    // Generate combinations of addressing modes and test values
    for (auto mode : modes) {
        for (auto accumulator : testAccumulators) {
            for (auto operand : testOperands) {
                cases.push_back(std::make_tuple(mode, accumulator, operand));
            }
        }
    }
    
    return cases;
}

INSTANTIATE_TEST_SUITE_P(
    ORA,
    ORAAddressingModeTest,
    ::testing::ValuesIn(generateTestCases()),
    GetTestName
);

// Test page boundary crossing for modes that have different cycle counts
TEST(ORATest, PageCrossing) {
    // Test AbsoluteX page crossing
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::X, 0xFF);
    cpu->setRegister(Register::A, 0x12);
    uint8_t operand = 0x34;
    mem->writeWord(pc, 0x2000); // Base address
    mem->writeByte(0x20FF, operand); // Target value at 0x2000 + 0xFF = 0x20FF
    
    std::unique_ptr<ORA> ora(new ORA(cpu, AddressingMode::AbsoluteX, 5)); // 5 cycles with page crossing
    ora->run();
    
    verifyORAResults(0x12, operand);
    
    // Test AbsoluteY page crossing
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::Y, 0xFF);
    cpu->setRegister(Register::A, 0x12);
    mem->writeWord(pc, 0x2000); // Base address
    mem->writeByte(0x20FF, operand); // Target value at 0x2000 + 0xFF = 0x20FF
    
    ora.reset(new ORA(cpu, AddressingMode::AbsoluteY, 5)); // 5 cycles with page crossing
    ora->run();
    
    verifyORAResults(0x12, operand);
    
    // Test IndirectY page crossing
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::Y, 0xFF);
    cpu->setRegister(Register::A, 0x12);
    mem->writeByte(pc, 0x80); // Zero page pointer
    mem->writeWord(0x80, 0x2000); // Base address from zero page
    mem->writeByte(0x20FF, operand); // Target value at 0x2000 + 0xFF = 0x20FF
    
    ora.reset(new ORA(cpu, AddressingMode::IndirectY, 6)); // 6 cycles with page crossing
    ora->run();
    
    verifyORAResults(0x12, operand);
}

// Test zero page wrap-around behavior for ZeroPageX
TEST(ORATest, ZeroPageWrapAround) {
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::X, 0xFF);
    cpu->setRegister(Register::A, 0x12);
    uint8_t operand = 0x34;
    mem->writeByte(pc, 0x80); // Zero page address 0x80
    mem->writeByte(0x7F, operand); // Target value at (0x80 + 0xFF) & 0xFF = 0x7F
    
    std::unique_ptr<ORA> ora(new ORA(cpu, AddressingMode::ZeroPageX, 4));
    ora->run();
    
    verifyORAResults(0x12, operand);
}

// Test that ORA only affects A, Z, and N flags
TEST(ORATest, OnlyAffectsAZNFlags) {
    setup();
    
    // Set all flags beforehand
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false); // Will be changed
    
    // Setup ORA Immediate with value 0x80 and A=0
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x00);
    mem->writeByte(pc, 0x80); // Will make result negative
    
    std::unique_ptr<ORA> ora(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    // Check affected registers and flags
    ASSERT_EQ(cpu->getRegister(Register::A), 0x80);
    ASSERT_FALSE(cpu->getFlag(Flag::Z));
    ASSERT_TRUE(cpu->getFlag(Flag::N));
    
    // Check unaffected flags
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
    
    // Now test that produces zero result (0 | 0 = 0)
    setup();
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false); // Will be set
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, true); // Will be cleared
    
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x00);
    mem->writeByte(pc, 0x00);
    
    ora.reset(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    // Check zero flag and negative flag
    ASSERT_EQ(cpu->getRegister(Register::A), 0x00);
    ASSERT_TRUE(cpu->getFlag(Flag::Z));
    ASSERT_FALSE(cpu->getFlag(Flag::N));
    
    // Check unaffected flags
    ASSERT_TRUE(cpu->getFlag(Flag::C));
    ASSERT_TRUE(cpu->getFlag(Flag::I));
    ASSERT_TRUE(cpu->getFlag(Flag::D));
    ASSERT_TRUE(cpu->getFlag(Flag::B));
    ASSERT_TRUE(cpu->getFlag(Flag::V));
}

// Test specific cases of interest
TEST(ORATest, SpecialCases) {
    // Test OR with 0 (shouldn't change value)
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x42);
    mem->writeByte(pc, 0x00);
    
    std::unique_ptr<ORA> ora(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x42);
    
    // Test OR with itself (shouldn't change value)
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x42);
    mem->writeByte(pc, 0x42);
    
    ora.reset(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0x42);
    
    // Test OR with FF (should make all bits 1)
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x42);
    mem->writeByte(pc, 0xFF);
    
    ora.reset(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF);
    
    // Test OR with alternating bit patterns
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::A, 0x55); // 01010101
    mem->writeByte(pc, 0xAA);            // 10101010
    
    ora.reset(new ORA(cpu, AddressingMode::Immediate, 2));
    ora->run();
    
    ASSERT_EQ(cpu->getRegister(Register::A), 0xFF); // Should be all 1's
}

// Randomized test for extra coverage (reduced from 10000 to 100 iterations)
TEST(ORATest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Randomly choose an addressing mode
        int modeInt = rand() % 8;
        AddressingMode mode;
        
        switch(modeInt) {
            case 0: mode = AddressingMode::Immediate; break;
            case 1: mode = AddressingMode::ZeroPage; break;
            case 2: mode = AddressingMode::ZeroPageX; break;
            case 3: mode = AddressingMode::Absolute; break;
            case 4: mode = AddressingMode::AbsoluteX; break;
            case 5: mode = AddressingMode::AbsoluteY; break;
            case 6: mode = AddressingMode::IndirectX; break;
            default: mode = AddressingMode::IndirectY; break;
        }
        
        // Random test values
        uint8_t accumulator = rand() & 0xFF;
        uint8_t operand = rand() & 0xFF;
        
        // Setup memory and execute ORA
        setupAddressingMode(mode, operand, accumulator);
        
        std::unique_ptr<ORA> ora(new ORA(cpu, mode, getCycleCount(mode)));
        ora->run();
        
        // Verify results
        verifyORAResults(accumulator, operand);
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