#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "Memory.h"
#include "test_paths.h"

class MemoryTest : public ::testing::Test {
protected:
    Memory mem;

    void SetUp() override {
        mem.initialize();
    }
};

TEST_F(MemoryTest, WriteAndReadByte_FirstAndLast) {
    mem.writeByte(0, 0x11);
    mem.writeByte(mem.getMaxMemory() - 1, 0x22);
    EXPECT_EQ(mem.readByte(0), 0x11);
    EXPECT_EQ(mem.readByte(mem.getMaxMemory() - 1), 0x22);
}

TEST_F(MemoryTest, WriteAndReadByte_OutOfBounds) {
    mem.writeByte(mem.getMaxMemory(), 0x33); // Should wrap
    EXPECT_EQ(mem.readByte(mem.getMaxMemory()), 0x33); // Should return 0x33
}

TEST_F(MemoryTest, WriteAndReadWord_AtStart) {
    mem.writeWord(0, 0xBEEF);
    EXPECT_EQ(mem.readWord(0), 0xBEEF);
}

TEST_F(MemoryTest, WriteAndReadWord_AtEndMinusOne) {
    mem.writeWord(mem.getMaxMemory() - 2, 0x1234);
    EXPECT_EQ(mem.readWord(mem.getMaxMemory() - 2), 0x1234);
}

TEST_F(MemoryTest, WriteAndReadWord_AtLastByte) {
    mem.writeWord(mem.getMaxMemory() - 1, 0x5678);
    EXPECT_EQ(mem.readWord(mem.getMaxMemory() - 1), 0x5678);
}

TEST_F(MemoryTest, WriteAndReadWord_OutOfBounds) {
    mem.writeWord(mem.getMaxMemory(), 0x9999); // Should wrap
    EXPECT_EQ(mem.readWord(mem.getMaxMemory()), 0x9999);
}

TEST_F(MemoryTest, RandomizeChangesMemory) {
    mem.writeByte(0, 0);
    mem.writeByte(1, 0);
    mem.randomize();
    // Not guaranteed, but very likely
    EXPECT_NE(mem.readByte(0), 0);
    EXPECT_NE(mem.readByte(1), 0);
}

TEST_F(MemoryTest, RandomizeWholeMemory) {
    mem.initialize();
    mem.randomize();
    int nonzero = 0;
    for (uint32_t i = 0; i < 100; ++i) {
        if (mem.readByte(i) != 0) ++nonzero;
    }
    EXPECT_GT(nonzero, 80); // Most bytes should be nonzero
}

TEST_F(MemoryTest, InitializeSetsAllZero) {
    mem.randomize();
    mem.initialize();
    for (uint32_t i = 0; i < 100; ++i) {
        EXPECT_EQ(mem.readByte(i), 0);
    }
}

TEST_F(MemoryTest, LoadBinaryFile_DefaultAddress) {
    const char* filename = "memtest_tmp.bin";
    std::vector<uint8_t> data = {0xA5, 0x12, 0xA6, 0x56, 0xA4, 0x78, 0x4C, 0x00, 0x00};
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
    ofs.close();

    ASSERT_TRUE(mem.loadBinary(filename));
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(mem.readByte(i), data[i]);
    }
    std::remove(filename);
}

TEST_F(MemoryTest, LoadBinaryFile_CustomStartAddress) {
    const char* filename = "memtest_tmp2.bin";
    std::vector<uint8_t> data = {0xDE, 0xAD, 0xBE, 0xEF};
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
    ofs.close();

    uint16_t startAddr = 100;
    ASSERT_TRUE(mem.loadBinary(filename, startAddr));
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(mem.readByte(startAddr + i), data[i]);
    }
    std::remove(filename);
}

TEST_F(MemoryTest, LoadBinaryFile_TooLarge) {
    const char* filename = "memtest_big.bin";
    std::vector<uint8_t> data(mem.getMaxMemory() + 10, 0xAA);
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
    ofs.close();

    // Should only load up to memory limit
    ASSERT_TRUE(mem.loadBinary(filename));
    for (uint32_t i = 0; i < mem.getMaxMemory(); ++i) {
        EXPECT_EQ(mem.readByte(i), 0xAA);
    }
    std::remove(filename);
}

TEST_F(MemoryTest, LoadBinaryFile_NotFound) {
    EXPECT_FALSE(mem.loadBinary("nonexistent_file.bin"));
}

TEST_F(MemoryTest, DumpMemory_Basic) {
    // Just check that it doesn't crash
    mem.writeByte(0, 0x42);
    mem.writeByte(1, 0x43);
    mem.dumpMemory(0, 1);
}

TEST_F(MemoryTest, ValidateAddress_PrivateMethod) {
    // This test can't access private methods directly, but we can infer behavior
    EXPECT_EQ(mem.readByte(mem.getMaxMemory()), 0);
    mem.writeByte(mem.getMaxMemory(), 0xFF); // Should wrap write
    EXPECT_EQ(mem.readByte(mem.getMaxMemory()), 0xFF);
}

TEST_F(MemoryTest, OverwriteMemory) {
    mem.writeByte(10, 0xAA);
    EXPECT_EQ(mem.readByte(10), 0xAA);
    mem.writeByte(10, 0xBB);
    EXPECT_EQ(mem.readByte(10), 0xBB);
}

TEST_F(MemoryTest, LoadCustomBinaryFile) {
    // Create a temp binary file
    ASSERT_TRUE(mem.loadBinary(MEMORY_BINARY_PATH));
    std::vector<uint8_t> data = {0xA5, 0x12, 0xA6, 0x56, 0xA4, 0x78, 0x4C, 0x00, 0x00};
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(mem.readByte(i), data[i]);
    }
}

TEST_F(MemoryTest, WordWriteReadAcrossBoundary) {
    // Should not write/read across memory boundary
    mem.writeWord(mem.getMaxMemory() - 1, 0x1234); //should wrap
    EXPECT_EQ(mem.readWord(mem.getMaxMemory() - 1), 0x1234);
}

// Add these tests to your existing file

// Test byte operations with all possible byte values
TEST_F(MemoryTest, WriteAndReadAllByteValues) {
    for (int i = 0; i <= 0xFF; i++) {
        mem.writeByte(i, i);
        EXPECT_EQ(mem.readByte(i), i);
    }
}

// Test endianness of word operations
TEST_F(MemoryTest, WordEndianness) {
    uint16_t value = 0xABCD;
    mem.writeWord(0x100, value);
    
    // For 6502, little-endian means low byte first, high byte second
    EXPECT_EQ(mem.readByte(0x100), 0xCD); // Low byte
    EXPECT_EQ(mem.readByte(0x101), 0xAB); // High byte
}

// Test sequential memory access patterns
TEST_F(MemoryTest, SequentialAccess) {
    // Write a pattern
    for (uint32_t i = 0; i < 256; i++) {
        mem.writeByte(i * 4, i);
    }
    
    // Verify the pattern
    for (uint32_t i = 0; i < 256; i++) {
        EXPECT_EQ(mem.readByte(i * 4), i);
        
        // Check that adjacent bytes weren't affected
        if (i > 0) {
            EXPECT_EQ(mem.readByte(i * 4 - 1), 0);
        }
        EXPECT_EQ(mem.readByte(i * 4 + 1), 0);
    }
}

// Test memory regions near the end boundary
TEST_F(MemoryTest, NearEndBoundary) {
    uint32_t nearEnd = mem.getMaxMemory() - 10;
    
    for (uint32_t i = 0; i < 10; i++) {
        mem.writeByte(nearEnd + i, 0xA0 + i);
    }
    
    for (uint32_t i = 0; i < 10; i++) {
        EXPECT_EQ(mem.readByte(nearEnd + i), 0xA0 + i);
    }
}

// Test for memory isolation between writes
TEST_F(MemoryTest, MemoryIsolation) {
    // Fill memory with a pattern
    for (uint32_t i = 0; i < 100; i++) {
        mem.writeByte(i, 0xFF);
    }
    
    // Overwrite every 10th byte
    for (uint32_t i = 0; i < 100; i += 10) {
        mem.writeByte(i, 0x55);
    }
    
    // Verify pattern
    for (uint32_t i = 0; i < 100; i++) {
        if (i % 10 == 0) {
            EXPECT_EQ(mem.readByte(i), 0x55);
        } else {
            EXPECT_EQ(mem.readByte(i), 0xFF);
        }
    }
}

// Test loading binary that would overflow at a custom address
TEST_F(MemoryTest, LoadBinaryFile_OverflowAtCustomAddress) {
    const char* filename = "memtest_overflow.bin";
    std::vector<uint8_t> data(1000, 0x42);
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
    ofs.close();

    uint16_t startAddr = mem.getMaxMemory() - 500; // This will cause overflow
    ASSERT_TRUE(mem.loadBinary(filename, startAddr));
    
    // Check bytes were loaded up to the limit
    for (uint32_t i = 0; i < 500; i++) {
        EXPECT_EQ(mem.readByte(startAddr + i), 0x42);
    }
    
    std::remove(filename);
}

// Test writing and reading words with specific patterns
TEST_F(MemoryTest, WordPatterns) {
    const std::vector<uint16_t> patterns = {0x0000, 0xFFFF, 0xAAAA, 0x5555, 0x1234, 0xABCD};
    
    for (size_t i = 0; i < patterns.size(); i++) {
        uint32_t addr = i * 10;
        mem.writeWord(addr, patterns[i]);
        EXPECT_EQ(mem.readWord(addr), patterns[i]);
    }
}

// Test random access pattern
TEST_F(MemoryTest, RandomAccessPattern) {
    std::vector<uint32_t> addresses;
    std::vector<uint8_t> values;
    
    // Generate random addresses and values
    srand(42); // Fixed seed for reproducibility
    for (int i = 0; i < 100; i++) {
        addresses.push_back(rand() % (mem.getMaxMemory() - 1));
        values.push_back(rand() % 256);
    }
    
    // Write values
    for (size_t i = 0; i < addresses.size(); i++) {
        mem.writeByte(addresses[i], values[i]);
    }
    
    // Verify values
    for (size_t i = 0; i < addresses.size(); i++) {
        EXPECT_EQ(mem.readByte(addresses[i]), values[i]);
    }
}

// Test dumpMemory with different ranges
TEST_F(MemoryTest, DumpMemory_Ranges) {
    // Fill memory with a pattern
    for (uint32_t i = 0; i < 256; i++) {
        mem.writeByte(i, i);
    }
    
    // Test various ranges (mainly checking that it doesn't crash)
    mem.dumpMemory(0, 15);
    mem.dumpMemory(16, 31);
    mem.dumpMemory(0, 255);
    mem.dumpMemory(mem.getMaxMemory() - 16, mem.getMaxMemory() - 1);
    
    // Test invalid ranges
    mem.dumpMemory(100, 50); // Start > end
    mem.dumpMemory(mem.getMaxMemory() - 10, mem.getMaxMemory() + 10); // End out of bounds
}

// Test wrap-around behavior if your Memory class implements it
TEST_F(MemoryTest, WordReadWrite_Wrapping) {
    // Some 6502 systems wrap memory addresses
    // This test assumes your implementation doesn't validate or wrap
    // If it does, adjust the expectations accordingly
    
    uint16_t value = 0xBEEF;
    mem.writeWord(mem.getMaxMemory() - 1, value);
    
    // If no wrapping, only the low byte should be written
    EXPECT_EQ(mem.readByte(mem.getMaxMemory() - 1), 0xEF);
    
    // High byte might be at address 0 if wrapping is implemented
    // Uncomment if your Memory class implements wrapping
    EXPECT_EQ(mem.readByte(0), 0xBE);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    srand(time(nullptr));
    return RUN_ALL_TESTS();
}