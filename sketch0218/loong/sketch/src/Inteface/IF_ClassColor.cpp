#include "sketch/STCore.h"

#include <Windows.h>
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/StrUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/View/ViewManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Color/ColorManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC const uint32_t* classColors()
{
	sketch::ColorManager* pColorMgr = STCore::inst->docMgr()->cfgMgr()->colorMgr();
	return pColorMgr->colors();
}

extern "C" CORE_DLL_DECLSPEC BOOL saveClassColors(unsigned int* colors, int nColors)
{
	Document* pDoc = STCore::inst->docMgr()->document();
	if (pDoc->path().empty())
		return false;

	sketch::ColorManager* pColorMgr = STCore::inst->docMgr()->cfgMgr()->colorMgr();
	pColorMgr->setColors(colors, nColors);
	pColorMgr->dirty();
	return true;
}