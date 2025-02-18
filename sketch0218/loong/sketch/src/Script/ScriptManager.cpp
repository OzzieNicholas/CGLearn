#include "sketch/Script/ScriptManager.h"


#include "Engine/Util/DObject.h"

namespace sketch
{
ScriptManager::ScriptManager() {}

ScriptManager::~ScriptManager() {}

Engine::DValue ScriptManager::evalExpr(const std::wstring& expr, bool bOperOverload)
{
    return Engine::DValue();
}

bool ScriptManager::init()
{
    //TODO:
    return true;
}
}// namespace sketch
