// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifdef WIN32
    #define __ARMCLIB_VERSION 5000000
#endif

#include "common_defines.h"


#define DEVICE

#define ENABLE_LOG


#define SERVER_TCP_IS_ENABLED


// Через такое время обновляются все показания
static const int TIME_UPDATE_SENSORS = 30000;

// Время между посылками
static const int TIME_BETWEEN_SENDED = 200;


#include "Globals.h"
#include "Utils/Debug.h"

#if __ARMCLIB_VERSION < 6000000
    #define nullptr 0
#endif
