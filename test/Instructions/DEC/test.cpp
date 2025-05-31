#include <gtest/gtest.h>
#include "DEC.h"
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

// Helper to verify DEC results
void verifyDECResults(uint16_t address, uint8_t originalValue) {
    uint8_t expectedValue = (originalValue - 1) & 0xFF;
    uint8_t actualValue = mem->readByte(address);
    
    // Check that memory was decremented correctly
    ASSERT_EQ(actualValue, expectedValue) << "Memory not decremented correctly: address=0x" << std::hex << address 
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
class DECSpecificTest : public ::testing::Test {
protected:
    void SetUp() override {
        setup();
    }
};

// Test decrementing to zero
TEST_F(DECSpecificTest, DecrementToZero) {
    setup();
    
    // Set up memory with value 0x01
    uint16_t address = 0x1234;
    uint8_t value = 0x01;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute DEC with absolute addressing
    std::unique_ptr<DEC> dec(new DEC(cpu, AddressingMode::Absolute, 6));
    dec->run();
    
    // Verify memory was decremented to zero and Z flag is set
    ASSERT_EQ(mem->readByte(address), 0x00) << "Memory should be decremented to 0x00";
    ASSERT_TRUE(cpu->getFlag(Flag::Z)) << "Zero flag should be set when result is zero";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Test decrementing to negative value
TEST_F(DECSpecificTest, DecrementToNegative) {
    setup();
    
    // Set up memory with value 0x00 (will wrap to 0xFF)
    uint16_t address = 0x1234;
    uint8_t value = 0x00;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute DEC with absolute addressing
    std::unique_ptr<DEC> dec(new DEC(cpu, AddressingMode::Absolute, 6));
    dec->run();
    
    // Verify memory wrapped around to 0xFF and N flag is set
    ASSERT_EQ(mem->readByte(address), 0xFF) << "Memory should be decremented to 0xFF (wrap around)";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_TRUE(cpu->getFlag(Flag::N)) << "Negative flag should be set when bit 7 is set";
}

// Test decrementing a regular value
TEST_F(DECSpecificTest, DecrementRegularValue) {
    setup();
    
    // Set up memory with value 0x42
    uint16_t address = 0x1234;
    uint8_t value = 0x42;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute DEC with absolute addressing
    std::unique_ptr<DEC> dec(new DEC(cpu, AddressingMode::Absolute, 6));
    dec->run();
    
    // Verify memory was decremented correctly and flags are set correctly
    ASSERT_EQ(mem->readByte(address), 0x41) << "Memory should be decremented to 0x41";
    ASSERT_FALSE(cpu->getFlag(Flag::Z)) << "Zero flag should be clear";
    ASSERT_FALSE(cpu->getFlag(Flag::N)) << "Negative flag should be clear";
}

// Parametrized tests for different addressing modes
class DECAddressingModeTest : public ::testing::TestWithParam<AddressingMode> {
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
TEST_P(DECAddressingModeTest, DecrementAcrossAddressingModes) {
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
        default: FAIL() << "Unsupported addressing mode"; cycles = 0;
    }
    
    // Execute DEC with the specified addressing mode
    std::unique_ptr<DEC> dec(new DEC(cpu, mode, cycles));
    dec->run();
    
    // Verify results
    verifyDECResults(effectiveAddress, value);
}

// Test with edge values
class DECValueTest : public ::testing::TestWithParam<uint8_t> {
protected:
    void SetUp() override {
        setup();
    }
};

TEST_P(DECValueTest, DecrementVariousValues) {
    uint8_t value = GetParam();
    
    // Set up memory with test value
    uint16_t address = 0x1234;
    mem->writeByte(address, value);
    
    // Set up CPU to point to this address
    uint16_t pc = 0x0100;
    cpu->setRegister(Register::PC, pc);
    mem->writeWord(pc, address);  // Write absolute address at PC
    
    // Execute DEC with absolute addressing
    std::unique_ptr<DEC> dec(new DEC(cpu, AddressingMode::Absolute, 6));
    dec->run();
    
    // Verify results
    verifyDECResults(address, value);
}

// Generate test cases for addressing modes
INSTANTIATE_TEST_SUITE_P(
    AddressingModes,
    DECAddressingModeTest,
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
    DECValueTest,
    ::testing::Values(
        0x00,  // Decrement from 0x00 wraps to 0xFF (tests negative flag)
        0x01,  // Decrement to 0x00 (tests zero flag)
        0x7F,  // Decrement from positive to positive
        0x80,  // Decrement from negative to negative
        0xFF   // Decrement from max negative
    )
);

// Test with randomized values (reduced from 1000 to 100 iterations)
TEST(DECRandomTest, RandomizedValues) {
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
        
        // Execute DEC
        std::unique_ptr<DEC> dec(new DEC(cpu, AddressingMode::Absolute, 6));
        dec->run();
        
        // Verify results
        verifyDECResults(address, value);
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