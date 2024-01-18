// 2024/01/16 12:03:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Utils/Buffer.h"
#include "Hardware/Timer.h"
#include "Utils/RingBuffer.h"
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
            int num_after = size - n;

            RemoveFirst(n);

            std::memset(buffer + num_after, 0, (uint)n);
        }

        void SecurityClear()
        {
            while (Size() != Capacity())
            {
                Append(0);
            }

            SecurityRemoveFirst(Capacity());
        }

        // Возвращает true, если буфер содержит символ symbol
        bool ConsistSymbol(char symbol, pchar *);
    };

    static pchar ProcessUPDATE(pchar);

    static StructSCPI commands[] =
    {
        { "UPDATE", ProcessUPDATE },
        { nullptr,  nullptr }
    };

    static RingBuffer<128> ring;

    static BufferSCPI buffer;

#define SYMBOL_END 0x0A
}


void SCPI::Init()
{
    buffer.SecurityClear();
}


void SCPI::Append(char symbol)
{
    if (symbol == 0x01 || symbol == 0x0d)
    {
        return;
    }

    symbol = (char)std::toupper(symbol);

    LOG_WRITE("SCPI:%c", symbol);

    ring.Push(symbol);
}


void SCPI::Update()
{
    while (!ring.IsEmpty())
    {
        buffer.Append(ring.Pop());
    }

    pchar end = nullptr;

    if (buffer.ConsistSymbol(SYMBOL_END, &end))
    {
        LOG_WRITE("          consist");
        
        pchar result = Process(buffer.Data(), commands);

        if (result)
        {
            buffer.SecurityRemoveFirst(result - buffer.Data());
        }
    }
}


pchar SCPI::Process(pchar message, StructSCPI *_commands)
{
    // Пропускаем все символы до первого слэша
    while (*message != '/')
    {
        if (*message == SYMBOL_END)
        {
            return message + 1;
        }
        message++;
    }

    message++;

    if (*message == SYMBOL_END)
    {
        return message + 1;
    }

    LOG_WRITE("           point 1");

    for (int i = 0; ;i++)
    {
        StructSCPI &command = _commands[i];

        if (command.string)
        {
            uint length = std::strlen(command.string);

            if (std::memcmp(message, command.string, length) == 0)
            {
                LOG_WRITE("SCPI : %s", message);

                pchar pointer = message + length;

                while (*pointer == '|')
                {
                    pointer++;
                }

                return command.func(pointer);
            }
        }
        else
        {
            break;
        }
    }

    return nullptr;
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


pchar SCPI::ProcessUPDATE(pchar message)
{
    return nullptr;
}
