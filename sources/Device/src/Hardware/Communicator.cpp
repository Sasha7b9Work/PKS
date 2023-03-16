// 2022/08/29 14:00:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Communicator.h"
#include "Hardware/HAL/HAL.h"
#include <Modules/W25Q80DV/W25Q80DV.h>
#include <Modules/LIS2DH12/LIS2DH12.h>
#include <stm32f1xx_hal.h>
#include <cstring>
#include <cstdio>
#include <cctype>


namespace Communicator
{
    template<int size>
    struct Buffer
    {
        Buffer() : pointer(0)
        {
            std::memset(buffer, 0, size);
        }
        int Size() const { return pointer; }
        char *Data() { return buffer; }
        void Append(uint8 byte)
        {
            if (pointer < size)
            {
                buffer[pointer++] = std::toupper((char)byte);
            }
        }
        // Удаляет первый байт
        void RemoveFirst()
        {
            if (pointer < 2)
            {
                pointer = 0;
            }
            else
            {
                std::memmove(buffer, buffer + 1, (uint)(pointer - 1));
                pointer--;
            }
        }
        void RemoveFirstBytes(int num_bytes)
        {
            for (int i = 0; i < num_bytes; i++)
            {
                RemoveFirst();
            }
        }

    private:
        int pointer;
        char buffer[size];
    };

    static bool in_update = false;         // Установленное в true значение означает, что функци

    static Buffer<128> buffer;
}


void Communicator::AppendByte(uint8 byte)
{
    static Buffer<128> buffer_thread;       // Здесь будем хранить данные, пришедшие во время обработки пришедших данных

    if (in_update)
    {
        buffer_thread.Append(byte);
    }
    else
    {
        while (buffer_thread.Size())
        {
            buffer.Append((uint8)buffer_thread.Data()[0]);
            buffer_thread.RemoveFirst();
        }

        buffer.Append(byte);
    }
}


void Communicator::Update()
{
    in_update = true;

    static char const * const request = "#MSR\x0D\x0A";

    static const int SIZE_REQUEST = (int)std::strlen(request);

    while (buffer.Size() >= SIZE_REQUEST)
    {
        if (std::memcmp(buffer.Data(), request, (uint)SIZE_REQUEST) == 0) //-V1086
        {
            HAL_USART2_WG26::SwitchToUART();

            char message[100];

            std::sprintf(message, "OK;%02Xh;%3.1fV;%3.2fg;%3.2fg;%3.2fg;%3.1fC\x0D\x0A",
                W25Q80DV::TestValue(),
                HAL_ADC::GetVoltage(),
                LIS2DH12::GetAccelerationX().ToAccelearation(),
                LIS2DH12::GetAccelerationY().ToAccelearation(),
                LIS2DH12::GetAccelerationZ().ToAccelearation(),
                LIS2DH12::GetRawTemperature().ToTemperatrue());

            HAL_USART2_WG26::TransmitRAW(message);

            buffer.RemoveFirstBytes(SIZE_REQUEST);
        }
        else
        {
            buffer.RemoveFirst();
        }
    }

    in_update = false;
}
