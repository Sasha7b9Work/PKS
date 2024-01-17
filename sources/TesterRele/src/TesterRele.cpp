// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "TesterRele.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Modem/SIM800.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Measurer/Contactors.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include <gd32f30x_rcu.h>


void TesterRele::Init()
{
    HAL::Init();
    
    Contactors::Init();

    Display::Init();
}


void TesterRele::Update()
{
    HAL_FWDGT::Update();

    Display::Update();

    Contactors::Test();
}
