#include "sketch/STCore.h"

#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Common/EventPool.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
#include <Windows.h>

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const Engine::DObject* getCommonCfg(const std::wstring& type, const std::wstring& name)
{
    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    const Engine::DObject* pCfg = pCfgMgr->getConfig(type, name);

    if(pCfg == nullptr)
        return nullptr;

    return pCfg;
}

CORE_DLL_DECLSPEC void setCommonCfg(const std::wstring& type, const std::wstring& name, const Engine::DObject& cfg, bool save, bool invokeUI)
{
    STCore::inst->docMgr()->updateModifyFrame();

    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();

    pCfgMgr->setConfig(type, name, &cfg, save);

    if(invokeUI)
    {
        EventPool::instance()->push_event("cfg_update", {
                                                            { "cfg_type", type },
                                                            { "cfg_name", name },
                                                        });
    }
}

CORE_DLL_DECLSPEC const Engine::DObject* getGlobalCommonCfg(const std::wstring& type, const std::wstring& name)
{
    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    const Engine::DObject* pCfg = pCfgMgr->getGlobalConfig(type, name);

    if(pCfg == nullptr)
        return nullptr;

    return pCfg;
}

CORE_DLL_DECLSPEC void setGlobalCommonCfg(const std::wstring& type, const std::wstring& name, const Engine::DObject& cfg, bool save, bool invokeUI)
{
    STCore::inst->docMgr()->updateModifyFrame();

    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();

    pCfgMgr->setGlobalConfig(type, name, &cfg, save);

    if(invokeUI)
    {
        EventPool::instance()->push_event("global_cfg_update", {
                                                                   { "cfg_type", type },
                                                                   { "cfg_name", name },
                                                               });
    }
}

CORE_DLL_DECLSPEC bool getGlobalCommonCfgsByType(std::vector<std::wstring>& names, const std::wstring& type, bool sys)
{
    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();

    auto& cfgs = pCfgMgr->globalConfigs(type);

    for(auto it: cfgs)
    {
        if(it.first.empty())
            continue;

        if(sys || it.first.empty() || it.first[0] != L'$')
            names.push_back(it.first);
    }

    return true;
}

CORE_DLL_DECLSPEC bool getCommonCfgsByType(std::vector<std::wstring>& names, const std::wstring& type, bool sys)
{
    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();

    auto& cfgs = pCfgMgr->configs(type);

    for(auto it: cfgs)
    {
        if(it.first.empty())
            continue;

        if(sys || it.first.empty() || it.first[0] != L'$')
            names.push_back(it.first);
    }

    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL deleteCommonCfg(const wchar_t* type, const wchar_t* name, BOOL save)
{
    if(!type || !name)
        return false;

    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    pCfgMgr->deleteConfig(type, name, save);
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL deleteGlobalCommonCfg(const wchar_t* type, const wchar_t* name, BOOL save)
{
    if(!type || !name)
        return false;

    sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    pCfgMgr->deleteGlobalConfig(type, name, save);
    return true;
}