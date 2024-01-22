// 2024/01/16 12:03:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Utils/Buffer.h"
#include "Hardware/Timer.h"
#include "Utils/RingBuffer.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Bootloader.h"
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

        // ���������� true, ���� ����� �������� ������ symbol
        bool ConsistSymbol(char symbol, pchar *);
    };

    static void ProcessUPDATE(pchar);

    static StructSCPI commands[] =
    {
        { "update", ProcessUPDATE },
        { nullptr,  nullptr }
    };

    static RingBuffer<128> ring;

    static BufferSCPI buffer;

#define SYMBOL_END 0x00
}


void SCPI::Init()
{
    buffer.SecurityClear();
}


void SCPI::Append(char symbol)
{
    if (symbol < ' ')
    {
        symbol = 0;
    }

    ring.Push(symbol);
}


void SCPI::Update()
{
    while (!ring.IsEmpty())
    {
        buffer.Append(ring.Pop());
    }

    pchar end = nullptr;

    while (buffer.ConsistSymbol(SYMBOL_END, &end))
    {
        Process(buffer.Data(), commands);

        buffer.SecurityRemoveFirst(end - buffer.Data() + 1);
    }
}


void SCPI::Process(pchar message, StructSCPI *_commands)
{
    // ���������� ��� ������� �� ������� �����
    while (*message != '/')
    {
        if (*message == SYMBOL_END)
        {
            return;
        }
        message++;
    }

    message++;

    if (*message == SYMBOL_END)
    {
        return;
    }

    for (int i = 0; ;i++)
    {
        StructSCPI &command = _commands[i];

        if (command.string)
        {
            uint length = std::strlen(command.string);

            if (std::memcmp(message, command.string, length) == 0)
            {
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


void SCPI::ProcessUPDATE(pchar message)
{
    message += 2;

    static const int SIZE_MESSAGE = 128;

    char data[SIZE_MESSAGE];

    std::memset(data, 0, SIZE_MESSAGE);

    std::strcpy(data, message);

    data[std::strlen(message) - 1] = '\0';

    HAL_ROM::ErasePage(HAL_ROM::PAGE_UPGRADE_DATA);

    HAL_ROM::WriteData(HAL_ROM::AddressPage(HAL_ROM::PAGE_UPGRADE_DATA), data, SIZE_MESSAGE);

    Bootloader::Run();
}
