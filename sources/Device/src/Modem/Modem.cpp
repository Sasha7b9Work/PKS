// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Commands.h"
#include "Modem/MQTT.h"
#include <gd32f30x.h>
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

/*
    AT+SAPBR=3,1,"APN","internet"
    AT+SAPBR=3,1,"USER",""
    AT+SAPBR=3,1,"PWD",""
    AT+SAPBR=1,1
    AT+HTTPINIT
    AT+HTTPPARA="CID",1
*/


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
            WAIT_REGISTRATION,      // Ожидание регистрации
            RUNNING                 // Подключились, зарегистрировались, работаем
        };
    };

    static State::E state = State::IDLE;

    namespace Answer
    {
        static char buffer[MAX_LENGTH_ANSWERR] = { '\0' };
        static int pointer = 0;
        static bool ready = false;

        static void Clear()
        {
            pointer = 0;
            ready = false;
            buffer[0] = '\0';
        }

        static void Push(char symbol)
        {
            if (symbol == 0x0a)
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

            if (symbol == 0x0d)
            {
                if (ready == true)
                {
                    Clear();
                }
                else
                {
                    buffer[pointer++] = '\0';
                    ready = true;
                }
            }
            else
            {
                buffer[pointer++] = symbol;
            }
        }
    }

    namespace GSM_PG
    {
        static void ToOutLow();

        static void ToInPullDown();

        static bool ReadInput();
    }

    void TransmitUINT8(uint8);
    void TransmitUINT(uint);

    // Передаёт сообщение и возвращает true, если принят ответ answer
    bool TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout = TIME_WAIT_ANSWER_DEFAULT);
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
            state = State::WAIT_REGISTRATION;
            meter.Reset();
            Transmit("ATE0");
            if (!TransmitAndWaitAnswer("AT+GSMBUSY=1", "OK"))
            {
                state = State::IDLE;
            }
        }
        if (meter.ElapsedTime() > 5000)
        {
            state = State::IDLE;
        }
        break;

    case State::WAIT_REGISTRATION:

        if (Command::RegistrationIsOk())
        {
            if (!TransmitAndWaitAnswer("AT+SAPBR=3,1,\"APN\",\"internet\"", "OK") ||
                !TransmitAndWaitAnswer("AT+SAPBR=3,1,\"USER\",\"\"", "OK") ||
                !TransmitAndWaitAnswer("AT+SAPBR=3,1,\"PWD\",\"\"", "OK") ||
                !TransmitAndWaitAnswer("AT+SAPBR =1,1", "OK") ||
                !Command::ConnectToTCP())
//                !TransmitAndWaitAnswer("AT+HTTPINIT", "OK") ||
//                !TransmitAndWaitAnswer("AT+HTTPPARA=\"CID\",1", "OK"))
            {
                state = State::IDLE;
            }
            else
            {
                MQTT::Connect();

                state = State::RUNNING;
            }
        }
        else if (meter.ElapsedTime() > 30000)
        {
            state = State::IDLE;
        }
        break;

    case State::RUNNING:
        break;
    }
}


bool Modem::IsRunning()
{
    return state == State::RUNNING;
}


void Modem::Init()
{
    pinGSM_PWR.Init();
    pinGSM_PWRKEY.Init();
    pinGSM_STATUS.Init();

    pinGSM_PWR.Set();
    pinGSM_PWRKEY.Set();
}


bool Modem::ExistUpdate()
{
    return false;
}


uint Modem::Transmit(pchar message, uint timeout)
{
    TimeMeterMS meter;

    Answer::Clear();

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);

    while ((meter.ElapsedTime() < timeout) && !Answer::ready)
    {
    }

    return meter.ElapsedTime();
}


void Modem::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void Modem::TransmitUINT(uint value)
{
    HAL_USART_GPRS::Transmit(&value, 4);
}


bool Modem::TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout)
{
    Transmit(message, timeout);

    char *last_answer = LastAnswer().c_str();

    return std::strcmp(last_answer, answer) == 0;
}


void Modem::CallbackOnReceive(char symbol)
{
    Answer::Push(symbol);
}


bool Modem::LastAnswer(char out[MAX_LENGTH_ANSWERR])
{
    if(Answer::ready)
    {
        std::strcpy(out, Answer::buffer);
        return true;
    }

    out[0] = '\0';

    return false;
}


String Modem::LastAnswer()
{
    char buffer[MAX_LENGTH_ANSWERR];

    LastAnswer(buffer);

    return String(buffer);
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
