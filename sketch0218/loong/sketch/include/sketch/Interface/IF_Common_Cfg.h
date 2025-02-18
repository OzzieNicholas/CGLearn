#pragma once

#include <vector>
#include <string>

#include "../Core.h"
#include "../Define.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC const Engine::DObject* getCommonCfg(const std::wstring& type, const std::wstring& name);
CORE_DLL_DECLSPEC void setCommonCfg(const std::wstring& type, const std::wstring& name, const Engine::DObject& cfg, bool save, bool invokeUI);
CORE_DLL_DECLSPEC bool getCommonCfgsByType(std::vector<std::wstring>& names, const std::wstring& type, bool sys);
extern "C" CORE_DLL_DECLSPEC BOOL deleteCommonCfg(const wchar_t* type, const wchar_t* name, BOOL save);

CORE_DLL_DECLSPEC const Engine::DObject* getGlobalCommonCfg(const std::wstring& type, const std::wstring& name);
CORE_DLL_DECLSPEC void setGlobalCommonCfg(const std::wstring& type, const std::wstring& name, const Engine::DObject& cfg, bool save, bool invokeUI);
CORE_DLL_DECLSPEC bool getGlobalCommonCfgsByType(std::vector<std::wstring>& names, const std::wstring& type, bool sys);
extern "C" CORE_DLL_DECLSPEC BOOL deleteGlobalCommonCfg(const wchar_t* type, const wchar_t* name, BOOL save);


