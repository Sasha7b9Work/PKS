// 2023/04/17 13:30:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct FontDef
{
    const uint8 width;      // Font width in pixels
    uint8 height;           // Font height in pixels
    const uint16 *data;     // Pointer to data font data array
} ;

