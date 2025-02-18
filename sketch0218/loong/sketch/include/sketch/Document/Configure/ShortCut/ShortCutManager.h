#pragma once

#include "..\..\..\Core.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../../Define.h"

namespace Engine
{
class Path;
}

namespace sketch
{
class ShortCutManager
{
public:
    static const std::unordered_set<std::wstring> D5_VALID_SC;
    static const std::unordered_set<int> VALID_VKS;
    static const std::unordered_map<uint32_t, std::wstring> BUILTIN_SCS;
    static const uint32_t BITS_CELL_TYPE = 0xf0 << 24;
    static const uint32_t BITS_MOD = 0xf << 24;
    static const uint32_t BIT_ALT = 1 << 24;
    static const uint32_t BIT_CONTROL = 1 << 25;
    static const uint32_t BIT_SHIFT = 1 << 26;

#define SC_CELL_TYPE(sc) Define::LayoutCellType(((sc) & BITS_CELL_TYPE) >> 28)

public:
    ShortCutManager(void);
    ~ShortCutManager(void);

    bool init();

    void clearShortCuts();

    void loadSysShortCuts();
    bool loadUserShortCuts();
    void deleteUserFiles();

    bool onDocPrepare(const Engine::Path& projPath);// 场景初始化/加载前调用
    bool onDocNew(const Engine::Path& projPath);    // 初始化新场景
    bool onDocLoad(const Engine::Path& projPath);   // 加载场景
    bool onDocLoaded(const Engine::Path& projPath); // 场景初始化/加载完毕后调用
    bool onDocWillClose(void);
    bool onDocClose(void);
    bool onAppClose();

    bool onKeyDown(int key);
    bool onKeyUp(int key);

    void initShortCut(const std::wstring& fu, uint32_t sc);

    const std::unordered_map<std::wstring, uint32_t>& fu2scMap() const;
    void setShortCut(const std::wstring& fu, uint32_t sc);
    void setShortCuts(const std::vector<std::wstring>& fus, const std::vector<uint32_t> scs);

    bool importShortCuts(const Engine::Path& path, std::vector<std::wstring>& fus, std::vector<uint32_t>& scs);
    bool exportShortCuts(const Engine::Path& path, const std::vector<std::wstring>& fus, const std::vector<uint32_t>& scs);

    static uint32_t shortCut(Define::LayoutCellType cellType, char key0, char key1, char key2, bool ctrl, bool alt, bool shift);

protected:
    bool _saveUserShortCuts();

protected:
    std::unordered_map<std::wstring, uint32_t> m_initFu2sc;

    std::unordered_map<std::wstring, uint32_t> m_fu2sc;
    bool m_shortCutsDirty;

    uint32_t m_activeShortCut;
};

}// namespace sketch
