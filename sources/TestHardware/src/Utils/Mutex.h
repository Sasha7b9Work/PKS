// 2022/08/29 15:56:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

//  ласс дл€ защиты данных от прерываний
class Mutex
{
public:

    Mutex() : locked(false) {}

    // ¬ызываетс€ в основном цикле перед действи€ с данными, конкурирующими с прерыванием
    void Lock()
    {
        locked = true;
    };

    // ¬ызываетс€ в основном цикле после действи€ с данными, конкурирующими с прерывани€ми
    void Unlock()
    {
        locked = false;
    };

    // «десь прерывание может проверить, зан€т ли мьютекс
    bool Locked() const
    {
        return locked;
    };

private:

    bool locked;
};
