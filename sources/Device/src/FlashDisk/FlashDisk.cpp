// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"


#ifndef WIN32
#pragma pack(push, 1)
#endif

namespace FlashDisk
{
    struct DataInfo
    {
        uint transmited : 1;
    };

    struct Record
    {
        uint       crc;
        PackedTime time;
        float      ampl[3];
        float      curr[3];
        DataInfo   info;
    };
}

#ifndef WIN32
#pragma pack(pop)
#endif


void FlashDisk::Init()
{

}
