// 2023/5/11 13:06:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_WWDGT::Init()
{
    wwdgt_config(127, 80, WWDGT_CFG_PSC_DIV8);
    wwdgt_enable();
    wwdgt_interrupt_enable();
}


void HAL_WWDGT::Update()
{
    wwdgt_counter_update(127);
}