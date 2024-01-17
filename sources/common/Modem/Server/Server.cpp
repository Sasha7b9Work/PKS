// 2024/01/17 08:33:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Server/Server.h"
#include <cstdarg>


namespace Server
{
    struct State
    {
        enum E
        {
            IDLE,

        };
    };

    namespace Buffer
    {
        void Append(pchar);


    }

    // ѕризнак того, что нужен запрос на передачу данных
    static bool need_send = true;
}


void Server::Update(pchar answer)
{
    if (answer[0] == '>')
    {

    }
}


bool Server::IsConnected()
{
    return false;
}


void Server::Reset()
{

}


void Server::Send(char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);

    Buffer::Append(message);

    need_send = true;
}
