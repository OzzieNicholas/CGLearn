#include "sketch/Document/Configure/ConfigureManager.h"

#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"

#include "sketch/Document/Configure/Color/ColorManager.h"
#include "sketch/Document/Configure/History/HistoryManager.h"
#include "sketch/Document/Configure/Layout/LayoutManager.h"
#include "sketch/Document/Configure/Localization/LocalManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Configure/ShortCut/ShortCutManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
bool ConfigureManager::init()
{
    this->_loadGlobalCommonProps();

    m_pSettings = new Settings();
    m_pSettings->init();

    m_pOptions = new Options();
    m_pOptions->init();

    m_pLayoutMgr = new LayoutManager();
    m_pLayoutMgr->init();

    m_pShortCutMgr = new ShortCutManager();
    m_pShortCutMgr->init();

    m_pColorMgr = new ColorManager();
    m_pColorMgr->init();

    m_pPalMgr = new PalleteManager();
    m_pPalMgr->init();

    m_pLocalMgr = new LocalManager();
    m_pLocalMgr->init();

    m_pHistoryMgr = new HistoryManager();
    m_pHistoryMgr->init();

    return true;
}

bool ConfigureManager::onDocPrepare(const Engine::Path& projPath) { return false; }
bool ConfigureManager::onDocNew(const Engine::Path& projPath) { return false; }
bool ConfigureManager::onDocLoad(const Engine::Path& projPath) { return false; }
bool ConfigureManager::onDocLoaded(const Engine::Path& projPath) { return false; }
bool ConfigureManager::onDocWillClose() { return false; }
bool ConfigureManager::onDocClose() { return false; }

bool ConfigureManager::onAppClose()
{
    // TODO
    return false;
}
void ConfigureManager::save(const Engine::Path& projPath, const std::wstring& progName, float progRange)
{
    // TODO
}

bool ConfigureManager::hasGlobalConfig(const std::wstring& type, const std::wstring& name) const
{
    auto itm = m_globalCfgs.find(type);
    if(itm == m_globalCfgs.end())
        return false;

    return itm->second.find(name) != itm->second.end();
}

const std::unordered_map<std::wstring, Engine::DObject*>& ConfigureManager::globalConfigs(const std::wstring& type) const
{
    static const std::unordered_map<std::wstring, Engine::DObject*> s_null;

    auto itm = m_globalCfgs.find(type);
    if(itm == m_globalCfgs.end())
        return s_null;

    return itm->second;
}

const Engine::DObject* ConfigureManager::getGlobalConfig(const std::wstring& type, const std::wstring& name) const
{
    auto itm = m_globalCfgs.find(type);
    if(itm == m_globalCfgs.end())
        return nullptr;

    auto it = itm->second.find(name);
    if(it == itm->second.end())
        return nullptr;

    return it->second;
}

void ConfigureManager::setGlobalConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas, bool updateFile)
{
    auto& map = m_globalCfgs[type];

    auto it = map.find(name);
    if(it == map.end())
    {
        auto pCfg = Engine::DObject::alloc(pParas);
        pCfg->set("cfg_type", type);
        pCfg->set("cfg_name", name);
        map[name] = pCfg;
    }
    else
    {
        it->second->copyFrom(pParas);

        // 防止传入的pParas本身带cfg_type/cfg_name被篡改
        it->second->set("cfg_type", type);
        it->second->set("cfg_name", name);
    }

    if(updateFile)
        m_dirtyGlobalCfgs[type + L'$' + name] = 1;
}

void ConfigureManager::createGlobalConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas)
{
    auto itType = m_globalCfgs.find(type);
    if(itType == m_globalCfgs.end())
        m_globalCfgs[type] = std::unordered_map<std::wstring, Engine::DObject*>();

    auto& map = m_globalCfgs[type];

    auto it = map.find(name);
    if(it != map.end())
        return;

    Engine::DObject* pCfg = Engine::DObject::alloc(pParas);
    pCfg->set("cfg_type", type);
    pCfg->set("cfg_name", name);
    map[name] = pCfg;
}

bool ConfigureManager::deleteGlobalConfig(const std::wstring& type, const std::wstring& name, bool updateFile)
{
    auto itType = m_globalCfgs.find(type);
    if(itType == m_globalCfgs.end())
        return false;

    auto& map = m_globalCfgs[type];

    auto it = map.find(name);
    if(it == map.end())
        return false;

    DObject::free(it->second);
    map.erase(it);

    if(updateFile)
        m_dirtyGlobalCfgs[type + L'$' + name] = 2;

    return true;
}

bool ConfigureManager::hasConfig(const std::wstring& type, const std::wstring& name) const
{
    auto itm = m_cfgs.find(type);
    if(itm == m_cfgs.end())
        return false;

    return itm->second.find(name) != itm->second.end();
}

const std::unordered_map<std::wstring, Engine::DObject*>& ConfigureManager::configs(const std::wstring& type) const
{
    static const std::unordered_map<std::wstring, Engine::DObject*> s_null;

    auto itm = m_cfgs.find(type);
    if(itm == m_cfgs.end())
        return s_null;

    return itm->second;
}

const Engine::DObject* ConfigureManager::getConfig(const std::wstring& type, const std::wstring& name) const
{
    auto itm = m_cfgs.find(type);
    if(itm == m_cfgs.end())
        return nullptr;

    auto it = itm->second.find(name);
    if(it == itm->second.end())
        return nullptr;

    return it->second;
}

void ConfigureManager::setConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas, bool updateFile)
{
    auto& map = m_cfgs[type];

    auto it = map.find(name);
    if(it == map.end())
    {
        auto pCfg = Engine::DObject::alloc(pParas);
        pCfg->set("cfg_type", type);
        pCfg->set("cfg_name", name);
        map[name] = pCfg;
    }
    else
    {
        it->second->copyFrom(pParas);

        // 防止传入的pParas本身带cfg_type/cfg_name被篡改
        it->second->set("cfg_type", type);
        it->second->set("cfg_name", name);
    }

    if(updateFile)
        m_dirtyCfgs[type + L'$' + name] = 1;
}

void ConfigureManager::createConfig(const std::wstring& type, const std::wstring& name, const Engine::DObject* pParas)
{
    auto itType = m_cfgs.find(type);
    if(itType == m_cfgs.end())
        m_cfgs[type] = std::unordered_map<std::wstring, Engine::DObject*>();

    auto& map = m_cfgs[type];

    auto it = map.find(name);
    if(it != map.end())
        return;

    Engine::DObject* pCfg = Engine::DObject::alloc(pParas);
    pCfg->set("cfg_type", type);
    pCfg->set("cfg_name", name);
    map[name] = pCfg;
}

bool ConfigureManager::deleteConfig(const std::wstring& type, const std::wstring& name, bool updateFile)
{
    auto itType = m_cfgs.find(type);
    if(itType == m_cfgs.end())
        return false;

    auto& map = m_cfgs[type];

    auto it = map.find(name);
    if(it == map.end())
        return false;

    DObject::free(it->second);
    map.erase(it);

    if(updateFile)
        m_dirtyCfgs[type + L'$' + name] = 2;

    return true;
}

bool ConfigureManager::_loadSysCommonProps()
{
    auto& env = STCore::inst->docMgr()->document()->env();
    auto cfgPath = m_pSettings->sysCfgFullPath() / L"env" / env / L"common";

    // Json config files
    {
        std::vector<Path> vFiles;
        FileUtil::parseFiles(vFiles, cfgPath, "json", true);

        for(auto itf = vFiles.begin(); itf != vFiles.end(); ++itf)
        {
            std::string text = FileUtil::loadTextFile(*itf);

            rapidjson::Document doc;
            doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

            if(doc.HasParseError())
            {
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: %s\n", (*itf).str().c_str());
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: [%s] %s \n",
                         std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
                         rapidjson::GetParseError_En(doc.GetParseError()));
                continue;
            }

            if(!doc.IsObject())
            {
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: %s\n", (*itf).str().c_str());
                continue;
            }

            for(auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
            {
                std::string typeName = StrUtil::trim(it->name.GetString());
                auto& type = StrUtil::s2w(typeName);
                rapidjson::Value& root = it->value;

                for(auto itt = root.MemberBegin(); itt != root.MemberEnd(); ++itt)
                {
                    std::wstring cfgName = StrUtil::s2w(StrUtil::trim(itt->name.GetString()));
                    rapidjson::Value& paras = itt->value;

                    DObject* pProps = DObject::alloc();
                    pProps->fromJson(&paras, "utf8");
                    pProps->set("cfg_type", type);
                    pProps->set("cfg_name", cfgName);

                    auto& map = m_cfgs[type];
                    auto it = map.find(cfgName);
                    if(it != map.end())
                    {
                        Log::log("", "[Warning] Duplicated %s config name: %s\n", typeName.c_str(), StrUtil::w2s(cfgName).c_str());
                        DObject::free(it->second);
                        map.erase(it);
                    }

                    map[cfgName] = pProps;
                }
            }
        }
    }

    // Binary config files.
    {
        std::vector<Path> vFiles;
        FileUtil::parseFiles(vFiles, cfgPath, "lgst", true);

        for(auto& file: vFiles)
        {
            FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
            if(!FileUtil::parseMagic(fp, "STUCG"))
                continue;

            DObject* pProps = DObject::alloc();
            pProps->fromFile(fp);
            fclose(fp);

            auto itType = pProps->find("cfg_type");
            auto itName = pProps->find("cfg_name");
            if(itType == pProps->end() || itName == pProps->end())
            {
                Log::log("", "[Error][ConfigureManager::_loadUserCommonProps] Config parse error: %s\n", file.str().c_str());
                continue;
            }

            auto& type = itType->second._wstr();
            auto& cfgName = itName->second._wstr();

            auto& typeCfgs = m_cfgs[type];
            auto it = typeCfgs.find(cfgName);
            if(it != typeCfgs.end())
            {
                DObject::free(it->second);
                typeCfgs.erase(it);
            }

            typeCfgs[cfgName] = pProps;
        }
    }

    return true;
}

bool ConfigureManager::_loadUserCommonProps(const Engine::Path& projPath)
{
    auto cfgPath = projPath / "config/common";

    // Json config files
    {
        std::vector<Path> vFiles;
        FileUtil::parseFiles(vFiles, cfgPath, "json", true);

        for(auto itf = vFiles.begin(); itf != vFiles.end(); ++itf)
        {
            std::string text = FileUtil::loadTextFile(*itf);

            rapidjson::Document doc;
            doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

            if(doc.HasParseError())
            {
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: %s\n", (*itf).str().c_str());
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: [%s] %s \n",
                         std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
                         rapidjson::GetParseError_En(doc.GetParseError()));
                continue;
            }

            if(!doc.IsObject())
            {
                Log::log("", "[Error][ConfigureManager::_loadSysCommonProps] Config parse error: %s\n", (*itf).str().c_str());
                continue;
            }

            for(auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
            {
                std::string typeName = StrUtil::trim(it->name.GetString());
                auto& type = StrUtil::s2w(typeName);
                rapidjson::Value& root = it->value;

                for(auto itt = root.MemberBegin(); itt != root.MemberEnd(); ++itt)
                {
                    std::wstring cfgName = StrUtil::s2w(StrUtil::trim(itt->name.GetString()));
                    rapidjson::Value& paras = itt->value;

                    DObject* pProps = DObject::alloc();
                    pProps->fromJson(&paras, "utf8");
                    pProps->set("cfg_type", type);
                    pProps->set("cfg_name", cfgName);

                    auto& map = m_cfgs[type];
                    auto it = map.find(cfgName);
                    if(it != map.end())
                    {
                        Log::log("", "[Warning] Duplicated %s config name: %s\n", typeName.c_str(), StrUtil::w2s(cfgName).c_str());
                        DObject::free(it->second);
                        map.erase(it);
                    }

                    map[cfgName] = pProps;
                }
            }
        }
    }

    // Binary config files
    {
        std::vector<Path> vFiles;
        FileUtil::parseFiles(vFiles, cfgPath, "lgst", true);

        for(auto& file: vFiles)
        {
            FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
            if(!FileUtil::parseMagic(fp, "STUCG"))
                continue;

            DObject* pProps = DObject::alloc();
            pProps->fromFile(fp);
            fclose(fp);

            auto itType = pProps->find("cfg_type");
            auto itName = pProps->find("cfg_name");
            if(itType == pProps->end() || itName == pProps->end())
            {
                Log::log("", "[Error][ConfigureManager::_loadUserCommonProps] Config parse error: %s\n", file.str().c_str());
                continue;
            }

            auto& type = itType->second._wstr();
            auto& cfgName = itName->second._wstr();

            auto& typeCfgs = m_cfgs[type];
            auto it = typeCfgs.find(cfgName);
            if(it != typeCfgs.end())
            {
                DObject::free(it->second);
                typeCfgs.erase(it);
            }

            typeCfgs[cfgName] = pProps;
        }
    }

    return true;
}

void ConfigureManager::_processDirtyCfgs()
{
    do
    {
        if(STCore::inst->docMgr()->document()->readOnly())
            break;

        if(m_dirtyCfgs.empty())
            break;

        auto& projPath = STCore::inst->docMgr()->document()->path();
        if(projPath.empty())
            break;

        auto cfgPath = projPath / "config/common";
        if(!FileUtil::hasDir(cfgPath))
            FileUtil::createDir(cfgPath);

        for(auto& it: m_dirtyCfgs)
        {
            auto isplit = it.first.find(L'$');
            auto type = it.first.substr(0, isplit);
            auto name = it.first.substr(isplit + 1);
            auto file = cfgPath / Path(type + L'$' + SU::encodeURI(name) + L".lgst");

            if(type == L"CFG_OBJ_FILTER_PROPS" && name == L"standard")
            {                        // 选择配置的默认配置项不存盘
                if(FU::hasFile(file))// NOTE: 向下兼容，未来可删除
                    FU::deleteFile(file);

                continue;
            }

            if(it.second == 1)
            {// modify
                auto pCfg = this->getConfig(type, name);
                if(!pCfg)
                    continue;

                FILE* fp = fopen(file.str().c_str(), "wb");
                if(!fp)
                    continue;

                fwrite("STUCG", 5, 1, fp);
                pCfg->toFile(fp, "");
                fclose(fp);
            }
            else if(it.second == 2)
            {// delete
                auto file = cfgPath / Path(type + L'$' + SU::encodeURI(name) + L".lgst");
                if(FU::hasFile(file))
                    FU::deleteFile(file);
            }
        }
        m_dirtyCfgs.clear();
    } while(false);

    do
    {
        if(STCore::inst->docMgr()->document()->readOnly())
            break;

        if(m_dirtyGlobalCfgs.empty())
            break;

        auto cfgPath = STCore::inst->appDataPath() / "config/common";
        if(!FileUtil::hasDir(cfgPath))
            FileUtil::createDir(cfgPath);

        for(auto& it: m_dirtyGlobalCfgs)
        {
            auto isplit = it.first.find(L'$');
            auto type = it.first.substr(0, isplit);
            auto name = it.first.substr(isplit + 1);
            auto file = cfgPath / Path(type + L'$' + SU::encodeURI(name) + L".lgst");

            if(it.second == 1)
            {// modify
                auto pCfg = this->getConfig(type, name);
                if(!pCfg)
                    continue;

                FILE* fp = fopen(file.str().c_str(), "wb");
                if(!fp)
                    continue;

                fwrite("STUCG", 5, 1, fp);
                pCfg->toFile(fp, "");
                fclose(fp);
            }
            else if(it.second == 2)
            {// delete
                auto file = cfgPath / Path(type + L'$' + SU::encodeURI(name) + L".lgst");
                if(FU::hasFile(file))
                    FU::deleteFile(file);
            }
        }
        m_dirtyGlobalCfgs.clear();
    } while(false);
}

bool ConfigureManager::_loadGlobalCommonProps()
{
    auto cfgPath = STCore::inst->appDataPath() / "config/common";

    std::vector<Path> vFiles;
    FileUtil::parseFiles(vFiles, cfgPath, "lgst", true);

    for(auto& file: vFiles)
    {
        FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
        if(!FU::parseMagic(fp, "STUCG"))
            continue;

        DObject* pProps = DObject::alloc();
        pProps->fromFile(fp);
        fclose(fp);

        auto itType = pProps->find("cfg_type");
        auto itName = pProps->find("cfg_name");
        if(itType == pProps->end() || itName == pProps->end())
        {
            Log::log("", "[Error][ConfigureManager::_loadGlobalCommonProps] Config parse error: %s\n", file.str().c_str());
            continue;
        }

        auto& type = itType->second._wstr();
        auto& cfgName = itName->second._wstr();

        auto& typeCfgs = m_globalCfgs[type];
        auto it = typeCfgs.find(cfgName);
        if(it != typeCfgs.end())
        {
            DObject::free(it->second);
            typeCfgs.erase(it);
        }

        typeCfgs[cfgName] = pProps;
    }

    return true;
}

}// namespace sketch
