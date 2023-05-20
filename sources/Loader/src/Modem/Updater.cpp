// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/Updater.h"
#include <gd32f30x.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>


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


using namespace Parser;
using namespace std;


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
            NEED_FTPCID,            // Находимся в состоянии обновления
            NEED_FTPSERV,           // Имя сервера
            NEED_FTPUN,             // Имя пользователя
            NEED_FTPPW,             // Пароль
            NEED_FTPGETPATH,        // Папка с файлом

            NEED_SET_NAME_VER,      // Установить имя файла с версией
            NEED_REQUEST_VER,       // Запрос на чтение файла версии
            NEED_WAIT_ANSWER_VER,   // Ждём ответа на файл версии
            GET_BYTES_VER,          // Получение файла версии

            NEED_REQUEST_CRC,       // Запрос на чтение файла контрольной суммы
            NEED_WAIT_ANSWER_CRC,   // Ждём ответа на файл контрольной суммы
            GET_BYTES_CRC,          // Получение файла контрольной суммы

            COMPLETED               // В этом состоянии находися, если обновление завершено или не требуется
        };
    };

    static State::E state = State::IDLE;

    static pchar address("s92153gg.beget.tech");
    static pchar login("s92153gg_1");
    static pchar password("Qwerty123!");
    static pchar directory("/");

    static pchar file_firmware("Meter.bin");
    static pchar file_crc("Meter.crc");
    static pchar file_ver("Meter.ver");

    void Update(const pchar);

    void LoadFirmware();

    // Сохранить часть прошивки 
    static void SaveParthFirmware(int part, uint8 data[2048]);

    static int GetSizeFirmware();

    // Получить часть прошивки
    static void GetPartFirmware(int, uint8[2048]);

    static void Reset()
    {
        state = State::IDLE;
        Modem::Reset();
    }

    namespace HandlerGetBytesFTP
    {
        static const int SIZE_DATA_BUFFER = 64;

        bool received_command = false;          // Если true, то команда принята, принимаем байты данных
        int pointer_command = 0;
        char buffer_command[32];

        int need_bytes = 0;                     // Столько байт должно быть принято
        int pointer_data = 0;                   // Столько байт уже принято
        char buffer_data[SIZE_DATA_BUFFER];

        bool received_FTPGET_1_0 = false;       // Признако того, что FTPGET 1,0 получено
        
        void Reset()
        {
            pointer_command = 0;
            received_command = false;

            pointer_data = 0;
            need_bytes = 0;

            received_FTPGET_1_0 = false;
        }

        void AppendByte(char symbol)
        {
            if (received_command)
            {
                buffer_data[pointer_data++] = symbol;

                if (pointer_data == need_bytes)
                {
                    received_command = false;
                    pointer_command = 0;
                }
            }
            else
            {
                if (symbol == 0x0d)
                {

                }
                else if (symbol == 0x0a)
                {
                    if (pointer_command)
                    {
                        buffer_command[pointer_command++] = 0;

                        pchar first_word = GetWord(buffer_command, 1);

                        if (strcmp(first_word, "+FTPGET") == 0)
                        {
                            pchar second_word = GetWord(buffer_command, 2);

                            if (second_word[0] == '1')
                            {
                                received_FTPGET_1_0 = true;
                            }
                            else if (second_word[0] == '2')
                            {
                                pchar num_readed_bytes = GetWord(buffer_command, 3);

                                need_bytes = atoi(num_readed_bytes);

                                received_command = need_bytes > 0;

                                if (!received_command)
                                {
                                    pointer_command = 0;
                                }
                            }
                        }
                        else
                        {
                            received_command = false;
                            pointer_command = 0;
                        }
                    }
                }
                else
                {
                    buffer_command[pointer_command++] = symbol;
                }
            }
        }
    }
}


bool Updater::IsCompleted()
{
    return state == State::COMPLETED;
}


void Updater::Update(pchar answer)
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
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"APN\",\"internet\"");
            state = State::NEED_SAPBR_3_USER;
            meter.Reset();
        }
        break;

    case State::NEED_SAPBR_3_USER:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"USER\",\"\"");
            state = State::NEED_SAPBR_3_PWD;
            meter.Reset();
        }
        break;

    case State::NEED_SAPBR_3_PWD:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            SIM800::Transmit("AT+SAPBR=3,1,\"PWD\",\"\"");
            state = State::NEED_SAPBR_1_1;
            meter.Reset();
        }

        break;

    case State::NEED_SAPBR_1_1:
        if (meter.ElapsedTime() > 85000)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
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
        else if (strcmp(answer, "OK") == 0)
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
        else if (strcmp(answer, "OK") == 0)
        {
            char _address[64];
            std::sprintf(_address, "AT+FTPSERV=\"%s\"", address);
            SIM800::Transmit(_address);
            state = State::NEED_FTPUN;
            meter.Reset();
        }
        break;

    case State::NEED_FTPUN:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            char _login[64];
            std::sprintf(_login, "AT+FTPUN=\"%s\"", login);
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
        else if (strcmp(answer, "OK") == 0)
        {
            char _password[64];
            std::sprintf(_password, "AT+FTPPW=\"%s\"", password);
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
        else if (strcmp(answer, "OK") == 0)
        {
            char _directory[64];
            std::sprintf(_directory, "AT+FTPGETPATH=\"%s\"", directory);
            SIM800::Transmit(_directory);
            state = State::NEED_SET_NAME_VER;
            meter.Reset();
        }
        break;

    case State::NEED_SET_NAME_VER:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            state = State::NEED_REQUEST_VER;
            char _version[64];
            std::sprintf(_version, "AT+FTPGETNAME=\"%s\"", file_ver);
            SIM800::Transmit(_version);
            meter.Reset();
        }
        break;

    case State::NEED_REQUEST_VER:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            SIM800::Transmit("AT+FTPGET=1");
            state = State::NEED_WAIT_ANSWER_VER;
            meter.Reset();
        }
        break;

    case State::NEED_WAIT_ANSWER_VER:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
        }
        else if (strcmp(Parser::GetWord(answer, 1), "+FTPGET") == 0)
        {
            if (strcmp(answer, "+FTPGET: 1,1") == 0)
            {
                state = State::GET_BYTES_VER;
                HandlerGetBytesFTP::Reset();
                SIM800::Transmit("AT+FTPGET=2,64");
            }
            else
            {
                Reset();
            }
        }
        break;

    case State::GET_BYTES_VER:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
        }
        else if (HandlerGetBytesFTP::received_FTPGET_1_0)
        {
            if (HandlerGetBytesFTP::pointer_data == HandlerGetBytesFTP::need_bytes)
            {
                char buffer[32];

                char *pointer = buffer;

                for (int i = 0; i < HandlerGetBytesFTP::pointer_data; i++)
                {
                    *pointer++ = HandlerGetBytesFTP::buffer_data[i];
                }

                pointer = '\0';

                // \todo здесь сверяем нужную версию с уже имеющейся    


                state = State::NEED_REQUEST_CRC;
                char _crc[64];
                sprintf(_crc, "AT+FTPGETNAME=\"%s\"", file_crc);
                SIM800::Transmit(_crc);
                meter.Reset();
            }
            else
            {
                SIM800::Transmit("AT+FTPGET=1");
                state = State::NEED_WAIT_ANSWER_VER;
                meter.Reset();
            }
        }

        break;

    case State::NEED_REQUEST_CRC:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            state = State::NEED_WAIT_ANSWER_CRC;
            meter.Reset();
            SIM800::Transmit("AT+FTPGET=1");
        }
        break;

    case State::NEED_WAIT_ANSWER_CRC:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
        }
        else if (strcmp(GetWord(answer, 1), "+FTPGET") == 0)
        {
            if (strcmp(answer, "+FTPGET: 1,1") == 0)
            {
                state = State::GET_BYTES_CRC;
                HandlerGetBytesFTP::Reset();
                SIM800::Transmit("AT+FTPGET=2,64");
            }
            else
            {
                Reset();
            }
        }
        break;

    case State::GET_BYTES_CRC:
        if (meter.ElapsedTime() > 75000)
        {
            Reset();
        }
        else if (HandlerGetBytesFTP::received_FTPGET_1_0)
        {
            int i = 0;
        }
        break;

    case State::COMPLETED:
        break;
    }
}


void Updater::CallbackByteFromFTP(char symbol)
{
    HandlerGetBytesFTP::AppendByte(symbol);
}


bool Updater::InModeReceiveDataFromFTP()
{
    return state == State::GET_BYTES_VER;
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
