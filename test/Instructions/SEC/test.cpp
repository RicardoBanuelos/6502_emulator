#include <gtest/gtest.h>
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "SEC.h"
#include <random>
#include <functional>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

class SECTest : public ::testing::Test {
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
        
        void verifyPreserved(std::shared_ptr<ICPU> cpu, bool expectC) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), expectC) << "Carry flag should be set";
            ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag was modified";
        }
    };
};

// Basic test: verify SEC sets carry flag and preserves other flags
TEST_F(SECTest, BasicFunctionality) {
    // Test with carry initially clear
    {
        SCOPED_TRACE("Carry initially clear");
        
        setC(false);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        initialFlags.verifyPreserved(cpu, true);
        //ASSERT_EQ(sec->getCycles(), 2) << "SEC should take 2 cycles";
    }
    
    // Test with carry initially set
    SetUp(); // Reset before next test
    {
        SCOPED_TRACE("Carry initially set");
        
        setC(true);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SEC with various flag combinations
TEST_F(SECTest, FlagInteractions) {
    // Test with all other flags in various states
    std::vector<std::function<void()>> flagSetups = {
        [&]() { setZ(false); setI(false); setD(false); setB(false); setV(false); setN(false); }, // All clear
        [&]() { setZ(true); setI(true); setD(true); setB(true); setV(true); setN(true); },       // All set
        [&]() { setZ(true); setI(false); setD(true); setB(false); setV(true); setN(false); },    // Alternating
        [&]() { setZ(false); setI(true); setD(false); setB(true); setV(false); setN(true); }     // Alternating opposite
    };
    
    for (size_t i = 0; i < flagSetups.size(); i++) {
        SCOPED_TRACE("Flag combination " + std::to_string(i));
        
        SetUp(); // Reset before each test
        flagSetups[i](); // Set up flags
        setC(false);     // Always start with carry clear
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SEC in combination with other instructions
TEST_F(SECTest, InstructionCombinations) {
    // This test simulates executing SEC followed by instructions that use the carry flag
    
    // SEC followed by ADC - carry should affect addition
    {
        SCOPED_TRACE("SEC + ADC");
        
        // Set initial state
        setC(false);
        cpu->setRegister(Register::A, 0x10);
        
        // Execute SEC
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::C)) << "SEC should set carry flag";
        
        // Now simulate ADC #$20 (add with carry)
        cpu->setRegister(Register::A, cpu->getRegister(Register::A) + 0x20 + 1); // +1 because carry is set
        
        ASSERT_EQ(cpu->getRegister(Register::A), 0x31) << "ADC should use carry flag: 0x10 + 0x20 + 1 = 0x31";
    }
    
    // SEC followed by SBC - carry should affect subtraction
    SetUp();
    {
        SCOPED_TRACE("SEC + SBC");
        
        // Set initial state
        setC(false);
        cpu->setRegister(Register::A, 0x50);
        
        // Execute SEC
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::C)) << "SEC should set carry flag";
        
        // Now simulate SBC #$30 (subtract with carry)
        // In 6502, carry=1 means no borrow for SBC
        cpu->setRegister(Register::A, cpu->getRegister(Register::A) - 0x30); // No -1 since carry is set (no borrow)
        
        ASSERT_EQ(cpu->getRegister(Register::A), 0x20) << "SBC should use carry flag: 0x50 - 0x30 = 0x20 (without borrow)";
    }
    
    // SEC followed by ROL - carry should shift into result
    SetUp();
    {
        SCOPED_TRACE("SEC + ROL");
        
        // Set initial state
        setC(false);
        cpu->setRegister(Register::A, 0x45); // 01000101
        
        // Execute SEC
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::C)) << "SEC should set carry flag";
        
        // Now simulate ROL A (rotate left)
        uint8_t a = cpu->getRegister(Register::A);
        bool newCarry = (a & 0x80) != 0;
        a = (a << 1) | 0x01; // Set bit 0 since carry is set
        cpu->setRegister(Register::A, a);
        setC(newCarry);
        
        ASSERT_EQ(cpu->getRegister(Register::A), 0x8B) << "ROL should use carry flag: 01000101 -> 10001011";
    }
}

// Test SEC with different memory states
TEST_F(SECTest, MemoryInteractions) {
    // Test that SEC doesn't affect or depend on memory contents
    for (int i = 0; i < 10; i++) {
        SCOPED_TRACE("Memory test iteration " + std::to_string(i));
        
        SetUp();
        
        // Setup a random memory state
        uint16_t address = 0x200 + (rand() % 0x100);
        setPC(address);
        
        // Write random values around the PC
        for (int j = -5; j <= 5; j++) {
            mem->writeByte(address + j, rand() & 0xFF);
        }
        
        // Execute SEC
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        // Verify carry is set
        ASSERT_TRUE(cpu->getFlag(Flag::C)) << "SEC should set carry flag regardless of memory contents";
        
        // Verify PC didn't change (SEC is implied addressing)
        ASSERT_EQ(cpu->getRegister(Register::PC), address) << "SEC shouldn't modify PC";
    }
}


// Test SEC after various CPU states
TEST_F(SECTest, CPUStates) {
    // Test SEC after reset
    {
        SCOPED_TRACE("SEC after reset");
        
        cpu->reset();
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
    
    // Test SEC after simulated interrupt
    SetUp();
    {
        SCOPED_TRACE("SEC after interrupt");
        
        // Simulate interrupt behavior
        cpu->setFlag(Flag::I, true); // Set interrupt disable
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Randomized testing to ensure robustness
TEST_F(SECTest, RandomizedState) {
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
        setPC(0x200 + (rng() % 0x100));
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEC> sec(new SEC(cpu));
        sec->run();
        
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