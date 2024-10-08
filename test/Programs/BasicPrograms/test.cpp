#include <gtest/gtest.h>
#include <math.h>
#include "Instruction/Instructions/ADC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<CPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(programs, basic_program_test_0)
{

    for(int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        ASSERT_EQ(true, mem->loadBinary("../../../../test/Programs/BasicPrograms/basic_program.bin"));
        
        cpu->reset();

        cpu->setRegister(Register::PC, 0);

        Instruction *current = cpu->fetchInstruction();
        ASSERT_NE(current, nullptr);

        while(current->name() != "BRK")
        {
            current->run();
            current = cpu->fetchInstruction();
            ASSERT_EQ(current != nullptr, true);
        }

        ASSERT_EQ(cpu->getRegister(Register::A), 0x8F);
        ASSERT_EQ(cpu->getRegister(Register::X), 0X0A);
        ASSERT_EQ(cpu->getRegister(Register::Y), 0x18);
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF);

        ASSERT_EQ(cpu->getFlag(Flag::N), true);
        ASSERT_EQ(cpu->getFlag(Flag::V), false);
        ASSERT_EQ(cpu->getFlag(Flag::Z), false);
        ASSERT_EQ(cpu->getFlag(Flag::C), false);

    }

}


TEST(programs, basic_program_test_1)
{
    cpu->setFlag(Flag::B, true);
    for(int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        ASSERT_EQ(true, mem->loadBinary("../../../../test/Programs/BasicPrograms/basic_program_2.bin"));
        
        cpu->reset();

        cpu->setRegister(Register::PC, 0);


        Instruction *current = cpu->fetchInstruction();
        ASSERT_NE(current, nullptr);

        while(current->name() != "BRK")
        {
            current->run();
            current = cpu->fetchInstruction();
            ASSERT_EQ(current != nullptr, true);
        }

        ASSERT_EQ(cpu->getRegister(Register::A), 0x8F);
        ASSERT_EQ(cpu->getRegister(Register::X), 0X0A);
        ASSERT_EQ(cpu->getRegister(Register::Y), 0X18);
        ASSERT_EQ(cpu->getRegister(Register::SP), 0xFF);

        ASSERT_EQ(cpu->getFlag(Flag::N), true);
        ASSERT_EQ(cpu->getFlag(Flag::V), false);
        ASSERT_EQ(cpu->getFlag(Flag::Z), false);
        ASSERT_EQ(cpu->getFlag(Flag::C), false);

    }

}

TEST(programs, basic_program_square_root)
{


    //00F0: number to find square root low byte
    //00F1: number to find square root high byte
    //00F2: remainder low byte
    //00F3: remainder high byte
    //00F6: square root (answer)
    
    uint16_t INPUT_LOW_ADDR = 0x00F0;
    uint16_t OUTPUT_ADDR = 0x00F6;
    
    
    for(int i = 0; i < 1000; ++i)
    {
        mem->randomize();
        //ASSERT_EQ(true, mem->loadBinary("../../../../test/Programs/BasicPrograms/squareRoot.bin", 0x8000));
        ASSERT_EQ(true, mem->loadBinary("P:/Github/6502_emulator/test/Programs/BasicPrograms/squareRoot.bin", 0x8000));
        

        cpu->reset();
        cpu->setRegister(Register::PC, 0x8000);

        //write input
        mem->writeWord(INPUT_LOW_ADDR, rand() % UINT16_MAX);

        Instruction *current = cpu->fetchInstruction();
        ASSERT_NE(current, nullptr);

        uint8_t expected = sqrt(mem->readWord(INPUT_LOW_ADDR));
        uint8_t remainder = 1;

        while(current->name() != "RTS")
        {
            current->run();
            current = cpu->fetchInstruction();
            ASSERT_EQ(current != nullptr, true);
        }    

        ASSERT_EQ(expected, mem->readByte(OUTPUT_ADDR));
    }
   
}

int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    mem->initialize();
    mem->randomize();
   

    cpu->init();

    cpu->connectBus(bus);
    bus->connectMemory(mem);
    
   
    return RUN_ALL_TESTS();
}