// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common_defines.h"


#define DEVICE

// Версия, которая у меня, недоработанная
#define OLD_VERSION

#define ENABLE_LOG


// Через такое время обновляются все показания
static const int TIME_UPDATE_SENSORS = 10000;

// Время между посылками
static const int TIME_BETWEEN_SENDED = 100;


#include "Globals.h"
#include "Utils/Debug.h"
