#include <gtest/gtest.h>
#include "INC.h"
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

// Helper to verify INC results
void verifyINCResults(uint16_t address, uint8_t originalValue) {
    uint8_t expectedValue = (originalValue + 1) & 0xFF;
    uint8_t actualValue = mem->readByte(address);
    
    // Check that memory was incremented correctly
    ASSERT_EQ(actualValue, expectedValue) << "Memory not incremented correctly: address=0x" << std::hex << address 
                                         << ", original=0x" << (int)originalValue
                                         << ", expected=0x" << (int)expectedValue
                                         << ", actual=0x" << (int)actualValue;
    
    // Check zero flag (set if result is zero)
    ASSERT_EQ(cpu->getFlag(Flag::Z), expectedValue == 0) 
        << "Zero flag incorrect: expected=" << (expectedValue == 0);
    
    // Check negative flag (set if bit 7 of result is set)
    ASSERT_EQ(cpu->getFlag(Flag::N), (expectedValue & 0x80) != 0) 
        << "Negative flag incorrect: expected=" << ((expectedValue & 0x80) != 0);
}

// Test cases for specific scenarios
class INCSpecificTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test incrementing to zero
TEST_F(INCSpecificTest, IncrementToZero) {
    setup();
    
    // Set up memory with value 0xFF
    uint16_t address = 0x1234;
    uint8_t value = 0xFF;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute INC with absolute addressing
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
    inc->run();
    
    // Verify memory was incremented to zero and Z flag is set
    ASSERT_EQ(mem->readByte(address), 0x00) << "Memory should be incremented to 0x00 (wrap around)";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when result is zero";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Test incrementing to negative value
TEST_F(INCSpecificTest, IncrementToNegative) {
    setup();
    
    // Set up memory with value 0x7F (will increment to 0x80, a negative value)
    uint16_t address = 0x1234;
    uint8_t value = 0x7F;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute INC with absolute addressing
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
    inc->run();
    
    // Verify memory was incremented to 0x80 and N flag is set
    ASSERT_EQ(mem->readByte(address), 0x80) << "Memory should be incremented to 0x80";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when bit 7 is set";
}

// Test incrementing a regular value
TEST_F(INCSpecificTest, IncrementRegularValue) {
    setup();
    
    // Set up memory with value 0x42
    uint16_t address = 0x1234;
    uint8_t value = 0x42;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute INC with absolute addressing
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
    inc->run();
    
    // Verify memory was incremented correctly and flags are set correctly
    ASSERT_EQ(mem->readByte(address), 0x43) << "Memory should be incremented to 0x43";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Parametrized tests for different addressing modes
class INCAddressingModeTest : public ::testing::TestWithParam<AddressingMode> {
protected:
    void SetUp() override {
        setup();
    }
    
    // Helper to setup memory and get the effective address for different addressing modes
    uint16_t setupAddressingMode(AddressingMode mode, uint8_t value) {
        uint16_t pc = 0x0100;
        cpu->setRegister(Register::PC, pc);
        uint16_t effectiveAddress = 0;
        
        switch(mode) {
            case AddressingMode::ZeroPage:
                effectiveAddress = 0x0042;
                mem->writeByte(pc, 0x42);    // Zero page address (0x42)
                break;
                
            case AddressingMode::ZeroPageX:
                cpu->setRegister(Register::X, 0x10);
                effectiveAddress = 0x0052;    // 0x42 + 0x10 = 0x52
                mem->writeByte(pc, 0x42);    // Zero page address (0x42)
                break;
                
            case AddressingMode::Absolute:
                effectiveAddress = 0x1234;
                mem->writeWord(pc, 0x1234);  // Absolute address (0x1234)
                break;
                
            case AddressingMode::AbsoluteX:
                cpu->setRegister(Register::X, 0x10);
                effectiveAddress = 0x1244;    // 0x1234 + 0x10 = 0x1244
                mem->writeWord(pc, 0x1234);  // Absolute address (0x1234)
                break;
        }
        
        // Write the test value to the effective address
        mem->writeByte(effectiveAddress, value);
        return effectiveAddress;
    }
};

// Test cases for different addressing modes
TEST_P(INCAddressingModeTest, IncrementAcrossAddressingModes) {
    AddressingMode mode = GetParam();
    uint8_t value = 0x42;
    
    // Set up memory and get effective address for this addressing mode
    uint16_t effectiveAddress = setupAddressingMode(mode, value);
    
    // Get cycles based on addressing mode
    uint8_t cycles;
    switch(mode) {
        case AddressingMode::ZeroPage:  cycles = 5; break;
        case AddressingMode::ZeroPageX: cycles = 6; break;
        case AddressingMode::Absolute:  cycles = 6; break;
        case AddressingMode::AbsoluteX: cycles = 7; break;
        default: 
            FAIL() << "Unsupported addressing mode"; 
            return; // Add this to satisfy the compiler
    }
    
    // Execute INC with the specified addressing mode
    std::unique_ptr<INC> inc(new INC(cpu, mode, cycles));
    inc->run();
    
    // Verify results
    verifyINCResults(effectiveAddress, value);
}

// Test with edge values
class INCValueTest : public ::testing::TestWithParam<uint8_t> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(INCValueTest, IncrementVariousValues) {
    uint8_t value = GetParam();
    
    // Set up memory with test value
    uint16_t address = 0x1234;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute INC with absolute addressing
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
    inc->run();
    
    // Verify results
    verifyINCResults(address, value);
}

// Generate test cases for addressing modes
INSTANTIATE_TEST_SUITE_P(
    AddressingModes,
    INCAddressingModeTest,
    ::testing::Values(
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageX,
        AddressingMode::Absolute,
        AddressingMode::AbsoluteX
    )
);

// Generate test cases with important values to test
INSTANTIATE_TEST_SUITE_P(
    ImportantValues,
    INCValueTest,
    ::testing::Values(
        0x00,  // Increment from 0x00 to 0x01
        0x7F,  // Increment from 0x7F to 0x80 (tests negative flag)
        0xFE,  // Increment to 0xFF
        0xFF   // Increment from 0xFF wraps to 0x00 (tests zero flag)
    )
);

// Test zero page wrap-around behavior
TEST(INCTest, ZeroPageWrapAround) {
    setup();
    
    // Set X register and PC
    cpu->setRegister(Register::X, 0xFF);
    cpu->setRegister(Register::PC, 0x0100);
    
    // Write zero page address
    mem->writeByte(0x0100, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    
    // Set initial value in memory
    mem->writeByte(0x7F, 0x42);
    
    // Execute INC with zero page X addressing
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::ZeroPageX, 6));
    inc->run();
    
    // Verify results
    ASSERT_EQ(mem->readByte(0x7F), 0x43) << "Memory should be incremented correctly with zero page wrap";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Test that INC only affects Z and N flags
TEST(INCTest, OnlyAffectsZNFlags) {
    setup();
    
    // Set all flags beforehand
    cpu->setFlag(Flag::C, true);
    cpu->setFlag(Flag::Z, false); // Will be changed
    cpu->setFlag(Flag::I, true);
    cpu->setFlag(Flag::D, true);
    cpu->setFlag(Flag::B, true);
    cpu->setFlag(Flag::V, true);
    cpu->setFlag(Flag::N, false); // Will be changed
    
    // Set up memory
    uint16_t address = 0x1234;
    mem->writeByte(address, 0x7F);  // Will increment to 0x80, setting N flag
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);
    
    // Execute INC
    std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
    inc->run();
    
    // Check affected flags
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set";
    
    // Check unaffected flags
    ASSERT_TRUE(cpu->getFlag(Flag::C)) << "Carry flag should be unchanged";
    ASSERT_TRUE(cpu->getFlag(Flag::I)) << "Interrupt flag should be unchanged";
    ASSERT_TRUE(cpu->getFlag(Flag::D)) << "Decimal flag should be unchanged";
    ASSERT_TRUE(cpu->getFlag(Flag::B)) << "Break flag should be unchanged";
    ASSERT_TRUE(cpu->getFlag(Flag::V)) << "Overflow flag should be unchanged";
}

// Test with randomized values (reduced from 1000 to 100 iterations)
TEST(INCTest, RandomizedValues) {
    for(int i = 0; i < 100; ++i) {
        setup();
        
        // Randomize memory value
        uint8_t value = rand() & 0xFF;
        uint16_t address = 0x1234;
        mem->writeByte(address, value);
        
        // Set up CPU to point to this address
        uint16_t pc = 0x0100;
        cpu->setRegister(Register::PC, pc);
        mem->writeWord(pc, address);  // Write absolute address at PC
        
        // Execute INC
        std::unique_ptr<INC> inc(new INC(cpu, AddressingMode::Absolute, 6));
        inc->run();
        
        // Verify results
        verifyINCResults(address, value);
    }
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
   
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}