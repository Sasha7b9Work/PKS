// 2023/09/14 11:48:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


struct Measurements
{
    uint number;

    uint16 volts[Phase::Count];     // «десь хран€тс€ значени€, увеличенные в 10 раз
    uint16 currents[Phase::Count];  // «десь хран€тс€ значени€, увеличенные в 10 раз

    void SetFullMeasure(FullMeasure &);
    FullMeasure GetFullMeasure() const;

    struct Flags
    {
        //  0     ..  1          - kma1     -1,0,1
        //  8 * 2 ... 8 * 2 + 1  - kma9
        //  9 * 2 ... 9 * 2 + 1  - kmb1
        // 17 * 2 ... 17 * 2 + 1 - kmb9
        // 18 * 2 ... 18 * 2 + 1 - kmc1
        // 26 * 2 ... 26 * 2 + 1 - kmc9
        // 27 * 2                - 100V
        // 55                    - gpA
        // 56                    - gpB
        // 57                    - gpC
        uint64 bits;
        int stageA : 5;     // 
        int stageB : 5;
        int stageC : 5;

        // num - от нул€ до 8 (KM1...KM9)       state = -1,0,1
        void SetKM(Phase::E, int num, int state);
        int GetKM(Phase::E, int num) const;

        void SetGP(Phase::E, bool);
        bool GetGP(Phase::E) const;

        void Set100V(bool);
        bool Get100V() const;

        void SetStageRele(Phase::E, int);
        int GetStageRele(Phase::E);

    private:

        // Ќа столько нужно сместить вправо, чтобы в двух младших битах было значение
        int ShiftBitsStateKM(Phase::E, int) const;
        int NumBitGP(Phase::E) const;

    } flags;
};


namespace Storage
{
    void Init();

    void Update();
}
