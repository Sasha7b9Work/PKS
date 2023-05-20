// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include <cstring>
#include "Modem/Parser.h"


/*
        5. РАБОТА С GSM МОДЕМОМ
    Устройство имеет GSM/GPRS модем Simcom SIM800C, служащий для передачи данных на сервер по GPRS и удаленного управления контроллером.
    Модем имеет собственный источник питания +4В, управляемый портом PD2 (GSM_PWR), 0 – питание включено, 1 – питание выключено.
    Контроль питания модема осуществляется портом PE2 (GSM_PG). Данный порт является толерантным к 5В, поэтому подключен к цепи +4В без делителя.Порядок включения модема.
    1)  Выход GSM_PWR перевести в 1, тем самым обесточив модем.
    2)  Порт GSM_PG перевести в состояние выход 0. Необходимо убедиться, что конденсаторы фильтра питания модема разряжены и на цепи его питания
        нет остаточного напряжения, могущего привести его в неопределенное состояние или зависание. Резистор 1k в цепи станет нагрузкой для разряда конденсаторов.
    3)  Выждать 1000мс для полного разряда емкостей.
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


using namespace std;


namespace SIM800
{
    void Update(pchar);
    bool IsRegistered();
    pchar LevelSignal();
}


namespace MQTT
{
    bool IsConnected();

    namespace Send
    {
        void Measure(const FullMeasure &);
        void GP(int num, bool state);
        void StateContactors(const bool[NUM_PINS_MX]);
        void LevelContactors(int[Phase::Count]);
    }
}


namespace Modem
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_DISCHARGE_CAPS,    // Ожидание разряда конденсатора после обесточивания модема
            WAIT_HI_GSM_PG,         // Ожидаине появления 1 на входе GSM_PG после включения модема
            WAIT_500_MS,            // Ожидать полсекунды после появления 1 на входе GSM_PG
            WAIT_1250_MS,           // Ожидать 1.25 сек после перевода GSM_PWRKEY в 0
            WAIT_5000_MS,           // Ожидать 5 секунд единицу на GSM_STATUS
            HARDWARE_IS_OK,         // Железо отработало, работают команды
        };
    };

    static State::E state = State::IDLE;

    const int MAX_LENGTH_ANSWERR = 64;

    namespace Answer
    {
        static const int MAX_ANSWERS = 10;
        static char answers[MAX_ANSWERS][MAX_LENGTH_ANSWERR];
        static int num_answers = 0;

        static char buffer[MAX_LENGTH_ANSWERR] = { '\0' };
        static int pointer = 0;

        static void Push(char symbol)
        {
            if (symbol == 0x0a || symbol == 0x00)
            {
                return;
            }

            if (pointer == MAX_LENGTH_ANSWERR - 1)
            {
                pointer = 0;
            }

            if (symbol == 0x0d && pointer == 0)
            {
                return;
            }

            buffer[pointer++] = symbol;

            if (symbol == 0x0d)
            {
                buffer[pointer - 1] = '\0';

                if (num_answers < MAX_ANSWERS)
                {
                    strcpy(answers[num_answers++], buffer);
                }

                pointer = 0;
            }
        }
    }

    namespace GSM_PG
    {
        static void ToOutLow();

        static void ToInPullDown();

        static bool ReadInput();
    }

    // После любой необработанной ошибки SIM800 вызывать эту функцию
    void CallbackOnErrorSIM800();
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
        if (meter.ElapsedTime() > 1000)
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
            meter.Reset();
            state = State::WAIT_500_MS;
        }
        if (meter.ElapsedTime() > 100)
        {
            state = State::IDLE;
        }
#ifdef OLD_VERSION
        meter.Reset();
        state = State::WAIT_500_MS;
#endif
        break;

    case State::WAIT_500_MS:
        if (meter.ElapsedTime() > 500)
        {
            pinGSM_PWRKEY.Reset();
            meter.Reset();
            state = State::WAIT_1250_MS;
        }
        break;

    case State::WAIT_1250_MS:
        if (meter.ElapsedTime() > 1250)
        {
            pinGSM_PWRKEY.Set();
            meter.Reset();
            state = State::WAIT_5000_MS;
        }
        break;

    case State::WAIT_5000_MS:
        if (pinGSM_STATUS.IsHi())
        {
            state = State::HARDWARE_IS_OK;
            meter.Reset();
        }
        if (meter.ElapsedTime() > 5000)
        {
            state = State::IDLE;
        }
        break;

    case State::HARDWARE_IS_OK:
        if (Answer::num_answers == 0)
        {
            SIM800::Update("");
        }
        else
        {
            for (int i = 0; i < Answer::num_answers; i++)
            {
                SIM800::Update(Answer::answers[i]);
                Answer::answers[i][0] = '\0';
            }
            Answer::num_answers = 0;
        }
    }
}


bool Modem::Mode::Power()
{
    return (state == State::HARDWARE_IS_OK);
}


bool Modem::Mode::Registered()
{
    return SIM800::IsRegistered();
}


bool Modem::Mode::ConnectedToMQTT()
{
    return MQTT::IsConnected();
}


pchar Modem::Mode::LevelSignal()
{
    return SIM800::LevelSignal();
}


void Modem::Init()
{
    pinGSM_PWR.Init();
    pinGSM_PWRKEY.Init();
    pinGSM_STATUS.Init(GPIO_MODE_IPD);

    pinGSM_PWR.Set();
    pinGSM_PWRKEY.Set();
}


bool Modem::ExistUpdate()
{
    return false;
}


void Modem::CallbackOnErrorSIM800()
{
    state = State::IDLE;
}


void Modem::CallbackOnReceive(char symbol)
{
    Answer::Push(symbol);

    if (symbol == '>')
    {
        Answer::Push(0x0d);
    }
}


void Modem::GSM_PG::ToOutLow()
{
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    GPIO_BC(GPIOE) = GPIO_PIN_2;
}


void Modem::GSM_PG::ToInPullDown()
{
    gpio_init(GPIOE, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
}


bool Modem::GSM_PG::ReadInput()
{
    return gpio_input_bit_get(GPIOE, GPIO_PIN_2) == SET;
}


void Modem::Send::Measure(const FullMeasure &measure)
{
    MQTT::Send::Measure(measure);
}


void Modem::Send::GP(int num, bool is_low)
{
    MQTT::Send::GP(num, is_low);
}


void Modem::Send::StateContactors(const bool state_contactors[NUM_PINS_MX])
{
    MQTT::Send::StateContactors(state_contactors);
}


void Modem::Send::LevelContactors(int levels[Phase::Count])
{
    MQTT::Send::LevelContactors(levels);
}
