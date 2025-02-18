#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC const Engine::DObject & options();
CORE_DLL_DECLSPEC const Engine::DValue* options(const std::string& pname);
CORE_DLL_DECLSPEC void setOptions(const Engine::DObject& props);
CORE_DLL_DECLSPEC void setOptions(const std::string& pname, BOOL pval);



