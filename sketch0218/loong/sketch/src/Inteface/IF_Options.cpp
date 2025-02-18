#include "sketch/STCore.h"

#include <Windows.h>
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Math/Plane.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/View/ViewManager.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/History/HistoryManager.h"
#include "sketch/Document/Configure/Localization/LocalManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Scene/SceneDoc.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const Engine::DObject & options()
{
	sketch::ConfigureManager* pCfgMgr = STCore::inst->docMgr()->cfgMgr();
	return pCfgMgr->options()->props();
}

CORE_DLL_DECLSPEC const Engine::DValue* options(const std::string & pname)
{
	auto& opts = STCore::inst->docMgr()->cfgMgr()->options()->props();
	auto it = opts.find(pname);
	return (it != opts.end()) ? &it->second : nullptr;
}

CORE_DLL_DECLSPEC void setOptions(const std::string& pname, BOOL pval)
{
	STCore::inst->docMgr()->updateModifyFrame();

	auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();
	pOpts->modify({ {pname, (bool)pval} });
	pOpts->dirty();
	STCore::inst->viewMgr()->layout()->dirtyCells();
}

CORE_DLL_DECLSPEC void setOptions(const Engine::DObject& props)
{
	STCore::inst->docMgr()->updateModifyFrame();

	auto pOpts = STCore::inst->docMgr()->cfgMgr()->options();

	pOpts->modify(props);
	pOpts->dirty();
	STCore::inst->viewMgr()->layout()->dirtyCells();
}