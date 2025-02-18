#include "sketch/STCore.h"

#include "Engine/Math/Plane.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "V8Wrapper/V8ContextScope.h"
#include "V8Wrapper/V8Core.h"
#include <Windows.h>

#include "sketch/Command/CmdManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/History/HistoryManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/Script/ScriptManager.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
// #include "../Script/ScriptContext.h"
// #include "../Script/ScriptObject/ScriptST.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC void queryValidDocumentEnv(std::vector<std::wstring>& envs)
{
    auto envRoot = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / L"env";
    if(!FU::hasDir(envRoot))
        return;

    std::vector<Path> dirs;
    FU::parseDirs(dirs, envRoot, false);

    for(auto& dir: dirs)
        envs.push_back(dir.file().wstr());
}

extern "C" CORE_DLL_DECLSPEC BOOL hasPath(const wchar_t* path) { return FU::hasDir(path); }

extern "C" CORE_DLL_DECLSPEC int testDocumentLockFile(const wchar_t* projPath)
{// 0: no lock file 1: locking by other run 2: crash on last run
    auto file = STCore::inst->appDataPath() / "model_locked" / SU::encodeURI(projPath) + ".locked";
    if(!FU::hasFile(file))
        return 0;

    FILE* fp = fopen(file.str().c_str(), "w");
    if(!fp)
        return 1;

    fclose(fp);
    return 2;
}

CORE_DLL_DECLSPEC std::wstring documentName() { return STCore::inst->docMgr()->document()->path().file().wstr(); }

CORE_DLL_DECLSPEC const Engine::Path& documentPath() { return STCore::inst->docMgr()->document()->path(); }

CORE_DLL_DECLSPEC bool getDocumentBaseInfosFromBasePath(const std::wstring& basePath, std::vector<std::wstring>& paths,
                                                        std::vector<long long>& modifyTms, std::vector<std::wstring>& designers,
                                                        std::vector<std::wstring>& comments)
{
    std::vector<Path> vDirs;
    FileUtil::parseDirs(vDirs, basePath, false);

    for(auto itd: vDirs)
    {
        DObject cfg;

        auto idxPath = itd / "index";
        if(FU::hasFile(idxPath))
        {
            FILE* fp = _wfopen(idxPath.wstr().c_str(), L"rb");
            if(fp && FU::parseMagic(fp, "SDOC"))
                cfg.fromFile(fp);

            if(fp)
                fclose(fp);
        }

        if(cfg.empty())
        {// try to read backup index
            auto backIdxPath = itd / "temp/auto_save/index";
            if(FU::hasFile(backIdxPath))
            {
                FILE* fp = _wfopen(backIdxPath.wstr().c_str(), L"rb");
                if(fp && FU::parseMagic(fp, "SDOC"))
                    cfg.fromFile(fp);

                if(fp)
                    fclose(fp);
            }
        }

        if(cfg.empty())
            continue;

        auto pProjInfo = cfg.getDObject("proj_info");

        paths.push_back(itd.wstr());

        long long modifyTm = 0;
        cfg.tryGet("modify_tm", modifyTm);
        modifyTms.push_back(modifyTm);

        std::wstring designer;
        pProjInfo->tryGet("designer", designer);
        designers.push_back(designer);

        std::wstring cmt;
        pProjInfo->tryGet("comment", cmt);
        comments.push_back(cmt);
    }

    return true;
}

CORE_DLL_DECLSPEC BOOL queryDocumentProjProps(Engine::DObject& projProps)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(!pDoc)
        return false;

    pDoc->getProjectProps(projProps);
    return true;
}

CORE_DLL_DECLSPEC void modifyDocument(const Engine::DObject& props) { STCore::inst->docMgr()->document()->modify(props); }

CORE_DLL_DECLSPEC Engine::DValue evalJSExpr(const std::wstring& expr)
{
    ScriptManager* pScriptMgr = STCore::inst->scriptMgr();
    return pScriptMgr->evalExpr(expr, false);
}

CORE_DLL_DECLSPEC bool runCommonScript(const std::wstring& script, bool debug, std::wstring& result, std::wstring& errMsg)
{
    // Document* pDoc = STCore::inst->docMgr()->document();
    // ScriptManager* pScriptMgr = STCore::inst->scriptMgr();

    // ScriptContext* pCtx = pScriptMgr->defCtx();
    // pCtx->reset();

    // DObject retPars;

    // V8Wrapper::V8ContextScope cs(pCtx->ctx());
    // pCtx->st()->setProject(pDoc);
    // pCtx->st()->setScene(pDoc->scene());
    // bool bSucc = pCtx->run(script, debug, &retPars, true);

    // if (debug) {
    //	result = pCtx->output();
    //	errMsg = retPars.has("err_msg") ? retPars["err_msg"]._wstr() : L"";
    // }

    ////refresh views
    // STCore::inst->viewMgr()->layout()->dirtyCells();

    ////invoke v8 gc
    // while(!V8Wrapper::V8Core::instance()->gc(300));

    // return bSucc;
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL isPathHasDocument(const wchar_t* path)
{
    if(!path || wcslen(path) == 0)
        return false;

    Path projPath = path;
    if(!FileUtil::hasDir(projPath))
        return false;

    auto idxPath = projPath / L"index";
    FU::wait(idxPath);

    FILE* fp = _wfopen(idxPath.wstr().c_str(), L"rb");
    if(!fp)
        return false;

    if(!FileUtil::parseMagic(fp, "SDOC"))
    {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

extern "C" CORE_DLL_DECLSPEC BOOL isDocumentHasNewerBackup(const wchar_t* path)
{
    if(!path || wcslen(path) == 0)
        return false;

    Path projPath = path;
    auto pathAutoSave = projPath / "temp/auto_save";
    if(!FU::hasDir(projPath) || !FU::hasFile(pathAutoSave / "index"))
        return false;

    auto tm = FU::fileModifyTm(projPath / "index");
    auto tmAutoSave = FU::fileModifyTm(pathAutoSave / "index");
    return tmAutoSave > tm;
}

extern "C" CORE_DLL_DECLSPEC BOOL isDocumentModifiedAfterLastSave()
{
    auto pDocMgr = STCore::inst->docMgr();
    if(pDocMgr->document()->path().empty())
        return false;

    return pDocMgr->lastSaveFrame() != pDocMgr->lastChangeFrame();
}

extern "C" CORE_DLL_DECLSPEC BOOL isDocumentReadOnly()
{
    auto pDocMgr = STCore::inst->docMgr();
    return pDocMgr->document()->readOnly();
}

extern "C" CORE_DLL_DECLSPEC BOOL newDocument(const wchar_t* projPath, const wchar_t* env)
{
    auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();

    if(!projPath || wcslen(projPath) == 0)
        return false;

    STCore::inst->docMgr()->closeDocument();
    if(env)
    {
        STCore::inst->docMgr()->newDocument(projPath, env);
    }
    else
    {
        STCore::inst->docMgr()->newDocument(projPath, L"");
    }

    // STCore::inst->updateUIActiveViewList();
    STCore::inst->uiOnDocNew(projPath);

    // update project history
    auto pHistory = pHistoryMgr->historyGlobal(L"project_history");
    if(!pHistory)
    {
        pHistoryMgr->setGlobal(L"project_history", {
                                                       { "proj_arr", std::vector<std::wstring>{ projPath } },
                                                   });
    }
    else
    {
        auto projVec = pHistory->get("proj_arr")._wstrList();
        auto it = std::find(projVec.begin(), projVec.end(), projPath);
        if(it != projVec.end())
            projVec.erase(it);

        projVec.push_front(projPath);
        if(projVec.size() > 10)
            projVec.pop_back();

        pHistoryMgr->setGlobal(L"project_history", {
                                                       { "proj_arr", projVec },
                                                   });
    }

    return true;
}

extern "C" CORE_DLL_DECLSPEC void openDocument(const wchar_t* projPathStr, const wchar_t* backup, BOOL readOnly, const wchar_t* progress)
{
    auto pDocMgr = STCore::inst->docMgr();
    if(!projPathStr || wcslen(projPathStr) == 0)
        return;

    std::wstring projPath = projPathStr;

    // 设计师喜欢打开同一模型视为放弃保存
    // if (pDocMgr->document()->path() == projPath)
    //	return true;

    pDocMgr->closeDocument();

    pDocMgr->loadDocument(projPath, backup, readOnly, progress, [projPath](bool bSucc) {
        auto pHistoryMgr = STCore::inst->docMgr()->cfgMgr()->historyMgr();

        // STCore::inst->updateUIActiveViewList();
        STCore::inst->uiOnDocLoaded(bSucc ? projPath : L"");

        // update project history
        auto pHistory = pHistoryMgr->historyGlobal(L"project_history");
        if(!pHistory)
        {
            pHistoryMgr->setGlobal(L"project_history", {
                                                           { "proj_arr", std::vector<std::wstring>{ projPath } },
                                                       });
        }
        else
        {
            auto projVec = pHistory->get("proj_arr")._wstrList();
            auto it = std::find(projVec.begin(), projVec.end(), projPath);
            if(it != projVec.end())
                projVec.erase(it);

            projVec.push_front(projPath);
            if(projVec.size() > 10)
                projVec.pop_back();

            pHistoryMgr->setGlobal(L"project_history", {
                                                           { "proj_arr", projVec },
                                                       });
        }
    });
}

extern "C" CORE_DLL_DECLSPEC Define::DocumentSaveResult saveDocument(BOOL async, const wchar_t* progress)
{
    Document* pDoc = STCore::inst->docMgr()->document();

    if(pDoc->readOnly())
    {
        ::MessageBox(STCore::inst->mainHandle(), L"无法修改只读模型", L"提示", MB_OK);
        return Define::DOC_SAVE_LOCKED;
    }

    bool bSucc = STCore::inst->docMgr()->save(pDoc->path(), async, progress);
    STCore::inst->docMgr()->updateSaveFrame();
    STCore::inst->docMgr()->updateModifyFrame();
    return bSucc ? Define::DOC_SAVE_SUCCESS : Define::DOC_SAVE_FAILED;
}

extern "C" CORE_DLL_DECLSPEC Define::DocumentSaveResult saveAsDocument(const wchar_t* projPath, BOOL async, const wchar_t* progress)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    if(!projPath || wcslen(projPath) == 0)
        return Define::DOC_SAVE_FAILED;

    bool bSucc = STCore::inst->docMgr()->save(projPath, async, progress);
    return bSucc ? Define::DOC_SAVE_SUCCESS : Define::DOC_SAVE_FAILED;
}

extern "C" CORE_DLL_DECLSPEC Define::DocumentSaveResult saveTempDocument(const wchar_t* progress)
{
    bool bSucc = STCore::inst->docMgr()->saveTemp(progress);
    return bSucc ? Define::DOC_SAVE_SUCCESS : Define::DOC_SAVE_FAILED;
}

extern "C" CORE_DLL_DECLSPEC BOOL packageDocument()
{
    Document* pDoc = STCore::inst->docMgr()->document();
    pDoc->package();
    return 1;
}
