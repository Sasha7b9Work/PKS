// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   
*/



void Measurer_AppendMeasures(uint16 adc0, uint16 adc1, uint16 adc2, uint16 adc3, uint16 adc4, uint16 adc5);

// ¬озвращает не ноль, если готово новое измерение
int Measurer_MeasureReady(void);

