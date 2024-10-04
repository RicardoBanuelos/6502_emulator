#include <gtest/gtest.h>
#include <vector>
#include "Memory/Memory.h"


static Memory mem;
static const int SIZE = 1000;

TEST(MemoryTest, byteTest)
{

    for(int i = 0; i < SIZE; ++i)
    {
        uint16_t address = rand() % UINT16_MAX;
        uint8_t expected = rand() % UINT8_MAX;

        mem.writeByte(address, expected);

        uint8_t read = mem.readByte(address);

        ASSERT_EQ(read, expected); 
    }
}

TEST(MemoryTest, WordTest)
{

    for(int i = 0; i < SIZE; ++i)
    {
        uint16_t address = rand() % UINT16_MAX;
        uint16_t expected = rand() % UINT16_MAX;

        mem.writeWord(address, expected);

        uint16_t read = mem.readWord(address);

        ASSERT_EQ(read, expected); 
    }
}

TEST(MemoryTest, loadProgram)
{

    ASSERT_EQ(true, mem.loadBinary("../../../test/Programs/BasicProgram0/basic_program_1.bin"));
    std::vector<uint8_t> expected({0xA5, 0x12, 0xA6, 0x56, 0xA4, 0x78, 0x4C, 0x00, 0x00});

    for(uint8_t i = 0; i < expected.size(); ++i)
    {
        ASSERT_EQ(mem.readByte(i), expected[i]);
    }
}



int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    ///add more instructonions
    mem.initialize();
    mem.randomize();

    return RUN_ALL_TESTS();
}