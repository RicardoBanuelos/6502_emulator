#include <gtest/gtest.h>
#include "Nes.h"

class PPuRegisterAccessTest : public ::testing::Test {
protected:
    std::unique_ptr<Nes> nes;

    void SetUp() override {
        nes = std::make_unique<Nes>();
    }
};

TEST_F(PPuRegisterAccessTest, ppumirrortest)
{
    // Write to PPU register $2000 (PPUCTRL)
    nes->writeByte(0x2000, 0xAB);
    // Read back from $2000 (should be 0xAB)
    EXPECT_EQ(nes->readByte(0x2000), 0xAB);

    // Write to a mirrored address $2008 (also maps to PPUCTRL)
    nes->writeByte(0x2008, 0xCD);
    // Read back from $2000 (should now be 0xCD, since $2008 mirrors $2000)
    EXPECT_EQ(nes->readByte(0x2000), 0xCD);

    // Write to another mirrored address $3FFF (maps to $2007)
    nes->writeByte(0x3FFF, 0xEF);
    // Read back from $2007 (should be 0xEF)
    EXPECT_EQ(nes->readByte(0x2007), 0xEF);
}

TEST_F(PPuRegisterAccessTest, ppuRegisterReadWriteComplex)
{
    // Write to all PPU registers with unique values
    nes->writeByte(0x2000, 0x11); // PPUCTRL
    nes->writeByte(0x2001, 0x22); // PPUMASK
    nes->writeByte(0x2002, 0x33); // PPUSTATUS
    nes->writeByte(0x2003, 0x44); // OAMADDR
    nes->writeByte(0x2004, 0x55); // OAMDATA
    nes->writeByte(0x2005, 0x66); // PPUSCROLL
    nes->writeByte(0x2006, 0x77); // PPUADDR
    nes->writeByte(0x2007, 0x88); // PPUDATA

    // Read back from the base addresses
    EXPECT_EQ(nes->readByte(0x2000), 0x11);
    EXPECT_EQ(nes->readByte(0x2001), 0x22);
    EXPECT_EQ(nes->readByte(0x2002), 0x33);
    EXPECT_EQ(nes->readByte(0x2003), 0x44);
    EXPECT_EQ(nes->readByte(0x2004), 0x55);
    EXPECT_EQ(nes->readByte(0x2005), 0x66);
    EXPECT_EQ(nes->readByte(0x2006), 0x77);
    EXPECT_EQ(nes->readByte(0x2007), 0x88);

    // Write to mirrored addresses and check base registers
    nes->writeByte(0x2008, 0x99); // Mirror of $2000 (PPUCTRL)
    EXPECT_EQ(nes->readByte(0x2000), 0x99);

    nes->writeByte(0x3FFA, 0xAA); // $3FFA is mirror of $2002 (PPUSTATUS)
    EXPECT_EQ(nes->readByte(0x2002), 0xAA);

    // Ensure writing to one register does not affect others
    EXPECT_EQ(nes->readByte(0x2001), 0x22); // PPUMASK should remain unchanged
    EXPECT_EQ(nes->readByte(0x2003), 0x44); // OAMADDR should remain unchanged
}


/*

Writes and reads all registers and their mirrors.
Checks multiple mirror addresses for each register.
Ensures writing to one register/mirror does not affect others.
Tests the highest mirrored address ($3FFF).

*/
TEST_F(PPuRegisterAccessTest, ppuRegisterReadWriteExhaustive)
{
    // Write unique values to all base PPU registers
    const uint8_t values[8] = {0x10, 0x21, 0x32, 0x43, 0x54, 0x65, 0x76, 0x87};
    for (uint32_t i = 0; i < 8; ++i) {
        nes->writeByte(0x2000 + i, values[i]);
    }

    // Read back from all base registers
    for (uint32_t i = 0; i < 8; ++i) {
        EXPECT_EQ(nes->readByte(0x2000 + i), values[i]) << "Base register " << i << " failed";
    }

    // Test several mirrored addresses for each register
    for (uint32_t i = 0; i < 8; ++i) {
        // Write to a mirror far from the base
        uint32_t mirror_addr = 0x2000 + i + 0x20; // e.g., 0x2020 mirrors 0x2000
        nes->writeByte(mirror_addr, 0xA0 + i);
        EXPECT_EQ(nes->readByte(0x2000 + i), 0xA0 + i) << "Mirror write/read failed for reg " << i;

        // Write to another mirror in the $3F00-$3FFF range
        uint32_t high_mirror = 0x3FF8 + i; // 0x3FF8-0x3FFF mirrors 0x2000-0x2007
        nes->writeByte(high_mirror, 0xB0 + i);
        EXPECT_EQ(nes->readByte(0x2000 + i), 0xB0 + i) << "High mirror write/read failed for reg " << i;
    }

    // Ensure writing to one register does not affect others
    for (uint32_t i = 0; i < 8; ++i) {
        // Write a new value to this register's mirror
        uint32_t mirror_addr = 0x2000 + i + 0x100; // another mirror
        nes->writeByte(mirror_addr, 0xC0 + i);

        // Check only this register changed
        for (uint32_t j = 0; j < 8; ++j) {
            if (i == j) {
                EXPECT_EQ(nes->readByte(0x2000 + j), 0xC0 + i) << "Register " << j << " should have new value";
            } else {
                // Previous value should remain unchanged
                EXPECT_NE(nes->readByte(0x2000 + j), 0xC0 + i) << "Register " << j << " should not be affected";
            }
        }
    }

    // Edge case: Write to the very last mirrored address
    nes->writeByte(0x3FFF, 0xEE);
    EXPECT_EQ(nes->readByte(0x2007), 0xEE); // 0x3FFF mirrors 0x2007
}
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}