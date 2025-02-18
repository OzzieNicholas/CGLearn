#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_Layout.h"
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
    array<long long>^ Native::removeCellsFromLayout(array<long long>^ ids)
    {
        pin_ptr<long long> eIds = &ids[0];
        auto eFailIds = ::removeCellsFromLayout(eIds, ids->Length);

        array<long long>^ res = gcnew array<long long>((int)eFailIds.size());
        int n = 0;
        for (auto id : eFailIds)
            res[n++] = id;
        return res;
    }

}
