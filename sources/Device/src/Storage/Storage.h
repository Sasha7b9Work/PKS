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
        //  0 - kma1
        //  8 - kma9
        //  9 - kmb1
        // 17 - kmb9
        // 18 - kmc1
        // 26 - kmc9
        // 27 - 100V
        // 28 - gpA
        // 29 - gpB
        // 30 - gpC
        uint bits;
        int stateA : 2;
        int stateB : 2;
        int stateC : 2;

        // num - от нул€ до 8 (KM1...KM9)
        void SetKM(Phase::E, int num, bool);
        bool GetKM(Phase::E, int num) const;
        void SetGP(Phase::E, bool);
        bool GetGP(Phase::E) const;
        void Set100V(bool);
        bool Get100V() const;
        void SetStateRele(Phase::E, int);
        int GetStateRele(Phase::E);

    private:

        int NumBitKM(Phase::E, int) const;
        int NumBItGP(Phase::E) const;

    } flags;
};


namespace Storage
{
    void Init();

    void Update();
}
