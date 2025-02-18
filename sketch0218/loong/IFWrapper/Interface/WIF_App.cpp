#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_App.h"
#include "../StrUtil.h"


using namespace System;
using namespace System::Drawing;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace sketch;
using namespace LoongCAD::Geometry;
using namespace CLRUtils;

namespace IFWrapper
{
    System::Drawing::Rectangle Native::screenWorkAreaRelativeToPrimary(IntPtr ptr)
    {
        auto eWork = ::screenWorkAreaRelativeToPrimary(ptr.ToPointer());
        return System::Drawing::Rectangle(eWork.left, eWork.top, eWork.right - eWork.left, eWork.bottom - eWork.top);
    }

    void Native::setProgress(String^ progName, float newProgress)
    {
        ::setProgress(WSTR(progName), newProgress); }

    void Native::sketch_active_controller(String ^ name)
    { 
        ::sketch_active_controller(WSTR(name)); 
    }

    void Native::sketch_enter(String ^ para) { ::sketch_enter(STR(para)); }

    void Native::sketch_escape(String ^ para) { ::sketch_escape(STR(para)); }
    }
