#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"

CORE_DLL_DECLSPEC const std::wstring& projBaseDir();
CORE_DLL_DECLSPEC const Engine::DObject* globalHistory(const std::wstring& name);
CORE_DLL_DECLSPEC const Engine::DValue* globalHistory(const std::wstring& name, const std::string& pname);
CORE_DLL_DECLSPEC const Engine::DObject* history(const std::wstring& name);
CORE_DLL_DECLSPEC const Engine::DValue* history(const std::wstring& name, const std::string& pname);
CORE_DLL_DECLSPEC BOOL saveGlobalHistory(const std::wstring& name, const Engine::DObject& props);
CORE_DLL_DECLSPEC BOOL saveHistory(const std::wstring& name, const Engine::DObject& props);


