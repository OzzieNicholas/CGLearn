#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_UILayout.h"
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
    LoongCAD::Util::DObject^ Native::queryUILayout(String^ name)
    {
        if (name == nullptr)
            return nullptr;

        auto pCfg = ::queryUILayout(WSTR(name));
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }

    LoongCAD::Util::DObject^ Native::queryUIBar(String^ name)
    {
        if (name == nullptr)
            return nullptr;

        auto pCfg = ::queryUIBar(WSTR(name));
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }


    LoongCAD::Util::DObject^ Native::queryUIMiniBar(String^ name)
    {
        if (name == nullptr)
            return nullptr;

        auto pCfg = ::queryUIMiniBar(WSTR(name));
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }

    LoongCAD::Util::DObject^ Native::queryUIMiniBars()
    {
        auto pCfg = ::queryUIMiniBars();
        if (!pCfg)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pCfg);
        return res;
    }

    bool Native::provisionBar(String^ name, bool builtIn, String^ tip, bool provisioned, String^ location, int x, int y, array<String^>^ funcUnits)
    {
        std::vector<std::wstring> eFuncUnits;
        if (funcUnits != nullptr) {
            for (int i = 0; i < funcUnits->Length; ++i)
                eFuncUnits.push_back(WSTR(funcUnits[i]));
        }
        
        return ::provisionBar(WSTR(name), builtIn, WSTR(tip), provisioned, WSTR(location), x, y, eFuncUnits);
    }

    bool Native::provisionLayout(String^ name, String^ tip, LoongCAD::Util::DObject^ subNodes)
    {
        if (subNodes != nullptr) {
            Engine::DObject eSubNodes;
            wrUtil::copyDObj(&eSubNodes, subNodes);
            return ::provisionLayout(WSTR(name), WSTR(tip), &eSubNodes);
        }
        else
            return ::provisionLayout(WSTR(name), WSTR(tip), nullptr);
    }
}
