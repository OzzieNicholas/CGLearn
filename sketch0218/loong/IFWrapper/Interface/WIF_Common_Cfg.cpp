#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_Common_Cfg.h"
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
    LoongCAD::Util::DObject^ Native::getCommonCfg(String^ type, String^ name)
    {
        if (type == nullptr || name == nullptr)
            return nullptr;

        auto pCfg = ::getCommonCfg(WSTR(type), WSTR(name));
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }

    void Native::setCommonCfg(String^ type, String^ name, LoongCAD::Util::DObject^ cfg, bool save, bool invokeUI)
    {
        if (type == nullptr || name == nullptr)
            return;

        Engine::DObject eCfg;
        wrUtil::copyDObj(&eCfg, cfg);
        ::setCommonCfg(WSTR(type), WSTR(name), eCfg, save, invokeUI);
    }

    array<String^>^ Native::getCommonCfgsByType(String^ type, bool sys)
    {
        std::vector<std::wstring> names;
        if (!::getCommonCfgsByType(names, WSTR(type), sys))
            return gcnew array<String^>(0);

        array<String^>^ res = gcnew array<String^>((int)names.size());
        for (int i = 0; i < names.size(); ++i)
            res[i] = WSTR(names[i]);
        return res;
    }

    LoongCAD::Util::DObject^ Native::getGlobalCommonCfg(String^ type, String^ name)
    {
        if (type == nullptr || name == nullptr)
            return nullptr;

        auto pCfg = ::getGlobalCommonCfg(WSTR(type), WSTR(name));
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }

    void Native::setGlobalCommonCfg(String^ type, String^ name, LoongCAD::Util::DObject^ cfg, bool save, bool invokeUI)
    {
        if (type == nullptr || name == nullptr)
            return;

        Engine::DObject eCfg;
        wrUtil::copyDObj(&eCfg, cfg);
        ::setGlobalCommonCfg(WSTR(type), WSTR(name), eCfg, save, invokeUI);
    }

    array<String^>^ Native::getGlobalCommonCfgsByType(String^ type, bool sys)
    {
        std::vector<std::wstring> names;
        if (!::getGlobalCommonCfgsByType(names, WSTR(type), sys))
            return gcnew array<String^>(0);

        array<String^>^ res = gcnew array<String^>((int)names.size());
        for (int i = 0; i < names.size(); ++i)
            res[i] = WSTR(names[i]);
        return res;
    }

}
