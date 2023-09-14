// 2023/09/14 11:48:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Measurer/Measurer.h"
#include "Measurer/Contactors.h"


void Storage::Init()
{
    Contactors::Init();
}


void Storage::Update()
{
    Measurer::Update();

    Contactors::Update(Measurer::Measure5Sec());
}
