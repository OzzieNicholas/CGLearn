#include "sketch/STCore.h"

#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/View/Controller/Controller.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"
#include <Windows.h>

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const std::wstring& controller(int slot) { return L""; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, const std::wstring& val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, const Engine::Guid& val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, int val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, short val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, char val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const std::string& key0, bool val0) { return true; }

CORE_DLL_DECLSPEC BOOL activateController(const std::wstring& name, const Engine::DObject* paras) { return true; }

extern "C" CORE_DLL_DECLSPEC void deactivateController(const wchar_t* name) {}

extern "C" CORE_DLL_DECLSPEC void popController() {}

CORE_DLL_DECLSPEC BOOL modifyController(const std::wstring& name, const Engine::DObject& paras) { return true; }