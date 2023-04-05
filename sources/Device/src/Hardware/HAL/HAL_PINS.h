// 2023/04/05 23:31:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


class PinADC
{
public:
    PinADC(uint _port, uint _pin, uint8 _channel) : port(_port), pin(_pin), channel(_channel) { }

    void Init();

    uint8 Channel() const { return channel; }

private:

    uint port;
    uint pin;
    uint8 channel;
};


extern PinADC pinVolt1;     // ACVOLTOUT1
extern PinADC pinVolt2;     // ACVOLTOUT2
extern PinADC pinVolt3;     // ACVOLTOUT3

extern PinADC pinCur1L;     // ACCURROUTL1
extern PinADC pinCur2L;     // ACCURROUTL2
extern PinADC pinCur3L;     // ACCURROUTL3

extern PinADC pinCur1H;     // ACCURROUTH1
extern PinADC pinCur2H;     // ACCURROUTH2
extern PinADC pinCur3H;     // ACCURROUTH3
