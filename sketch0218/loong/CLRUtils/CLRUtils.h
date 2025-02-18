#pragma once

#pragma warning( disable: 4251 4996 26812 )

#include "targetver.h"

#ifdef	CLRUTILS_DLL_EXPORTS
	#define CLRUTILS_DLL_DECLSPEC    __declspec(dllexport)
#else
	#define CLRUTILS_DLL_DECLSPEC    __declspec(dllimport)
#endif
