#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_Settings.h"
#include "../StrUtil.h"



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
    LoongCAD::Util::DObject^ Native::settings()
    {
        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, &::settings());
        return res;
    }

    System::Object^ Native::settings(String^ pname)
    {
        auto pVal = ::settings(STR(pname));
        if (pVal == nullptr)
            return nullptr;

        return wrUtil::wrap(*pVal);
    }

    void Native::setSettings(LoongCAD::Util::DObject^ props)
    {
        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        ::setSettings(eProps);
    }

    void Native::setSettings(String^ pname, bool pval)
    {
        ::setSettings(STR(pname), pval);
    }

}
