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

    struct PinOUT *pinsKM[3][10] =
    {
        {nullptr, &pinOutKMA1, &pinOutKMA2, &pinOutKMA3, &pinOutKMA4, &pinOutKMA5, &pinOutKMA6, &pinOutKMA7, &pinOutKMA8, &pinOutKMA9},
        {nullptr, &pinOutKMB1, &pinOutKMB2, &pinOutKMB3, &pinOutKMB4, &pinOutKMB5, &pinOutKMB6, &pinOutKMB7, &pinOutKMB8, &pinOutKMB9},
        {nullptr, &pinOutKMC1, &pinOutKMC2, &pinOutKMC3, &pinOutKMC4, &pinOutKMC5, &pinOutKMC6, &pinOutKMC7, &pinOutKMC8, &pinOutKMC9}
    };

    struct PinIN *pinsInKM[3][10] =
    {
        {nullptr, &pinInKMA1, &pinInKMA2, &pinInKMA3, &pinInKMA4, &pinInKMA5, &pinInKMA6, &pinInKMA7, &pinInKMA8, &pinInKMA9},
        {nullptr, &pinInKMB1, &pinInKMB2, &pinInKMB3, &pinInKMB4, &pinInKMB5, &pinInKMB6, &pinInKMB7, &pinInKMB8, &pinInKMB9},
        {nullptr, &pinInKMC1, &pinInKMC2, &pinInKMC3, &pinInKMC4, &pinInKMC5, &pinInKMC6, &pinInKMC7, &pinInKMC8, &pinInKMC9}
    };

    static void UpdatePhase(Phase::E, const PhaseMeasure &);

#define KM_ON               true
#define KM_OFF              false
#define KM_PAUSE            5000U   //be aware to set pause less than watchdog interval
#define KM_TRANSITION_DELAY 130U

    static void km_action(Phase::E, uint8 _n, bool _state);

    static void km_error(uint8 _n);

    static float adc_160v[Phase::Count] = { 160.0f, 160.0f, 160.0f };
    static float adc_190v[Phase::Count] = { 190.0f, 190.0f, 190.0f };
    static float adc_200v[Phase::Count] = { 200.0f, 200.0f, 200.0f };
    static float adc_210v[Phase::Count] = { 210.0f, 210.0f, 210.0f };
    static float adc_220v[Phase::Count] = { 220.0f, 220.0f, 220.0f };
    static float adc_230v[Phase::Count] = { 230.0f, 230.0f, 230.0f };
    static float adc_240v[Phase::Count] = { 240.0f, 240.0f, 240.0f };
    static float adc_250v[Phase::Count] = { 250.0f, 250.0f, 250.0f };
    static float adc_260v[Phase::Count] = { 260.0f, 260.0f, 260.0f };
    static float adc_270v[Phase::Count] = { 270.0f, 270.0f, 270.0f };

    static uint8 systick_f = 0;
}


void Contactor::Init()
{
    for (int i = 0; i < Cout; i++)
    {
        pins[i]->Init();
    }
}


void Contactor::Update(const FullMeasure &)
{
    for (int i = 0; i < 3; i++)
    {
//        UpdatePhase((Phase::E)i, measure.measures[i]);
    }
}


void Contactor::UpdatePhase(Phase::E phase, const PhaseMeasure &measure)
{
    static int _current_stage_num[Phase::Count] = { 0, 0, 0 };
    static int _old_stage_num[Phase::Count];
    //static uint32_t _voltage_old = 0;

    _old_stage_num[phase] = _current_stage_num[phase];

    float _adc_voltage = measure.voltage;

    //determine new stage according to voltage and prev stage
    if (_adc_voltage <= adc_160v[phase])
        _current_stage_num[phase] = 0;
    else if (_adc_voltage <= adc_190v[phase])
        _current_stage_num[phase] = 4;
    else if (_adc_voltage <= adc_200v[phase])
    {
        if (_old_stage_num[phase] >= 4)
            _current_stage_num[phase] = 4;
        else
            _current_stage_num[phase] = 3;
    }
    else if (_adc_voltage <= adc_210v[phase])
    {
        if (_old_stage_num[phase] >= 3)
            _current_stage_num[phase] = 3;
        else
            _current_stage_num[phase] = 2;
    }
    else if (_adc_voltage <= adc_220v[phase])
    {
        if (_old_stage_num[phase] >= 2)
            _current_stage_num[phase] = 2;
        else
            _current_stage_num[phase] = 1;
    }
    else if (_adc_voltage <= adc_230v[phase])
    {
        if (_old_stage_num[phase] >= 1)
            _current_stage_num[phase] = 1;
        else
            _current_stage_num[phase] = 0;
    }
    else if (_adc_voltage <= adc_240v[phase])
    {
        if (_old_stage_num >= 0)
            _current_stage_num[phase] = 0;
        else
            _current_stage_num[phase] = -1;
    }
    else if (_adc_voltage <= adc_250v[phase])
    {
        if (_old_stage_num[phase] >= -1)
            _current_stage_num[phase] = -1;
        else
            _current_stage_num[phase] = -2;
    }
    else if (_adc_voltage <= adc_260v[phase])
    {
        if (_old_stage_num[phase] >= -2)
            _current_stage_num[phase] = -2;
        else
            _current_stage_num[phase] = -3;
    }
    else if (_adc_voltage <= adc_270v[phase])
    {
        if (_old_stage_num[phase] >= -3)
            _current_stage_num[phase] = -3;
        else
            _current_stage_num[phase] = -4;
    }
    else
        _current_stage_num[phase] = -4;

    //switch to another stage
    if (_old_stage_num[phase] != _current_stage_num[phase])
    {
        //first, switch to bypass
        if (_old_stage_num[phase] != 0)
        {
            km_action(phase, 2, KM_ON);
            km_action(phase, 3, KM_ON);
            km_action(phase, 1, KM_OFF);
            km_action(phase, 2, KM_OFF);
            delay_1ms(KM_PAUSE);
//            HAL_IWDG_Refresh(&hiwdg);
            km_action(phase, 3, KM_OFF);
        };

        //set current stage
        if ((_current_stage_num[phase] == 0) || (_current_stage_num[phase] == 1))
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_OFF);
            km_action(phase, 6, KM_OFF);
            km_action(phase, 7, KM_OFF);
            km_action(phase, 8, KM_OFF);
        };

        if (_current_stage_num[phase] == 2)
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_OFF);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_OFF);
        };

        if (_current_stage_num[phase] == 3)
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_ON);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_OFF);
        };

        if (_current_stage_num[phase] == 4)
        {
            km_action(phase, 4, KM_ON);
            km_action(phase, 5, KM_ON);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_OFF);
        };

        if (_current_stage_num[phase] == -1)
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_OFF);
            km_action(phase, 6, KM_OFF);
            km_action(phase, 7, KM_OFF);
            km_action(phase, 8, KM_ON);
        };

        if (_current_stage_num[phase] == -2)
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_OFF);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_ON);
        };

        if (_current_stage_num[phase] == -3)
        {
            km_action(phase, 4, KM_OFF);
            km_action(phase, 5, KM_ON);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_ON);
        };

        if (_current_stage_num[phase] == -4)
        {
            km_action(phase, 4, KM_ON);
            km_action(phase, 5, KM_ON);
            km_action(phase, 6, KM_ON);
            km_action(phase, 7, KM_ON);
            km_action(phase, 8, KM_ON);
        };

        //final sequence if not bypass
        if (_current_stage_num[phase] != 0)
        {
            km_action(phase, 2, KM_ON);
            km_action(phase, 3, KM_ON);
            km_action(phase, 1, KM_ON);
            km_action(phase, 2, KM_OFF);
            delay_1ms(KM_PAUSE);
//            HAL_IWDG_Refresh(&hiwdg);
            km_action(phase, 3, KM_OFF);
        };
    };
}


void Contactor::km_action(Phase::E phase, uint8 _n, bool _state)
{
    uint _delay = 50U;          //2.5 periods of grid is a timeout to acquisite pulses from AC detectors
    uint _off_cnt = 0, _on_cnt = 0;

    systick_f = 0;

    pinsKM[phase][_n]->SetState(_state);
    delay_1ms(KM_TRANSITION_DELAY);

    while (_delay)
    {
        if (pinsInKM[phase][_n]->IsLow())
            _on_cnt++;

        if (pinsInKM[phase][9]->IsLow())
            _off_cnt++;

        if (systick_f)
        {
            systick_f = 0;
            _delay--;
        };
    };


    //check contactorts except KM8
    if (_n != 8U)
    {
        if (_state && (_off_cnt || !_on_cnt))
            km_error(_n);

        if (!_state && (!_off_cnt || _on_cnt))
            km_error(_n);
    };
}


void Contactor::km_error(uint8)
{

}
