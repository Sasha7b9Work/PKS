// 2023/05/18 13:52:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Loader.h"
#include "Modem/Updater.h"
#include "Application.h"
#include "Hardware/HAL/HAL.h"


void Loader::Init()
{
    HAL::Init();
}


void Loader::Update()
{
    if (Updater::IsCompleted())
    {
        Application::Run();
    }
}
