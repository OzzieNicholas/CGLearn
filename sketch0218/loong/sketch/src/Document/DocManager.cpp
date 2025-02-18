#include "sketch/Document/DocManager.h"

#include <Windows.h>
#include <ppl.h>
#include <ppltasks.h>
#include <stdio.h>
#include <string>
#include <sysinfoapi.h>
#include <vcruntime.h>

#include "Engine/Util/Log.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/ZipUtil.h"
#include "Engine/util/FileUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/RPC/RPCManager.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewManager.h"

#include "sketch/Serializer/SketchSerializer.h"

#include <iostream>

using namespace Engine;

namespace sketch
{
DocManager::DocManager()
    : m_version{ 0, 2 }, m_pDocument(nullptr), m_pCfgMgr(nullptr), m_lastSaveFrame(0L), m_lastChangeFrame(0L), m_lastAutoSaveTm(GetTickCount64()),
      m_loading(false), m_closing(false)
{
}

DocManager::~DocManager() { this->release(); }

bool DocManager::init()
{
    Log::log("", "sketch::DocManager::init >>>>>>>>>>>>>>>>> \n");

    this->release();

    m_pCfgMgr = new ConfigureManager();
    m_pCfgMgr->init();

    m_pDocument = Document::create();
    m_pDocument->init();

    Log::log("", "sketch::DocManager::init <<<<<<<<<<<<<<<<< \n");

    return true;
}
void DocManager::release()
{
    if(m_pDocument)
    {
        delete m_pDocument;
        m_pDocument = nullptr;
    }
}

void DocManager::onAppClose()
{
    this->closeDocument();

    STCore::inst->rpcMgr()->onAppClose();
    STCore::inst->viewMgr()->onAppClose();
    STCore::inst->docMgr()->cfgMgr()->onAppClose();

    // waiting for all writing files done
    FU::wait("");
}

void DocManager::newDocument(const Engine::Path& projPath, const std::wstring& env)
{
    CmdManager* pCmdMgr = STCore::inst->cmdMgr();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    RPCManager* pRPCMgr = STCore::inst->rpcMgr();

    m_loading = true;

    m_pDocument->setEnv(env);

    m_pCfgMgr->onDocPrepare(projPath);
    m_pDocument->onDocPrepare(projPath);
    pViewMgr->onDocPrepare(projPath);
    pRPCMgr->onDocPrepare(projPath);

    m_pCfgMgr->onDocNew(projPath);
    m_pDocument->onDocNew(projPath);
    pViewMgr->onDocNew(projPath);
    pRPCMgr->onDocNew(projPath);

    m_pCfgMgr->onDocLoaded(projPath);
    m_pDocument->onDocLoaded(projPath);
    pViewMgr->onDocLoaded(projPath, L"", 0);
    pRPCMgr->onDocLoaded(projPath);

    m_pDocument->createLockFile(projPath);

    this->updateSaveFrame();
    this->updateModifyFrame();
    m_lastAutoSaveTm = GetTickCount64();

    m_loading = false;

    if(UIFuncs::onDocOpen)
    {
        UIFuncs::onDocOpen(projPath.wstr());
    }
}

void DocManager::closeDocument()
{
    m_closing = true;

    STCore::inst->viewMgr()->onDocWillClose();
    m_pCfgMgr->onDocWillClose();
    m_pCfgMgr->onDocClose();

    Path pathOld = m_pDocument ? m_pDocument->path() : "";
    if(m_pDocument)
    {
        if(!pathOld.empty())
        {
            if(UIFuncs::onDocClose)
            {
                UIFuncs::onDocClose(pathOld.wstr());
            }
        }

        m_pDocument->onDocWillClose();
        m_pDocument->onDocClose();

        delete m_pDocument;
        m_pDocument = nullptr;
    }

    m_pDocument = Document::create();
    m_pDocument->init();
    m_pDocument->addListener(STCore::inst->viewMgr());

    STCore::inst->viewMgr()->onDocClose();
    STCore::inst->rpcMgr()->onDocClose();

    // waiting for all writing files done
    FU::wait("");

    if(!pathOld.empty() && FU::hasFile(pathOld / "index"))
    {
        // FileUtil::deleteDir(pathOld);
    }

    m_closing = false;
}

void DocManager::loadDocument(const Engine::Path& projPath, const std::wstring& backup, bool readOnly, const std::wstring& progName,
                              std::function<void(bool)> onFinish)
{
    if(!FU::hasDir(projPath))
    {
        if(onFinish)
        {
            onFinish(false);
        }
        return;
    }

    auto pViewMgr = STCore::inst->viewMgr();

    int ver[2] = { 0, 0 };

    m_loading = true;

    // load index file
    {
        Path idxPath = (backup == L"load") ? projPath / "temp/auto_save/index" : projPath / "index";
        FILE* fp = _wfopen(idxPath.wstr().c_str(), L"rb");
        if(!fp)
        {
            if(onFinish)
            {
                onFinish(false);
            }
            return;
        }

        if(!FU::parseMagic(fp, "SDOC"))// check file header magic string
        {
            fclose(fp);
            if(onFinish)
            {
                onFinish(false);
            }
            return;
        }

        try
        {
            DObject root;
            root.fromFile(fp);// BINU : unzipped, BINL : lzma

            // version
            auto& verstr = root["ver"]._wstr();
            size_t dotpos = verstr.find(L'.');
            ver[0] = std::stoi(verstr.substr(0, dotpos));
            ver[1] = std::stoi(verstr.substr(dotpos + 1));

            STCore::inst->setUniqueIdCounter(root["id_counter"]._long());
            m_pDocument->modify(root["proj_info"]._dobj());
            m_pDocument->setPath(projPath);
            m_pDocument->setReadOnly(readOnly);
            fclose(fp);
        }
        catch(...)
        {
            fclose(fp);
            if(onFinish)
            {
                onFinish(false);
            }
            return;
        }
    }

    if(!progName.empty())
    {
        STCore::inst->updateUIProgress(progName, 5, L"");
    }

    // load prepare
    m_pCfgMgr->onDocPrepare(projPath);
    m_pDocument->onDocPrepare(projPath);
    pViewMgr->onDocPrepare(projPath);

    // load configs
    if(m_pCfgMgr->onDocLoad(projPath))
    {
        this->closeDocument();
        m_loading = false;
        if(onFinish)
        {
            onFinish(false);
        }
        return;
    }

    if(!progName.empty())
    {
        STCore::inst->updateUIProgress(progName, 10, L"");
    }

    std::string sketch_file = projPath.str() + "/scene.sketch";
    SketchSerializer* serializer = new SketchSerializer();
    serializer->advance_load(sketch_file);
    serializer->begin_read_object("document");
    m_pDocument->load(projPath, serializer, progName, 50.0, [=](bool bSucc) {
        serializer->end_read_object();

        // load scene
        if(!bSucc)
        {
            this->closeDocument();
            m_loading = false;
            if(onFinish)
            {
                onFinish(false);
            }
            return;
        }

        // load layouts
        serializer->begin_read_object("view");
        pViewMgr->load(*serializer, projPath);
        serializer->end_read_object();

        m_pCfgMgr->onDocLoaded(projPath);
        m_pDocument->onDocLoaded(projPath);
        pViewMgr->onDocLoaded(projPath, progName, 10);

        if(!progName.empty())
        {
            STCore::inst->updateUIProgress(progName, 100, L"");
        }

        if(backup == L"clear")// clear backup files
        {
            FU::deleteDir(projPath / "temp/auto_save");
        }

        if(!m_pDocument->readOnly())
        {
            m_pDocument->createLockFile(m_pDocument->path());
        }

        this->updateSaveFrame();
        this->updateModifyFrame();
        m_lastAutoSaveTm = GetTickCount64();

        m_loading = false;

        if(UIFuncs::onDocOpen)
        {
            UIFuncs::onDocOpen(m_pDocument->path().wstr());
        }

        if(onFinish)
        {
            onFinish(true);
        }
    });

    std::cout << "Here";
}

bool DocManager::saveIndex(const Engine::Path& projPath, bool async)
{
    if(!FU::hasDir(projPath))
    {
        FU::createDir(projPath);
    }

    DObject* pRoot = new DObject();// the ptr will be released after job done
    pRoot->set("ver", std::to_wstring(m_version[0]) + L"." + std::to_wstring(m_version[1]));
    pRoot->set("id_counter", STCore::inst->uniqueIdCounter());
    pRoot->set("modify_tm", STCore::inst->frame());

    DObject* pProjInfo = new DObject();
    m_pDocument->getProjectProps(*pProjInfo);
    pRoot->set("proj_info", pProjInfo, true);

    FU::saveAsync(projPath / "index", "SDOC", pRoot, "");
    return true;
}

bool DocManager::save(const Engine::Path& projPath, bool async, const std::wstring& progName)
{
    if(!m_pDocument || m_pDocument->readOnly())
    {
        return false;
    }

    if(projPath.empty())
    {
        return false;
    }

    if(!FU::hasDir(projPath))
    {
        FU::createDir(projPath);
    }

    if(projPath != m_pDocument->path())// save as, copy to new path
    {
        FU::copyDir(m_pDocument->path(), projPath);
    }

    SketchSerializer serializer;

    // scene file
    {
        serializer.begin_write_object("document");
        m_pDocument->save(serializer, progName, 60);
        serializer.end_write_object();

        serializer.begin_write_object("view");
        STCore::inst->viewMgr()->save(serializer, progName, 10);
        serializer.end_write_object();

        serializer.advance_save(projPath.str() + "/scene.sketch");
    }

    // configures
    m_pCfgMgr->save(projPath, progName, 10);

    // update cached keys
    STCore::inst->viewMgr()->resMgr()->updateValidKeys();

    // save index
    this->saveIndex(projPath, async);

    if(!progName.empty())
    {
        STCore::inst->updateUIProgress(progName, 100);
    }

    return true;
}

bool DocManager::saveTemp(const std::wstring& progName)
{
    if(!m_pDocument || m_pDocument->path().empty())
    {
        return false;
    }

    auto tempPath = m_pDocument->path() / "temp/auto_save";

    if(!FU::hasDir(tempPath))
    {
        FU::createDir(tempPath);
    }

    SketchSerializer serializer;
    // scene file
    {
        serializer.begin_write_object("document");
        m_pDocument->save(serializer, progName, 60);
        serializer.end_write_object();

        serializer.begin_write_object("view");
        STCore::inst->viewMgr()->save(serializer, progName, 10);
        serializer.end_write_object();

        serializer.advance_save(tempPath.str() + "/scene.sketch");
    }

    this->saveIndex(tempPath, true);

    if(!progName.empty())
    {
        STCore::inst->updateUIProgress(progName, 100);
    }

    return true;
}

void DocManager::updateSaveFrame() { m_lastSaveFrame = STCore::inst->frame(); }

void DocManager::updateModifyFrame() { m_lastChangeFrame = STCore::inst->frame(); }

void DocManager::update(unsigned long long tm)
{
    auto pCfgMgr = STCore::inst->docMgr()->cfgMgr();
    if(m_pDocument && m_pDocument->scene() && !m_pDocument->scene()->isLoading())
    {
        long long interval = pCfgMgr->settings()->autoSaveInterval() * 60LL * 1000LL;
        if(interval > 0 && (long long)(tm - m_lastAutoSaveTm) > interval)
        {
            m_lastAutoSaveTm = tm;
            if(UIFuncs::clickFuncUnit)
            {
                UIFuncs::clickFuncUnit(L"file_autosave");
            }
        }
    }
}

}// namespace sketch
