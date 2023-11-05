// 2023/04/09 14:54:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Contactors
{
    void Init();

    namespace Test
    {
        void Update();

        int GetCounterBad(Phase::E phase, int num);

        int GetCountSteps();
    }

    namespace Serviceability
    {
        // ��������� ����������� ������� ����������
        // ����� �� ����������� ����������� ������� 100 � - � ���� ������ 27
        void GetStates(int states[NUM_PINS_MX]);

        void Update();

        bool AllIsOK(Phase::E);
    }
}
