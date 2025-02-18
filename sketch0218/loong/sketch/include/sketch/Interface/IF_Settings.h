#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC const Engine::DObject & settings();
CORE_DLL_DECLSPEC const Engine::DValue* settings(const std::string& pname);
CORE_DLL_DECLSPEC void setSettings(const Engine::DObject& props);
CORE_DLL_DECLSPEC void setSettings(const std::string& pname, BOOL pval);




