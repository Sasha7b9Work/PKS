// 2023/04/09 14:54:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Contactors
{
    void Init();

    void Update(const FullMeasure &);

    namespace Serviceability
    {
        // ��������� ����������� ������� ����������
        // ����� �� ����������� ����������� ������� 100 � - � ���� ������ 27
        void Update(int states[NUM_PINS_MX], bool valid[NUM_PINS_MX]);
    }

    void Test();
}
