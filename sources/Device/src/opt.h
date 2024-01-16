// 2023/11/29 14:46:26 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// Включить отлючение контакторов при неисправности одного из них
//#define ENABLE_CONTROL_CONTACTORS

// Если определено, то данные сохраняеют во внутренней памяти. При наличии сети передаются на сервер
#define ENABLE_STORAGE

// Через такое время обновляются все показания
static const int TIME_UPDATE_SENSORS = 30000;

// Время между посылками
static const int TIME_BETWEEN_SENDED = 200;


#define SERVER_IP   "power.avkpanel.ru"
#define SERVER_PORT 1620
