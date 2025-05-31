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

// Add these tests after your existing test cases

TEST_F(RAMAccessTest, WordOperationsAcrossMirrors) {
    // Test word operations across mirrored regions
    uint16_t testWord = 0xABCD;
    
    // Write to base RAM
    nes->writeWord(RAM_BASE, testWord);
    
    // Verify in mirrors
    EXPECT_EQ(nes->readWord(MIRROR1_BASE), testWord);
    EXPECT_EQ(nes->readWord(MIRROR2_BASE), testWord);
    EXPECT_EQ(nes->readWord(MIRROR3_BASE), testWord);
    
    // Write to a mirror
    testWord = 0xDEAD;
    nes->writeWord(MIRROR2_BASE + 0x100, testWord);
    
    // Verify in other regions
    EXPECT_EQ(nes->readWord(RAM_BASE + 0x100), testWord);
    EXPECT_EQ(nes->readWord(MIRROR1_BASE + 0x100), testWord);
    EXPECT_EQ(nes->readWord(MIRROR3_BASE + 0x100), testWord);
}

TEST_F(RAMAccessTest, CrossMirrorBoundary) {
    // Test reading/writing words across mirror boundaries
    uint16_t boundaryAddress = MIRROR1_BASE - 1;  // 0x0FFF
    uint16_t testWord = 0xCAFE;
    
    // Write word at boundary between mirrors
    nes->writeWord(boundaryAddress, testWord);
    
    // Low byte should be at end of base RAM
    EXPECT_EQ(nes->readByte(boundaryAddress), testWord & 0xFF);
    
    // High byte should be at start of next mirror
    EXPECT_EQ(nes->readByte(MIRROR1_BASE), testWord >> 8);
    
    // Reading as word should work correctly
    EXPECT_EQ(nes->readWord(boundaryAddress), testWord);
    
    // Do the same test at other mirror boundaries
    testWord = 0xBEEF;
    boundaryAddress = MIRROR2_BASE - 1;  // 0x17FF
    nes->writeWord(boundaryAddress, testWord);
    EXPECT_EQ(nes->readWord(boundaryAddress), testWord);
    
    testWord = 0xFACE;
    boundaryAddress = MIRROR3_BASE - 1;  // 0x1FFF
    nes->writeWord(boundaryAddress, testWord);
    EXPECT_EQ(nes->readWord(boundaryAddress), testWord);
}

TEST_F(RAMAccessTest, ZeroPageAccess) {
    // Test zero page access (important for many 6502 operations)
    uint8_t zeroPageValues[256];
    
    // Fill zero page with incrementing values
    for (int i = 0; i < 256; ++i) {
        zeroPageValues[i] = i;
        nes->writeByte(i, i);
    }
    
    // Verify zero page
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(nes->readByte(i), zeroPageValues[i]);
    }
    
    // Verify zero page is mirrored properly
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(nes->readByte(i + MIRROR1_BASE), zeroPageValues[i]);
        EXPECT_EQ(nes->readByte(i + MIRROR2_BASE), zeroPageValues[i]);
        EXPECT_EQ(nes->readByte(i + MIRROR3_BASE), zeroPageValues[i]);
    }
}

TEST_F(RAMAccessTest, StackPageAccess) {
    // Test stack page (0x0100-0x01FF) - important for 6502 stack operations
    uint8_t stackPageValues[256];
    uint16_t stackBase = 0x0100;
    
    // Fill stack page
    for (int i = 0; i < 256; ++i) {
        stackPageValues[i] = 0xFF - i;  // Descending values
        nes->writeByte(stackBase + i, stackPageValues[i]);
    }
    
    // Verify stack page
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(nes->readByte(stackBase + i), stackPageValues[i]);
    }
    
    // Verify stack page mirrors
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(nes->readByte(stackBase + i + MIRROR1_BASE), stackPageValues[i]);
        EXPECT_EQ(nes->readByte(stackBase + i + MIRROR2_BASE), stackPageValues[i]);
        EXPECT_EQ(nes->readByte(stackBase + i + MIRROR3_BASE), stackPageValues[i]);
    }
}

TEST_F(RAMAccessTest, RapidAlternatingAccess) {
    // Test rapidly alternating between different mirrors
    std::vector<uint16_t> addresses = {
        RAM_BASE + 0x123,
        MIRROR1_BASE + 0x123,
        MIRROR2_BASE + 0x123,
        MIRROR3_BASE + 0x123
    };
    
    // Rapidly write different values to the same location in different mirrors
    for (int i = 0; i < 100; ++i) {
        uint8_t testValue = i & 0xFF;
        uint16_t addr = addresses[i % addresses.size()];
        
        nes->writeByte(addr, testValue);
        
        // All mirrors should reflect the latest write
        for (uint16_t mirrorAddr : addresses) {
            EXPECT_EQ(nes->readByte(mirrorAddr), testValue)
                << "Mirror at " << std::hex << mirrorAddr 
                << " not updated on iteration " << i;
        }
    }
}

TEST_F(RAMAccessTest, MemoryInitialization) {
    // Test RAM initial state - should be all zeros after Nes construction
    // First create a new instance to ensure we test initial state
    auto freshNes = std::make_unique<Nes>();
    
    // Check RAM is initialized to zero (or some default pattern)
    int nonZeroCount = 0;
    for (uint16_t addr = RAM_BASE; addr <= RAM_LAST_ADDRESS; addr += 64) {
        if (freshNes->readByte(addr) != 0) {
            nonZeroCount++;
        }
    }
    
    // RAM should be zeroed or follow a consistent pattern
    EXPECT_TRUE(nonZeroCount == 0 || nonZeroCount == (RAM_LAST_ADDRESS - RAM_BASE + 1) / 64)
        << "RAM initialization is inconsistent";
}

TEST_F(RAMAccessTest, RandomAccessPattern) {
    // Test random access patterns across all mirrors
    std::vector<uint16_t> testAddresses;
    std::map<uint16_t, uint8_t> expectedValues;
    
    // Generate random addresses across all RAM mirrors
    std::uniform_int_distribution<uint16_t> addrDist(RAM_BASE, RAM_LAST_ADDRESS);
    for (int i = 0; i < 100; ++i) {
        uint16_t addr = addrDist(rng);
        uint8_t value = dist(rng);
        testAddresses.push_back(addr);
        
        // Save expected value for the base address
        uint16_t baseAddr = getBaseAddress(addr);
        expectedValues[baseAddr] = value;
        
        // Write to the address
        nes->writeByte(addr, value);
    }
    
    // Verify all writes across all mirrors
    for (uint16_t baseAddr = RAM_BASE; baseAddr <= RAM_END; ++baseAddr) {
        if (expectedValues.find(baseAddr) != expectedValues.end()) {
            uint8_t expected = expectedValues[baseAddr];
            
            // Check base address and all mirrors
            EXPECT_EQ(nes->readByte(baseAddr), expected);
            EXPECT_EQ(nes->readByte(baseAddr + 0x0800), expected);
            EXPECT_EQ(nes->readByte(baseAddr + 0x1000), expected);
            EXPECT_EQ(nes->readByte(baseAddr + 0x1800), expected);
        }
    }
}

TEST_F(RAMAccessTest, RAMBoundaryIsolation) {
    // Ensure RAM is properly isolated from other memory regions
    
    // First fill RAM with a pattern
    for (uint16_t addr = RAM_BASE; addr <= RAM_LAST_ADDRESS; ++addr) {
        nes->writeByte(addr, 0xAA);
    }
    
    // Now write different patterns to surrounding regions
    for (uint16_t addr = RAM_LAST_ADDRESS + 1; addr < RAM_LAST_ADDRESS + 20; ++addr) {
        nes->writeByte(addr, 0x55);  // PPU registers start here
    }
    
    // Verify RAM boundary is respected
    EXPECT_EQ(nes->readByte(RAM_LAST_ADDRESS), 0xAA);
    EXPECT_NE(nes->readByte(RAM_LAST_ADDRESS + 1), 0xAA);
    
    // And verify writing beyond RAM doesn't affect RAM
    nes->writeByte(RAM_LAST_ADDRESS + 1, 0x33);
    EXPECT_EQ(nes->readByte(RAM_LAST_ADDRESS), 0xAA); // Last RAM address unchanged
}

TEST_F(RAMAccessTest, AddressingModesSimulation) {
    // Test common 6502 addressing modes that interact with memory
    
    // Zero page
    nes->writeByte(0x20, 0x40);      // Address to use is stored at 0x20
    nes->writeByte(0x40, 0xCD);      // Value is stored at 0x40
    
    // Zero page, X
    uint8_t x_reg = 0x05;
    nes->writeByte(0x20 + x_reg, 0x50); // Address with X offset
    nes->writeByte(0x50, 0xEF);      // Value
    
    
    // Verify that "code" using these addressing modes would read correct values
    uint8_t zp_value = nes->readByte(nes->readByte(0x20));
    EXPECT_EQ(zp_value, 0xCD);
    
    uint8_t zpx_value = nes->readByte(nes->readByte(0x20 + x_reg));
    EXPECT_EQ(zpx_value, 0xEF);
    
    // Absolute
    nes->writeWord(0x100, 0x0400);    // Address to use is 0x0400
    nes->writeByte(0x0400, 0xAB);    // Value
    
    // Indirect (JMP indirect simulation)
    nes->writeWord(0x80, 0x0500);    // Address points to 0x0500
    nes->writeWord(0x0500, 0xBEEF);  // Target address

    uint8_t abs_value = nes->readByte(nes->readWord(0x100));
    EXPECT_EQ(abs_value, 0xAB);
    
    uint16_t indirect_target = nes->readWord(nes->readWord(0x80));
    EXPECT_EQ(indirect_target, 0xBEEF);
    
    // Verify these addresses are mirrored properly
    EXPECT_EQ(nes->readByte(nes->readByte(0x20 + MIRROR1_BASE)), 0xCD);
    EXPECT_EQ(nes->readByte(nes->readByte(0x20 + MIRROR2_BASE)), 0xCD);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}