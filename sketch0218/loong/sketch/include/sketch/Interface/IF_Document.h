#pragma once

#include "../Core.h"
#include <vector>
#include <string>
#include "../Define.h"
#include "Engine/Util/Path.h"

CORE_DLL_DECLSPEC void queryValidDocumentEnv(std::vector<std::wstring>& envs);

extern "C" CORE_DLL_DECLSPEC BOOL hasPath(const wchar_t* path);
extern "C" CORE_DLL_DECLSPEC int testDocumentLockFile(const wchar_t* projPath); //0: no lock file 1: locking by other run 2: crash on last run

CORE_DLL_DECLSPEC std::wstring documentName();
CORE_DLL_DECLSPEC const Engine::Path& documentPath();
CORE_DLL_DECLSPEC BOOL queryDocumentProjProps(Engine::DObject& projProps);
CORE_DLL_DECLSPEC void modifyDocument(const Engine::DObject& props);
CORE_DLL_DECLSPEC bool getDocumentBaseInfosFromBasePath(const std::wstring& basePath,
														std::vector<std::wstring>& paths,
														std::vector<long long>& modifyTms,
														std::vector<std::wstring>& designers,
														std::vector<std::wstring>& comments);
CORE_DLL_DECLSPEC bool runCommonScript(const std::wstring& script, bool debug, std::wstring& result, std::wstring& errMsg);
CORE_DLL_DECLSPEC Engine::DValue evalJSExpr(const std::wstring& expr);

extern "C" CORE_DLL_DECLSPEC BOOL isPathHasDocument(const wchar_t* path);
extern "C" CORE_DLL_DECLSPEC BOOL isDocumentHasNewerBackup(const wchar_t* path);
extern "C" CORE_DLL_DECLSPEC BOOL isDocumentModifiedAfterLastSave();
extern "C" CORE_DLL_DECLSPEC BOOL isDocumentReadOnly();
extern "C" CORE_DLL_DECLSPEC BOOL newDocument(const wchar_t* projPath, const wchar_t* env);
extern "C" CORE_DLL_DECLSPEC void openDocument(const wchar_t* projPath, const wchar_t* backup, BOOL readOnly, const wchar_t* progress);
extern "C" CORE_DLL_DECLSPEC sketch::Define::DocumentSaveResult saveDocument(BOOL async, const wchar_t* progress);
extern "C" CORE_DLL_DECLSPEC sketch::Define::DocumentSaveResult saveAsDocument(const wchar_t* projPath, BOOL async, const wchar_t* progress);
extern "C" CORE_DLL_DECLSPEC sketch::Define::DocumentSaveResult saveTempDocument(const wchar_t* progress);
extern "C" CORE_DLL_DECLSPEC BOOL packageDocument();
