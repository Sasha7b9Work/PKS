// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"


#ifndef WIN32
#pragma pack(push, 1)
#endif

namespace FlashDisk
{
    static const int SIZE = 128 / 8 * 1024 * 1024;

    struct Sector
    {
        Record records[113];
    };

    struct Block
    {
        Sector sectors[16];
    };

    struct Memory
    {
        Block blocks[256];
    };
}

#ifndef WIN32
#pragma pack(pop)
#endif


void FlashDisk::Init()
{

}
