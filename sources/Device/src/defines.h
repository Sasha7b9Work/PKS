// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifndef WIN32
    #pragma clang diagnostic ignored "-Wdeprecated-register"
    #pragma clang diagnostic ignored "-Wwritable-strings"
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
    #pragma clang diagnostic ignored "-Wpadded"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wmissing-noreturn"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wglobal-constructors"
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wformat-nonliteral"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wsign-compare"
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wdouble-promotion"
    #pragma clang diagnostic ignored "-Wunreachable-code"
#endif


#include "common_defines.h"
#include "opt.h"

#define DEVICE

#define ENABLE_LOG

#define SERVER_TCP_IS_ENABLED

#include "Globals.h"
#include "Utils/Debug.h"
