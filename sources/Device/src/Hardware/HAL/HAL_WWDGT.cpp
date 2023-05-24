// 2023/5/11 13:06:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_WWDGT::Init()
{
    fwdgt_config(0xFFF, FWDGT_PSC_DIV16);                               // 16000 мс

    fwdgt_enable();
}


void HAL_WWDGT::Update()
{
    fwdgt_counter_reload();
}
