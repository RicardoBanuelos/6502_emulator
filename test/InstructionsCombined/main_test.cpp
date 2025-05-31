#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

#include "CPU.h"
#include "Memory.h"
#include "Bus.h"
#include "OpCodes.h"
#include "Instructions.h"

class CombinedInstructionsTest : public ::testing::Test {
protected:
    std::shared_ptr<ICPU> cpu;
    std::shared_ptr<Memory> mem;
    std::shared_ptr<Bus> bus;
    // No need for separate InstructionFactory

    void SetUp() override {
        cpu = std::make_shared<CPU>();
        mem = std::make_shared<Memory>();
        bus = std::make_shared<Bus>();

        mem->initialize();
        mem->randomize();
        
        cpu->reset();
        static_cast<CPU*>(cpu.get())->init();
        static_cast<CPU*>(cpu.get())->connectBus(bus);
        bus->connectMemory(mem);
    }

    // Helper to load a program into memory at specified address
    void loadProgram(uint16_t address, const std::vector<uint8_t>& program) {
        for (size_t i = 0; i < program.size(); i++) {
            mem->writeByte(address + i, program[i]);
        }
    }
    
    // Helper to execute a program until a BRK instruction or max cycles
    void runProgram(uint16_t startAddress, int maxCycles = 1000) {
        cpu->setRegister(Register::PC, startAddress);
        
        int cycles = 0;
        CPU* cpuImpl = static_cast<CPU*>(cpu.get());
        
        while (cycles < maxCycles) {
            uint8_t opcode = mem->readByte(cpu->getRegister(Register::PC));
            if (opcode == 0x00) { // BRK instruction
                break;
            }
            
            // Use CPU's existing instruction table and methods
            OpCode op = static_cast<OpCode>(opcode);
            
            // Increment PC before we execute the instruction (CPU fetches the opcode)
            cpu->setRegister(Register::PC, cpu->getRegister(Register::PC) + 1);
            
            // Get the instruction and execute it
            try {
                Instruction* instruction = (Instruction*)cpuImpl->getInstruction(op);
                if (instruction) {
                    instruction->run();
                    //cycles += instruction->getCycles();
                } else {
                    FAIL() << "Invalid opcode encountered: " << std::hex << static_cast<int>(opcode);
                    break;
                }
            } catch (const std::exception& e) {
                FAIL() << "Exception: " << e.what() << " for opcode: " << std::hex << static_cast<int>(opcode);
                break;
            }
        }
        
        ASSERT_LT(cycles, maxCycles) << "Program did not terminate within maximum cycles";
    }
    
    // Stack helpers that respect descending stack behavior
    void push(uint8_t value) {
        CPU* cpuImpl = static_cast<CPU*>(cpu.get());
        cpuImpl->pushByte(value);
    }
    
    uint8_t pull() {
        CPU* cpuImpl = static_cast<CPU*>(cpu.get());
        return cpuImpl->popByte();
    }
    
    void pushWord(uint16_t value) {
        CPU* cpuImpl = static_cast<CPU*>(cpu.get());
        cpuImpl->pushWord(value);
    }
    
    uint16_t pullWord() {
        CPU* cpuImpl = static_cast<CPU*>(cpu.get());
        return cpuImpl->popWord();
    }
    
    // Helper methods
    void setA(uint8_t value) { cpu->setRegister(Register::A, value); }
    void setX(uint8_t value) { cpu->setRegister(Register::X, value); }
    void setY(uint8_t value) { cpu->setRegister(Register::Y, value); }
    void setSP(uint8_t value) { cpu->setRegister(Register::SP, value); }
    void setPC(uint16_t value) { cpu->setRegister(Register::PC, value); }
    
    void setFlag(Flag flag, bool value) { cpu->setFlag(flag, value); }
    
    // Full CPU state capture for verification
    struct CPUState {
        uint8_t A, X, Y, SP, PS;
        uint16_t PC;
        bool C, Z, I, D, B, V, N;
        
        void capture(std::shared_ptr<ICPU> cpu) {
            A = cpu->getRegister(Register::A);
            X = cpu->getRegister(Register::X);
            Y = cpu->getRegister(Register::Y);
            SP = cpu->getRegister(Register::SP);
            PC = cpu->getRegister(Register::PC);
            PS = cpu->getRegister(Register::PS);
            
            C = cpu->getFlag(Flag::C);
            Z = cpu->getFlag(Flag::Z);
            I = cpu->getFlag(Flag::I);
            D = cpu->getFlag(Flag::D);
            B = cpu->getFlag(Flag::B);
            V = cpu->getFlag(Flag::V);
            N = cpu->getFlag(Flag::N);
        }
        
        void verify(std::shared_ptr<ICPU> cpu, 
                    bool checkA = true, bool checkX = true, bool checkY = true,
                    bool checkSP = true, bool checkPC = true) const {
            if (checkA) {
                ASSERT_EQ(cpu->getRegister(Register::A), A) << "A register mismatch";
            }
            if (checkX) {
                ASSERT_EQ(cpu->getRegister(Register::X), X) << "X register mismatch";
            }
            if (checkY) {
                ASSERT_EQ(cpu->getRegister(Register::Y), Y) << "Y register mismatch";
            }
            if (checkSP) {
                ASSERT_EQ(cpu->getRegister(Register::SP), SP) << "Stack pointer mismatch";
            }
            if (checkPC) {
                ASSERT_EQ(cpu->getRegister(Register::PC), PC) << "Program counter mismatch";
            }
        }
        
        void verifyFlags(std::shared_ptr<ICPU> cpu) const {
            ASSERT_EQ(cpu->getFlag(Flag::C), C) << "Carry flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::Z), Z) << "Zero flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::I), I) << "Interrupt flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::D), D) << "Decimal flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::B), B) << "Break flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::V), V) << "Overflow flag mismatch";
            ASSERT_EQ(cpu->getFlag(Flag::N), N) << "Negative flag mismatch";
        }
    };
};

// Now implement your test cases...

// Test stack operations (PHA, PHP, PLA, PLP) with descending stack
TEST_F(CombinedInstructionsTest, StackOperations) {
    // Program: PHA, PHP, PLA, PLP
    std::vector<uint8_t> program = {
        0x48,  // PHA - Push A to stack
        0x08,  // PHP - Push processor status to stack
        0x68,  // PLA - Pull from stack to A
        0x28,  // PLP - Pull from stack to processor status
        0x00   // BRK
    };
    
    // Initialize registers
    setA(0x42);
    setX(0x24);
    setSP(0xFF);  // Initial stack pointer (top of stack)
    
    // Set some processor flags
    setFlag(Flag::C, true);
    setFlag(Flag::Z, false);
    setFlag(Flag::N, true);
    
    // Save initial state
    CPUState initialState;
    initialState.capture(cpu);
    
    // Load and run program
    loadProgram(0x0200, program);
    runProgram(0x0200);
    
    // After PHA, PHA, PLA, PLP sequence, CPU state should match initial state
    CPUState finalState;
    finalState.capture(cpu);
    
    // Verify A and flags are preserved through the push/pull operations
    ASSERT_EQ(finalState.A, initialState.A) << "A register should be preserved";
    ASSERT_EQ(finalState.PS & 0xEF, initialState.PS & 0xEF) << "Processor status should be preserved"; // Mask B flag
    
    // Stack pointer should be the same after push/push/pull/pull
    ASSERT_EQ(finalState.SP, initialState.SP) << "Stack pointer should be unchanged";
}

// Add more test cases here...

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}