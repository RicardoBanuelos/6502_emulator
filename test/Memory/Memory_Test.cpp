#include <gtest/gtest.h>

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



int main(int argc, char** argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    ///add more instructonions
    mem.initialize();

    return RUN_ALL_TESTS();
}