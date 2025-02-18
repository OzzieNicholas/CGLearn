#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/DObject.h"

//因为FU需要先初始化默认快捷键，所以shortcut不能在STCore::init中加载
//需要手工调用此接口加载
extern "C" CORE_DLL_DECLSPEC void initShortCuts();

extern "C" CORE_DLL_DECLSPEC unsigned int queryShortCut(const wchar_t* fu);
CORE_DLL_DECLSPEC BOOL queryShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs);
CORE_DLL_DECLSPEC BOOL queryBuiltInShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs);
extern "C" CORE_DLL_DECLSPEC BOOL initShortCut(const wchar_t* fu, sketch::Define::LayoutCellType cellType, int vk, BOOL ctrl, BOOL alt, BOOL shift);
CORE_DLL_DECLSPEC BOOL setShortCuts(std::vector<std::wstring>& fus, std::vector<uint32_t>& scs);
extern "C" CORE_DLL_DECLSPEC BOOL setShortCut(const wchar_t* fu, sketch::Define::LayoutCellType cellType, int vk, BOOL ctrl, BOOL alt, BOOL shift);
extern "C" CORE_DLL_DECLSPEC BOOL restoreShortCuts();

CORE_DLL_DECLSPEC BOOL importShortCuts(const std::wstring& path, std::vector<std::wstring>&fus, std::vector<uint32_t>&scs);
CORE_DLL_DECLSPEC BOOL exportShortCuts(const std::wstring& path, const std::vector<std::wstring>&fus, const std::vector<uint32_t>&scs);

extern "C" CORE_DLL_DECLSPEC BOOL isValidShortCutKey(int vk);
extern "C" CORE_DLL_DECLSPEC BOOL isBuiltInShortCut(uint32_t sc);




