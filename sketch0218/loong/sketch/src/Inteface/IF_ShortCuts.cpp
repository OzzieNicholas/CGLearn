#include "sketch/STCore.h"

#include <Windows.h>
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Math/Plane.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/View/ViewManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/ShortCut/ShortCutManager.h"
#include "sketch/Document/Scene/SceneDoc.h"

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC void initShortCuts()
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	pShortCutMgr->loadSysShortCuts();
	pShortCutMgr->loadUserShortCuts();
}

extern "C" CORE_DLL_DECLSPEC unsigned int queryShortCut(const wchar_t* fu)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	auto& fu2sc = pShortCutMgr->fu2scMap();
	auto it = fu2sc.find(fu);
	return (it != fu2sc.end()) ? it->second : 0u;
}

CORE_DLL_DECLSPEC BOOL queryShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	auto& shortCuts = pShortCutMgr->fu2scMap();
	for (auto& it : shortCuts) {
		fus.push_back(it.first);
		scs.push_back(it.second);
	}
	return true;
}

CORE_DLL_DECLSPEC BOOL queryBuiltInShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs)
{
	for (auto& it : ShortCutManager::BUILTIN_SCS) {
		fus.push_back(it.second);
		scs.push_back(it.first);
	}
	return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL initShortCut(const wchar_t* fu, sketch::Define::LayoutCellType cellType, int vk, BOOL ctrl, BOOL alt, BOOL shift)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	pShortCutMgr->initShortCut(fu, ShortCutManager::shortCut(cellType, 0, 0, vk, ctrl, alt, shift));
	return true;
}

CORE_DLL_DECLSPEC BOOL setShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	pShortCutMgr->setShortCuts(fus, scs);
	return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL setShortCut(const wchar_t* fu, sketch::Define::LayoutCellType cellType, int vk, BOOL ctrl, BOOL alt, BOOL shift)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	pShortCutMgr->setShortCut(fu, ShortCutManager::shortCut(cellType, 0, 0, vk, ctrl, alt, shift));
	return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL restoreShortCuts()
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	pShortCutMgr->clearShortCuts();
	pShortCutMgr->deleteUserFiles();
	pShortCutMgr->loadSysShortCuts();
	return true;
}

CORE_DLL_DECLSPEC BOOL importShortCuts(const std::wstring& path, std::vector<std::wstring>&fus, std::vector<uint32_t>&scs)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	return pShortCutMgr->importShortCuts(path, fus, scs);
}

CORE_DLL_DECLSPEC BOOL exportShortCuts(const std::wstring& path, const std::vector<std::wstring>&fus, const std::vector<uint32_t>&scs)
{
	ShortCutManager* pShortCutMgr = STCore::inst->docMgr()->cfgMgr()->shortCutMgr();
	return pShortCutMgr->exportShortCuts(path, fus, scs);
}

extern "C" CORE_DLL_DECLSPEC BOOL isValidShortCutKey(int vk) {
	return ShortCutManager::VALID_VKS.find(vk & 0xff) != ShortCutManager::VALID_VKS.end();
}

extern "C" CORE_DLL_DECLSPEC BOOL isBuiltInShortCut(uint32_t sc) {
	return (ShortCutManager::BUILTIN_SCS.find(sc) != ShortCutManager::BUILTIN_SCS.end())
		|| (ShortCutManager::BUILTIN_SCS.find(sc & ~ShortCutManager::BITS_CELL_TYPE) != ShortCutManager::BUILTIN_SCS.end());
}

