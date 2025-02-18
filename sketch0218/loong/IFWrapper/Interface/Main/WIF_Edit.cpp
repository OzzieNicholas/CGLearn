#include <Windows.h>
#include <string>

#include "../../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/Main/IF_Edit.h"
#include "../../StrUtil.h"



using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace sketch;
using namespace LoongCAD::Geometry;
using namespace LoongCAD::STC;
using namespace CLRUtils;

namespace IFWrapper
{
    LoongCAD::STC::Define::CopyResult Native::copyCurObjsByLineStrideInWorkSpace(array<double>^ dx, array<double>^ dy, array<double>^ dz, int count)
    {
        if (dx == nullptr || dy == nullptr || dz == nullptr)
            return LoongCAD::STC::Define::CopyResult::COPY_RESULT_FAILED;

        std::vector<double> edx, edy, edz;
        for (int i = 0; i < dx->Length; ++i)
            edx.push_back((double)dx[i]);
        for (int i = 0; i < dy->Length; ++i)
            edy.push_back((double)dy[i]);
        for (int i = 0; i < dz->Length; ++i)
            edz.push_back((double)dz[i]);

        return (LoongCAD::STC::Define::CopyResult)::copyCurObjsByLineStrideInWorkSpace(edx, edy, edz, count);
    }

    LoongCAD::STC::Define::CopyResult Native::copyCurObjsByRotateInWorkSpace(double x0, double y0, double z0, LoongCAD::STC::Define::SpaceAxis axis, array<double>^ angls, double zoff, int count)
    {
        if (angls == nullptr || angls->Length == 0)
            return LoongCAD::STC::Define::CopyResult::COPY_RESULT_SUCCESS;

        pin_ptr<double> pAngls = &angls[0];

        return (LoongCAD::STC::Define::CopyResult)::copyCurObjsByRotateInWorkSpace(x0, y0, z0, (sketch::Define::SpaceAxis)axis, (double*)pAngls, angls->Length, zoff, count);
    }


}
