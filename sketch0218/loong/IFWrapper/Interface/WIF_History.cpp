#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
//#include "sketch/Interface/Main/IF_Inquire.h"
#include "sketch/Interface/IF_History.h"
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
    String^ Native::projBaseDir()
    {
        return WSTR(::projBaseDir());
    }

    LoongCAD::Util::DObject^ Native::globalHistory(String^ name)
    {
        auto eProps = ::globalHistory(WSTR(name));
        if (!eProps)
            return nullptr;

        auto res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, eProps);
        return res;
    }

    LoongCAD::Util::DObject^ Native::history(String^ name)
    {
        auto eProps = ::history(WSTR(name));
        if (!eProps)
            return nullptr;

        auto res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, eProps);
        return res;
    }
    
    System::Object^ Native::globalHistory(String^ name, String^ pname)
    {
        auto eProps = ::globalHistory(WSTR(name), STR(pname));
        if (!eProps)
            return nullptr;

        return wrUtil::wrap(*eProps);
    }

    System::Object^ Native::history(String^ name, String^ pname)
    {
        auto eProps = ::history(WSTR(name), STR(pname));
        if (!eProps)
            return nullptr;

        return wrUtil::wrap(*eProps);
    }

    bool Native::saveGlobalHistory(String^ name, LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        return ::saveGlobalHistory(WSTR(name), eProps);
    }

    bool Native::saveHistory(String^ name, LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        return ::saveHistory(WSTR(name), eProps);
    }
}
