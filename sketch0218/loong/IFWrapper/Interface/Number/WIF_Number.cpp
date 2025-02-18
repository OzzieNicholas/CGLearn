#include "../../Native.h"
#include <Windows.h>
#include <string>

#include "Core/STCore.h"
#include "Core/Define.h"
#include "Core/Interface/Number/IF_Number.h"
#include "../../StrUtil.h"



using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace Core;
using namespace LoongCAD::Geometry;
using namespace CLRUtils;

namespace IFWrapper
{

    bool Native::isPartNoDirty(String^ noStr)
    {
        return ::isPartNoDirty(WSTR(noStr));
    }

    bool Native::isAssemblyNoDirty(String^ noStr)
    {
        return ::isAssemblyNoDirty(WSTR(noStr));
    }

    array<long long>^ Native::queryPartsByNumber(String^ noStr)
    {
        std::vector<long long> partIds;
        array<long long>^ retIds = nullptr;
        if (::queryPartsByNumber(WSTR(noStr), partIds)) {
            retIds = gcnew array<long long>((int)partIds.size());
            for (int i = 0; i < partIds.size(); ++i) {
                retIds[i] = partIds[i];
            }
        }

        return retIds;
    }

    array<long long>^ Native::queryAssembliesByNumber(String^ noStr)
    {
        std::vector<long long> assIds;
        array<long long>^ retIds = nullptr;
        if (::queryAssembliesByNumber(WSTR(noStr), assIds)) {
            retIds = gcnew array<long long>((int)assIds.size());
            for (int i = 0; i < assIds.size(); ++i) {
                retIds[i] = assIds[i];
            }
        }

        return retIds;
    }

}