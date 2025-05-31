#include <gtest/gtest.h>
#include "Nes.h"
#include <array>
#include <string>
#include <random>

class RAMAccessTest : public ::testing::Test {
protected:
    std::unique_ptr<Nes> nes;
    
    // Random number generator for test values
    std::mt19937 rng;
    std::uniform_int_distribution<uint8_t> dist;

    // RAM address ranges
    static constexpr uint16_t RAM_BASE = 0x0000;
    static constexpr uint16_t RAM_SIZE = 0x0800; // 2KB
    static constexpr uint16_t RAM_END = RAM_BASE + RAM_SIZE - 1; // 0x07FF
    
    // Mirror address ranges
    static constexpr uint16_t MIRROR1_BASE = 0x0800;
    static constexpr uint16_t MIRROR2_BASE = 0x1000;
    static constexpr uint16_t MIRROR3_BASE = 0x1800;
    static constexpr uint16_t RAM_LAST_ADDRESS = 0x1FFF;

    void SetUp() override {
        nes = std::make_unique<Nes>();
        // Seed the random number generator
        rng.seed(std::random_device()());
        dist = std::uniform_int_distribution<uint8_t>(0, 255);
        
        // Initialize RAM with zeros
        for (uint16_t addr = RAM_BASE; addr <= RAM_END; ++addr) {
            nes->writeByte(addr, 0);
        }
    }
    
    // Helper to get the mirrored address in base RAM
    uint16_t getBaseAddress(uint16_t address) const {
        return address & 0x07FF; // Mask off the top bits to get base RAM address
    }
    
    // Generate a descriptive name for the memory region containing this address
    std::string getRegionName(uint16_t address) const {
        if (address <= RAM_END) {
            return "Base RAM";
        } else if (address <= MIRROR2_BASE - 1) {
            return "First Mirror";
        } else if (address <= MIRROR3_BASE - 1) {
            return "Second Mirror";
        } else if (address <= RAM_LAST_ADDRESS) {
            return "Third Mirror";
        }
        return "Outside RAM";
    }
    
    // Generate random test data for the entire RAM
    std::vector<uint8_t> generateRandomData() {
        std::vector<uint8_t> data(RAM_SIZE);
        for (auto& value : data) {
            value = dist(rng);
        }
        return data;
    }
};

TEST_F(RAMAccessTest, BasicWriteReadTest) {
    // Test basic write/read to base RAM addresses
    for (uint16_t addr = RAM_BASE; addr <= RAM_END; addr += 64) {
        uint8_t testValue = dist(rng);
        SCOPED_TRACE("Testing address $" + std::to_string(addr));
        
        nes->writeByte(addr, testValue);
        EXPECT_EQ(nes->readByte(addr), testValue) << "Direct read/write to RAM failed";
    }
}

TEST_F(RAMAccessTest, SimpleMirrorTest) {
    // Test simple mirroring by writing to base RAM and reading from mirrors
    for (uint16_t offset = 0; offset < RAM_SIZE; offset += 64) {
        uint8_t testValue = dist(rng);
        uint16_t baseAddr = RAM_BASE + offset;
        
        SCOPED_TRACE("Testing base address $" + std::to_string(baseAddr));
        
        // Write to base RAM
        nes->writeByte(baseAddr, testValue);
        
        // Read from all mirrors
        EXPECT_EQ(nes->readByte(MIRROR1_BASE + offset), testValue) 
            << "First mirror read failed";
            
        EXPECT_EQ(nes->readByte(MIRROR2_BASE + offset), testValue) 
            << "Second mirror read failed";
            
        EXPECT_EQ(nes->readByte(MIRROR3_BASE + offset), testValue) 
            << "Third mirror read failed";
    }
}

TEST_F(RAMAccessTest, ReverseMirrorTest) {
    // Test reverse mirroring by writing to mirrors and reading from base RAM
    for (uint16_t offset = 0; offset < RAM_SIZE; offset += 64) {
        SCOPED_TRACE("Testing offset $" + std::to_string(offset));
        
        // Test first mirror
        {
            uint8_t testValue = dist(rng);
            uint16_t mirrorAddr = MIRROR1_BASE + offset;
            uint16_t baseAddr = RAM_BASE + offset;
            
            nes->writeByte(mirrorAddr, testValue);
            EXPECT_EQ(nes->readByte(baseAddr), testValue) 
                << "Write to first mirror didn't affect base RAM";
        }
        
        // Test second mirror
        {
            uint8_t testValue = dist(rng);
            uint16_t mirrorAddr = MIRROR2_BASE + offset;
            uint16_t baseAddr = RAM_BASE + offset;
            
            nes->writeByte(mirrorAddr, testValue);
            EXPECT_EQ(nes->readByte(baseAddr), testValue) 
                << "Write to second mirror didn't affect base RAM";
        }
        
        // Test third mirror
        {
            uint8_t testValue = dist(rng);
            uint16_t mirrorAddr = MIRROR3_BASE + offset;
            uint16_t baseAddr = RAM_BASE + offset;
            
            nes->writeByte(mirrorAddr, testValue);
            EXPECT_EQ(nes->readByte(baseAddr), testValue) 
                << "Write to third mirror didn't affect base RAM";
        }
    }
}

TEST_F(RAMAccessTest, ConsistencyAcrossMirrors) {
    // Test consistency by writing to one mirror and reading from all others
    for (uint16_t offset = 0; offset < RAM_SIZE; offset += 64) {
        uint8_t testValue = dist(rng);
        SCOPED_TRACE("Testing offset $" + std::to_string(offset));
        
        uint16_t baseAddr = RAM_BASE + offset;
        uint16_t mirror1 = MIRROR1_BASE + offset;
        uint16_t mirror2 = MIRROR2_BASE + offset;
        uint16_t mirror3 = MIRROR3_BASE + offset;
        
        // Write to second mirror and read from all others
        nes->writeByte(mirror2, testValue);
        
        // All regions should reflect the same value
        EXPECT_EQ(nes->readByte(baseAddr), testValue) << "Base RAM inconsistent";
        EXPECT_EQ(nes->readByte(mirror1), testValue) << "First mirror inconsistent";
        EXPECT_EQ(nes->readByte(mirror3), testValue) << "Third mirror inconsistent";
    }
}

TEST_F(RAMAccessTest, CompleteMirrorVerification) {
    // Comprehensive test of entire addressable RAM range
    
    // Generate random values for all RAM
    std::vector<uint8_t> testData = generateRandomData();
    
    // Write to base RAM
    for (uint16_t addr = 0; addr < RAM_SIZE; ++addr) {
        nes->writeByte(RAM_BASE + addr, testData[addr]);
    }
    
    // Verify every single address in all mirrors
    for (uint16_t addr = RAM_BASE; addr <= RAM_LAST_ADDRESS; ++addr) {
        uint16_t baseAddr = getBaseAddress(addr);
        uint8_t expected = testData[baseAddr - RAM_BASE]; // Get expected value from base RAM
        
        EXPECT_EQ(nes->readByte(addr), expected)
            << "Address $" << std::hex << addr 
            << " in " << getRegionName(addr)
            << " should mirror to base RAM address $" << baseAddr;
    }
}

TEST_F(RAMAccessTest, BoundaryAddresses) {
    // Test boundary addresses for each region
    std::vector<uint16_t> boundaryAddresses = {
        RAM_BASE,             // Start of base RAM
        RAM_END,              // End of base RAM
        MIRROR1_BASE,         // Start of first mirror
        MIRROR1_BASE + RAM_SIZE - 1,  // End of first mirror
        MIRROR2_BASE,         // Start of second mirror
        MIRROR2_BASE + RAM_SIZE - 1,  // End of second mirror
        MIRROR3_BASE,         // Start of third mirror
        RAM_LAST_ADDRESS      // End of third mirror
    };
    
    for (uint16_t addr : boundaryAddresses) {
        SCOPED_TRACE("Testing boundary address $" + std::to_string(addr));
        
        uint8_t testValue = dist(rng);
        nes->writeByte(addr, testValue);
        
        // Check that the value is readable from the same address
        EXPECT_EQ(nes->readByte(addr), testValue) << "Direct read/write failed";
        
        // Check that the mirrored address in base RAM has the correct value
        uint16_t baseAddr = getBaseAddress(addr);
        if (addr != baseAddr) {
            EXPECT_EQ(nes->readByte(baseAddr), testValue)
                << "Mirroring to base address failed";
        }
        
        // Check that all other mirrors reflect this value
        std::vector<uint16_t> allMirrors = {
            baseAddr,             // In base RAM
            baseAddr + 0x0800,    // In first mirror
            baseAddr + 0x1000,    // In second mirror
            baseAddr + 0x1800     // In third mirror
        };
        
        for (uint16_t mirrorAddr : allMirrors) {
            if (mirrorAddr <= RAM_LAST_ADDRESS && mirrorAddr != addr) {
                EXPECT_EQ(nes->readByte(mirrorAddr), testValue)
                    << "Mirror at $" << std::hex << mirrorAddr << " not updated";
            }
        }
    }
}

TEST_F(RAMAccessTest, PatternTests) {
    // Test various bit patterns across all RAM
    std::vector<uint8_t> patterns = {
        0x00,       // All zeros
        0xFF,       // All ones
        0xAA,       // Alternating 10101010
        0x55,       // Alternating 01010101
        0x01,       // Single bit
        0x80,       // High bit
        0x7F,       // All bits except high
        0xFE        // All bits except low
    };
    
    for (uint8_t pattern : patterns) {
        SCOPED_TRACE("Testing pattern " + std::to_string(pattern));
        
        // Fill entire RAM with pattern
        for (uint16_t addr = RAM_BASE; addr <= RAM_END; ++addr) {
            nes->writeByte(addr, pattern);
        }
        
        // Verify pattern across all mirrors
        for (uint16_t addr = RAM_BASE; addr <= RAM_LAST_ADDRESS; ++addr) {
            EXPECT_EQ(nes->readByte(addr), pattern)
                << "Pattern verification failed at address $" 
                << std::hex << addr;
        }
        
        // Verify individual bits are preserved in mirrors
        uint16_t testAddr = RAM_BASE + 0x123;  // Arbitrary address to test
        for (int bit = 0; bit < 8; ++bit) {
            // Set a specific bit
            uint8_t bitPattern = 1 << bit;
            nes->writeByte(testAddr, bitPattern);
            
            // Check bit is set in all mirrors
            std::vector<uint16_t> mirrorAddrs = {
                testAddr,                    // Base
                testAddr + 0x0800,           // First mirror
                testAddr + 0x1000,           // Second mirror
                testAddr + 0x1800            // Third mirror
            };
            
            for (uint16_t addr : mirrorAddrs) {
                if (addr <= RAM_LAST_ADDRESS) {
                    EXPECT_EQ(nes->readByte(addr), bitPattern)
                        << "Bit " << bit << " not preserved in mirror at $"
                        << std::hex << addr;
                }
            }
        }
    }
}

TEST_F(RAMAccessTest, WriteThroughAllMirrors) {
    // Test writing to each mirror and verifying changes in all other mirrors
    std::vector<uint16_t> mirrorBases = {
        RAM_BASE,      // Base RAM
        MIRROR1_BASE,  // First mirror
        MIRROR2_BASE,  // Second mirror
        MIRROR3_BASE   // Third mirror
    };
    
    for (uint16_t testOffset = 0; testOffset < RAM_SIZE; testOffset += 64) {
        SCOPED_TRACE("Testing offset $" + std::to_string(testOffset));
        
        for (size_t i = 0; i < mirrorBases.size(); ++i) {
            uint16_t writeAddr = mirrorBases[i] + testOffset;
            uint8_t testValue = 0xA0 + i; // Unique value for each mirror
            
            nes->writeByte(writeAddr, testValue);
            
            // Check all mirrors reflect the change
            for (uint16_t baseAddr : mirrorBases) {
                uint16_t readAddr = baseAddr + testOffset;
                if (readAddr <= RAM_LAST_ADDRESS) {
                    EXPECT_EQ(nes->readByte(readAddr), testValue)
                        << "Write to $" << std::hex << writeAddr
                        << " not reflected in mirror at $" << readAddr;
                }
            }
        }
    }
}

TEST_F(RAMAccessTest, OutOfBoundsAccess) {
    // Test addresses outside the RAM range
    // Set a known state to RAM first
    uint8_t knownValue = 0xA5;
    for (uint16_t addr = RAM_BASE; addr <= RAM_END; ++addr) {
        nes->writeByte(addr, knownValue);
    }
    
    // Test access beyond RAM (into PPU registers range)
    uint16_t outsideAddr = RAM_LAST_ADDRESS + 1; // 0x2000, start of PPU registers
    uint8_t outsideValue = 0xBC;
    
    nes->writeByte(outsideAddr, outsideValue);
    
    // This write should not affect RAM contents
    for (uint16_t addr = RAM_BASE; addr <= RAM_LAST_ADDRESS; ++addr) {
        EXPECT_EQ(nes->readByte(addr), knownValue)
            << "Write outside RAM range affected RAM at $" << std::hex << addr;
    }
    
    // Reading from outside address should not return RAM contents
    EXPECT_NE(nes->readByte(outsideAddr), knownValue)
        << "Reading from outside RAM returned RAM contents";
}

TEST_F(RAMAccessTest, WordAccessAcrossMirrors) {
    // Test word (16-bit) access across mirrors
    for (uint16_t offset = 0; offset < RAM_SIZE - 1; offset += 64) {
        SCOPED_TRACE("Testing word access at offset $" + std::to_string(offset));
        
        uint16_t testWord = (dist(rng) << 8) | dist(rng);
        
        
        // Write a word to base RAM
        nes->writeWord(RAM_BASE + offset, testWord);
        
        // Read the word from each mirror
        EXPECT_EQ(nes->readWord(MIRROR1_BASE + offset), testWord)
            << "Word read from first mirror failed";
            
        EXPECT_EQ(nes->readWord(MIRROR2_BASE + offset), testWord)
            << "Word read from second mirror failed";
            
        EXPECT_EQ(nes->readWord(MIRROR3_BASE + offset), testWord)
            << "Word read from third mirror failed";
            
        // Write a word to a mirror and read from base
        uint16_t testWord2 = (dist(rng) << 8) | dist(rng);
        nes->writeWord(MIRROR2_BASE + offset, testWord2);
        
        EXPECT_EQ(nes->readWord(RAM_BASE + offset), testWord2)
            << "Word write to mirror not reflected in base RAM";
    }
}

TEST_F(RAMAccessTest, CrossBoundaryWordAccess) {
    // Test word access that crosses RAM mirror boundaries
    
    // Test at end of base RAM into first mirror
    uint16_t addr = RAM_END; // 0x07FF
    uint16_t testWord = 0xABCD;
    
    nes->writeWord(addr, testWord);
    
    // Low byte should be at 0x07FF, high byte at 0x0800 (first mirror)
    EXPECT_EQ(nes->readByte(addr), 0xCD) << "Low byte write failed";
    EXPECT_EQ(nes->readByte(addr + 1), 0xAB) << "High byte write failed (crossed to mirror)";
    
    // Reading the word should get both bytes
    EXPECT_EQ(nes->readWord(addr), testWord) << "Word read across boundary failed";
    
    // Same test at end of first mirror into second mirror
    addr = MIRROR1_BASE + RAM_SIZE - 1; // 0x0FFF
    testWord = 0x1234;
    
    nes->writeWord(addr, testWord);
    
    EXPECT_EQ(nes->readByte(addr), 0x34) << "Low byte write failed at mirror boundary";
    EXPECT_EQ(nes->readByte(addr + 1), 0x12) << "High byte write failed (crossed mirrors)";
    EXPECT_EQ(nes->readWord(addr), testWord) << "Word read across mirrors failed";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}