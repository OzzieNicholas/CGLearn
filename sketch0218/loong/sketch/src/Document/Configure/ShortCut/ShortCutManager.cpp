#include "sketch/Document/Configure/ShortCut/ShortCutManager.h"
#include <Windows.h>
#include <stack>
#include <unordered_set>

#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/TeklaLis.h"

#include "sketch/Document/DocManager.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/ViewLayout/LayoutCell.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
const std::unordered_set<int> ShortCutManager::VALID_VKS = {
    'A',        'B',        'C',       'D',        'E',        'F',        'G',         'H',        'I',        'G',         'K',
    'L',        'M',        'N',       'O',        'P',        'Q',        'R',         'S',        'T',        'U',         'V',
    'W',        'X',        'Y',       'Z',        '0',        '1',        '2',         '3',        '4',        '5',         '6',
    '7',        '8',        '9',       VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3,  VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6,  VK_NUMPAD7,
    VK_NUMPAD8, VK_NUMPAD9, VK_F1,     VK_F2,      VK_F3,      VK_F4,      VK_F5,       VK_F6,      VK_F7,      VK_F8,       VK_F9,
    VK_F10,     VK_F11,     VK_F12,    VK_SPACE,   VK_LEFT,    VK_RIGHT,   VK_UP,       VK_DOWN,    VK_ADD,     VK_SUBTRACT, VK_MULTIPLY,
    VK_DIVIDE,  VK_INSERT,  VK_DELETE, VK_HOME,    VK_END,     SB_PAGEUP,  SB_PAGEDOWN, VK_BACK,    VK_ESCAPE,  VK_RETURN
};

const std::unordered_set<std::wstring> ShortCutManager::D5_VALID_SC = { L"edit_del", L"edit_stop", L"edit_redo", L"edit_undo" };

const std::unordered_map<uint32_t, std::wstring> ShortCutManager::BUILTIN_SCS = {
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '0', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '1', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '2', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '3', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '4', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '5', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '6', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '7', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '8', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '9', false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD0, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD1, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD2, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD3, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD4, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD5, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD6, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD7, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD8, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD9, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_SUBTRACT, false, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '0', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '1', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '2', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '3', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '4', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '5', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '6', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '7', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '8', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, '9', true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD0, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD1, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD2, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD3, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD4, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD5, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD6, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD7, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD8, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_NUMPAD9, true, false, false), L"tool_snap_point_by_input" },
    //{ ShortCutManager::shortCut(Define::CELL_NONE, 0, 0, VK_SUBTRACT, true, false, false), L"tool_snap_point_by_input" },
};

ShortCutManager::ShortCutManager(void) : m_shortCutsDirty(false), m_activeShortCut(0u) {}

ShortCutManager::~ShortCutManager(void) {}

bool ShortCutManager::init() { return true; }

bool ShortCutManager::onDocPrepare(const Engine::Path& projPath) { return true; }

bool ShortCutManager::onDocNew(const Engine::Path& projPath) { return true; }

bool ShortCutManager::onDocLoad(const Engine::Path& projPath) { return true; }

bool ShortCutManager::onDocLoaded(const Engine::Path& projPath) { return true; }

bool ShortCutManager::onDocWillClose(void) { return true; }

bool ShortCutManager::onDocClose(void) { return true; }

bool ShortCutManager::onAppClose()
{
    if(m_shortCutsDirty)
    {
        this->_saveUserShortCuts();
        m_shortCutsDirty = false;
    }

    return true;
}

void ShortCutManager::clearShortCuts() { m_fu2sc.clear(); }

void ShortCutManager::deleteUserFiles()
{
    auto cfgPath = STCore::inst->appDataPath() / "config/shortcuts.lgst";
    FU::deleteFile(cfgPath);
}

void ShortCutManager::loadSysShortCuts()
{
    // ¼ÓÔØFUµÄ³õÊ¼¿ì½Ý¼ü
    for(auto& it: m_initFu2sc)
        m_fu2sc.insert(it);

    auto cfgPath = STCore::inst->appDataPath() / "config/shortcuts.lgst";

    FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
    if(fp == nullptr)
        return;

    if(!FileUtil::parseMagic(fp, "STSHCUT"))
    {
        fclose(fp);
        return;
    }

    DObject cfg;
    cfg.fromFile(fp);
    fclose(fp);

    for(auto& it: cfg)
    {
        auto fu = SU::s2w(it.first);
        auto sc = it.second._uint();
        if((sc & 0xffffff) == 0)
            m_fu2sc.erase(fu);
        else
            m_fu2sc[fu] = sc;
    }
}

bool ShortCutManager::loadUserShortCuts()
{
    // auto cfgPath = STCore::inst->appDataPath() / "config/shortcuts.lgst";
    //
    // FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
    // if (fp == nullptr)
    //	return false;
    //
    // if (!FileUtil::parseMagic(fp, "STSHCUT")) {
    //	fclose(fp);
    //	return false;
    // }
    //
    // DObject cfg;
    // cfg.fromFile(fp);
    // fclose(fp);
    //
    // for (auto& it : cfg) {
    //	auto fu = SU::s2w(it.first);
    //	auto sc = it.second._uint();
    //	if ((sc & 0xffffff) == 0)
    //		m_fu2sc.erase(fu);
    //	else
    //		m_fu2sc[fu] = sc;
    // }

    return true;
}

bool ShortCutManager::_saveUserShortCuts()
{
    // auto basePath = STCore::inst->appDataPath() / "config";
    // if (!FU::hasDir(basePath))
    //	FU::createDir(basePath);
    //
    // DObject cfg;
    // for (auto& it : m_fu2sc) {
    //	if ((it.second & 0xffffff) == 0)
    //		continue;
    //
    //	cfg[SU::w2s(it.first)] = it.second;
    // }
    //
    // auto cfgPath = basePath / "shortcuts.lgst";
    // if (!FU::save(cfgPath, "STSHCUT", cfg, "", true)) {
    //	Log::log("", "[Error][ShortCutManager::_saveUserShortCuts] Failed to save shortcut user config: %s \n", cfgPath.str().c_str());
    //	return false;
    // }

    return true;
}

bool ShortCutManager::importShortCuts(const Engine::Path& path, std::vector<std::wstring>& fus, std::vector<uint32_t>& scs)
{
    // FILE* fp = _wfopen(path.wstr().c_str(), L"rb");
    // if (fp == nullptr)
    //	return false;
    //
    // if (!FileUtil::parseMagic(fp, "STSHCUT")) {
    //	fclose(fp);
    //	return false;
    // }
    //
    // DObject cfg;
    // cfg.fromFile(fp);
    // fclose(fp);
    //
    // for (auto& it : cfg) {
    //	auto fu = SU::s2w(it.first);
    //	auto sc = it.second._uint();
    //	if ((sc & 0xffffff) == 0)
    //		continue;
    //
    //	fus.push_back(fu);
    //	scs.push_back(sc);
    // }
    //
    // m_shortCutsDirty = true;
    return true;
}

bool ShortCutManager::exportShortCuts(const Engine::Path& path, const std::vector<std::wstring>& fus, const std::vector<uint32_t>& scs)
{
    // DObject cfg;
    // for (int i = 0; i < fus.size(); ++i) {
    //	if ((scs[i] & 0xffffff) == 0)
    //		continue;
    //
    //	cfg[SU::w2s(fus[i])] = scs[i];
    // }
    //
    // if (!FU::save(path, "STSHCUT", cfg, "", true)) {
    //	Log::log("", "[Error][ShortCutManager::exportUserShortCuts] Failed to export shortcut config: %s \n", path.str().c_str());
    //	return false;
    // }

    return true;
}

void ShortCutManager::initShortCut(const std::wstring& fu, uint32_t sc)
{
    if(D5_VALID_SC.find(fu) == D5_VALID_SC.end())
        return;

    if((sc & 0xffffff) == 0)
        m_initFu2sc.erase(fu);
    else
        m_initFu2sc[fu] = sc;
}

void ShortCutManager::setShortCut(const std::wstring& fu, uint32_t sc)
{
    // if ((sc & 0xffffff) == 0)
    //	m_fu2sc.erase(fu);
    // else
    //	m_fu2sc[fu] = sc;
    //
    // m_shortCutsDirty = true;
}

void ShortCutManager::setShortCuts(const std::vector<std::wstring>& fus, const std::vector<uint32_t> scs)
{
    // add new scs
    for(int i = 0; i < fus.size(); ++i)
    {
        auto& fu = fus[i];
        auto sc = scs[i];
        if((sc & 0xffffff) == 0)
            m_fu2sc.erase(fu);
        else
            m_fu2sc[fu] = sc;
    }

    m_shortCutsDirty = true;
}

bool ShortCutManager::onKeyDown(int key)
{
    if(key == VK_CONTROL || key == VK_MENU || key == VK_SHIFT)
        return false;

    key &= 0xff;
    if((m_activeShortCut & 0xff) == key || ((m_activeShortCut >> 8) & 0xff) == key || ((m_activeShortCut >> 16) & 0xff) == key)// ÖØ¸´¼ü
        return false;

    m_activeShortCut = ((m_activeShortCut & 0xffff) << 8) | key;
    if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
        m_activeShortCut |= BIT_CONTROL;
    if(GetAsyncKeyState(VK_MENU) & 0x8000)
        m_activeShortCut |= BIT_ALT;
    if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
        m_activeShortCut |= BIT_SHIFT;

    auto pCell = STCore::inst->viewMgr()->layout()->focusedCell();
    if(!pCell)
    {
        for(auto& it: BUILTIN_SCS)
        {
            uint32_t sc = it.first;
            if((sc & BITS_CELL_TYPE) || m_activeShortCut != sc)
                continue;

            if(!it.second.empty())
            {
                if(UIFuncs::callShortCut)
                {
                    UIFuncs::callShortCut(it.second, sc);
                }
            }

            m_activeShortCut = 0;
            return true;
        }

        for(auto& it: m_fu2sc)
        {
            uint32_t sc = it.second;
            if((sc & BITS_CELL_TYPE) || m_activeShortCut != sc)
                continue;

            if(UIFuncs::callShortCut)
            {
                UIFuncs::callShortCut(it.first, sc);
            }
            m_activeShortCut = 0;
            return true;
        }

        return false;
    }

    std::wstring deffu;
    uint32_t defsc = 0u;

    for(auto& it: BUILTIN_SCS)
    {
        uint32_t sc = it.first;
        if((m_activeShortCut & 0xfffffff) != (sc & 0xfffffff))
            continue;

        if(SC_CELL_TYPE(sc) == pCell->cellType())
        {
            if(!it.second.empty())
            {
                if(UIFuncs::callShortCut)
                {
                    UIFuncs::callShortCut(it.second, sc);
                }
            }

            m_activeShortCut = 0;
            return true;
        }

        if((sc & BITS_CELL_TYPE) == 0)
        {
            deffu = it.second;
            defsc = sc;
        }
    }

    for(auto& it: m_fu2sc)
    {
        uint32_t sc = it.second;
        if((m_activeShortCut & 0xfffffff) != (sc & 0xfffffff))
            continue;

        if(SC_CELL_TYPE(sc) == pCell->cellType())
        {
            if(UIFuncs::callShortCut)
            {
                UIFuncs::callShortCut(it.first, sc);
            }
            m_activeShortCut = 0;
            return true;
        }

        if((sc & BITS_CELL_TYPE) == 0)
        {
            deffu = it.first;
            defsc = sc;
        }
    }

    if(defsc)
    {
        if(!deffu.empty())
        {
            if(UIFuncs::callShortCut)
            {
                UIFuncs::callShortCut(deffu, defsc);
            }
        }

        m_activeShortCut = 0;
        return true;
    }

    // ÒÑ°´ÂúÈý¸öÖ÷¼ü£¬ÈÔÎ´ÕÒµ½Æ¥Åä¿ì½Ý¼ü£¬Çå¿Õ
    if(m_activeShortCut & 0xff0000)
        m_activeShortCut = 0;

    return false;
}

bool ShortCutManager::onKeyUp(int key)
{
    if(key == VK_CONTROL || key == VK_MENU || key == VK_SHIFT)
        return false;

    m_activeShortCut = 0u;
    return true;
}

const std::unordered_map<std::wstring, uint32_t>& ShortCutManager::fu2scMap() const { return m_fu2sc; }

uint32_t ShortCutManager::shortCut(Define::LayoutCellType cellType, char key0, char key1, char key2, bool ctrl, bool alt, bool shift)
{
    if(key0 == 0 && key1 == 0 && key2 == 0)
        return 0u;

    char bytes[4] = { key2, key1, key0, ((char)cellType << 4) | (ctrl ? MOD_CONTROL : 0) | (alt ? MOD_ALT : 0) | (shift ? MOD_SHIFT : 0) };
    return *(uint32_t*)bytes;
}

}// namespace sketch