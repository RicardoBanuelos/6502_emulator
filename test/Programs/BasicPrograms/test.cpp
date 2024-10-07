#include <gtest/gtest.h>
#include "Instruction/Instructions/ADC.h"
#include "CPU/CPU.h"
#include "Memory/Memory.h"
#include "Bus/Bus.h"

static std::shared_ptr<CPU> cpu(new CPU());
static std::shared_ptr<Memory> mem(new Memory());
static std::shared_ptr<Bus> bus(new Bus());

TEST(programs, subroutine_test)
{
    ASSERT_EQ(true, mem->loadBinary("P:/Github/6502_emulator/test/Programs/BasicPrograms/basic_program.bin"));
    
    cpu->reset();
    cpu->setRegister(Register::PC, 0);
    cpu->setRegister(Register::PS, 0x30);
    
    while(true)
    {
        cpu->execute();
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