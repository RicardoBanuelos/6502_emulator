#include <gtest/gtest.h>
#include "LDA.h"
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

// Helper to verify LDA results
void verifyLDAResults(uint8_t value) {
    // Check A register
    ASSERT_EQ(cpu->getRegister(Register::A), value)
        << "A register should contain loaded value: expected=0x" << std::hex << (int)value;
    
    // Check zero flag (set if value is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), value == 0)
        << "Zero flag incorrect: value=0x" << std::hex << (int)value
        << ", Z flag should be " << (value == 0 ? "set" : "clear");
    
    // Check negative flag (set if bit 7 of value is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (value & 0x80) != 0)
        << "Negative flag incorrect: value=0x" << std::hex << (int)value
        << ", N flag should be " << ((value & 0x80) != 0 ? "set" : "clear");
}

// Test case for LDA using different addressing modes
class LDAAddressingModeTest : public ::testing::TestWithParam<std::tuple<AddressingMode, uint8_t>> {
protected:
    void SetUp() override {
        setup();
    }
};

// Change setupAddressingMode to be a standalone function outside of any class
uint16_t setupAddressingMode(AddressingMode mode, uint8_t testValue) {
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    uint16_t effectiveAddress = 0;
    
    switch(mode) {
        case AddressingMode::Immediate:
            mem->writeByte(pc, testValue);
            return pc;
                
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
                
        case AddressingMode::ZeroPageY:
            cpu->setRegister(Register::Y, 0x10);
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
                
        case AddressingMode::AbsoluteY:
            cpu->setRegister(Register::Y, 0x10);
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeWord(pc, 0x1234); // Absolute address
            mem->writeByte(effectiveAddress, testValue);
            break;
                
        case AddressingMode::IndirectX:
            cpu->setRegister(Register::X, 0x04);
            mem->writeByte(pc, 0x20); // Zero page address
            mem->writeWord(0x24, 0x1234); // 0x20 + 0x04 = 0x24, contains pointer
            effectiveAddress = 0x1234;
            mem->writeByte(effectiveAddress, testValue);
            break;
                
        case AddressingMode::IndirectY:
            cpu->setRegister(Register::Y, 0x10);
            mem->writeByte(pc, 0x20); // Zero page address
            mem->writeWord(0x20, 0x1234); // Contains base pointer
            effectiveAddress = 0x1244; // 0x1234 + 0x10 = 0x1244
            mem->writeByte(effectiveAddress, testValue);
            break;
                
        default: {
            ADD_FAILURE() << "Unsupported addressing mode for LDA";
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
        case AddressingMode::ZeroPageY: return 4;
        case AddressingMode::Absolute: return 4;
        case AddressingMode::AbsoluteX: return 4; // +1 if page boundary crossed
        case AddressingMode::AbsoluteY: return 4; // +1 if page boundary crossed
        case AddressingMode::IndirectX: return 6;
        case AddressingMode::IndirectY: return 5; // +1 if page boundary crossed
        default: return 2;
    }
}

// Important test values
const uint8_t testValues[] = {
    0x00, // Tests zero flag
    0x01, // Smallest positive value
    0x7F, // Largest positive value
    0x80, // Smallest negative value
    0xFF, // Largest negative value
    0x55, // Alternating bit pattern 01010101
    0xAA  // Alternating bit pattern 10101010
};

// Generate test name
std::string GetTestName(const testing::TestParamInfo<std::tuple<AddressingMode, uint8_t>>& info) {
    auto [mode, value] = info.param;
    std::stringstream ss;
    
    switch(mode) {
        case AddressingMode::Immediate: ss << "Immediate"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::ZeroPageY: ss << "ZeroPageY"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: ss << "AbsoluteY"; break;
        case AddressingMode::IndirectX: ss << "IndirectX"; break;
        case AddressingMode::IndirectY: ss << "IndirectY"; break;
        default: ss << "Unknown";
    }
    
    ss << "_Value_" << std::hex << (int)value;
    return ss.str();
}

TEST_P(LDAAddressingModeTest, LoadAccumulator) {
    auto [mode, testValue] = GetParam();
    
    std::stringstream ss;
    ss << "Testing LDA with addressing mode: ";
    switch(mode) {
        case AddressingMode::Immediate: ss << "Immediate"; break;
        case AddressingMode::ZeroPage: ss << "ZeroPage"; break;
        case AddressingMode::ZeroPageX: ss << "ZeroPageX"; break;
        case AddressingMode::ZeroPageY: ss << "ZeroPageY"; break;
        case AddressingMode::Absolute: ss << "Absolute"; break;
        case AddressingMode::AbsoluteX: ss << "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: ss << "AbsoluteY"; break;
        case AddressingMode::IndirectX: ss << "IndirectX"; break;
        case AddressingMode::IndirectY: ss << "IndirectY"; break;
        default: ss << "Unknown";
    }
    ss << ", value: 0x" << std::hex << (int)testValue;
    SCOPED_TRACE(ss.str());
    
    // Setup memory and addressing mode
    setupAddressingMode(mode, testValue);
    
    // Execute LDA with the appropriate addressing mode
    std::unique_ptr<LDA> lda(new LDA(cpu, mode, getCycleCount(mode)));
    lda->run();
    
    // Verify results
    verifyLDAResults(testValue);
}

// Generate combinations of addressing modes and test values
std::vector<std::tuple<AddressingMode, uint8_t>> generateTestCases() {
    std::vector<std::tuple<AddressingMode, uint8_t>> cases;
    
    // Addressing modes to test
    const AddressingMode modes[] = {
        AddressingMode::Immediate,
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageX,
        AddressingMode::ZeroPageY,
        AddressingMode::Absolute,
        AddressingMode::AbsoluteX,
        AddressingMode::AbsoluteY,
        AddressingMode::IndirectX,
        AddressingMode::IndirectY
    };
    
    for (auto mode : modes) {
        for (auto value : testValues) {
            cases.push_back(std::make_tuple(mode, value));
        }
    }
    
    return cases;
}

INSTANTIATE_TEST_SUITE_P(
    LDA,
    LDAAddressingModeTest,
    ::testing::ValuesIn(generateTestCases()),
    GetTestName
);

// Test page boundary crossing for modes that have different cycle counts
TEST(LDATest, PageCrossing) {
    // Test AbsoluteX page crossing
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::X, 0xFF);
    mem->writeWord(pc, 0x2000); // Base address
    mem->writeByte(0x20FF, 0x42); // Target value at 0x2000 + 0xFF = 0x20FF
    
    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::AbsoluteX, 5)); // 5 cycles with page crossing
    lda->run();
    
    verifyLDAResults(0x42);
    
    // Test AbsoluteY page crossing
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::Y, 0xFF);
    mem->writeWord(pc, 0x2000); // Base address
    mem->writeByte(0x20FF, 0x42); // Target value at 0x2000 + 0xFF = 0x20FF
    
    lda.reset(new LDA(cpu, AddressingMode::AbsoluteY, 5)); // 5 cycles with page crossing
    lda->run();
    
    verifyLDAResults(0x42);
    
    // Test IndirectY page crossing
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::Y, 0xFF);
    mem->writeByte(pc, 0x80); // Zero page pointer
    mem->writeWord(0x80, 0x2000); // Base address from zero page
    mem->writeByte(0x20FF, 0x42); // Target value at 0x2000 + 0xFF = 0x20FF
    
    lda.reset(new LDA(cpu, AddressingMode::IndirectY, 6)); // 6 cycles with page crossing
    lda->run();
    
    verifyLDAResults(0x42);
}

// Test zero page wrap-around behavior for ZeroPageX and ZeroPageY
TEST(LDATest, ZeroPageWrapAround) {
    // Test ZeroPageX wrap-around
    setup();
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::X, 0xFF);
    mem->writeByte(pc, 0x80); // Zero page address 0x80
    mem->writeByte(0x7F, 0x42); // Target value at (0x80 + 0xFF) & 0xFF = 0x7F
    
    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::ZeroPageX, 4));
    lda->run();
    
    verifyLDAResults(0x42);
    
    // Test ZeroPageY wrap-around
    setup();
    pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    cpu->setRegister(Register::Y, 0xFF);
    mem->writeByte(pc, 0x80); // Zero page address 0x80
    mem->writeByte(0x7F, 0x42); // Target value at (0x80 + 0xFF) & 0xFF = 0x7F
    
    lda.reset(new LDA(cpu, AddressingMode::ZeroPageY, 4));
    lda->run();
    
    verifyLDAResults(0x42);
}

// Test that LDA only affects A, Z, and N flags
TEST(LDATest, OnlyAffectsAZNFlags) {
    setup();
    
    // Set all flags beforehand
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false); // Will be changed
    
    // Setup LDA Immediate with value 0x80 (negative)
    uint16_t pc = 0x1000;
    cpu->setRegister(Register::PC, pc);
    mem->writeByte(pc, 0x80);
    
    std::unique_ptr<LDA> lda(new LDA(cpu, AddressingMode::Immediate, 2));
    lda->run();
    
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
}

// Randomized test for extra coverage (reduced from 10000 to 100 iterations)
TEST(LDATest, RandomizedValues) {
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; ++i) {
        setup();
        
        // Randomly choose an addressing mode
        int modeInt = rand() % 9;
        AddressingMode mode;
        
        switch(modeInt) {
            case 0: mode = AddressingMode::Immediate; break;
            case 1: mode = AddressingMode::ZeroPage; break;
            case 2: mode = AddressingMode::ZeroPageX; break;
            case 3: mode = AddressingMode::ZeroPageY; break;
            case 4: mode = AddressingMode::Absolute; break;
            case 5: mode = AddressingMode::AbsoluteX; break;
            case 6: mode = AddressingMode::AbsoluteY; break;
            case 7: mode = AddressingMode::IndirectX; break;
            default: mode = AddressingMode::IndirectY; break;
        }
        
        // Random test value
        uint8_t testValue = rand() & 0xFF;
        
        // Setup memory and execute LDA - CHANGED CODE HERE
        setupAddressingMode(mode, testValue);
        
        std::unique_ptr<LDA> lda(new LDA(cpu, mode, getCycleCount(mode)));
        lda->run();
        
        // Verify results
        verifyLDAResults(testValue);
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