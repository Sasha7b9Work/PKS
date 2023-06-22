// 2023/06/22 13:59:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Settings
{
    float GetKoeffCurrent();        // Коэфццициент преобразования тока

    bool IsControllingRelays();     // Если true, то контролировать состояние реле и отключать фазу, если хотя бы одно реле на ней неисправно
};


extern Settings gset;
