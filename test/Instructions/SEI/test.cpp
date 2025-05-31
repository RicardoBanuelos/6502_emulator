#include <gtest/gtest.h>
#include "SEI.h"
#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include <random>
#include <functional>

static std::shared_ptr<ICPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

class SEITest : public ::testing::Test {
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
        
        void verifyPreserved(std::shared_ptr<ICPU> cpu, bool expectI) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::I), expectI) << "Interrupt flag should be set";
            ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag was modified";
            ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag was modified";
        }
    };
};

// Basic test: verify SEI sets interrupt disable flag and preserves other flags
TEST_F(SEITest, BasicFunctionality) {
    // Test with interrupt flag initially clear
    {
        SCOPED_TRACE("Interrupt flag initially clear");
        
        setI(false);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        initialFlags.verifyPreserved(cpu, true);
      
    }
    
    // Test with interrupt flag initially set
    SetUp(); // Reset before next test
    {
        SCOPED_TRACE("Interrupt flag initially set");
        
        setI(true);
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SEI with various flag combinations
TEST_F(SEITest, FlagInteractions) {
    // Test with all other flags in various states
    std::vector<std::function<void()>> flagSetups = {
        [&]() { setC(false); setZ(false); setD(false); setB(false); setV(false); setN(false); }, // All clear
        [&]() { setC(true); setZ(true); setD(true); setB(true); setV(true); setN(true); },       // All set
        [&]() { setC(true); setZ(false); setD(true); setB(false); setV(true); setN(false); },    // Alternating
        [&]() { setC(false); setZ(true); setD(false); setB(true); setV(false); setN(true); }     // Alternating opposite
    };
    
    for (size_t i = 0; i < flagSetups.size(); i++) {
        SCOPED_TRACE("Flag combination " + std::to_string(i));
        
        SetUp(); // Reset before each test
        flagSetups[i](); // Set up flags
        setI(false);     // Always start with interrupt flag clear
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        initialFlags.verifyPreserved(cpu, true);
    }
}

// Test SEI's effect on interrupt handling
TEST_F(SEITest, InterruptBehavior) {
    // This test conceptually verifies that SEI properly disables interrupts
    // by checking the I flag is set which would prevent IRQs in hardware
    
    {
        SCOPED_TRACE("SEI effect on interrupt handling");
        
        // Initialize CPU with interrupt flag clear
        setI(false);
        
        // Execute SEI
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        // Verify interrupt flag is now set
        ASSERT_TRUE(cpu->getFlag(Flag::I)) << "SEI should set interrupt flag";
        
        // Conceptually, with I flag set, IRQs would be ignored by the CPU
        // We can't directly test hardware behavior, but we can check the flag
    }
}

// Test SEI with different register and memory states
TEST_F(SEITest, RegisterAndMemoryInteractions) {
    // Test that SEI doesn't affect registers or memory
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
        std::vector<uint8_t> memValues;
        for (int j = 0; j < 10; j++) {
            uint8_t value = rand() & 0xFF;
            mem->writeByte(address + j, value);
            memValues.push_back(value);
        }
        
        // Execute SEI
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        // Verify interrupt flag is set but nothing else changed
        ASSERT_TRUE(cpu->getFlag(Flag::I)) << "SEI should set interrupt flag";
        ASSERT_EQ(cpu->getRegister(Register::A), a) << "SEI shouldn't affect A register";
        ASSERT_EQ(cpu->getRegister(Register::X), x) << "SEI shouldn't affect X register";
        ASSERT_EQ(cpu->getRegister(Register::Y), y) << "SEI shouldn't affect Y register";
        
        // Verify memory wasn't changed
        for (int j = 0; j < 10; j++) {
            ASSERT_EQ(mem->readByte(address + j), memValues[j]) 
                << "SEI shouldn't affect memory at address " << std::hex << (address + j);
        }
    }
}

// Test SEI followed by RTI behavior
TEST_F(SEITest, SEIandRTI) {
    // SEI followed by RTI should preserve interrupt state on the stack
    {
        SCOPED_TRACE("SEI and RTI interaction");
        
        // Setup: Initialize stack with PS having I=0
        uint8_t ps_on_stack = 0x20; // Some arbitrary PS value with I=0
        uint16_t pc_on_stack = 0x1234;
        uint8_t initialSP = 0xFA;
        
        // Set stack pointer
        cpu->setRegister(Register::SP, initialSP);
        
        // Push PC and PS on the stack (as if from an interrupt)
        // For a descending stack, values are stored at addresses:
        // 0x01FD (initialSP): PS
        // 0x01FC (initialSP-1): PC low byte
        // 0x01FB (initialSP-2): PC high byte
        mem->writeByte(0x0100 + initialSP, ps_on_stack);          // PS at top of stack
        mem->writeByte(0x0100 + initialSP - 1, pc_on_stack & 0xFF);      // PC low byte
        mem->writeByte(0x0100 + initialSP - 2, (pc_on_stack >> 8) & 0xFF); // PC high byte
        
        // Set current PS with I=0
        uint8_t current_ps = 0x00; // All flags clear
        cpu->setRegister(Register::PS, current_ps);
        
        // Execute SEI
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        // Verify I flag is now set in current PS
        ASSERT_TRUE(cpu->getFlag(Flag::I)) << "SEI should set interrupt flag";
        
        // Now simulate RTI reading from the stack (without actually popping)
        // PS is at the top of stack
        uint8_t popped_ps = mem->readByte(0x0100 + initialSP);
        // PC low byte
        uint16_t popped_pc_low = mem->readByte(0x0100 + initialSP - 1);
        // PC high byte
        uint16_t popped_pc_high = mem->readByte(0x0100 + initialSP - 2);
        uint16_t popped_pc = (popped_pc_high << 8) | popped_pc_low;
        
        // RTI should restore PS from stack, which had I=0
        ASSERT_EQ(popped_ps, ps_on_stack) << "PS on stack should be unchanged";
        ASSERT_EQ(popped_pc, pc_on_stack) << "PC on stack should be unchanged";
        
        // The stack should be unaffected by SEI - specifically, I flag in PS on stack
        ASSERT_EQ((popped_ps & 0x04), 0) << "I flag should be 0 in the PS on stack";
    }
}


// Test SEI in various CPU states
TEST_F(SEITest, CPUStates) {
    // Test SEI after reset
    {
        SCOPED_TRACE("SEI after reset");
        
        cpu->reset();
        // Get the current I flag state (might be set by reset)
        bool i_after_reset = cpu->getFlag(Flag::I);
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        ASSERT_TRUE(cpu->getFlag(Flag::I)) << "SEI should set interrupt flag after reset";
    }
    
    // Test SEI in interrupt context
    SetUp();
    {
        SCOPED_TRACE("SEI in interrupt context");
        
        // Simulate the start of an interrupt handler where I is already set
        setI(true);
        
        CPUFlags initialFlags;
        initialFlags.captureFrom(cpu);
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
        initialFlags.verifyPreserved(cpu, true);
        ASSERT_TRUE(cpu->getFlag(Flag::I)) << "SEI should keep interrupt flag set in interrupt context";
    }
}

// Randomized testing for comprehensive coverage
TEST_F(SEITest, RandomizedState) {
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
        
        std::unique_ptr<SEI> sei(new SEI(cpu));
        sei->run();
        
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