#include <gtest/gtest.h>
#include "STX.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <vector>
#include <string>

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

void setA(uint8_t value) { cpu->setRegister(Register::A, value); }
void setX(uint8_t value) { cpu->setRegister(Register::X, value); }
void setY(uint8_t value) { cpu->setRegister(Register::Y, value); }
void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
void setPS(uint8_t value) { cpu->setRegister(Register::PS, value); }

// Capture flags before STX operation
struct FlagState {
    bool C, Z, I, D, B, V, N;
    
    void captureFrom(std::shared_ptr<ICPU> cpu) {
        C = cpu->getFlag(Flag::C);
        Z = cpu->getFlag(Flag::Z);
        I = cpu->getFlag(Flag::I);
        D = cpu->getFlag(Flag::D);
        B = cpu->getFlag(Flag::B);
        V = cpu->getFlag(Flag::V);
        N = cpu->getFlag(Flag::N);
    }
    
    void verifyUnchanged(std::shared_ptr<ICPU> cpu) const {
        ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
        ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag was modified";
    }
};

class STXTest : public ::testing::TestWithParam<AddressingMode> {
protected:
    void SetUp() override {
        setup();
    }
    
    // Test values for X register
    std::vector<uint8_t> testValues = {
        0x00,       // Zero
        0x01,       // Smallest positive
        0x7F,       // Largest positive (7-bit)
        0x80,       // Smallest negative (8-bit)
        0xFF,       // Largest value/negative
        0x55,       // 01010101
        0xAA,       // 10101010
        0xF0,       // 11110000
        0x0F        // 00001111
    };
};

// Helper to set up memory for different addressing modes and return the effective address
uint16_t setupAddressing(AddressingMode mode) {
    setPC(0x0200);
    uint16_t effectiveAddr = 0;
    
    switch (mode) {
        case AddressingMode::ZeroPage:
            mem->writeByte(0x0200, 0x42);
            effectiveAddr = 0x0042;
            break;
            
        case AddressingMode::ZeroPageY:  // Note: STX uses Y indexing, not X
            setY(0x05);
            mem->writeByte(0x0200, 0x40);
            effectiveAddr = 0x0045; // 0x40 + 0x05
            break;
            
        case AddressingMode::Absolute:
            mem->writeWord(0x0200, 0x1234);
            effectiveAddr = 0x1234;
            break;
    }
    
    return effectiveAddr;
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::ZeroPage: return 3;
        case AddressingMode::ZeroPageY: return 4;
        case AddressingMode::Absolute: return 4;
        default: return 2;
    }
}

// The main parameterized test
TEST_P(STXTest, StoreAllTestValues) {
    AddressingMode mode = GetParam();
    std::string modeName;
    
    switch (mode) {
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::ZeroPageY: modeName = "ZeroPageY"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        default: modeName = "Unknown"; break;
    }
    
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("Mode: " + modeName + ", X: 0x" + 
                    (testValue < 16 ? "0" : "") + 
                    std::to_string(testValue));
        
        setup();
        setX(testValue);
        
        // Set random values for flags to ensure they're preserved
        uint8_t randomFlags = rand() & 0xFF;
        setPS(randomFlags);
        
        // Capture flags before operation
        FlagState initialFlags;
        initialFlags.captureFrom(cpu);
        
        // Setup addressing mode and get the effective address
        uint16_t effectiveAddr = setupAddressing(mode);
        
        // Write a different value at the target memory location
        mem->writeByte(effectiveAddr, ~testValue); // Complement to ensure change is detected
        
        // Create and run the STX instruction
        std::unique_ptr<STX> stx(new STX(cpu, mode, getCycleCount(mode)));
        stx->run();
        
        // Verify the value was stored correctly
        ASSERT_EQ(mem->readByte(effectiveAddr), testValue)
            << "Failed to store X register value to memory";
        
        // Verify no flags were affected
        initialFlags.verifyUnchanged(cpu);
    }
}

// Specific test for zero-page wraparound behavior
TEST_F(STXTest, ZeroPageWrapAround) {
    setup();
    setX(0x42);
    setY(0xFF);
    setPC(0x0200);
    
    // Store 0x80 at PC
    mem->writeByte(0x0200, 0x80);
    
    // Effective address should be (0x80 + 0xFF) & 0xFF = 0x7F
    uint16_t effectiveAddr = 0x007F;
    
    // Create and run the STX instruction with ZeroPageY addressing
    std::unique_ptr<STX> stx(new STX(cpu, AddressingMode::ZeroPageY, 4));
    stx->run();
    
    // Verify the value was stored correctly
    ASSERT_EQ(mem->readByte(effectiveAddr), 0x42)
        << "Zero-page wraparound failed: value not stored at 0x007F";
}

// Test that STX doesn't modify any registers
TEST_F(STXTest, PreserveRegisters) {
    for (int i = 0; i < 10; i++) {
        setup();
        
        // Set random register values
        uint8_t a = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        setA(a);
        setX(x);
        setY(y);
        
        // Setup addressing mode
        setupAddressing(AddressingMode::ZeroPage);
        
        // Run instruction
        std::unique_ptr<STX> stx(new STX(cpu, AddressingMode::ZeroPage, 3));
        stx->run();
        
        // Verify registers are preserved
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "A register was modified";
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "X register was modified";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "Y register was modified";
    }
}



// Generate a vector of all addressing modes used by STX
INSTANTIATE_TEST_SUITE_P(
    STXAddressingModes,
    STXTest,
    ::testing::Values(
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageY,
        AddressingMode::Absolute
    )
);

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