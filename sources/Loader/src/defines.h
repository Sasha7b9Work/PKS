// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "common_defines.h"


#define ENABLE_LOG

#define LOADER

#if __ARMCLIB_VERSION < 6000000
    #define nullptr 0
#endif
