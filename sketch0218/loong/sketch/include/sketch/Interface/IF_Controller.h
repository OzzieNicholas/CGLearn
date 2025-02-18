#pragma once

#include <vector>

#include "../Core.h"
#include "../Define.h"
#include "Engine/Util/Guid.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC const std::wstring& controller(int slot);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, const std::wstring& val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, const Engine::Guid& val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, int val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, short val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, char val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, bool val0);
CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const Engine::DObject* paras);
extern "C" CORE_DLL_DECLSPEC void deactivateController(const wchar_t* name);
extern "C" CORE_DLL_DECLSPEC void popController();
CORE_DLL_DECLSPEC BOOL modifyController(const std::wstring& name, const Engine::DObject& paras);




