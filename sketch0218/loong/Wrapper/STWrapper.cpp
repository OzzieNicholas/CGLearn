#include "stdafx.h"
#include <windows.h>
#include "STWrapper.h"
#include "sketch/STCore.h"
#include "sketch/Command/Command.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/UIFuncs.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/Log.h"
#include "FuncProxies.h"
#include "StrUtil.h"

#include <string>

using namespace Engine;
using namespace sketch;
using namespace System;
using namespace CLRUtils;

namespace Wrapper {
	void STWrapper::init(System::IntPtr mainHwnd, System::IntPtr viewHwnd, System::IntPtr topBarHwnd, Action ^onInit)
	{
		//init native callback
		FuncProxies::init(this);

		//set onInit callback
		m_onInitFunc = onInit;

		//init core module
		STCore::inst->init((HWND)mainHwnd.ToPointer(), (HWND)viewHwnd.ToPointer(), (HWND)topBarHwnd.ToPointer());
	}

	void STWrapper::onCoreInited()
	{
		Log::log("", "STWrapper::onCoreInit >>>>>>>>>>>>>>>>>>>>>>>  \n");

		if (m_onInitFunc != nullptr)
			m_onInitFunc();
	}

	void STWrapper::_onExecuteCallback(Engine::DObject *pResult)
	{

	}

	LoongCAD::Util::DObject^ STWrapper::execute(String ^cmd, LoongCAD::Util::DObject ^paras)
	{
		Command* pCmd = Command::alloc(STR(cmd));
		if (paras != nullptr)
			wrUtil::copyDObj(pCmd, paras);

		long long cmdKey = (long long)pCmd;

		pCmd->cbFunc = FuncProxies::onExeCmdCallback;

		STCore::inst->cmdMgr()->execute(pCmd);

		LoongCAD::Util::DObject^ pRet = nullptr;
		if (m_exeCbParas->TryGetValue(cmdKey, pRet))
			m_exeCbParas->Remove(cmdKey);
		return pRet;
	}

	void STWrapper::pushCommand(String ^cmd, LoongCAD::Util::DObject ^paras, Action<LoongCAD::Util::DObject^> ^cbFunc)
	{
		Command *pCmd = Command::alloc(STR(cmd));
		if (paras != nullptr)
			wrUtil::copyDObj(pCmd, paras);

		if (cbFunc != nullptr) {
			pCmd->cbFunc = FuncProxies::onCmdCallback;
			m_cmdCbFuncs->Add((long long)pCmd, cbFunc);
		}
		
		STCore::inst->cmdMgr()->pushCmd(pCmd);
	}

	Engine::DObject* STWrapper::executeUICmd(sketch::Command *pCmd)
	{
		LoongCAD::Util::DObject ^paras = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(paras, pCmd);

		String ^cmd = STR(pCmd->cmd());

		LoongCAD::Util::DObject^ ret = LoongCAD::Common::Callback::onUICommand(cmd, paras);
		
		DObject* pNativeRet = nullptr;
		if (ret != nullptr) {
			pNativeRet = DObject::alloc();
			wrUtil::copyDObj(pNativeRet, ret);
		}

		return pNativeRet;
	}

	void STWrapper::onExeCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult)
	{
		if (!pResult)
			return;

		long long cmdKey = (long long)pCmd;
		LoongCAD::Util::DObject ^pRefObj = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(pRefObj, pResult);
		m_exeCbParas[cmdKey] = pRefObj;
	}

	void STWrapper::onCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult)
	{
		long long cmdKey = (long long)pCmd;
		Action<LoongCAD::Util::DObject^>^ cbFunc;
		if (!m_cmdCbFuncs->TryGetValue(cmdKey, cbFunc))
			return;

		if (pResult) {
			LoongCAD::Util::DObject ^pRefObj = gcnew LoongCAD::Util::DObject();
			wrUtil::copyDObj(pRefObj, pResult);
			cbFunc(pRefObj);
		}
		else {
			cbFunc(nullptr);
		}

		m_cmdCbFuncs->Remove(cmdKey);
	}
}