// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/Updater.h"
#include <gd32f30x.h>
#include <cstdio>


/*
*   *** FTP-сервер ***
*   s92153gg.beget.tech/files
*   s92153gg_1
*   Qwerty123!
*/


/*

AT+CREG?
AT+CIPSTATUS
+INITIAL
AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"
AT+SAPBR=3,1,\"APN\",\"internet\"
AT+SAPBR=3,1,\"USER\",\"\"
AT+SAPBR=3,1,\"PWD\",\"\"
AT+SAPBR=1,1
AT+FTPCID=1
AT+FTPSERV=\"%s\"", address
AT+FTPUN=\"%s\"", login
AT+FTPPW=\"%s\"", password
AT+FTPGETPATH=\"%s\"", directory
AT+FTPGETNAME=\"%s\"", firmware
AT+FTPGET=1



*/


namespace SIM800
{
    void Transmit(pchar);
}


namespace Updater
{
    struct State
    {
        enum E
        {
            IDLE,
            NEED_SAPBR_3_GPRS,
            NEED_SAPBR_3_APN,
            NEED_SAPBR_3_USER,
            NEED_SAPBR_3_PWD,
            NEED_SAPBR_1_1,
            NEED_FTPCID,        // Находимся в состоянии обновления
            NEED_FTPSERV,       // Имя сервера
            NEED_FTPPORT,
            NEED_FTPUN,         // Имя пользователя
            NEED_FTPPW,         // Пароль
            NEED_FTPGETPATH,    // Папка с файлом
            NEED_FTPGETNAME,    // Имя файла
            NEED_FTPGET,        // Подключение к серверу
            NEED_FTPGET_BYTES,  // Запрос на получение данных
            GET_BYTES,          // Получение данных
            COMPLETED           // В этом состоянии находися, если обновление завершено или не требуется
        };
    };

    static State::E state = State::IDLE;

    static String address("s92153gg.beget.tech");
    static String login("s92153gg_1");
    static String password("Qwerty123!");
    static String firmware("sample.txt");
    static String directory("/");

    void Update(const String &);

    void LoadFirmware();

    // Сохранить часть прошивки 
    static void SaveParthFirmware(int part, uint8 data[2048]);

    static int GetSizeFirmware();

    // Получить часть прошивки
    static void GetPartFirmware(int, uint8[2048]);
}


bool Updater::IsCompleted()
{
    return state == State::COMPLETED;
}


void Updater::Update(const String &answer)
{
    const uint DEFAULT_TIME = 10000;

    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        state = State::NEED_SAPBR_3_GPRS;
        break;

    case State::NEED_SAPBR_3_GPRS:
        SIM800::Transmit("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        meter.Reset();
        state = State::NEED_SAPBR_3_APN;
        break;

    case State::NEED_SAPBR_3_APN:
        if (meter.ElapsedTime() > 85000)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"APN\",\"internet\"");
            state = State::NEED_SAPBR_3_USER;
            meter.Reset();
        }
        break;

    case State::NEED_SAPBR_3_USER:
        if (meter.ElapsedTime() > 85000)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"USER\",\"\"");
            state = State::NEED_SAPBR_3_PWD;
            meter.Reset();
        }
        break;

    case State::NEED_SAPBR_3_PWD:
        if (meter.ElapsedTime() > 85000)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"PWD\",\"\"");
            state = State::NEED_SAPBR_1_1;
            meter.Reset();
        }

        break;

    case State::NEED_SAPBR_1_1:
        if (meter.ElapsedTime() > 85000)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=1,1");
            state = State::NEED_FTPCID;
            meter.Reset();
        }
        break;

    case State::NEED_FTPCID:
        if (meter.ElapsedTime() > 85000)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPCID=1");
            meter.Reset();
            state = State::NEED_FTPSERV;
        }
        break;

    case State::NEED_FTPSERV:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            char _address[64];
            std::sprintf(_address, "AT+FTPSERV=\"%s\"", address.c_str());
            SIM800::Transmit(_address);
            state = State::NEED_FTPUN;
            meter.Reset();
        }
        break;

    case State::NEED_FTPPORT:
//        if (meter.ElapsedTime() > DEFAULT_TIME)
//        {
//            Reset(meter);
//        }
//        else if (answer == "OK")
//        {
//            state = State::NEED_FTPUN;
//            SIM800::Transmit("AT+FTPPORT=21");
//            meter.Reset();
//        }
        break;

    case State::NEED_FTPUN:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            char _login[64];
            std::sprintf(_login, "AT+FTPUN=\"%s\"", login.c_str());
            SIM800::Transmit(_login);
            state = State::NEED_FTPPW;
            meter.Reset();
        }
        break;

    case State::NEED_FTPPW:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            char _password[64];
            std::sprintf(_password, "AT+FTPPW=\"%s\"", password.c_str());
            SIM800::Transmit(_password);
            state = State::NEED_FTPGETPATH;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGETPATH:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            char _directory[64];
            std::sprintf(_directory, "AT+FTPGETPATH=\"%s\"", directory.c_str());
            SIM800::Transmit(_directory);
            state = State::NEED_FTPGETNAME;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGETNAME:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            char _firmware[64];
            std::sprintf(_firmware, "AT+FTPGETNAME=\"%s\"", firmware.c_str());
            SIM800::Transmit(_firmware);
            state = State::NEED_FTPGET;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGET:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Modem::Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGET=1");
            state = State::NEED_FTPGET_BYTES;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            Modem::Reset();
        }
        else if (Parser::GetWord(answer, 1) == "+FTPGET")
        {
            if (answer == "+FTPGET: 1,1")
            {
                SIM800::Transmit("AT+FTPGET=2,100");
                state = State::GET_BYTES;
            }
            else
            {
                Modem::Reset();
            }
            meter.Reset();
        }
        break;

    case State::GET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            Modem::Reset();
        }
        else if (Parser::GetWord(answer, 1) == "+FTPGET")
        {
            state = State::IDLE;
        }
        break;

    case State::COMPLETED:
        break;
    }
}


int Updater::GetSizeFirmware()
{
    return 0;
}


void Updater::GetPartFirmware(int, uint8[2048])
{

}


void Updater::SaveParthFirmware(int part, uint8 data[2048])
{
    HAL_ROM::ErasePage(part + 50);

    HAL_ROM::WriteData(HAL_ROM::ADDR_SAVED_FIRMWARE + part * HAL_ROM::SIZE_PAGE, data, 2048);
}


void Updater::LoadFirmware()
{
    int size = GetSizeFirmware();

    uint8 data[2048];

    int part = 0;

    while (size > 0)
    {
        GetPartFirmware(part, data);

        SaveParthFirmware(part, data);

        size -= 2048;

        part++;
    }
}
