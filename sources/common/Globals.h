// 2023/06/06 10:35:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace GL
{
    extern uint _RCU_RSTSCK;

    namespace Firmware
    {
        bool IsExist();

        pchar Address();

        pchar Directory();

        pchar FileName();

        pchar Login();

        pchar Password();
    }
}
