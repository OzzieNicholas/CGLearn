#pragma once

#pragma warning( disable: 4251 4996 26812)

#include "targetver.h"
#include <Windows.h>

#ifdef CORE_DLL_EXPORTS
    #define CORE_DLL_DECLSPEC __declspec(dllexport)
#else
    #define CORE_DLL_DECLSPEC __declspec(dllimport)
#endif

#define ST_DEBUG

#define ST_MT_FOR