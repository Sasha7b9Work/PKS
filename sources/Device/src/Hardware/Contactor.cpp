// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Contactor.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"


namespace Contactor
{
    enum E
    {
        MX0,
        MX1,
        MX2,
        MX3,
        MX4,
        KMA1,
        KMA2,
        KMA3,
        KMA4,
        KMA5,
        KMA6,
        KMA7,
        KMA8,
        KMA9,
        KMB1,
        KMB2,
        KMB3,
        KMB4,
        KMB5,
        KMB6,
        KMB7,
        KMB8,
        KMB9,
        KMC1,
        KMC2,
        KMC3,
        KMC4,
        KMC5,
        KMC6,
        KMC7,
        KMC8,
        KMC9,
        Cout
    };

    static PinOUT *pins[Cout] =
    {
        &pinOutMX0,
        &pinOutMX1,
        &pinOutMX2,
        &pinOutMX3,
        &pinOutMX4,
        &pinOutKMA1,
        &pinOutKMA2,
        &pinOutKMA3,
        &pinOutKMA4,
        &pinOutKMA5,
        &pinOutKMA6,
        &pinOutKMA7,
        &pinOutKMA8,
        &pinOutKMA9,
        &pinOutKMB1,
        &pinOutKMB2,
        &pinOutKMB3,
        &pinOutKMB4,
        &pinOutKMB5,
        &pinOutKMB6,
        &pinOutKMB7,
        &pinOutKMB8,
        &pinOutKMB9,
        &pinOutKMC1,
        &pinOutKMC2,
        &pinOutKMC3,
        &pinOutKMC4,
        &pinOutKMC5,
        &pinOutKMC6,
        &pinOutKMC7,
        &pinOutKMC8,
        &pinOutKMC9
    };

    static void Enable(E);

    static void Disable(E);

    static void UpdatePhase(const PhaseMeasure &);

#define KM_ON       1U
#define KM_OFF      0
#define KM_PAUSE    5000U   //be aware to set pause less than watchdog interval

    static void km_action(uint8 _n, uint8 _state);

    static float adc_160v = 160.0f;
    static float adc_190v = 190.0f;
    static float adc_200v = 200.0f;
    static float adc_210v = 210.0f;
    static float adc_220v = 220.0f;
    static float adc_230v = 230.0f;
    static float adc_240v = 240.0f;
    static float adc_250v = 250.0f;
    static float adc_260v = 260.0f;
    static float adc_270v = 270.0f;

    static uint8 systick_f = 0;
}


void Contactor::Init()
{
    for (int i = 0; i < Cout; i++)
    {
        pins[i]->Init();
    }
}


void Contactor::Enable(E pin)
{
    pins[pin]->Set();
}


void Contactor::Disable(E pin)
{
    pins[pin]->Reset();
}


void Contactor::Update(const FullMeasure &measure)
{
    for (int i = 0; i < 3; i++)
    {
        UpdatePhase(measure.measures[i]);
    }
}


void Contactor::UpdatePhase(const PhaseMeasure &measure)
{
    static int32_t _current_stage_num = 0, _old_stage_num;
    //static uint32_t _voltage_old = 0;

    _old_stage_num = _current_stage_num;

    float _adc_voltage = measure.voltage;

    //determine new stage according to voltage and prev stage
    if (_adc_voltage <= adc_160v)
        _current_stage_num = 0;
    else
        if (_adc_voltage <= adc_190v)
            _current_stage_num = 4;
        else
            if (_adc_voltage <= adc_200v)
            {
                if (_old_stage_num >= 4)
                    _current_stage_num = 4;
                else
                    _current_stage_num = 3;
            }
            else
                if (_adc_voltage <= adc_210v)
                {
                    if (_old_stage_num >= 3)
                        _current_stage_num = 3;
                    else
                        _current_stage_num = 2;
                }
                else
                    if (_adc_voltage <= adc_220v)
                    {
                        if (_old_stage_num >= 2)
                            _current_stage_num = 2;
                        else
                            _current_stage_num = 1;
                    }
                    else
                        if (_adc_voltage <= adc_230v)
                        {
                            if (_old_stage_num >= 1)
                                _current_stage_num = 1;
                            else
                                _current_stage_num = 0;
                        }
                        else
                            if (_adc_voltage <= adc_240v)
                            {
                                if (_old_stage_num >= 0)
                                    _current_stage_num = 0;
                                else
                                    _current_stage_num = -1;
                            }
                            else
                                if (_adc_voltage <= adc_250v)
                                {
                                    if (_old_stage_num >= -1)
                                        _current_stage_num = -1;
                                    else
                                        _current_stage_num = -2;
                                }
                                else
                                    if (_adc_voltage <= adc_260v)
                                    {
                                        if (_old_stage_num >= -2)
                                            _current_stage_num = -2;
                                        else
                                            _current_stage_num = -3;
                                    }
                                    else
                                        if (_adc_voltage <= adc_270v)
                                        {
                                            if (_old_stage_num >= -3)
                                                _current_stage_num = -3;
                                            else
                                                _current_stage_num = -4;
                                        }
                                        else
                                            _current_stage_num = -4;

    //switch to another stage
    if (_old_stage_num != _current_stage_num)
    {
        //first, switch to bypass
        if (_old_stage_num != 0)
        {
            km_action(2, KM_ON);
            km_action(3, KM_ON);
            km_action(1, KM_OFF);
            km_action(2, KM_OFF);
            delay_1ms(KM_PAUSE);
//            HAL_IWDG_Refresh(&hiwdg);
            km_action(3, KM_OFF);
        };

        //set current stage
        if ((_current_stage_num == 0) || (_current_stage_num == 1))
        {
            km_action(4, KM_OFF);
            km_action(5, KM_OFF);
            km_action(6, KM_OFF);
            km_action(7, KM_OFF);
            km_action(8, KM_OFF);
        };

        if (_current_stage_num == 2)
        {
            km_action(4, KM_OFF);
            km_action(5, KM_OFF);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_OFF);
        };

        if (_current_stage_num == 3)
        {
            km_action(4, KM_OFF);
            km_action(5, KM_ON);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_OFF);
        };

        if (_current_stage_num == 4)
        {
            km_action(4, KM_ON);
            km_action(5, KM_ON);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_OFF);
        };

        if (_current_stage_num == -1)
        {
            km_action(4, KM_OFF);
            km_action(5, KM_OFF);
            km_action(6, KM_OFF);
            km_action(7, KM_OFF);
            km_action(8, KM_ON);
        };

        if (_current_stage_num == -2)
        {
            km_action(4, KM_OFF);
            km_action(5, KM_OFF);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_ON);
        };

        if (_current_stage_num == -3)
        {
            km_action(4, KM_OFF);
            km_action(5, KM_ON);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_ON);
        };

        if (_current_stage_num == -4)
        {
            km_action(4, KM_ON);
            km_action(5, KM_ON);
            km_action(6, KM_ON);
            km_action(7, KM_ON);
            km_action(8, KM_ON);
        };

        //final sequence if not bypass
        if (_current_stage_num != 0)
        {
            km_action(2, KM_ON);
            km_action(3, KM_ON);
            km_action(1, KM_ON);
            km_action(2, KM_OFF);
            delay_1ms(KM_PAUSE);
//            HAL_IWDG_Refresh(&hiwdg);
            km_action(3, KM_OFF);
        };
    };
}


void Contactor::km_action(uint8 _n, uint8 _state)
{
    uint _delay = 50U;          //2.5 periods of grid is a timeout to acquisite pulses from AC detectors
    uint _off_cnt = 0, _on_cnt = 0;

    systick_f = 0;

    //KM1
    if (_n == 1)
    {
        HAL_GPIO_WritePin(KM1_GPIO_Port, KM1_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM1_GPIO_Port, TSTKM1_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM9_GPIO_Port, TSTKM9_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM2 on
    if (_n == 2)
    {
        HAL_GPIO_WritePin(KM2_GPIO_Port, KM2_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM2_GPIO_Port, TSTKM2_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM10_GPIO_Port, TSTKM10_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM3 on
    if (_n == 3)
    {
        HAL_GPIO_WritePin(KM3_GPIO_Port, KM3_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM3_GPIO_Port, TSTKM3_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM11_GPIO_Port, TSTKM11_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM4 on
    if (_n == 4)
    {
        HAL_GPIO_WritePin(KM4_GPIO_Port, KM4_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM4_GPIO_Port, TSTKM4_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM12_GPIO_Port, TSTKM12_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM5 on
    if (_n == 5)
    {
        HAL_GPIO_WritePin(KM5_GPIO_Port, KM5_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM5_GPIO_Port, TSTKM5_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM13_GPIO_Port, TSTKM13_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM6 on
    if (_n == 6)
    {
        HAL_GPIO_WritePin(KM6_GPIO_Port, KM6_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM6_GPIO_Port, TSTKM6_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM14_GPIO_Port, TSTKM14_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM7 on
    if (_n == 7)
    {
        HAL_GPIO_WritePin(KM7_GPIO_Port, KM7_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM7_GPIO_Port, TSTKM7_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM15_GPIO_Port, TSTKM15_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

    //KM8 on
    if (_n == 8)
    {
        HAL_GPIO_WritePin(KM8_GPIO_Port, KM8_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_Delay(KM_TRANSITION_DELAY);

        while (_delay)
        {
            if (HAL_GPIO_ReadPin(TSTKM8_GPIO_Port, TSTKM8_Pin) == GPIO_PIN_RESET)
                _on_cnt++;
            if (HAL_GPIO_ReadPin(TSTKM16_GPIO_Port, TSTKM16_Pin) == GPIO_PIN_RESET)
                _off_cnt++;

            if (systick_f)
            {
                systick_f = 0;
                _delay--;
            };
        };
    };

#ifndef CONTACTORS_NO_FEEDBACK
    //check contactorts except KM8
    if (_n != 8U)
    {
        if (_state && (_off_cnt || !_on_cnt))
            km_error(_n);

        if (!_state && (!_off_cnt || _on_cnt))
            km_error(_n);
    };
#endif
}
