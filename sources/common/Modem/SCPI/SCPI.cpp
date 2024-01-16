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

        // "����������" �������� ������ n ���� �� ������. ��� ���� �������� ������� ���������� ������
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

        // ������� ������� ������ ������ � �� ����������
        void RemoveAngleBrackets();

        // ���������� true, ���� ����� �������� ������ symbol
        bool ConsistSymbol(char symbol, pchar *);
    };

    static StructSCPI commands[] =
    {
        { "INFO",   INFO::Process },
        { "HELLO",  INFO::Process },
        { "PHASE",  INFO::Process },
        { "ERROR",  INFO::Process },
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
