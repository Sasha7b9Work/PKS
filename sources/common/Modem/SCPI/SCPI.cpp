// 2024/01/16 12:03:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Utils/Buffer.h"
#include "Hardware/Timer.h"
#include <cctype>


namespace SCPI
{
    class BufferSCPI : public Buffer<1024>
    {
    public:
        BufferSCPI() : Buffer<1024>() { }

        // "Безопасное" удаление первых n байт из буфера. При этом удалённые символы заменяются нулями
        void SecurityRemoveFirst(int n)
        {
            RemoveFirst(n);
        }

        void SecurityClear()
        {
            while (Size() != Capacity())
            {
                Append(0);
            }

            Clear();
        }

        // Удалить угловые скобки вместе с их содержимым
        void RemoveAngleBrackets();

        // Возвращает true, если буфер содержит символ symbol
        bool ConsistSymbol(char symbol, pchar *);
    };

    static StructSCPI commands[] =
    {
        { "INFO",   INFO::Info },
        { "HELLO",  INFO::Hello },
        { "PHASE",  INFO::Phase },
        { "ERROR",  INFO::Error },
        { "VER",    BASE::Ver },
        { "ADDR",   BASE::Addr },
        { "COM",    BASE::Com },
        { "TIMER",  BASE::Timer },
        { "TIME",   BASE::Time },
        { "RESET",  BASE::Reset },
        { "WDT",    BASE::Wdt },
        { "TASK",   BASE::Task },
        { "GPRS",   MODEM::Gprs },
        { "SERVER", MODEM::Server },
        { "MODEM",  MODEM::Modem },
        { "SET",    CONTROL::Set },
        { "NET",    NET::Net },
        { nullptr,  nullptr }
    };

    static BufferSCPI buffer;

    static uint prev_time = 0;
}


void SCPI::Init()
{
    buffer.SecurityClear();
}


void SCPI::Append(char symbol)
{
    if (symbol == 0x0D)
    {
        prev_time = 0;
    }
    else
    {
        buffer.Append((char)std::toupper(symbol));
    }
}


void SCPI::Update()
{
    if (TIME_MS - prev_time < 50)
    {
        return;
    }

    while (buffer.Size() != 0)
    {
        buffer.RemoveAngleBrackets();

        StructString str;
        str.string = buffer.Data();
        str.next = nullptr;

        str.ProcessCommands(commands);

        if (str.next)
        {
            buffer.SecurityRemoveFirst(str.next - buffer.Data());
        }
        else
        {
            break;
        }
    }

    buffer.SecurityRemoveFirst(buffer.Size());
}


void StructString::ProcessCommands(StructSCPI *_commands)
{
    for(int i = 0; ; i++)
    {
        StructSCPI &command = _commands[i];

        if (command.string)
        {
            if (std::memcmp(string, command.string, std::strlen(command.string)) == 0)
            {
                next = command.func(string + std::strlen(command.string));
                break;
            }
        }
        else
        {
            break;
        }
    }
}


pchar SCPI::Process(pchar message, StructSCPI *_commands)
{
    for (int i = 0; ;i++)
    {
        StructSCPI &command = _commands[i];

        if (command.string)
        {
            if (std::memcmp(message, command.string, std::strlen(command.string)) == 0)
            {
                return command.func(message + std::strlen(command.string));
            }
        }
        else
        {
            break;
        }
    }

    return nullptr;
}


void SCPI::BufferSCPI::RemoveAngleBrackets()
{
    if (buffer[0] == '<')
    {
        pchar pointer;

        if (ConsistSymbol('>', &pointer))
        {
            SecurityRemoveFirst(pointer - Data() + 1);
        }
    }
}


bool SCPI::BufferSCPI::ConsistSymbol(char symbol, pchar *pointer)
{
    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == symbol)
        {
            *pointer = &buffer[i];

            return true;
        }
    }

    *pointer = nullptr;

    return false;
}


void SCPI::SendUnsupportedCommand()
{

}
