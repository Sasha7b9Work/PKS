// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/SIM800C/SIM800C.h"
#include "Blocks/Measurer/Measurer.h"
#include "Blocks/Calculator/Calculator.h"


namespace Device
{

}


void Device::Init()
{
    HAL::Init();

    Timer::Init();

    Timer::Delay(500);

    SIM800C::Init();
}


void Device::Update()
{
    InputData in = Measurer::ReadInputData();

    OutputData out = Calculator::CalculateOutput(in);
}
