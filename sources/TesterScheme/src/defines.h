// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common_defines.h"


#define DEVICE

#define ENABLE_LOG


// Через такое время обновляются все показания
static const int TIME_UPDATE_SENSORS = 30000;

// Время между посылками
static const int TIME_BETWEEN_SENDED = 200;


#include "Globals.h"
#include "Utils/Debug.h"
