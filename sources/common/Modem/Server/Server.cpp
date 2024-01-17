// 2024/01/17 08:33:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Server/Server.h"
#include "Modem/SIM800.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Server
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_PERMISSION_FOR_SEND,   // ќжидаем разрешение на передачу данных
            WAIT_CONFIRM_FOR_SEND       // ќжидаем подтверждение отправки данных
        };

        static void Set(E v)
        {
            current = v;
        }

        static E Current() { return current; }

    private:

        static E current;
    };

    State::E State::current = State::IDLE;

    struct Buffer
    {
        static void Append(pchar);

        static pchar GetData()
        {
            return buffer;
        }

        static void Clear()
        {
            pointer = 0;
        }

        static int Size()
        {
            return pointer;
        }

    private:

        static const int SIZE = 512;
        static char buffer[SIZE];
        static int pointer;
    };

    int Buffer::pointer = 0;
    char Buffer::buffer[Buffer::SIZE] = { '\0' };
}


void Server::Update(pchar answer)
{
    switch (State::Current())
    {
    case State::IDLE:
        if (Buffer::Size())
        {
            SIM800::Transmit::With0D("AT+CIPSEND");

            State::Set(State::WAIT_PERMISSION_FOR_SEND);
        }
        break;

    case State::WAIT_PERMISSION_FOR_SEND:
        if (answer[0] == '>')
        {
            SIM800::Transmit::RAW(Buffer::GetData());
            SIM800::Transmit::UINT8(0x1A);
            Buffer::Clear();

            State::Set(State::WAIT_CONFIRM_FOR_SEND);
        }
        break;

    case State::WAIT_CONFIRM_FOR_SEND:
        if (std::strcmp(answer, "SEND OK") == 0)
        {
            State::Set(State::IDLE);
        }
        break;
    }
}


bool Server::IsConnected()
{
    return false;
}


void Server::Reset()
{
    Buffer::Clear();
}


void Server::Send(char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);

    std::strcat(message, "\0x0D");

    Buffer::Append(message);
}


void Server::Buffer::Append(pchar data)
{
    if (pointer + std::strlen(data) < SIZE)
    {
        std::strcat(buffer, data);

        pointer += std::strlen(data);
    }
}


void Server::SendUnsupportedCommand()
{

}
