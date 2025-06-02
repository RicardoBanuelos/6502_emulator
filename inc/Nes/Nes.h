#pragma once

/*
+----------------------------------------------+
|                  NES SYSTEM                  |
+----------------------------------------------+

+------------+        +------------+        +-----------+
|            |        |            |        |           |
|  CPU       |<------>|  MEMORY    |<------>|   PPU     |
| (6502)     |        |  MAPPER    |        |           |
|            |        |            |        |           |
+------------+        +------------+        +-----------+
      ^                     ^                    ^
      |                     |                    |
      v                     v                    v
+-----------+        +-----------+         +-----------+
| CPU       |        | CARTRIDGE |         | PPU       |
| ADDRESS   |        | ROM       |         | ADDRESS   |
| SPACE     |        |           |         | SPACE     |
| (64KB)    |        +-----------+         | (16KB)    |
+-----------+                              +-----------+
      |                                          |
      v                                          v
+-----------+                              +-----------+
| $0000     | 2KB Internal RAM             | $0000     | Pattern Tables
| $0800     | RAM Mirrors                  | $1000     | (CHR ROM)
| $2000     | PPU Registers                | $2000     | Name Tables
| $2008     | PPU Register Mirrors         | $2400     | (VRAM)
| $4000     | APU/IO Registers             | $2800     |
| $4020     | Expansion ROM                | $2C00     |
| $6000     | SRAM (Battery Backup)        | $3000     | Mirror of $2000-$2EFF
| $8000     |                              | $3F00     | Palette RAM
| $C000     | PRG-ROM                      | $3F20     | Sprite Palette
|           | (Game Program)               | $4000     | Mirrors
| $FFFA     | Interrupt Vectors            |           |
+-----------+                              +-----------+

*/

#include <memory>
#include "NesBus.h"
#include "CPU.h"
#include "PPU.h"
#include "NesCatridge.h"

class Nes
{

public:
    Nes();
    ~Nes();


#ifdef UNIT_TEST

    public:
        void writeByte(uint32_t address, uint8_t data);
        void writeWord(uint32_t address, uint16_t data);
        uint8_t readByte(uint32_t address) const;
        uint16_t readWord(uint32_t address) const;
#endif

private:
    std::shared_ptr<Memory> mMemory;
    std::shared_ptr<PPU> mPPU;
    std::shared_ptr<CPU> mCPU;
    std::shared_ptr<NesBus> mBus;
    std::shared_ptr<NesCatridge> mCartridge;

};
