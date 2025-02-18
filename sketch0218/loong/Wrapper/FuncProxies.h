#pragma once

#include "STWrapper.h"

#include <vcclr.h>
#include <msclr\auto_gcroot.h>
#include "sketch/Define.h"
#include "sketch/UIFuncs.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Engine {
	class DObject;
}

namespace sketch {
	class STCore;
	class Command;
}

namespace Wrapper {

	class FuncProxies
	{
	public:
		static void init(STWrapper ^wrapper);

 		//native command callbacks
		static void onCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult);
		static void onExeCmdCallback(sketch::Command *pCmd, Engine::DObject *pResult);
		
	private:
		static msclr::auto_gcroot<STWrapper^> m_wrapper;
	};
}
