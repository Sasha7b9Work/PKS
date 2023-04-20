// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Modules/SIM800C/SIM800C.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <cstring>


/*
        5. РАБОТА С GSM МОДЕМОМ
    Устройство имеет GSM/GPRS модем Simcom SIM800C, служащий для передачи данных на сервер по GPRS и удаленного управления контроллером.
    Модем имеет собственный источник питания +4В, управляемый портом PD2 (GSM_PWR), 0 – питание включено, 1 – питание выключено.
    Контроль питания модема осуществляется портом PE2 (GSM_PG). Данный порт является толерантным к 5В, поэтому подключен к цепи +4В без делителя.
    Порядок включения модема.
    1)  Выход GSM_PWR перевести в 1, тем самым обесточив модем.
    2)  Порт GSM_PG перевести в состояние выход 0. Необходимо убедиться, что конденсаторы фильтра питания модема разряжены и на цепи его питания
        нет остаточного напряжения, могущего привести его в неопределенное состояние или зависание. Резистор 1k в цепи станет нагрузкой для разряда конденсаторов.
    3)  Выждать 100мс для полного разряда емкостей.
    4)  Порт GSM_PG перевести в состояние вход с подтяжкой к земле (pull-down).
    5)  Выход GSM_PWR перевести в 0, тем самым включив модем.
    6)  Ожидать появления 1 на входе GSM_PG, либо выход по таймауту 100мс, при наступлении таймаута считать, что питание модема неисправно и работать оффлайн.
    7)  Ждать не менее 0,5с
    8)  Выход GSM_PWRKEY (PD0) перевести в 0 (аналог удержания кнопки включения) на 1,25с.
    9)  Если в течение 5с на порту PD1 (GSM_STATUS) лог.0 и не появилась единица, то модем по какой-то причине не включился, повторить всю процедуру сначала.
    10)  С момента появления лог.1 на GSM_STATUS готов к работе UART модема, ожидать регистрации в сети не менее 30с, если за 30с регистрации в сети нет, то
        начать всю процедуру сначала.

        Исходные состояния

        GSM_PWR = 1
        GSM_PWRKEY = 1
*/


namespace Modem
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_DISCHARGE_CAPS,    // Ожидание разряда конденсатора после обесточивания модема
            WAIT_HI_GSM_PG          // Ожидаине появления 1 на входе GSM_PG после включения модема
        };
    };

    static State::E state = State::IDLE;

    static const int SIZE_BUFFER = 128;
    static char answer[SIZE_BUFFER] = { '\0' };
    static int pointer = 0;

    namespace GSM_PG
    {
        static void ToOutLow();

        static void ToInPullDown();

        static bool ReadInput();
    }
}


void Modem::Update()
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        pinGSM_PWR.Set();
        GSM_PG::ToOutLow();
        state = State::WAIT_DISCHARGE_CAPS;
        meter.Reset();
        break;

    case State::WAIT_DISCHARGE_CAPS:
        if (meter.ElapsedTime() >= 100)
        {
            GSM_PG::ToInPullDown();
            pinGSM_PWR.Reset();
            state = State::WAIT_HI_GSM_PG;
            meter.Reset();
        }
        break;

    case State::WAIT_HI_GSM_PG:
        if (GSM_PG::ReadInput())
        {

        }
        break;
    }
}


void Modem::Init()
{
    pinGSM_PWR.Init();
    pinGSM_PWRKEY.Init();
    pinGSM_STATUS.Init();

    SIM800C::Init();

    Modem::Transmit("ATE0");
    Modem::Transmit("ATV0");
}


bool Modem::ExistUpdate()
{
    return false;
}


void Modem::Transmit(pchar message)
{
    answer[0] = '\0';
    pointer = 0;

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


pchar Modem::LastAnswer()
{
    return answer;
}


void Modem::CallbackOnReceive(char symbol)
{
    if (pointer < SIZE_BUFFER - 1)
    {
        static char buffer[2] = { 0, 0 };
        buffer[0] = symbol;

        std::strcat(answer, buffer);
        pointer++;
    }
}


void Modem::GSM_PG::ToOutLow()
{

}


void Modem::GSM_PG::ToInPullDown()
{

}


bool Modem::GSM_PG::ReadInput()
{
    return false;
}
