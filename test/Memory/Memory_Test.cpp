#include <gtest/gtest.h>

#include "Memory/Memory.h"


static Memory mem;


TEST(MemoryTest, readWrite0)
{
    uint32_t address = 0xFF00;
    uint8_t expected = 0x12;

    mem.writeByte(address, expected);

    uint8_t read = mem.readByte(address);

    ASSERT_EQ(read, expected); 
}


TEST(MemoryTest, readWrite1)
{
    uint32_t address = 0xFF00;
    uint16_t expected = 0x1215;

    mem.writeWord(address, expected);

    uint16_t read = mem.readWord(address);

    ASSERT_EQ(read, expected); 
}


TEST(MemoryTest, WordTest)
{

    for(int i = 0; i < 100; ++i)
    {
        uint32_t address = rand() % Memory::MAX_MEMORY;
        uint16_t expected = rand() % UINT16_MAX;

        mem.writeWord(address, expected);

        ASSERT_EQ(mem.readWord(address), expected); 
    }

}

TEST(MemoryTest, ByteTest)
{

    for(int i = 0; i < 100; ++i)
    {
        uint32_t address = rand() % Memory::MAX_MEMORY;
        uint8_t expected = rand() % UINT8_MAX;

        mem.writeByte(address, expected);

        ASSERT_EQ(mem.readByte(address), expected); 
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