/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once


namespace nhahn
{
    #define DEBUG_LEVEL DebugLevel::DEBUG

    enum class DebugLevel {
        WARNING = 0,
        INFO = 1,
        DEBUG = 2,
        VERBOSE = 3
    };

    #ifndef NDEBUG
    # define DBG(MODULE, LEVEL, ...) debugLog(MODULE, LEVEL, __VA_ARGS__)
    # define ASSERT(EXP, ...) debugAssert(__FILE__, __LINE__, (bool)(EXP), __VA_ARGS__)
    # define FAIL(...) debugFail(__FILE__, __LINE__, __VA_ARGS__)
    #else
    # define DBG(MODULE, LEVEL, FMT, ...)
    # define ASSERT(A, B, ...)
    # define FAIL(A, ...)
    #endif

    void debugLog(const char* module, DebugLevel level, const char* format, ...);
    void debugAssert(const char* file, int line, bool exp, const char* format, ...);
    void debugFail(const char* file, int line, const char* format, ...);
}