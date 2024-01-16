// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common_defines.h"


#define DEVICE
#define TESTER

#define ENABLE_LOG


#include "Globals.h"

#if __ARMCLIB_VERSION < 6000000
    #define nullptr 0
#endif
