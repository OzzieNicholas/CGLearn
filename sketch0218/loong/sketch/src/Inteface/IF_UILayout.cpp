#include "sketch/STCore.h"

#include <Windows.h>
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Math/Plane.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/View/ViewManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Layout/LayoutManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Scene/SceneDoc.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const Engine::DObject* queryUILayout(const std::wstring& name)
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->getLayout(name);
}

CORE_DLL_DECLSPEC const Engine::DObject* queryUIBar(const std::wstring& name)
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->getBar(name);
}

CORE_DLL_DECLSPEC const Engine::DObject* queryUIMiniBar(const std::wstring& name)
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->getMiniBar(name);
}

CORE_DLL_DECLSPEC const Engine::DObject* queryUIMiniBars()
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->getMiniBars();
}

extern "C" CORE_DLL_DECLSPEC BOOL saveUIUserLayouts()
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->saveUserLayouts();
}

extern "C" CORE_DLL_DECLSPEC BOOL saveUIUserBars()
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->saveUserBars();
}

CORE_DLL_DECLSPEC BOOL provisionBar(const std::wstring& name, bool builtIn, const std::wstring& tip, bool provisioned, const std::wstring& location, int x, int y, const std::vector<std::wstring>& funcUnits)
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->provisionBar(name, builtIn, tip, provisioned, location, x, y, funcUnits);
}

CORE_DLL_DECLSPEC BOOL provisionLayout(const std::wstring& name, const std::wstring& tip, const Engine::DObject* subNodes)
{
	LayoutManager* pLayoutMgr = STCore::inst->docMgr()->cfgMgr()->layoutMgr();
	return pLayoutMgr->provisionLayout(name, tip, subNodes);
}
