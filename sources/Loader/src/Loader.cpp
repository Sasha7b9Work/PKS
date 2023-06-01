// 2023/05/18 13:52:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Loader.h"
#include "Modem/Updater.h"
#include "Hardware/Application.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"


void Loader::Init()
{
    HAL::Init();

    Modem::Init();

    HAL_FWDGT::Init();
}


void Loader::Update()
{
    HAL_FWDGT::Update();

    Modem::Update();

    if (Updater::IsCompleted())
    {
        Application::Run();
    }
}
