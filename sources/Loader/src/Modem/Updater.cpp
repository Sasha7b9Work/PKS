// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Loader.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Parser.h"
#include "Modem/Updater.h"
#include "Hardware/Programmer.h"
#include "Modem/ReaderFTP.h"
#include "Modem/SIM800.h"
#include "Utils/Math.h"
#include "Display/Console.h"
#include "Globals.h"
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

        static pchar Name(E v)
        {
            static const pchar names[] =
            {
                "IDLE",
                "SAPBR_3_GPRS",
                "SAPBR_3_APN",
                "SAPBR_3_USER",
                "SAPBR_3_PWD",
                "SAPBR_1_1",
                "FTPCID",
                "FTPSERV",
                "FTPUN",
                "FTPPW",
                "FTPGETPATH",
    
                "SET_NAME_FIRMWARE",
                "REQUEST_CONNECT",
                "WAIT_CONNECT",
                "BYTES_VER",
                "BYTES_CRC",
                "BYTES_FIRMWARE",
    
                "COMPLETED"
            };

            return names[v];
        }
    };

    static State::E state = State::IDLE;

    void Reset()
    {
        state = State::IDLE;
    }

    void Update(const pchar);

    static TimeMeterMS state_meter;

#define SET_STATE(x)    SetState(x, __LINE__)

    static void SetState(State::E _state, int line)
    {
        state = _state;
        state_meter.Reset();

        char buffer[128];

        std::sprintf(buffer, "%d %s", line, State::Name(_state));

        Console::AppendLine(buffer);
    }

    static bool MeterIsRunning(uint time)
    {
        if (state_meter.ElapsedTime() < time)
        {
            return true;
        }

        SET_STATE(State::COMPLETED);

        return false;
    }

    static int version = 0;
    static uint source_crc = 0;         // Здесь хранится контрольная сумма из файла
    static uint crc = 0;                // Подсчитанная crc
}


bool Updater::IsCompleted()
{
    return state == State::COMPLETED;
}


void Updater::CallbackByteFromFTP(char symbol)
{
    ReaderFTP::AppendByte(symbol);
}


bool Updater::InModeReceiveDataFromFTP()
{
    return state == State::GET_BYTES_VER || state == State::GET_BYTES_CRC || state == State::GET_BYTES_FIRMWARE;
}


void Updater::Update(pchar answer)
{
    const uint DEFAULT_TIME = 10000;

    char buffer[32];

    switch (state)
    {
    case State::IDLE:
        SET_STATE(State::NEED_SAPBR_3_GPRS);
        break;

    case State::NEED_SAPBR_3_GPRS:
        SET_STATE(State::NEED_SAPBR_3_APN);
        SIM800::Transmit::With0D("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
        break;

    case State::NEED_SAPBR_3_APN:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_SAPBR_3_USER);
                SIM800::Transmit::With0D("AT+SAPBR=3,1,\"APN\",\"internet\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_USER:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_SAPBR_3_PWD);
                SIM800::Transmit::With0D("AT+SAPBR=3,1,\"USER\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_3_PWD:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_SAPBR_1_1);
                SIM800::Transmit::With0D("AT+SAPBR=3,1,\"PWD\",\"\"");
            }
        }
        break;

    case State::NEED_SAPBR_1_1:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_FTPCID);
                SIM800::Transmit::With0D("AT+SAPBR=1,1");
            }
        }
        break;

    case State::NEED_FTPCID:
        if (MeterIsRunning(85000))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_FTPSERV);
                SIM800::Transmit::With0D("AT+FTPCID=1");
            }
        }
        break;

    case State::NEED_FTPSERV:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_FTPUN);
                SIM800::Transmit::Format("AT+FTPSERV=\"%s\"", GL::Firmware::Address());
            }
        }
        break;

    case State::NEED_FTPUN:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_FTPPW);
                SIM800::Transmit::Format("AT+FTPUN=\"%s\"", GL::Firmware::Login());
            }
            else if (answer[0] != '\0')                 // При любом другом ответе завершаем обновление
            {
                SET_STATE(State::COMPLETED);
            }
        }
        break;

    case State::NEED_FTPPW:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_FTPGETPATH);
                SIM800::Transmit::Format("AT+FTPPW=\"%s\"", GL::Firmware::Password());
            }
            else if (answer[0] != '\0')                 // При любом другом ответе завершаем обновление
            {
                SET_STATE(State::COMPLETED);
            }
        }
        break;

    case State::NEED_FTPGETPATH:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_SET_NAME_FIRMWARE);
                SIM800::Transmit::Format("AT+FTPGETPATH=\"%s\"", GL::Firmware::Directory());
            }
            else if (answer[0] != '\0')                 // При любом другом ответе завершаем обновление
            {
                SET_STATE(State::COMPLETED);
            }
        }
        break;

    case State::NEED_SET_NAME_FIRMWARE:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_REQUEST_CONNECT);
                SIM800::Transmit::Format("AT+FTPGETNAME=\"%s\"", GL::Firmware::FileName());
            }
            else if (answer[0] != '\0')                 // При любом другом ответе завершаем обновление
            {
                SET_STATE(State::COMPLETED);
            }
        }
        break;

    case State::NEED_REQUEST_CONNECT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                SET_STATE(State::NEED_WAIT_CONNECT);
                SIM800::Transmit::With0D("AT+FTPGET=1");
            }
            else if (answer[0] != '\0')                 // При любом другом ответе завершаем обновление
            {
                SET_STATE(State::COMPLETED);
            }
        }
        break;

    case State::NEED_WAIT_CONNECT:
        if (MeterIsRunning(75000))
        {
            if (strcmp(Parser::GetWord(answer, 1, buffer), "+FTPGET") == 0)
            {
                if (strcmp(answer, "+FTPGET: 1,1") == 0)
                {
                    SET_STATE(State::GET_BYTES_VER);
                    crc = 0;
                    version = 0;
                    ReaderFTP::ReceiveBytes(4);
                }
                else if(answer[0] != '\0')              // Если нет файла на сервере  (77 == Operate error)
                {
                    SET_STATE(State::COMPLETED);
                }
            }
        }
        break;

    case State::GET_BYTES_VER:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::requested_bytes_received)
            {
                memcpy(&version, ReaderFTP::buffer_data, 4); //-V1086

                // \todo здесь сверяем нужную версию с уже имеющейся

                SET_STATE(State::GET_BYTES_CRC);
                ReaderFTP::ReceiveBytes(4);
            }
        }
        break;

    case State::GET_BYTES_CRC:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::requested_bytes_received)
            {
                memcpy(&source_crc, ReaderFTP::buffer_data, 4); //-V1086

                Programmer::Prepare(HAL_ROM::ADDR_STORAGE);

                SET_STATE(State::GET_BYTES_FIRMWARE);
                HAL_FWDGT::ToUpgradeMode();
                ReaderFTP::ReceiveBytes(ReaderFTP::SIZE_DATA_BUFFER);
                ReaderFTP::received_FTPGET_1_0 = false;
            }
        }
        break;

    case State::GET_BYTES_FIRMWARE:
        if (MeterIsRunning(75000))
        {
            if (ReaderFTP::received_FTPGET_1_0)
            {
                Programmer::WriteBytes(ReaderFTP::buffer_data, ReaderFTP::pointer_data);

                LOG_WRITE("%d recv %d", __LINE__, ReaderFTP::pointer_data);

                Console::AppendLineF("%d recv %d", __LINE__, Programmer::WrittenBytes());

                Programmer::CloseSession();

                int written_bytes = Programmer::WrittenBytes();

                crc = Math::CalculateCRC((void *)HAL_ROM::ADDR_STORAGE, written_bytes);

                if (crc == source_crc)
                {
                    do 
                    {
                        Programmer::CopyFirmware();

                    } while (source_crc != Math::CalculateCRC((void *)HAL_ROM::ADDR_APPLICATION, written_bytes));

                    SET_STATE(State::COMPLETED);
                }
                else
                {
                    SET_STATE(State::COMPLETED);
                }
            }
            else if (ReaderFTP::requested_bytes_received)
            {
                Programmer::WriteBytes(ReaderFTP::buffer_data, ReaderFTP::pointer_data);

                LOG_WRITE("%d recv %d", __LINE__, ReaderFTP::pointer_data);

                Console::AppendLineF("%d recv %d", __LINE__, Programmer::WrittenBytes());

                state_meter.Reset();
                ReaderFTP::ReceiveBytes(ReaderFTP::SIZE_DATA_BUFFER);
            }
        }
        break;

    case State::COMPLETED:
        break;
    }
}
