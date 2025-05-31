#include <gtest/gtest.h>
#include "Nes.h"
#include <array>
#include <string>
#include <random>

class PPURegisterAccessTest : public ::testing::Test {
protected:
    std::unique_ptr<Nes> nes;

    // PPU register names for better test failure messages
    const std::array<std::string, 8> regNames = {
        "PPUCTRL", "PPUMASK", "PPUSTATUS", "OAMADDR", 
        "OAMDATA", "PPUSCROLL", "PPUADDR", "PPUDATA"
    };
    
    // PPU register addresses
    static constexpr uint16_t PPUCTRL   = 0x2000;
    static constexpr uint16_t PPUMASK   = 0x2001;
    static constexpr uint16_t PPUSTATUS = 0x2002;
    static constexpr uint16_t OAMADDR   = 0x2003;
    static constexpr uint16_t OAMDATA   = 0x2004;
    static constexpr uint16_t PPUSCROLL = 0x2005;
    static constexpr uint16_t PPUADDR   = 0x2006;
    static constexpr uint16_t PPUDATA   = 0x2007;

    void SetUp() override {
        nes = std::make_unique<Nes>();
        // Reset PPU registers to known state
        for (uint16_t i = 0; i < 8; ++i) {
            nes->writeByte(PPUCTRL + i, 0);
        }
    }
    
    // Helper to convert register address to a readable name
    std::string getRegisterName(uint16_t addr) {
        addr &= 0x2007; // Map to base register
        uint16_t regIndex = addr - PPUCTRL;
        if (regIndex < 8) {
            return regNames[regIndex];
        }
        return "Unknown";
    }
    
    // Helper to convert address to mirrored base register
    uint16_t getMirroredRegister(uint16_t addr) {
        if (addr >= 0x2000 && addr <= 0x3FFF) {
            return PPUCTRL + (addr & 0x7); // Mirror down to 0x2000-0x2007
        }
        return addr; // Not a PPU register address
    }
};

TEST_F(PPURegisterAccessTest, CompleteMirroringTest) {
    // Initialize a unique value for each register for easy identification
    const std::array<uint8_t, 8> testValues = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    
    // First, test all possible addresses in the PPU register range ($2000-$3FFF)
    // This is a comprehensive test of all 8192 addresses (0x2000 to 0x3FFF inclusive)
    
    SCOPED_TRACE("Testing all 8192 mirrored addresses");
    
    // Step 1: Set initial values to all base registers
    for (uint16_t i = 0; i < 8; ++i) {
        nes->writeByte(PPUCTRL + i, testValues[i]);
    }
    
    // Step 2: Verify every single mirrored address in the entire range
    for (uint16_t addr = 0x2000; addr <= 0x3FFF; ++addr) {
        uint16_t baseReg = PPUCTRL + (addr & 0x7);
        uint8_t expectedValue = testValues[addr & 0x7];
        
        // Read from mirrored address
        uint8_t actualValue = nes->readByte(addr);
        
        EXPECT_EQ(actualValue, expectedValue)
            << "Address $" << std::hex << addr
            << " should mirror to " << getRegisterName(baseReg)
            << " with value $" << std::hex << static_cast<int>(expectedValue);
    }
    
    // Step 3: Test write to every 64th address (to avoid test taking too long)
    // while still having reasonable coverage across the entire range
    for (uint16_t addr = 0x2000; addr <= 0x3FFF; addr += 64) {
        uint16_t baseReg = PPUCTRL + (addr & 0x7);
        uint8_t newValue = ~testValues[addr & 0x7]; // Inverted value
        
        // Write to mirrored address
        nes->writeByte(addr, newValue);
        
        // Verify base register was updated
        EXPECT_EQ(nes->readByte(baseReg), newValue)
            << "Write to address $" << std::hex << addr
            << " didn't update " << getRegisterName(baseReg);
            
        // Verify other mirrors were also updated
        for (uint16_t mirror = baseReg; mirror <= 0x3FFF; mirror += 8) {
            EXPECT_EQ(nes->readByte(mirror), newValue)
                << "Mirror at $" << std::hex << mirror
                << " not updated after write to $" << addr;
        }
        
        // Restore original value
        nes->writeByte(baseReg, testValues[addr & 0x7]);
    }
    
    // Step 4: Test the boundary cases specifically (start, end, and block boundaries)
    
    // Start of PPU address range
    SCOPED_TRACE("Testing start of PPU address range");
    nes->writeByte(0x2000, 0xAA);
    EXPECT_EQ(nes->readByte(PPUCTRL), 0xAA);
    
    // End of PPU address range
    SCOPED_TRACE("Testing end of PPU address range");
    nes->writeByte(0x3FFF, 0xBB);
    EXPECT_EQ(nes->readByte(PPUDATA), 0xBB);
    
    // Outside PPU address range (too low)
    SCOPED_TRACE("Testing below PPU address range");
    nes->writeByte(0x1FFF, 0xCC);
    EXPECT_NE(nes->readByte(PPUCTRL), 0xCC);
    
    // Outside PPU address range (too high)
    SCOPED_TRACE("Testing above PPU address range");
    nes->writeByte(0x4000, 0xDD);
    EXPECT_NE(nes->readByte(PPUCTRL), 0xDD);
    
    // Step 5: Test each register separately with pattern changes
    const std::array<std::string, 8> patternNames = {
        "Sequential", "Alternating", "Powers of 2", "Inverted", 
        "Walking ones", "Walking zeros", "Random", "All bits"
    };
    
    for (uint16_t reg = 0; reg < 8; ++reg) {
        uint16_t baseReg = PPUCTRL + reg;
        
        // Define 8 different test patterns
        const std::array<uint8_t, 8> patterns = {
            static_cast<uint8_t>(reg),        // Sequential
            static_cast<uint8_t>(reg % 2 ? 0xAA : 0x55),  // Alternating
            static_cast<uint8_t>(1 << (reg % 8)),  // Powers of 2
            static_cast<uint8_t>(~reg),       // Inverted
            static_cast<uint8_t>(1 << (reg % 8)),  // Walking ones
            static_cast<uint8_t>(~(1 << (reg % 8))), // Walking zeros
            static_cast<uint8_t>(0x7F + reg), // "Random"
            0xFF                             // All bits
        };
        
        for (uint16_t patternIdx = 0; patternIdx < patterns.size(); ++patternIdx) {
            SCOPED_TRACE("Testing " + getRegisterName(baseReg) + 
                         " with " + patternNames[patternIdx] + " pattern");
            
            uint8_t pattern = patterns[patternIdx];
            
            // Write to base register
            nes->writeByte(baseReg, pattern);
            
            // Test mirrors at key positions
            std::vector<uint16_t> keyMirrors = {
                baseReg,             // Base register
                baseReg + 0x8,       // Next block
                baseReg + 0x100,     // Middle of range
                baseReg + 0x1000,    // Higher mirror
                0x3FF8 + (baseReg & 0x7) // Highest mirror
            };
            
            for (auto mirror : keyMirrors) {
                if (mirror <= 0x3FFF) {
                    EXPECT_EQ(nes->readByte(mirror), pattern)
                        << "Register " << getRegisterName(baseReg) 
                        << " pattern " << patternNames[patternIdx]
                        << " not mirrored at $" << std::hex << mirror;
                }
            }
        }
    }
    
    // Step 6: Test block boundary mirroring for all registers
    SCOPED_TRACE("Testing block boundaries");
    
    // Test boundaries between each 8-byte block
    for (uint16_t blockStart = 0x2008; blockStart <= 0x3FF8; blockStart += 0x8) {
        // Test the boundary between the previous block and this one
        uint16_t prevBlockLastAddr = blockStart - 1;
        uint16_t thisBlockFirstAddr = blockStart;
        
        // Set a different pattern for each block boundary test
        uint8_t pattern1 = blockStart & 0xFF;
        uint8_t pattern2 = ~pattern1;
        
        // Write to the last address of the previous block
        nes->writeByte(prevBlockLastAddr, pattern1);
        
        // Write to the first address of this block
        nes->writeByte(thisBlockFirstAddr, pattern2);
        
        // Verify correct mirroring
        EXPECT_EQ(nes->readByte(PPUDATA), pattern1) 
            << "Block boundary mirroring failed at $" << std::hex << prevBlockLastAddr;
        
        EXPECT_EQ(nes->readByte(PPUCTRL), pattern2)
            << "Block boundary mirroring failed at $" << std::hex << thisBlockFirstAddr;
    }
}

TEST_F(PPURegisterAccessTest, BasicMirroring) {
    // Test basic mirroring concept
    nes->writeByte(PPUCTRL, 0xAB);
    EXPECT_EQ(nes->readByte(PPUCTRL), 0xAB) << "Direct write/read to PPUCTRL failed";
    
    // Test mirroring within the first $20 bytes
    nes->writeByte(PPUCTRL + 0x8, 0xCD);
    EXPECT_EQ(nes->readByte(PPUCTRL), 0xCD) << "PPUCTRL mirroring at +$8 failed";
    
    // Test mirroring at end of PPU registers range
    nes->writeByte(0x3FFF, 0xEF);
    EXPECT_EQ(nes->readByte(PPUDATA), 0xEF) << "PPUDATA mirroring at $3FFF failed";
}

TEST_F(PPURegisterAccessTest, CompleteRegisterSet) {
    // Write unique values to all PPU registers
    const std::array<uint8_t, 8> testValues = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    
    // Write to base registers
    for (uint16_t i = 0; i < 8; ++i) {
        uint16_t addr = PPUCTRL + i;
        nes->writeByte(addr, testValues[i]);
        EXPECT_EQ(nes->readByte(addr), testValues[i]) 
            << getRegisterName(addr) << " write/read failed";
    }
    
    // Verify base register reads
    for (uint16_t i = 0; i < 8; ++i) {
        uint16_t addr = PPUCTRL + i;
        EXPECT_EQ(nes->readByte(addr), testValues[i]) 
            << getRegisterName(addr) << " read verification failed";
    }
    
    // Write to mirrored addresses and check base registers
    nes->writeByte(PPUCTRL + 0x8, 0x99);
    EXPECT_EQ(nes->readByte(PPUCTRL), 0x99) << "PPUCTRL mirror write failed";
    
    nes->writeByte(0x3FFA, 0xAA); // $3FFA mirrors PPUSTATUS (0x2002)
    EXPECT_EQ(nes->readByte(PPUSTATUS), 0xAA) << "PPUSTATUS high mirror write failed";
}

TEST_F(PPURegisterAccessTest, ExhaustiveMirrorTesting) {
    const std::array<uint8_t, 8> baseValues = {0x10, 0x21, 0x32, 0x43, 0x54, 0x65, 0x76, 0x87};
    
    // Write base values to all registers
    for (uint16_t i = 0; i < 8; ++i) {
        nes->writeByte(PPUCTRL + i, baseValues[i]);
    }
    
    // Test comprehensive mirroring across the entire address space
    // Test every 32nd mirror across the entire $2000-$3FFF range
    for (uint16_t base = 0x2000; base < 0x4000; base += 0x20) {
        for (uint16_t offset = 0; offset < 8; ++offset) {
            uint16_t mirrorAddr = base + offset;
            uint16_t baseReg = PPUCTRL + offset;
            
            // Read mirrored address, should equal base register
            EXPECT_EQ(nes->readByte(mirrorAddr), nes->readByte(baseReg))
                << "Read mirroring failed at $" << std::hex << mirrorAddr 
                << " for " << getRegisterName(baseReg);
                
            // Write to mirror, should affect base register
            uint8_t newValue = (baseValues[offset] + 1) & 0xFF;
            nes->writeByte(mirrorAddr, newValue);
            
            EXPECT_EQ(nes->readByte(baseReg), newValue)
                << "Write mirroring failed at $" << std::hex << mirrorAddr 
                << " for " << getRegisterName(baseReg);
                
            // And read back from mirror should match too
            EXPECT_EQ(nes->readByte(mirrorAddr), newValue)
                << "Read from mirror doesn't match after write at $" << std::hex << mirrorAddr;
        }
    }
}

TEST_F(PPURegisterAccessTest, IsolatedRegisterWrites) {
    // Test that writing to one register doesn't affect others
    for (uint16_t i = 0; i < 8; ++i) {
        uint16_t targetReg = PPUCTRL + i;
        
        // First, set all registers to known value
        for (uint16_t j = 0; j < 8; ++j) {
            nes->writeByte(PPUCTRL + j, 0xA0 + j);
        }
        
        // Now write to target register through a mirror
        uint16_t mirrorAddr = targetReg + 0x100; // A mirror further away
        nes->writeByte(mirrorAddr, 0xF0 + i);
        
        // Verify only target register changed
        for (uint16_t j = 0; j < 8; ++j) {
            uint16_t checkReg = PPUCTRL + j;
            if (j == i) {
                EXPECT_EQ(nes->readByte(checkReg), 0xF0 + i)
                    << getRegisterName(checkReg) << " should have changed";
            } else {
                EXPECT_EQ(nes->readByte(checkReg), 0xA0 + j)
                    << getRegisterName(checkReg) << " should not have changed";
            }
        }
    }
}

TEST_F(PPURegisterAccessTest, MirroringEdgeCases) {
    // Test the edge of each mirroring block
    // Each $8 bytes form a complete mirror
    
    // Test at boundaries of mirroring blocks
    for (uint16_t base = 0x2000; base < 0x4000; base += 0x8) {
        if (base == 0x2000) continue; // Skip first block as it's the base
        
        // Last address of previous block should NOT mirror to the start of this block
        uint16_t prevBlockEnd = base - 1; 
        nes->writeByte(prevBlockEnd, 0xBB);
        EXPECT_NE(nes->readByte(PPUCTRL), 0xBB) 
            << "Mirroring should not cross block boundary at $" << std::hex << prevBlockEnd;
        
        // First address of this block SHOULD mirror to PPUCTRL
        nes->writeByte(base, 0xCC);
        EXPECT_EQ(nes->readByte(PPUCTRL), 0xCC)
            << "Mirroring failed at block boundary $" << std::hex << base;
    }
    
    // Test very last address in PPU register space
    nes->writeByte(0x3FFF, 0xEE);
    EXPECT_EQ(nes->readByte(PPUDATA), 0xEE)
        << "Mirror at $3FFF to PPUDATA failed";
        
    // Test just beyond PPU register space (shouldn't mirror)
    uint16_t beyondPPU = 0x4000;
    nes->writeByte(beyondPPU, 0xFF);
    EXPECT_NE(nes->readByte(PPUCTRL), 0xFF)
        << "Address $4000 should not mirror to PPUCTRL";
}

TEST_F(PPURegisterAccessTest, LatchingRegisterBehavior) {
    // Test PPUSCROLL and PPUADDR register latching (toggle between X/Y and high/low byte)
    
    // PPUSCROLL tests
    nes->writeByte(PPUSCROLL, 0x12); // First write (X value)
    nes->writeByte(PPUSCROLL, 0x34); // Second write (Y value)
    
    // We'd need specific access to PPU internal state to verify this properly
    // But we can at least check that writes are accepted
    
    // PPUADDR tests
    nes->writeByte(PPUADDR, 0x21); // High byte
    nes->writeByte(PPUADDR, 0x43); // Low byte
    
    // Write to the address we just set
    nes->writeByte(PPUDATA, 0xBC);
    
    // Again, we'd need access to PPU internals for complete verification
    // In a real emulator, we'd verify the VRAM address was set to $2143 
    // and the data was written there
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}