#pragma once

#include "Core.h"

#include <unordered_set>
#include <vector>

#include "sketch/Define.h"

namespace Engine {
	class DObject;
	class Vec2i;
	class Guid;
	class Path;
}

namespace sketch {
	class Command;

	//native to ui function prototypes
	class CORE_DLL_DECLSPEC UIFuncs {
	public:
		static void(*onCoreInited)(void);
		static void(*onCoreFailed)(const std::string& errMsg);

		static void(*setStartPageTips)(const std::wstring& tips);
		static void(*setStartPageProgress)(int progress);
		static int(*startPageProgress)();

		static int(*initJobProgress)(const std::wstring& job);
		static void(*onInitJobDone)(const std::wstring& job);

		static void(*onDocOpen)(const std::wstring docPath);
		static void(*onDocClose)(const std::wstring docPath);
		static void(*onAppClose)();

		static float(*dpi)();
		static Engine::DObject* (*executeUICmd)(sketch::Command* pCmd);
		static bool(*filterMsg)(void* msg);
		static void(*showActiveSubMenu)(void* hwnd, int x, int y);
		static void(*setCursor)(int idx, sketch::Define::MouseCursor cursor);
		static void(*setTip)(const std::wstring& tip);
		static void(*setWarning)(const std::wstring& warning);
		static void(*setObjSelInfo)(const std::wstring& tip);
		static void(*setCellSelInfo)(const std::wstring& tip);
		static void(*updateActiveViewList)(const std::unordered_set<long long>& ids, const std::vector<std::wstring>& viewNames, long long focusId);
		static void(*updateProgressBar)(const std::wstring& name, int progress, const std::wstring& msg);
		static void(*updateLayoutCells)(const std::vector<Define::LayoutCellType>& types, const std::list<long long>& ids, const std::vector<std::wstring>& names);
		static void(*addLayoutCells)(const std::vector<Define::LayoutCellType>& types, const std::vector<long long>& ids, const std::vector<std::wstring>& names);
		static void(*addLayoutCell)(Define::LayoutCellType type, long long id, const std::wstring& name);
		static void(*removeLayoutCells)(const std::vector<long long>& ids);
		static void(*removeLayoutCell)(long long id);
		static void(*updateFocusedCells)(const std::list<long long>& ids);
		static void(*performAcceptButton)(void* hwnd);
		static void(*performEscClose)(void* hwnd);
		static void(*performTabSwitch)(void* hwnd);
		static std::string(*askForOverlappedObjs)();
		static void(*showWorkPropsDialog)(const Engine::DObject& workProps);
		static void(*focusMainForm)();
		static void(*showForm)(const std::string& form);
		static void(*updateForm)(const std::string& form, const Engine::DObject& paras);
		static void(*callShortCut)(const std::wstring& fu, unsigned int sc);
		static void(*clickFuncUnit)(const std::wstring& fu);
		static void(*clickFuncUnitWithParas)(const std::wstring& fu, const Engine::DObject& paras);
		static void(*showFadingMsgBox)(const std::wstring& text, const std::wstring& title, int fadingTm, double opacity);
		static void(*onCellNameUpdate)(const std::vector<long long>& ids, const std::vector<std::wstring>& names);
		static void(*updateSelectorInfo)(Define::SelectorType selType, Define::PickMode pickMode, uint32_t pickFlag, const std::wstring& pickObjFilterCfg, Define::SnapMode snapMode, Define::SnapRefPlane snapRefPlane, uint32_t snapFlagPt, uint32_t snapFlagPl);
		static void(*compEditBegin)(const Engine::Guid& cfgGUID, long long compObj);
		static void(*compEditUpdateCurObjs)(const std::vector<long long>& ids);
		static void(*compEditSetFocusVar)(long long varId);
		static void(*compEditRefeshVars)();
		static void(*compEditRefeshObjExplorer)();
		static void(*compEditRefeshEvalResult)(const std::wstring& output, const std::wstring& errMsg);
		static void(*compEditBeginUpdateResult)(const std::wstring& text);
		static void(*compEditAppendResult)(const std::wstring& text);
		static void(*showHandlerDistEditor)(long long objId, long long hId, double dist, const Engine::Vec2i& pos, Define::ObjectSide dir, float angl);

		//////////////////////////////////////// component plugins //////////////////////////////////////////////
		static bool(*loadCompBuiltInPlugin)(const Engine::Guid& guid, const Engine::Path& path, const Engine::Path& dllFile);
		static bool(*loadCompDocPlugin)(const Engine::Guid& guid, const Engine::Path& path, const Engine::Path& dllFile);
		static bool(*onCompInstanceCreate)(const Engine::Guid& guid, long long objId);
		static bool(*tryRunCompPlugin)(const Engine::Guid& guid, long long objId);
	};

}




