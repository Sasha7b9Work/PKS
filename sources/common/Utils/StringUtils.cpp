// 2024/01/18 16:41:31 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/StringUtils.h"
#include <cstring>


char *SU::GetString(const char *data, int num, char *out)
{
    if (num == 1)
    {
        out[0] = '\0';

        uint len = std::strlen(data);

        for (uint i = 0; i < len; i++)
        {
            if (data[i] == ' ' || data[i] == 0)
            {
                out[i] = '\0';
                return out;
            }

            out[i] = data[i];
        }

        return out;
    }

    num--;

    out[0] = 0;

    uint size = std::strlen(data);

    uint pos_start = 0;

    for (uint i = 0; i < size; i++)
    {
        if (i == size - 1)
        {
            return out;
        }
        if (data[i] == ' ')
        {
            num--;

            if (num == 0)
            {
                pos_start = i + 1;
                break;
            }
        }
    }

    uint pos_end = pos_start;

    for (uint i = pos_end; i <= size; i++)
    {
        if (data[i] == ' ' || data[i] == '\0')
        {
            pos_end = i;
            break;
        }
    }

    if (pos_end > pos_start)
    {
        int index = 0;

        for (uint i = pos_start; i < pos_end; i++)
        {
            out[index++] = data[i];
        }

        out[index] = 0;
    }

    return out;
}
