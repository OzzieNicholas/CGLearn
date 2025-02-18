#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
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
    array<unsigned char>^ Native::toBinary(LoongCAD::Util::DObject^ dobj)
    {
        if (dobj == nullptr)
            return nullptr;

        Engine::DObject eObj;
        wrUtil::copyDObj(&eObj, dobj);

        unsigned char* pBuf = nullptr;
        size_t bufSize = eObj.toBinary(&pBuf, "");
        if (!pBuf)
            return nullptr;

        array<unsigned char>^ res = gcnew array<unsigned char>((int)bufSize);
        pin_ptr<unsigned char> pArr = &res[0];
        memcpy(pArr, pBuf, bufSize);
        delete[] pBuf;

        return res;
    }

    LoongCAD::Util::DObject^ Native::fromBinary(array<unsigned char>^ buf)
    {
        if (buf == nullptr)
            return nullptr;

        Engine::DObject eObj;
        pin_ptr<unsigned char> pBuf = &buf[0];
        eObj.fromBinary(pBuf);

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, &eObj);
        return res;
    }

}
