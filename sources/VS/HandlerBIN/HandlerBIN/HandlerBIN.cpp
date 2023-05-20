// 2023/05/20 09:08:27 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <iostream>
#include <fstream>
#include <string>


using namespace std;


static unsigned int Hash(unsigned int, char);
static void ReplaceExtension(string &name, const char *ext);


int main(int argc, char *argv[])
{
    fstream input;

    ofstream out_crc;
    ofstream out_ver;

    if (argc < 2)       // Запускаем из студии
    {
        input.open("../../../Device/Meter.bin", ios_base::in | ios_base::binary);
        out_crc.open("../../../Device/Meter.crc", ios_base::out | ios_base::trunc);
        out_ver.open("../../../Device/Meter.ver", ios_base::out | ios_base::trunc);
    }
    else                // Запускаем из каталога *.bin с именем входного файла
    {
        string input_name(argv[1]);

        input.open(input_name, ios_base::in | ios_base::binary);

        string crc_name(input_name);

        ReplaceExtension(crc_name, "crc");

        out_crc.open(crc_name, ios_base::out | ios_base::trunc);

        string ver_name(input_name);

        ReplaceExtension(ver_name, "ver");

        out_ver.open(ver_name, ios_base::out | ios_base::trunc);
    }

    char byte = 0;

    unsigned int hash = 0;

    while (input.read(&byte, sizeof(byte)))
    {
        hash = Hash(hash, byte);
    }

    input.close();

    out_crc << hash;

    out_crc.close();

    ifstream input_ver;             // *.h - файл с версией ПО

    if (argc < 2)
    {
        input_ver.open("../../../common/common_defines.h", ios_base::in);
    }
    else
    {
        input_ver.open("../common/common_defines.h", ios_base::in);
    }

    string line;

    input_ver >> line;
    input_ver >> line;
    input_ver >> line;

    out_ver << line;

    out_ver.close();
}


unsigned int Hash(unsigned int hash, char byte)
{
    return (unsigned char)byte + (hash << 6) + (hash << 16) - hash;
}


void ReplaceExtension(string &name, const char *ext)
{
    for (size_t i = 0; i < name.size(); i++)
    {
        if (name[i] == '.')
        {
            while (name[name.size() - 1] != '.')
            {
                name.pop_back();
            }

            break;
        }
    }

    if (name[name.size() - 1] != '.')
    {
        name.push_back('.');
    }

    for (size_t i = 0; i < std::strlen(ext); i++)
    {
        name.push_back(ext[i]);
    }
}
