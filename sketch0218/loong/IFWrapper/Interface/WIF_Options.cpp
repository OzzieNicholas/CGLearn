#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_Options.h"
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
    LoongCAD::Util::DObject^ Native::options()
    {
        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, &::options());
        return res;
    }

    System::Object^ Native::options(String^ pname)
    {
        auto pVal = ::options(STR(pname));
        if (pVal == nullptr)
            return nullptr;

        return wrUtil::wrap(*pVal);
    }

    //LoongCAD::Util::DObject^ Native::compPrefs()
    //{
    //    LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
    //    wrUtil::copyDObj(res, &::compPrefs());
    //    return res;
    //}

    void Native::setOptions(LoongCAD::Util::DObject^ props)
    {
        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        ::setOptions(eProps);
    }

    void Native::setOptions(String^ pname, bool pval)
    {
        ::setOptions(STR(pname), pval);
    }

}
