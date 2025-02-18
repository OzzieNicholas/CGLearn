#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/DObject.h"

extern "C" CORE_DLL_DECLSPEC long long focusedCellId();
extern "C" CORE_DLL_DECLSPEC sketch::Define::LayoutCellType focusedCellType();
extern "C" CORE_DLL_DECLSPEC BOOL isFocusedCellsAllTopMost();
extern "C" CORE_DLL_DECLSPEC BOOL removeCellFromLayout(long long id);
std::unordered_set<long long> removeCellsFromLayout(long long* ids, int nIds);

