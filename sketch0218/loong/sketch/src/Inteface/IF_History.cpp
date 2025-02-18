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
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Scene/SceneDoc.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const std::wstring& projBaseDir()
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();

	auto pCfg = pHistoryMgr->historyGlobal(L"project_base_dir");
	if (!pCfg
		|| pCfg->get("dir_arr").len() == 0) {
		return SU::NULL_WSTR;
	}

	return pCfg->get("dir_arr")._wstrArr()[0];
}

CORE_DLL_DECLSPEC const Engine::DObject* globalHistory(const std::wstring& name)
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();
	return pHistoryMgr->historyGlobal(name);
}

CORE_DLL_DECLSPEC const Engine::DValue* globalHistory(const std::wstring& name, const std::string& pname)
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();
	auto pCfg = pHistoryMgr->historyGlobal(name);
	if (!pCfg)
		return nullptr;

	auto it = pCfg->find(pname);
	if (it == pCfg->end())
		return nullptr;

	return &it->second;
}

CORE_DLL_DECLSPEC const Engine::DObject* history(const std::wstring& name)
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();
	return pHistoryMgr->history(name);
}

CORE_DLL_DECLSPEC const Engine::DValue* history(const std::wstring& name, const std::string& pname)
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();
	auto pCfg = pHistoryMgr->history(name);
	if (!pCfg)
		return nullptr;

	auto it = pCfg->find(pname);
	if (it == pCfg->end())
		return nullptr;

	return &it->second;
}

CORE_DLL_DECLSPEC BOOL saveGlobalHistory(const std::wstring& name, const Engine::DObject& props)
{
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();

	pHistoryMgr->setGlobal(name, props);
	return true;
}

CORE_DLL_DECLSPEC BOOL saveHistory(const std::wstring& name, const Engine::DObject& props)
{
	auto pDoc = STCore::inst->docMgr()->document();
	auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();
	if (pDoc->path().empty())
		return false;

	pHistoryMgr->set(name, props);
	return true;
}

