#pragma once

#include "../../Core.h"

#include "Engine/Util/DObject.h"

namespace sketch
{
class Settings;
class Options;
class LocalManager;
class ShortCutManager;
class PalleteManager;
class ColorManager;
class LayoutManager;
class HistoryManager;
class MaterialManager;

class ConfigureManager
{
public:
    ConfigureManager() = default;
    ~ConfigureManager() = default;

    bool init();
    bool onDocPrepare(const Engine::Path& projPath);
    bool onDocNew(const Engine::Path& projPath);
    bool onDocLoad(const Engine::Path& projPath);
    bool onDocLoaded(const Engine::Path& projPath);
    bool onDocWillClose();
    bool onDocClose();
    bool onAppClose();

    bool hasConfig(const std::wstring& type, const std::wstring& name) const;
    const std::unordered_map<std::wstring, Engine::DObject*>& configs(const std::wstring& type) const;
    const Engine::DObject* getConfig(const std::wstring& type, const std::wstring& name) const;
    void setConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas, bool updateFile);
    void createConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas);
    bool deleteConfig(const std::wstring& type, const std::wstring& name, bool updateFile);

    bool hasGlobalConfig(const std::wstring& type, const std::wstring& name) const;
    const std::unordered_map<std::wstring, Engine::DObject*>& globalConfigs(const std::wstring& type) const;
    const Engine::DObject* getGlobalConfig(const std::wstring& type, const std::wstring& name) const;
    void setGlobalConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas, bool updateFile);
    void createGlobalConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas);
    bool deleteGlobalConfig(const std::wstring& type, const std::wstring& name, bool updateFile);

    inline LocalManager* localMgr() const { return m_pLocalMgr; }

    inline ShortCutManager* shortCutMgr() const { return m_pShortCutMgr; }

    void save(const Engine::Path& projPath, const std::wstring& progName, float progRange);

    inline Settings* settings() const { return m_pSettings; }

    inline Options* options() const { return m_pOptions; }

    inline PalleteManager* palMgr() const { return m_pPalMgr; }

    inline ColorManager* colorMgr() { return m_pColorMgr; }

    inline LayoutManager* layoutMgr() const { return m_pLayoutMgr; }

    inline HistoryManager* historyMgr() const { return m_pHistoryMgr; }

    inline MaterialManager* mtrlMgr() const { return m_pMtrlMgr; }

protected:
    bool _loadSysCommonProps();
    bool _loadUserCommonProps(const Engine::Path& projPath);
    bool _loadGlobalCommonProps();
    void _processDirtyCfgs();

protected:
    Settings* m_pSettings = nullptr;
    Options* m_pOptions = nullptr;
    LocalManager* m_pLocalMgr = nullptr;
    ShortCutManager* m_pShortCutMgr = nullptr;
    PalleteManager* m_pPalMgr = nullptr;
    ColorManager* m_pColorMgr = nullptr;
    LayoutManager* m_pLayoutMgr = nullptr;
    HistoryManager* m_pHistoryMgr = nullptr;
    MaterialManager* m_pMtrlMgr = nullptr;

    // common props
    std::unordered_map<std::wstring, std::unordered_map<std::wstring, Engine::DObject*>> m_cfgs;
    std::unordered_map<std::wstring, std::unordered_map<std::wstring, Engine::DObject*>> m_globalCfgs;
    std::unordered_map<std::wstring, int> m_dirtyCfgs;
    std::unordered_map<std::wstring, int> m_dirtyGlobalCfgs;
};
}// namespace sketch
