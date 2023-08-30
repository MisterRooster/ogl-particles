/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Debug.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


namespace nhahn
{
    const char* levelStr[] = { "WARN", "INFO", "DEBUG", "VERBOSE"};

    void debugLog(const char* module, DebugLevel level, const char* fmt, ...)
    {
        if (level > DEBUG_LEVEL)
            return;
        va_list argp;
        va_start(argp, fmt);
        printf("%s | %-10s | ", levelStr[static_cast<int>(level)], module);
        vprintf(fmt, argp);
        va_end(argp);
        fflush(stdout);
    }

    void debugAssert(const char* file, int line, bool exp, const char* format, ...)
    {
        if (!exp)
        {
            printf("ASSERTION FAILURE:  %s:%d:  ", file, line);
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
    }

    void debugFail(const char* file, int line, const char* format, ...)
    {
        printf("PROGRAM FAILURE:  %s:%d:  ", file, line);
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}