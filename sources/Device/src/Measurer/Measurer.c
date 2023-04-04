// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"


static struct FullMeasure measure;


void Measurer_AppendMeasures(uint16 adc_values[3])
{

}


int Measurer_BuffersFull()
{
    return 0;
}


void Measurer_Calculate()
{

}


int Measurer_MeasureReady()
{
    return 0;
}


struct FullMeasure Measurer_GetMeasure()
{
    return measure;
}
