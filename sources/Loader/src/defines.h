// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#ifdef WIN32
    #define __ARMCLIB_VERSION 5000000
#endif
#include "common_defines.h"

#define ENABLE_LOG

//#define SERVER_TCP_IS_ENABLED

#define LOADER

#ifndef WIN32
    #if __ARMCLIB_VERSION < 6000000
        #define nullptr 0
    #endif
#endif


