#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/IF_Controller.h"
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
    String^ Native::controller(int slot) {
        return WSTR(::controller(slot));
    }

    bool Native::activateController(String^ name) {
        if (name == nullptr)
            return false;

        return ::activateController(WSTR(name));
    }

    bool Native::activateController(String^ name, String^ key0, String^ val0) {
        if (name == nullptr || key0 == nullptr || val0 == nullptr)
            return false;

        return ::activateController(WSTR(name), STR(key0), WSTR(val0));
    }

    bool Native::activateController(String^ name, String^ key0, System::Guid val0) {
        if (name == nullptr || key0 == nullptr || val0 == System::Guid::Empty)
            return false;

        return ::activateController(WSTR(name), STR(key0), wrUtil::unwrap(val0));
    }

    bool Native::activateController(String^ name, String^ key0, int val0) {
        if (name == nullptr || key0 == nullptr)
            return false;

        return ::activateController(WSTR(name), STR(key0), val0);
    }

    bool Native::activateController(String^ name, String^ key0, short val0) {
        if (name == nullptr || key0 == nullptr)
            return false;

        return ::activateController(WSTR(name), STR(key0), val0);
    }

    bool Native::activateController(String^ name, String^ key0, char val0) {
        if (name == nullptr || key0 == nullptr)
            return false;

        return ::activateController(WSTR(name), STR(key0), val0);
    }

    bool Native::activateController(String^ name, String^ key0, bool val0) {
        if (name == nullptr || key0 == nullptr)
            return false;

        return ::activateController(WSTR(name), STR(key0), val0);
    }

    bool Native::activateController(String^ name, LoongCAD::Util::DObject^ paras) {
        if (paras == nullptr) {
            ::activateController(WSTR(name), nullptr);
            return true;
        }
        
        Engine::DObject eParas;
        wrUtil::copyDObj(&eParas, paras);
        return ::activateController(WSTR(name), &eParas);
    }

    bool Native::modifyController(String^ name, LoongCAD::Util::DObject^ paras) {
        if (name == nullptr || paras == nullptr)
            return false;

        Engine::DObject eParas;
        wrUtil::copyDObj(&eParas, paras);
        return ::modifyController(WSTR(name), eParas);
    }

}
