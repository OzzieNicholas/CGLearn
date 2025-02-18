#pragma once

using namespace System;
using namespace System::Collections::Generic;

#define WRAPPER_LIB

namespace Engine {
	class DObject;
}

namespace sketch {
	class Command;
}

namespace Wrapper {

	class NativeCallback;

	public ref class STWrapper
	{
	public:
		void init(System::IntPtr mainHwnd, System::IntPtr viewHwnd, System::IntPtr topBarHwnd, Action ^onInit);
		
		//UI to Native command
		void pushCommand(String^cmd, LoongCAD::Util::DObject ^paras, Action<LoongCAD::Util::DObject^> ^cbFunc); //push command to executing queue 
		LoongCAD::Util::DObject^ execute(String ^cmd, LoongCAD::Util::DObject ^paras); //execute command immediatly, can not undo/redo
		void onCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult);
		void onExeCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult);

		//called by native module
		void onCoreInited(void);
		Engine::DObject* executeUICmd(sketch::Command *pCmd);

	protected:
		static void _onExecuteCallback(Engine::DObject *pResult);
		
	private:
		static Action ^m_onInitFunc = nullptr;
		static Dictionary<long long, Action<LoongCAD::Util::DObject^>^> ^m_cmdCbFuncs = gcnew Dictionary<long long, Action<LoongCAD::Util::DObject^>^>();
		static Dictionary<long long, LoongCAD::Util::DObject^> ^m_exeCbParas = gcnew Dictionary<long long, LoongCAD::Util::DObject^>();
		static LoongCAD::Util::DObject ^m_executeRetParas = nullptr;
		static bool m_coreInit = false;
	};
}
