// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include <cstring>
#include <cstdio>


using namespace Parser;


namespace Modem
{
    void CallbackOnErrorSIM800();
}


namespace MQTT
{
    void Update(const String &);
    void Reset();
    void CallbackOnReceiveData(const String &);
}


namespace SIM800
{
    struct State
    {
        enum E
        {
            START,
            WAIT_ATE0,
            WAIT_GSMBUSY,
            WAIT_CREG,
            WAIT_IP_INITIAL,
            WAIT_CSTT,
            WAIT_IP_START,
            WAIT_CIICR,
            WAIT_IP_GPRSACT,
            WAIT_CIFSR,
            WAIT_IP_STATUS,
            WAIT_TCP_CONNECT,
            WAIT_CIPHEAD,
            RUNNING_MQTT,
            UPDATE_NEED_SAPBR,
            UPDATE_NEED_FTPCID,         // Находимся в состоянии обновления
            UPDATE_NEED_FTPSERV,        // Имя сервера
            UPDATE_NEED_FTPPORT,
            UPDATE_NEED_FTPUN,          // Имя пользователя
            UPDATE_NEED_FTPPW,          // Пароль
            UPDATE_NEED_FTPGETPATH,     // Папка с файлом
            UPDATE_NEED_FTPGETNAME,     // Имя файла
            UPDATE_NEED_FTPGET,         // Подключение к серверу
            UPDATE_NEED_FTPGET_BYTES,   // Запрос на получение данных
            UPDATE_GET_BYTES            // Получение данных
        };
    };

    static State::E state = State::START;

    // Возращает время до получения ответа
    void Transmit(pchar);
    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    void Update(const String &);

    void Reset();

    static bool ProcessUnsolicited(const String &);

    bool IsRegistered();

    String LevelSignal();

    static TimeMeterMS meterCSQ;

    static String levelSignal("0");

    String address;
    String login;
    String password;
    String firmware;
}


bool SIM800::ProcessUnsolicited(const String &answer)
{
    String first_word = Parser::GetWord(answer, 1);

    if (answer == "CLOSED")
    {
        Reset();
        return true;
    }
    else if (first_word == "/update")
    {
        int i = 0;
    }
    else if (first_word == "+CSQ")
    {
        levelSignal = Parser::GetWord(answer, 2);
    }
    else if (answer == "SEND FAIL")
    {
        return true;
    }
    else if (answer == "SEND OK")
    {
        return true;
    }
    else if (first_word == "+IPD")
    {
        address = Parser::GetWordInQuotes(answer, 0);
        login = Parser::GetWordInQuotes(answer, 1);
        password = Parser::GetWordInQuotes(answer, 2);
        firmware = Parser::GetWordInQuotes(answer, 3);

        if (firmware.Size())
        {
            state = State::UPDATE_NEED_SAPBR;
            return true;
        }
        else
        {
            MQTT::CallbackOnReceiveData(answer);
        }
        return true;
    }
    else if (answer.c_str()[0] == '>')
    {
        return false;
    }
    else if (first_word == "OK")
    {
        return false;
    }
    else if (first_word == "ERROR")
    {
        return false;
    }
    else if (first_word == "CONNECT")
    {
        return false;
    }
    else if(first_word == "+FTPGET")
    {
        return false;
    }

    return false;
}


void SIM800::Update(const String &answer)
{
    if (ProcessUnsolicited(answer))
    {
        return;
    }

    const uint DEFAULT_TIME = 10000;

    static TimeMeterMS meter;

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        state = State::WAIT_ATE0;
        meter.Reset();
        levelSignal.Set("0");
        break;

    case State::WAIT_ATE0:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+GSMBUSY=1");
            state = State::WAIT_GSMBUSY;
            meter.Reset();
        }

        break;

    case State::WAIT_GSMBUSY:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+CREG?");
            state = State::WAIT_CREG;
            meter.Reset();
        }
        break;

    case State::WAIT_CREG:
        if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        else
        {
            if (GetWord(answer, 1) == "+CREG")
            {
                int stat = GetWord(answer, 3).c_str()[0] & 0x0f;

                if (stat == 1 ||    // Registered, home network
                    stat == 5)      // Registered, roaming
                {
                    SIM800::Transmit("AT+CIPSTATUS");
                    state = State::WAIT_IP_INITIAL;
                    meter.Reset();
                }
                else
                {
                    SIM800::Transmit("AT+CREG?");
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "INITIAL")
        {
            SIM800::Transmit("AT+CSTT=\"internet\",\"\",\"\"");
            state = State::WAIT_CSTT;
            meter.Reset();
        }
        break;

    case State::WAIT_CSTT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else
        {
            if (GetWord(answer, 1) == "OK")
            {
                state = State::WAIT_IP_START;
                meter.Reset();
                SIM800::Transmit("AT+CIPSTATUS");
            }
            else if (GetWord(answer, 1) == "ERROR")
            {
                Reset();
            }
        }
        break;

    case State::WAIT_IP_START:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "START")
        {
            state = State::WAIT_CIICR;
            meter.Reset();
            SIM800::Transmit("AT+CIICR");
        }
        break;

    case State::WAIT_CIICR:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 1) == "OK")
        {
            state = State::WAIT_IP_GPRSACT;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTATUS");
        }
        break;

    case State::WAIT_IP_GPRSACT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "GPRSACT")
        {
            state = State::WAIT_CIFSR;
            meter.Reset();
            SIM800::Transmit("AT+CIFSR");
        }
        break;

    case State::WAIT_CIFSR:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 1) != "OK")
        {
            // Здесь получаем IP-адрес
            state = State::WAIT_IP_STATUS;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTATUS");
        }
        break;

    case State::WAIT_IP_STATUS:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "STATUS")
        {
            state = State::WAIT_TCP_CONNECT;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTART=\"TCP\",\"dev.rightech.io\",\"1883\"");
        }
        break;

    case State::WAIT_TCP_CONNECT:
        if (meter.ElapsedTime() > 160000)
        {
            Reset();
        }
        else if (GetWord(answer, 1) == "ALREADY" || GetWord(answer, 2) == "OK")
        {
            state = State::WAIT_CIPHEAD;
            meter.Reset();
            SIM800::Transmit("AT+CIPHEAD=1");
        }
        else if (GetWord(answer, 2) == "FAIL")
        {
            Reset();
        }
        break;

    case State::WAIT_CIPHEAD:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            meter.Reset();
            state = State::RUNNING_MQTT;
        }
        else if (answer == "ERROR")
        {
            Reset();
        }

        break;

    case State::RUNNING_MQTT:
        MQTT::Update(answer);
        if (meterCSQ.ElapsedTime() > 5000)
        {
            meterCSQ.Reset();
            SIM800::Transmit("AT+CSQ");
        }
        break;

    case State::UPDATE_NEED_SAPBR:
        SIM800::Transmit("AT+SAPBR=1,1");
        meter.Reset();
        state = State::UPDATE_NEED_FTPCID;
        break;

    case State::UPDATE_NEED_FTPCID:
        if (meter.ElapsedTime() > 85000)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer.Size() > 2)
        {
            int i = 0;
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPCID=1");
            meter.Reset();
            state = State::UPDATE_NEED_FTPSERV;
        }
        break;

    case State::UPDATE_NEED_FTPSERV:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            char _address[64];
            std::sprintf(_address, "AT+FTPSERV=\"%s\"", address.c_str());
            SIM800::Transmit(_address);
            state = State::UPDATE_NEED_FTPPORT;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPPORT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            state = State::UPDATE_NEED_FTPUN;
            SIM800::Transmit("AT+FTPPORT=22");
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPUN:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            char _login[64];
            std::sprintf(_login, "AT+FTPUN=\"%s\"", login.c_str());
            SIM800::Transmit(_login);
            state = State::UPDATE_NEED_FTPPW;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPPW:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            char _password[64];
            std::sprintf(_password, "AT+FTPPW=\"%s\"", password.c_str());
            SIM800::Transmit(_password);
            state = State::UPDATE_NEED_FTPGETPATH;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPGETPATH:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGETPATH=\"/root/\"");
            state = State::UPDATE_NEED_FTPGETNAME;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPGETNAME:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGETNAME=\"Meter.bin\"");
            state = State::UPDATE_NEED_FTPGET;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPGET:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            state = State::RUNNING_MQTT;
        } 
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGET=1");
            state = State::UPDATE_NEED_FTPGET_BYTES;
            meter.Reset();
        }
        break;

    case State::UPDATE_NEED_FTPGET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer == "OK")
        {
            int i = 0;
        }
        else if (answer == "ERROR")
        {
            int i = 0;
        }
        else if (answer == ">")
        {
            int i = 0;
        }
        else if (answer == "+FTPGET: 1,1")
        {
            SIM800::Transmit("AT+FTPGET=2,100");
            state = State::UPDATE_GET_BYTES;
            meter.Reset();
        }
        else if (answer.Size())
        {
            int i = 0;
        }
        break;

    case State::UPDATE_GET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            state = State::RUNNING_MQTT;
        }
        if (answer.Size())
        {
            int i = 0;
        }
        if (Parser::GetWord(answer, 1) == "+FTPGET")
        {
            state = State::RUNNING_MQTT;
        }
        break;
    }
}


bool SIM800::IsRegistered()
{
    return (state >= State::WAIT_IP_INITIAL);
}


String SIM800::LevelSignal()
{
    return levelSignal;
}


void SIM800::Transmit(pchar message)
{
    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::TransmitRAW(pchar message)
{
    HAL_USART_GPRS::Transmit(message);
}


void SIM800::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::Reset()
{
    state = State::START;
    Modem::CallbackOnErrorSIM800();
    MQTT::Reset();
}
