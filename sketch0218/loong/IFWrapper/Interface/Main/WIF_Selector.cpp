#include <Windows.h>
#include <string>

#include "../../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "../../StrUtil.h"



using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace sketch;
using namespace LoongCAD::Geometry;
using namespace CLRUtils;

namespace IFWrapper
{
    //String^ Native::getPickObjFilterCfg()
    //{
    //    return WSTR(::getPickObjFilterCfg());
    //}

}
