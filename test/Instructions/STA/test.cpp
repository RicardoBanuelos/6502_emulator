#include <gtest/gtest.h>
#include "STA.h"
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

// Capture flags before STA operation
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

class STATest : public ::testing::TestWithParam<AddressingMode> {
protected:
    void SetUp() override {
        setup();
    }
    
    // Test values for A register
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
            
        case AddressingMode::ZeroPageX:
            setX(0x05);
            mem->writeByte(0x0200, 0x40);
            effectiveAddr = 0x0045; // 0x40 + 0x05
            break;
            
        case AddressingMode::Absolute:
            mem->writeWord(0x0200, 0x1234);
            effectiveAddr = 0x1234;
            break;
            
        case AddressingMode::AbsoluteX:
            setX(0x01);
            mem->writeWord(0x0200, 0x1234);
            effectiveAddr = 0x1235; // 0x1234 + 0x01
            break;
            
        case AddressingMode::AbsoluteY:
            setY(0x02);
            mem->writeWord(0x0200, 0x1234);
            effectiveAddr = 0x1236; // 0x1234 + 0x02
            break;
            
        case AddressingMode::IndirectX:
            setX(0x04);
            mem->writeByte(0x0200, 0x20);
            mem->writeWord(0x0024, 0x1234); // 0x20 + 0x04 = 0x24
            effectiveAddr = 0x1234;
            break;
            
        case AddressingMode::IndirectY:
            setY(0x03);
            mem->writeByte(0x0200, 0x20);
            mem->writeWord(0x0020, 0x1234);
            effectiveAddr = 0x1237; // 0x1234 + 0x03
            break;
    }
    
    return effectiveAddr;
}

// Get cycle count for each addressing mode
int getCycleCount(AddressingMode mode) {
    switch (mode) {
        case AddressingMode::ZeroPage: return 3;
        case AddressingMode::ZeroPageX: return 4;
        case AddressingMode::Absolute: return 4;
        case AddressingMode::AbsoluteX: return 5; // Always 5 for STA (no +1 for page crossing)
        case AddressingMode::AbsoluteY: return 5; // Always 5 for STA (no +1 for page crossing)
        case AddressingMode::IndirectX: return 6;
        case AddressingMode::IndirectY: return 6; // Always 6 for STA (no +1 for page crossing)
        default: return 2;
    }
}

// The main parameterized test
TEST_P(STATest, StoreAllTestValues) {
    AddressingMode mode = GetParam();
    std::string modeName;
    
    switch (mode) {
        case AddressingMode::ZeroPage: modeName = "ZeroPage"; break;
        case AddressingMode::ZeroPageX: modeName = "ZeroPageX"; break;
        case AddressingMode::Absolute: modeName = "Absolute"; break;
        case AddressingMode::AbsoluteX: modeName = "AbsoluteX"; break;
        case AddressingMode::AbsoluteY: modeName = "AbsoluteY"; break;
        case AddressingMode::IndirectX: modeName = "IndirectX"; break;
        case AddressingMode::IndirectY: modeName = "IndirectY"; break;
        default: modeName = "Unknown"; break;
    }
    
    for (uint8_t testValue : testValues) {
        SCOPED_TRACE("Mode: " + modeName + ", A: 0x" + 
                    (testValue < 16 ? "0" : "") + 
                    std::to_string(testValue));
        
        setup();
        setA(testValue);
        
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
        
        // Create and run the STA instruction
        std::unique_ptr<STA> sta(new STA(cpu, mode, getCycleCount(mode)));
        sta->run();
        
        // Verify the value was stored correctly
        ASSERT_EQ(mem->readByte(effectiveAddr), testValue)
            << "Failed to store accumulator value to memory";
        
        // Verify no flags were affected
        initialFlags.verifyUnchanged(cpu);
    }
}

// Specific test for zero-page wraparound behavior
TEST_F(STATest, ZeroPageWrapAround) {
    setup();
    setA(0x42);
    setX(0xFF);
    setPC(0x0200);
    
    // Store 0x80 at PC
    mem->writeByte(0x0200, 0x80);
    
    // Effective address should be (0x80 + 0xFF) & 0xFF = 0x7F
    uint16_t effectiveAddr = 0x007F;
    
    // Create and run the STA instruction with ZeroPageX addressing
    std::unique_ptr<STA> sta(new STA(cpu, AddressingMode::ZeroPageX, 4));
    sta->run();
    
    // Verify the value was stored correctly
    ASSERT_EQ(mem->readByte(effectiveAddr), 0x42)
        << "Zero-page wraparound failed: value not stored at 0x007F";
}

// Test with page boundary crossing (AbsoluteX and AbsoluteY)
TEST_F(STATest, PageBoundaryCrossing) {
    // AbsoluteX crossing page boundary
    {
        SCOPED_TRACE("AbsoluteX page crossing");
        setup();
        setA(0x42);
        setX(0xFF);
        setPC(0x0200);
        
        mem->writeWord(0x0200, 0x1200);  // 0x1200 + 0xFF = 0x12FF (crossing to 0x1300)
        uint16_t effectiveAddr = 0x12FF;
        
        std::unique_ptr<STA> sta(new STA(cpu, AddressingMode::AbsoluteX, 5));
        sta->run();
        
        ASSERT_EQ(mem->readByte(effectiveAddr), 0x42)
            << "AbsoluteX page boundary crossing: value not stored at 0x12FF";
    }
    
    // AbsoluteY crossing page boundary
    {
        SCOPED_TRACE("AbsoluteY page crossing");
        setup();
        setA(0x42);
        setY(0xFF);
        setPC(0x0200);
        
        mem->writeWord(0x0200, 0x1200);  // 0x1200 + 0xFF = 0x12FF
        uint16_t effectiveAddr = 0x12FF;
        
        std::unique_ptr<STA> sta(new STA(cpu, AddressingMode::AbsoluteY, 5));
        sta->run();
        
        ASSERT_EQ(mem->readByte(effectiveAddr), 0x42)
            << "AbsoluteY page boundary crossing: value not stored at 0x12FF";
    }
    
    // IndirectY crossing page boundary
    {
        SCOPED_TRACE("IndirectY page crossing");
        setup();
        setA(0x42);
        setY(0xFF);
        setPC(0x0200);
        
        mem->writeByte(0x0200, 0x80);
        mem->writeWord(0x0080, 0x1200);  // 0x1200 + 0xFF = 0x12FF
        uint16_t effectiveAddr = 0x12FF;
        
        std::unique_ptr<STA> sta(new STA(cpu, AddressingMode::IndirectY, 6));
        sta->run();
        
        ASSERT_EQ(mem->readByte(effectiveAddr), 0x42)
            << "IndirectY page boundary crossing: value not stored at 0x12FF";
    }
}

// Test IndirectX with zero page wrap-around
TEST_F(STATest, IndirectXZeroPageWrap) {
    setup();
    setA(0x42);
    setX(0xFF);
    setPC(0x0200);
    
    mem->writeByte(0x0200, 0x80);  // 0x80 + 0xFF = 0x17F, but should wrap to 0x7F
    mem->writeWord(0x007F, 0x1234);  // At wrapped address, store target address
    
    std::unique_ptr<STA> sta(new STA(cpu, AddressingMode::IndirectX, 6));
    sta->run();
    
    ASSERT_EQ(mem->readByte(0x1234), 0x42)
        << "IndirectX zero page wrap: value not stored at final address 0x1234";
}



// Generate a vector of all addressing modes used by STA
INSTANTIATE_TEST_SUITE_P(
    STAAddressingModes,
    STATest,
    ::testing::Values(
        AddressingMode::ZeroPage,
        AddressingMode::ZeroPageX,
        AddressingMode::Absolute,
        AddressingMode::AbsoluteX,
        AddressingMode::AbsoluteY,
        AddressingMode::IndirectX,
        AddressingMode::IndirectY
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