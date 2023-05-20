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

            NEED_SET_NAME_FIRMWARE,     // Установить имя файла с версией
            NEED_REQUEST_CONNECT,       // Запрос на соединение
            NEED_WAIT_CONNECT,          // Ждём ответа на файл версии
            GET_BYTES_VER,              // Получение байтов версии
            GET_BYTES_CRC,              // Получение байтов контрольной суммы
            GET_BYTES_FIRMWARE,         // Получение байтов прошивки

            COMPLETED                   // В этом состоянии находися, если обновление завершено или не требуется
        };
    };

    static State::E state = State::IDLE;

    static pchar address("s92153gg.beget.tech");
    static pchar login("s92153gg_1");
    static pchar password("Qwerty123!");
    static pchar directory("/");

    static pchar file_firmware("Meter.frm");

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

    static TimeMeterMS state_meter;

    static void SetState(State::E _state)
    {
        state = _state;
        state_meter.Reset();
    }

    static bool MeterIsRunning(uint time)
    {
        if (state_meter.ElapsedTime() < time)
        {
            return true;
        }

        Reset();

        return false;
    }

    static int version = 0;
    static uint source_crc = 0;         // Здесь хранится контрольная сумма из файла
    static int received_bytes = 0;      // Количество байт, считанных из файла
    static uint crc = 0;                // Подсчитанная crc

    static uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    namespace HandlerFTP
    {
        static const int SIZE_DATA_BUFFER = 64;

        bool received_command = false;          // Если true, то команда принята, принимаем байты данных
        int pointer_command = 0;
        char buffer_command[32];

        int need_bytes = 0;                     // Столько байт должно быть принято
        int bytes_from_FTP = 0;                 // Столько FTP согласен отдать
        int pointer_data = 0;                   // Столько байт уже принято
        char buffer_data[SIZE_DATA_BUFFER];

        bool requested_bytes_received = false;  // Признак того, что запрошенные байты получены
        bool received_FTPGET_1_0 = false;       // Признак того, что все байты файла получены
        
        void Reset()
        {
            pointer_command = 0;
            received_command = false;

            pointer_data = 0;
            need_bytes = 0;

            requested_bytes_received = false;
        }

        void ReceiveBytes(int num_bytes)
        {
            Reset();
            need_bytes = num_bytes;
            char buffer[32];
            sprintf(buffer, "AT+FTPGET=2,%d", need_bytes);
            SIM800::Transmit(buffer);
        }

        void AppendByte(char symbol)
        {
            if (received_command)
            {
                buffer_data[pointer_data++] = symbol;

                if (pointer_data == bytes_from_FTP)
                {
                    received_command = false;
                    pointer_command = 0;
                    requested_bytes_received = true;
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
                                pchar third_word = GetWord(buffer_command, 3);
                                if (third_word[0] == '1')
                                {
                                    char buffer[32];
                                    sprintf(buffer, "AT+FTPGET=2,%d", need_bytes);
                                    SIM800::Transmit(buffer);
                                    pointer_command = 0;
                                }
                                else
                                {
                                    received_FTPGET_1_0 = true;
                                }
                            }
                            else if (second_word[0] == '2')
                            {
                                pchar num_readed_bytes = GetWord(buffer_command, 3);

                                bytes_from_FTP = atoi(num_readed_bytes);

                                received_command = bytes_from_FTP > 0;

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

    switch (state)
    {
    case State::IDLE:
        state = State::NEED_SAPBR_3_GPRS;
        break;

    case State::NEED_SAPBR_3_GPRS:
        SetState(State::NEED_SAPBR_3_APN);
        SIM800::Transmit("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        break;

    case State::NEED_SAPBR_3_APN:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SAPBR_3_USER);
                SIM800::Transmit("AT+SAPBR=3,1,\"APN\",\"internet\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_USER:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SAPBR_3_PWD);
                SIM800::Transmit("AT+SAPBR=3,1,\"USER\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_PWD:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SAPBR_1_1);
                SIM800::Transmit("AT+SAPBR=3,1,\"PWD\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_1_1:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPCID);
                SIM800::Transmit("AT+SAPBR=1,1");
            }
        }
        break;

    case State::NEED_FTPCID:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPSERV);
                SIM800::Transmit("AT+FTPCID=1");
            }
        }
        break;

    case State::NEED_FTPSERV:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPUN);
                char _address[64];
                std::sprintf(_address, "AT+FTPSERV=\"%s\"", address);
                SIM800::Transmit(_address);
            }
        }
        break;

    case State::NEED_FTPUN:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPPW);
                char _login[64];
                std::sprintf(_login, "AT+FTPUN=\"%s\"", login);
                SIM800::Transmit(_login);
            }
        }
        break;

    case State::NEED_FTPPW:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_FTPGETPATH);
                char _password[64];
                std::sprintf(_password, "AT+FTPPW=\"%s\"", password);
                SIM800::Transmit(_password);
            }
        }
        break;

    case State::NEED_FTPGETPATH:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_SET_NAME_FIRMWARE);
                char _directory[64];
                std::sprintf(_directory, "AT+FTPGETPATH=\"%s\"", directory);
                SIM800::Transmit(_directory);
            }
        }
        break;

    case State::NEED_SET_NAME_FIRMWARE:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_REQUEST_CONNECT);
                char _firmware[64];
                std::sprintf(_firmware, "AT+FTPGETNAME=\"%s\"", file_firmware);
                SIM800::Transmit(_firmware);
            }
        }
        break;

    case State::NEED_REQUEST_CONNECT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SetState(State::NEED_WAIT_CONNECT);
                SIM800::Transmit("AT+FTPGET=1");
            }
        }
        break;

    case State::NEED_WAIT_CONNECT:
        if (MeterIsRunning(75000))
        {
            if (strcmp(Parser::GetWord(answer, 1), "+FTPGET") == 0)
            {
                if (strcmp(answer, "+FTPGET: 1,1") == 0)
                {
                    SetState(State::GET_BYTES_VER);
                    crc = 0;
                    received_bytes = 0;
                    version = 0;
                    HandlerFTP::ReceiveBytes(4);
                }
            }
        }
        break;

    case State::GET_BYTES_VER:
        if (MeterIsRunning(75000))
        {
            if (HandlerFTP::requested_bytes_received)
            {
                memcpy(&version, HandlerFTP::buffer_data, 4);

                // \todo здесь сверяем нужную версию с уже имеющейся

                SetState(State::GET_BYTES_CRC);
                HandlerFTP::ReceiveBytes(4);
            }
        }
        break;

    case State::GET_BYTES_CRC:
        if (MeterIsRunning(75000))
        {
            if (HandlerFTP::requested_bytes_received)
            {
                memcpy(&source_crc, HandlerFTP::buffer_data, 4);

                SetState(State::GET_BYTES_FIRMWARE);
                HandlerFTP::ReceiveBytes(HandlerFTP::SIZE_DATA_BUFFER);
                HandlerFTP::received_FTPGET_1_0 = false;
            }
        }
        break;

    case State::GET_BYTES_FIRMWARE:
        {
            if(MeterIsRunning(75000))
            {
                if (HandlerFTP::received_FTPGET_1_0)
                {
                    received_bytes += HandlerFTP::pointer_data;
                    received_bytes = received_bytes;
                    for (int i = 0; i < HandlerFTP::pointer_data; i++)
                    {
                        crc = Hash(crc, HandlerFTP::buffer_data[i]);
                    }

                    if (crc == source_crc)
                    {
                        int i = 0;
                    }
                    else
                    {
                        int i = 0;
                    }
                }
                else if (HandlerFTP::requested_bytes_received)
                {
                    received_bytes += HandlerFTP::pointer_data;
                    for (int i = 0; i < HandlerFTP::pointer_data; i++)
                    {
                        crc = Hash(crc, HandlerFTP::buffer_data[i]);
                    }
                    HandlerFTP::pointer_data = 0;
                    state_meter.Reset();
                    HandlerFTP::ReceiveBytes(HandlerFTP::SIZE_DATA_BUFFER);
                }
            }
        }
        break;

    case State::COMPLETED:
        break;
    }
}


void Updater::CallbackByteFromFTP(char symbol)
{
    HandlerFTP::AppendByte(symbol);
}


bool Updater::InModeReceiveDataFromFTP()
{
    return state == State::GET_BYTES_VER || state == State::GET_BYTES_CRC || state == State::GET_BYTES_FIRMWARE;
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
