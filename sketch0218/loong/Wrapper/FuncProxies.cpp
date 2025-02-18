#include "stdafx.h"

#include <Windows.h>
#include <string>

#include "Engine/Math/Vec2i.h"
#include "Engine/Util/Guid.h"

#include "FuncProxies.h"
#include "sketch/STCore.h"
#include "sketch/Interface/IF_Layout.h"
#include "StrUtil.h"

using namespace sketch;
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace CLRUtils;

namespace Wrapper {
	msclr::auto_gcroot<STWrapper^> FuncProxies::m_wrapper = nullptr;

	void FuncProxies::init(STWrapper ^wrapper)
	{
		m_wrapper = wrapper;

		UIFuncs::setStartPageTips = [](const std::wstring& tips)->void {
			UserCtrl::Form_Start::instance->tips = WSTR(tips);
		};

		UIFuncs::setStartPageProgress = [](int progress)->void {
			UserCtrl::Form_Start::instance->progress = progress;
		};

		UIFuncs::startPageProgress = []()->int {
			return UserCtrl::Form_Start::instance->progress;
		};

		UIFuncs::onCoreInited = []()->void {
			m_wrapper->onCoreInited();
		};

		UIFuncs::initJobProgress = [](const std::wstring& job)->int {
			return UI::MainForm::instance->initJobProgress(WSTR(job));
		};

		UIFuncs::onInitJobDone = [](const std::wstring& job)->void {
			UI::MainForm::instance->onInitJobDone(WSTR(job));
		};

		UIFuncs::onDocOpen = [](const std::wstring docPath) {
			
			
		};

		UIFuncs::onDocClose = [](const std::wstring docPath) {
			
			
		};

		UIFuncs::onAppClose = []() {
			
			
		};

		UIFuncs::executeUICmd = [](sketch::Command* pCmd)->Engine::DObject* {
			return m_wrapper->executeUICmd(pCmd);
		};

		UIFuncs::filterMsg = [](void* msg)->bool {
			LPMSG m = (LPMSG)msg;
			return LoongCAD::Common::MsgFilterManager::PreFilterMessage((IntPtr)m->hwnd, m->message, (IntPtr)(int*)m->wParam, (IntPtr)m->lParam);
		};

		UIFuncs::showActiveSubMenu = [](void* hwnd, int x, int y)->void {
			auto ctrl = Windows::Forms::Control::FromHandle((IntPtr)hwnd);
			UI::SubMenu::showActive(ctrl, x, y);
		};

		UIFuncs::setCursor = [](int idx, sketch::Define::MouseCursor cursor)->void {
			UI::CursorManager::instance->setCursor(idx, (LoongCAD::STC::Define::MouseCursor)(int)cursor);
		};
		
		UIFuncs::setTip = [](const std::wstring& s)->void {
			UI::MainForm::instance->statusBar->tips = WSTR(s);
		};

		UIFuncs::setWarning = [](const std::wstring& s)->void {
			UI::MainForm::instance->statusBar->warnings = WSTR(s);
		};

		UIFuncs::setObjSelInfo = [](const std::wstring& s)->void {
			UI::MainForm::instance->statusBar->objSelInfo = WSTR(s);
		};

		UIFuncs::setCellSelInfo = [](const std::wstring& s)->void {
			UI::MainForm::instance->statusBar->cellSelInfo = WSTR(s);
		};

		UIFuncs::updateActiveViewList = [](const std::unordered_set<long long>& ids, const std::vector<std::wstring>& viewNames, long long focusId)->void {
			UI::Bar_View_Active^ bar = (UI::Bar_View_Active^)UI::LayoutManager::instance->getBuiltInBar("view_active");
			if (bar == nullptr)
				return;

			Dictionary<long long, String^>^ activeViews = gcnew Dictionary<long long, String^>();
			int n = 0;
			for (auto id : ids) {
				activeViews->Add(id, WSTR(viewNames[n++]));
			}
			bar->activeViews = activeViews;

			if (focusId >= 0)
				bar->focusedId = focusId;
		};

		UIFuncs::updateProgressBar = [](const std::wstring& name, int progress, const std::wstring& msg)->void {
			auto bar = UI::ProgressMsgBox::instance(WSTR(name));
			if (bar == nullptr)
				return;

			if (progress >= 0)
				bar->progress = progress;
			
			if (!msg.empty())
				bar->message = WSTR(msg);

			if (progress == 0)
				int a = 1;
		};

		UIFuncs::updateLayoutCells = [](const std::vector<Define::LayoutCellType>& types, const std::list<long long>& ids, const std::vector<std::wstring>& names)->void {
			auto headerTab = UI::MainForm::instance->headerTab;
			headerTab->clearCells();

			if (!ids.empty()) {
				auto it = ids.begin();
				int n = 0;

				headerTab->beginNativeLoad();
				while (it != ids.end()) {
					headerTab->addNativeCell((LoongCAD::STC::Define::LayoutCellType)(int)types[n], *it, WSTR(names[n]));
					it++;
					n++;
				}
				headerTab->endNativeLoad();
			}
		};

		UIFuncs::addLayoutCells = [](const std::vector<Define::LayoutCellType>& types, const std::vector<long long>& ids, const std::vector<std::wstring>& names)->void {
			auto headerTab = UI::MainForm::instance->headerTab;
			if (!ids.empty()) {
				auto it = ids.begin();
				int n = 0;

				headerTab->beginNativeLoad();
				while (it != ids.end()) {
					headerTab->addNativeCell((LoongCAD::STC::Define::LayoutCellType)(int)types[n], *it, WSTR(names[n]));
					it++;
					n++;
				}
				headerTab->endNativeLoad();
			}
		};

		UIFuncs::addLayoutCell = [](Define::LayoutCellType type, long long id, const std::wstring& name)->void {
			auto headerTab = UI::MainForm::instance->headerTab;
			headerTab->addCell((LoongCAD::STC::Define::LayoutCellType)(int)type, id, WSTR(name));
		};

		UIFuncs::removeLayoutCells = [](const std::vector<long long>& ids)->void {
			auto headerTab = UI::MainForm::instance->headerTab;
			if (!ids.empty()) {
				headerTab->beginNativeLoad();
				for (auto id : ids) {
					headerTab->removeNativeCell(id);
				}
				headerTab->endNativeLoad();
			}
		};

		UIFuncs::removeLayoutCell = [](long long id)->void {
			auto headerTab = UI::MainForm::instance->headerTab;
			headerTab->removeCell(id);
		};
		
		UIFuncs::updateFocusedCells = [](const std::list<long long>& ids)->void {
			auto bar = (UI::Bar_View_Active^)UI::LayoutManager::instance->getBuiltInBar("view_active");
			bar->focusedId = ids.empty() ? -1 : ids.front();
			UI::MainForm::instance->headerTab->selectedId = ids.empty()? -1 : ids.front();
		};

		UIFuncs::performAcceptButton = [](void* hwnd) {
			auto form = Control::FromChildHandle((IntPtr)hwnd);
			if (form == nullptr)
				return;

			if (form->GetType()->IsSubclassOf(UserCtrl::ParaFormBase::typeid)) 
				((UserCtrl::ParaFormBase^)form)->performAcceptButton();
			else if (form->GetType()->IsSubclassOf(LoongCAD::UI::ParamFormBase::typeid)) 
				((LoongCAD::UI::ParamFormBase^)form)->InvokeAcceptButton();
		};

		UIFuncs::performEscClose = [](void* hwnd) {
			auto form = Control::FromChildHandle((IntPtr)hwnd);
			if (form == nullptr)
				return;

			if (form->GetType()->IsSubclassOf(UserCtrl::ParaFormBase::typeid))
				((UserCtrl::ParaFormBase^)form)->performEscClose();
			else if (form->GetType()->IsSubclassOf(LoongCAD::UI::ParamFormBase::typeid))
				((LoongCAD::UI::ParamFormBase^)form)->InvokeEscClose();
		};

		UIFuncs::performTabSwitch = [](void* hwnd) {
			auto form = Control::FromChildHandle((IntPtr)hwnd);
			if (form == nullptr)
				return;

			if (form->GetType()->IsSubclassOf(UserCtrl::ParaFormBase::typeid))
				((UserCtrl::ParaFormBase^)form)->performTabSwitch();
			else if (form->GetType()->IsSubclassOf(LoongCAD::UI::ParamFormBase::typeid))
				((LoongCAD::UI::ParamFormBase^)form)->MoveTabToNext();
		};

		UIFuncs::askForOverlappedObjs = []()->std::string {
			auto dlg = gcnew UI::Form_AskOverlappedObjs();
			dlg->ShowDialog(UI::MainForm::instance);
			return STR(dlg->result);
		};

		UIFuncs::showWorkPropsDialog = [](const Engine::DObject& eWorkProps)->void {
			auto workProps = gcnew LoongCAD::Util::DObject();
			wrUtil::copyDObj(workProps, &eWorkProps);
			UI::FuncUnit::invokeClick(UI::FuncUnit::get("edit_props"), nullptr, nullptr, Keys::None, workProps, true);
		};

		UIFuncs::callShortCut = [](const std::wstring& funcName, unsigned int sc) -> void {
			UI::FuncUnit^ fu = UI::FuncUnit::get(WSTR(funcName));
			if (fu == nullptr)
				return;
			
			auto callMode = WSTR(fu->shortCutInvokeType);
			if (callMode == L"click")
				UI::FuncUnit::invokeClick(fu, sc, nullptr, Keys::None, nullptr);
			else if (callMode == L"check")
				UI::FuncUnit::invokeCheck(fu, sc, nullptr, Keys::None, nullptr);
		};

		UIFuncs::clickFuncUnit = [](const std::wstring& fu)->void {
			UI::FuncUnit::invokeClick(UI::FuncUnit::get(WSTR(fu)), nullptr, nullptr, Keys::None, nullptr, false);
		};

		UIFuncs::clickFuncUnitWithParas = [](const std::wstring& fu, const Engine::DObject& paras)->void {
			LoongCAD::Util::DObject^ wrObj = gcnew LoongCAD::Util::DObject();
			wrUtil::copyDObj(wrObj, &paras);
			UI::FuncUnit::invokeClick(UI::FuncUnit::get(WSTR(fu)), nullptr, nullptr, Keys::None, wrObj, false);
		};

		UIFuncs::showFadingMsgBox = [](const std::wstring& text, const std::wstring& title, int fadingTm, double opacity) {
			if (title.empty() && text.empty())
				return;

			if (fadingTm == 0)
				fadingTm = 500;
			
			if (opacity == 0.0)
				opacity = 0.6;
			
			UI::FadingMsgBox::show(WSTR(title), WSTR(text), fadingTm, opacity, UI::MainForm::instance);
		};

		UIFuncs::onCellNameUpdate = [](const std::vector<long long>& ids, const std::vector<std::wstring>& names) {
			array<long long>^ idArr = gcnew array<long long>((int)ids.size());
			array<String^>^ nameArr = gcnew array<String^>((int)names.size());

			if (!ids.empty()) {
				pin_ptr<long long> pArr = &idArr[0];
				memcpy(pArr, ids.data(), sizeof(long long)* ids.size());
			}

			for (int i = 0; i < names.size(); ++i) {
				nameArr[i] = WSTR(names[i]);
			}

			UI::MainForm::instance->headerTab->onCellNameUpdate(idArr, nameArr);
		};

		UIFuncs::updateSelectorInfo = [](Define::SelectorType selType, Define::PickMode pickMode, uint32_t pickFlag, const std::wstring& pickObjFilterCfg, Define::SnapMode snapMode, Define::SnapRefPlane snapRefPlane, uint32_t snapFlagPt, uint32_t snapFlagPl) {
			switch (selType) {
			case Define::SEL_TYPE_3D:
				UI::Bar_Pick::instance->refresh((LoongCAD::STC::Define::PickMode)pickMode, pickFlag, WSTR(pickObjFilterCfg));
				UI::Bar_Snap::instance->refresh((LoongCAD::STC::Define::SnapMode)snapMode, (LoongCAD::STC::Define::SnapRefPlane)snapRefPlane, snapFlagPt, snapFlagPl);
				break;
			}
		};

		UIFuncs::focusMainForm = []() {
			UI::MainForm::instance->Focus();
		};

		UIFuncs::showForm = [](const std::string& form) {
			
		};

		UIFuncs::updateForm = [](const std::string& form, const Engine::DObject& paras) {
			
		};

		
		UIFuncs::dpi = []()->float {
			return LoongCAD::Util::DPI::Dpi;
		};

		UIFuncs::showHandlerDistEditor = [](long long objId, long long hId, double dist, const Engine::Vec2i& pos, Define::ObjectSide dir, float angl) {
			if (UI::Form_Handler_Edit_Dist::instance->Visible)
				return;

			UI::Form_Handler_Edit_Dist::instance->show(objId, hId, dist, Point(pos.x, pos.y), (LoongCAD::STC::Define::ObjectSide)dir, angl);
		};

	}

	void FuncProxies::onCmdCallback(sketch::Command* pCmd, Engine::DObject* pResult)
	{
		m_wrapper->onCmdCallback(pCmd, pResult);
	}

	void FuncProxies::onExeCmdCallback(sketch::Command* pCmd, Engine::DObject* pResult)
	{
		m_wrapper->onExeCmdCallback(pCmd, pResult);
	}
}