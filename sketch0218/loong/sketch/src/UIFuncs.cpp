#include "sketch/UIFuncs.h"

#include <unordered_set>
#include <vector>

#include "sketch/Define.h"

namespace sketch {
	void(*UIFuncs::onCoreInited)(void) = nullptr;
	void(*UIFuncs::onCoreFailed)(const std::string& errMsg) = nullptr;

	void(*UIFuncs::setStartPageTips)(const std::wstring& tips) = nullptr;
	void(*UIFuncs::setStartPageProgress)(int progress) = nullptr;
	int(*UIFuncs::startPageProgress)() = nullptr;

	int(*UIFuncs::initJobProgress)(const std::wstring& job) = nullptr;
	void(*UIFuncs::onInitJobDone)(const std::wstring& job) = nullptr;

	void(*UIFuncs::onDocOpen)(const std::wstring docPath) = nullptr;
	void(*UIFuncs::onDocClose)(const std::wstring docPath) = nullptr;
	void(*UIFuncs::onAppClose)() = nullptr;

	float(*UIFuncs::dpi)() = nullptr;
	Engine::DObject* (*UIFuncs::executeUICmd)(sketch::Command* pCmd) = nullptr;
	bool(*UIFuncs::filterMsg)(void* msg) = nullptr;
	void(*UIFuncs::showActiveSubMenu)(void* hwnd, int x, int y) = nullptr;
	void(*UIFuncs::setCursor)(int idx, sketch::Define::MouseCursor cursor) = nullptr;
	void(*UIFuncs::setTip)(const std::wstring& tip) = nullptr;
	void(*UIFuncs::setWarning)(const std::wstring& warning) = nullptr;
	void(*UIFuncs::setObjSelInfo)(const std::wstring& tip) = nullptr;
	void(*UIFuncs::setCellSelInfo)(const std::wstring& tip) = nullptr;
	void(*UIFuncs::updateActiveViewList)(const std::unordered_set<long long>& ids, const std::vector<std::wstring>& viewNames, long long focusId) = nullptr;
	void(*UIFuncs::updateProgressBar)(const std::wstring& name, int progress, const std::wstring& msg) = nullptr;
	void(*UIFuncs::updateLayoutCells)(const std::vector<Define::LayoutCellType>& types, const std::list<long long>& ids, const std::vector<std::wstring>& names) = nullptr;
	void(*UIFuncs::addLayoutCells)(const std::vector<Define::LayoutCellType>& types, const std::vector<long long>& ids, const std::vector<std::wstring>& names) = nullptr;
	void(*UIFuncs::addLayoutCell)(Define::LayoutCellType type, long long id, const std::wstring& name) = nullptr;
	void(*UIFuncs::removeLayoutCells)(const std::vector<long long>& ids) = nullptr;
	void(*UIFuncs::removeLayoutCell)(long long id) = nullptr;
	void(*UIFuncs::updateFocusedCells)(const std::list<long long>& ids) = nullptr;
	void(*UIFuncs::performAcceptButton)(void* hwnd) = nullptr;
	void(*UIFuncs::performEscClose)(void* hwnd) = nullptr;
	void(*UIFuncs::performTabSwitch)(void* hwnd) = nullptr;
	std::string(*UIFuncs::askForOverlappedObjs)() = nullptr;
	void(*UIFuncs::showWorkPropsDialog)(const Engine::DObject& workProps) = nullptr;
	void(*UIFuncs::focusMainForm)() = nullptr;
	void(*UIFuncs::showForm)(const std::string& form) = nullptr;
	void(*UIFuncs::updateForm)(const std::string& form, const Engine::DObject& paras) = nullptr;
	void(*UIFuncs::callShortCut)(const std::wstring& fu, unsigned int sc) = nullptr;
	void(*UIFuncs::clickFuncUnit)(const std::wstring& fu) = nullptr;
	void(*UIFuncs::clickFuncUnitWithParas)(const std::wstring& fu, const Engine::DObject& paras) = nullptr;
	void(*UIFuncs::showFadingMsgBox)(const std::wstring& text, const std::wstring& title, int fadingTm, double opacity) = nullptr;
	void(*UIFuncs::onCellNameUpdate)(const std::vector<long long>& ids, const std::vector<std::wstring>& names) = nullptr;
	void(*UIFuncs::updateSelectorInfo)(Define::SelectorType selType, Define::PickMode pickMode, uint32_t pickFlag, const std::wstring& pickObjFilterCfg, Define::SnapMode snapMode, Define::SnapRefPlane snapRefPlane, uint32_t snapFlagPt, uint32_t snapFlagPl) = nullptr;
	void(*UIFuncs::compEditBegin)(const Engine::Guid& cfgGUID, long long compObj) = nullptr;
	void(*UIFuncs::compEditUpdateCurObjs)(const std::vector<long long>& ids) = nullptr;
	void(*UIFuncs::compEditSetFocusVar)(long long varId) = nullptr;
	void(*UIFuncs::compEditRefeshVars)() = nullptr;
	void(*UIFuncs::compEditRefeshObjExplorer)() = nullptr;
	void(*UIFuncs::compEditRefeshEvalResult)(const std::wstring& output, const std::wstring& errMsg) = nullptr;
	void(*UIFuncs::compEditBeginUpdateResult)(const std::wstring& text) = nullptr;
	void(*UIFuncs::compEditAppendResult)(const std::wstring& text) = nullptr;
	void(*UIFuncs::showHandlerDistEditor)(long long objId, long long hId, double dist, const Engine::Vec2i& pos, Define::ObjectSide dir, float angl) = nullptr;

	//////////////////////////////////////// component plugins //////////////////////////////////////////////
	bool(*UIFuncs::loadCompBuiltInPlugin)(const Engine::Guid& guid, const Engine::Path& path, const Engine::Path& dllFile) = nullptr;
	bool(*UIFuncs::loadCompDocPlugin)(const Engine::Guid& guid, const Engine::Path& path, const Engine::Path& dllFile) = nullptr;
	bool(*UIFuncs::onCompInstanceCreate)(const Engine::Guid& guid, long long objId);
	bool(*UIFuncs::tryRunCompPlugin)(const Engine::Guid& guid, long long objId) = nullptr;
}



