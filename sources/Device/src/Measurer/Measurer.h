// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Измеритель напряжения
*/


//struct PhaseMeasure
//{
//    float current;
//    float voltage;
//    float power;
//};


//struct FullMeasure
//{
////    struct PhaseMeasure measures[3];
//
//    float current[3];
//    float voltage[3];
//    float power[3];
//};


void Measurer_AppendMeasures(uint16 adc_values[6]);

// Не ноль, если буферы заполнены
int Measurer_BuffersFull(void);

// Возвращает не ноль, если готово новое измерение
int Measurer_MeasureReady(void);

float Measurer_Get

