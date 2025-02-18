#pragma once

#include "../Core.h"
#include "../Define.h"

extern "C"
{
    CORE_DLL_DECLSPEC void undoFocusedCell();
    CORE_DLL_DECLSPEC void redoFocusedCell();
}
