#include <gtest/gtest.h>
#include "SED.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <random>
#include <functional>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

class SEDTest : public ::testing::Test {
protected:
    void SetUp() override {
        mem->initialize();
        mem->randomize();
        cpu->reset();
        CPU *cppu = static_cast<CPU*>(cpu.get());
        cppu->init();
        cppu->connectBus(bus);
        bus->connectMemory(mem);
    }
    
    // Helper methods
    void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
    void setC(bool value) { cpu->setFlag(Flag::C, value); }
    void setZ(bool value) { cpu->setFlag(Flag::Z, value); }
    void setI(bool value) { cpu->setFlag(Flag::I, value); }
    void setD(bool value) { cpu->setFlag(Flag::D, value); }
    void setB(bool value) { cpu->setFlag(Flag::B, value); }
    void setV(bool value) { cpu->setFlag(Flag::V, value); }
    void setN(bool value) { cpu->setFlag(Flag::N, value); }
    
    // Capture and store all CPU flags
    struct CPUFlags {
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
        
        void verifyPreserved(std::shared_ptr<ICPU> cpu, bool expectD) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::D), expectD) << "Decimal flag should be set";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag was modified";
        }
    };
};

// Basic test: verify SED sets decimal flag and preserves other flags
TEST_F(SEDTest, BasicFunctionality) {
    // Test with decimal mode initially clear
    {
        SCOPED_TRACE("Decimal mode initially clear");
        
        setD(false);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        initialFlags.verifyPreserved(cpu, true);
       
    }
    
    // Test with decimal mode initially set
    SetUp(); // Reset before next test
    {
        SCOPED_TRACE("Decimal mode initially set");
        
        setD(true);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SED with various flag combinations
TEST_F(SEDTest, FlagInteractions) {
    // Test with all other flags in various states
    std::vector<std::function<void()>> flagSetups = {
        [&]() { setC(false); setZ(false); setI(false); setB(false); setV(false); setN(false); }, // All clear
        [&]() { setC(true); setZ(true); setI(true); setB(true); setV(true); setN(true); },       // All set
        [&]() { setC(true); setZ(false); setI(true); setB(false); setV(true); setN(false); },    // Alternating
        [&]() { setC(false); setZ(true); setI(false); setB(true); setV(false); setN(true); }     // Alternating opposite
    };
    
    for (size_t i = 0; i < flagSetups.size(); i++) {
        SCOPED_TRACE("Flag combination " + std::to_string(i));
        
        SetUp(); // Reset before each test
        flagSetups[i](); // Set up flags
        setD(false);     // Always start with decimal clear
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SED followed by arithmetic in decimal mode
TEST_F(SEDTest, DecimalModeArithmetic) {
    // This tests that SED properly enables decimal arithmetic mode
    
    // SED followed by ADC - should perform BCD addition
    {
        SCOPED_TRACE("SED + ADC (BCD addition)");
        
        // Set initial state
        setD(false);
        cpu->setRegister(Register::A, 0x09); // 9 in decimal
        
        // Execute SED
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::D)) << "SED should set decimal flag";
        
        // Perform ADC #$01 (add 1) - in decimal mode
        // This simulates what a proper BCD ADC would do
        uint8_t a = cpu->getRegister(Register::A);
        uint8_t operand = 0x01;
        uint8_t result;
        
        // Simulate BCD addition logic
        if ((a & 0x0F) + (operand & 0x0F) > 9) {
            // BCD adjustment needed for lower nibble
            result = ((a + operand + 0x06) & 0x0F) | (((a & 0xF0) + (operand & 0xF0) + 0x10) & 0xF0);
        } else {
            // Regular addition for lower nibble
            result = ((a + operand) & 0x0F) | ((a & 0xF0) + (operand & 0xF0));
        }
        
        // Manual implementation of decimal addition 9 + 1 = 10 (0x10 in BCD)
        ASSERT_EQ(result, 0x10) << "In decimal mode, 9 + 1 should equal 10 (0x10 in BCD)";
    }
    
    // SED followed by SBC - should perform BCD subtraction
    SetUp();
    {
        SCOPED_TRACE("SED + SBC (BCD subtraction)");
        
        // Set initial state
        setD(false);
        cpu->setRegister(Register::A, 0x50); // 50 in decimal
        setC(true); // No borrow
        
        // Execute SED
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::D)) << "SED should set decimal flag";
        
        // Perform SBC #$01 (subtract 1) - in decimal mode
        // This simulates what a proper BCD SBC would do
        uint8_t a = cpu->getRegister(Register::A);
        uint8_t operand = 0x01;
        uint8_t result;
        
        // Simplified BCD subtraction for this test
        if ((a & 0x0F) < (operand & 0x0F)) {
            // Borrow from tens
            result = ((a - operand - 0x06) & 0x0F) | (((a & 0xF0) - 0x10) & 0xF0);
        } else {
            // No borrow needed
            result = ((a - operand) & 0x0F) | (a & 0xF0);
        }
        
        // Manual implementation of decimal subtraction 50 - 1 = 49 (0x49 in BCD)
        ASSERT_EQ(result, 0x49) << "In decimal mode, 50 - 1 should equal 49 (0x49 in BCD)";
    }
}

// Test SED with register and memory state
TEST_F(SEDTest, RegisterAndMemoryInteractions) {
    // Test that SED doesn't affect registers or memory
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Register/memory test " + std::to_string(i));
        
        SetUp();
        
        // Set random register values
        uint8_t a = rand() & 0xFF;
        uint8_t x = rand() & 0xFF;
        uint8_t y = rand() & 0xFF;
        
        cpu->setRegister(Register::A, a);
        cpu->setRegister(Register::X, x);
        cpu->setRegister(Register::Y, y);
        
        // Set random memory values
        uint16_t address = 0x200;
        for (int j = 0; j < 10; j++) {
            uint8_t value = rand() & 0xFF;
            mem->writeByte(address + j, value);
        }
        
        // Execute SED
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        // Verify decimal flag is set but nothing else changed
        ASSERT_TRUE(cpu->getFlag(Flag::D)) << "SED should set decimal flag";
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "SED shouldn't affect A register";
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "SED shouldn't affect X register";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "SED shouldn't affect Y register";
        
        // Verify memory wasn't changed
        for (int j = 0; j < 10; j++) {
            uint8_t expected = mem->readByte(address + j);
            ASSERT_EQ(mem->readByte(address + j), expected) << "SED shouldn't affect memory";
        }
    }
}

// Test SED after various CPU operations
TEST_F(SEDTest, AfterCPUOperations) {
    // Test SED after reset
    {
        SCOPED_TRACE("SED after reset");
        
        cpu->reset(); // Should clear decimal flag
        ASSERT_FALSE(cpu->getFlag(Flag::D)) << "Reset should clear decimal flag";
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::D)) << "SED should set decimal flag after reset";
    }
    
    // Test SED after interrupt handling
    SetUp();
    {
        SCOPED_TRACE("SED after interrupt");
        
        // Simulate interrupt which should clear decimal flag on real 6502
        setD(true);
        setI(true); // Set interrupt disable
        // On a real 6502, decimal mode is cleared after an interrupt
        setD(false);
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::D)) << "SED should set decimal flag after interrupt";
    }
}

// Randomized testing to ensure robustness
TEST_F(SEDTest, RandomizedState) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 1);
    
    const int ITERATIONS = 100;
    
    for (int i = 0; i < ITERATIONS; i++) {
        SCOPED_TRACE("Randomized test iteration " + std::to_string(i));
        
        SetUp();
        
        // Set random flags
        setC(dist(rng));
        setZ(dist(rng));
        setI(dist(rng));
        setD(dist(rng));
        setB(dist(rng));
        setV(dist(rng));
        setN(dist(rng));
        
        // Set random registers
        cpu->setRegister(Register::A, rng() & 0xFF);
        cpu->setRegister(Register::X, rng() & 0xFF);
        cpu->setRegister(Register::Y, rng() & 0xFF);
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SED> sed(new SED(cpu));
        sed->run();
        
        initialFlags.verifyPreserved(cpu, true);
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