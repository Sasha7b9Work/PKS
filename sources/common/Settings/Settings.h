// 2023/06/22 13:59:09 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Settings
{
    int GetKoeffCurrent();          // ������������ �������������� ����

    int GetNumberSteps();          // ����� �������� �������������

    bool IsControllingRelays();     // ���� true, �� �������������� ��������� ���� � ��������� ����, ���� ���� �� ���� ���� �� ��� ����������
};


extern Settings gset;
