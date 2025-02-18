#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC const Engine::DObject* queryUILayout(const std::wstring& name);
CORE_DLL_DECLSPEC const Engine::DObject* queryUIBar(const std::wstring& name);
CORE_DLL_DECLSPEC const Engine::DObject* queryUIMiniBar(const std::wstring& name);
CORE_DLL_DECLSPEC const Engine::DObject* queryUIMiniBars();
CORE_DLL_DECLSPEC BOOL provisionBar(const std::wstring& name, bool builtIn, const std::wstring& tip, 
									bool provisioned, const std::wstring& location, int x, int y, 
									const std::vector<std::wstring>& funcUnits);
CORE_DLL_DECLSPEC BOOL provisionLayout(const std::wstring& name, const std::wstring& tip, const Engine::DObject* subNodes);

extern "C" CORE_DLL_DECLSPEC BOOL saveUIUserLayouts();
extern "C" CORE_DLL_DECLSPEC BOOL saveUIUserBars();
