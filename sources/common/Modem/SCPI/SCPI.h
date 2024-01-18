// 2024/01/16 12:03:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct StructSCPI
{
    pchar  string;
    void (*func)(pchar);       // Функция возвращает указатель на последний обработанный символ
};


namespace SCPI
{
    void Init();

    void Append(char);

    void Update();

    void Process(pchar, StructSCPI *);
}
