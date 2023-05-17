// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include <gd32f30x.h>
#include <cstdio>


/*
*   *** FTP-сервер ***
*   s92153gg.beget.tech/files
*   s92153gg_1
*   Qwerty123
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
            NEED_SAPBR_1_1,
            NEED_FTPCID,         // Находимся в состоянии обновления
            NEED_FTPSERV,        // Имя сервера
            NEED_FTPPORT,
            NEED_FTPUN,          // Имя пользователя
            NEED_FTPPW,          // Пароль
            NEED_FTPGETPATH,     // Папка с файлом
            NEED_FTPGETNAME,     // Имя файла
            NEED_FTPGET,         // Подключение к серверу
            NEED_FTPGET_BYTES,   // Запрос на получение данных
            GET_BYTES            // Получение данных
        };
    };

    static State::E state = State::IDLE;

    static String address;
    static String login;
    static String password;
    static String firmware;

    bool Update(const String &);

    static void Reset()
    {
        state = State::NEED_SAPBR_3_GPRS;
    }

    void JumpToBootloader();

    void LoadFirmware();

    // Сохранить часть прошивки 
    static void SaveParthFirmware(int part, uint8 data[2048]);

    static int GetSizeFirmware();

    // Получить часть прошивки
    static void GetPartFirmware(int, uint8[2048]);
}


bool Updater::Update(const String &answer)
{
//    if (Modem::ExistUpdate())
//    {
//        LoadFirmware();
//
//        JumpToBootloader();
//    }

    const uint DEFAULT_TIME = 10000;

    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        if (Parser::GetWord(answer, 1) == "+IPD")
        {
            address = Parser::GetWordInQuotes(answer, 0);
            login = Parser::GetWordInQuotes(answer, 1);
            password = Parser::GetWordInQuotes(answer, 2);
            firmware = Parser::GetWordInQuotes(answer, 3);

            if (firmware.Size())
            {
                state = State::NEED_SAPBR_3_GPRS;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
        break;

    case State::NEED_SAPBR_3_GPRS:
        SIM800::Transmit("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        meter.Reset();
        state = State::NEED_SAPBR_3_APN;
        break;

    case State::NEED_SAPBR_3_APN:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"APN\",\"internet\"");
            state = State::NEED_SAPBR_1_1;
            meter.Reset();
        }
        break;

    case State::NEED_SAPBR_1_1:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+SAPBR=1,1");
            state = State::NEED_FTPCID;
            meter.Reset();
        }
        break;

    case State::NEED_FTPCID:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPCID=1");
            meter.Reset();
            state = State::NEED_FTPSERV;
        }
        break;

    case State::NEED_FTPSERV:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
        {
            char _address[64];
            std::sprintf(_address, "AT+FTPSERV=\"%s\"", address.c_str());
            SIM800::Transmit(_address);
            state = State::NEED_FTPPORT;
            meter.Reset();
        }
        break;

    case State::NEED_FTPPORT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
        {
            state = State::NEED_FTPUN;
            SIM800::Transmit("AT+FTPPORT=21");
            meter.Reset();
        }
        break;

    case State::NEED_FTPUN:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
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
            Reset();
        }
        if (answer == "OK")
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
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGETPATH=\"/files/\"");
            state = State::NEED_FTPGETNAME;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGETNAME:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGETNAME=\"sample.txt\"");
            state = State::NEED_FTPGET;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGET:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+FTPGET=1");
            state = State::NEED_FTPGET_BYTES;
            meter.Reset();
        }
        break;

    case State::NEED_FTPGET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
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
                Reset();
            }
            meter.Reset();
        }
        break;

    case State::GET_BYTES:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
        }
        if (Parser::GetWord(answer, 1) == "+FTPGET")
        {
            state = State::IDLE;
        }
        break;
    }

    return true;
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


#ifdef GUI
#else
    //set Main Stack value
    __asm void MSR_MSP(uint)
    {
    #ifndef WIN32
        MSR MSP, r0
        BX r14
    #endif
    }
#endif


void Updater::JumpToBootloader()
{
    typedef void (*iapfun)(void);
    iapfun  jump2app;

    jump2app = (iapfun) * (volatile uint *)(HAL_ROM::ADDR_BOOTLOADER + 4); //-V566
#ifndef GUI
    MSR_MSP(*(volatile uint *)HAL_ROM::ADDR_BOOTLOADER);                        //initialize app pointer //-V566
#endif
    jump2app();                                                                 //jump to app
}
