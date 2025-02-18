#include "../../Native.h"

#include "Core/STCore.h"
#include "Core/Define.h"
#include "Core/Interface/Drawing/IF_Drawing.h"
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
    
    array<LoongCAD::STC::Define::DrawingType>^ Native::getDrawingTypes(array<long long>^ drIds)
    {
        if (drIds == nullptr)
            return nullptr;

        std::vector<long long> nativeIds;
        nativeIds.reserve(drIds->Length);
        for (int i = 0; i < drIds->Length; ++i) {
            nativeIds.push_back((long long)drIds[i]);
        }
        std::vector<Core::Define::DrawingType> nativeTypes = ::getDrawingTypes(nativeIds);
        if (nativeTypes.empty())
            return nullptr;

        array<LoongCAD::STC::Define::DrawingType>^ resTypes = gcnew array<LoongCAD::STC::Define::DrawingType>(nativeTypes.size());
        for (int i = 0; i < (int)nativeTypes.size(); ++i) {
            resTypes[i] = (LoongCAD::STC::Define::DrawingType)nativeTypes[i];
        }
        
        return resTypes;
    }

    String^ Native::getDrawingName(long long drId)
    {
        return WSTR(::getDrawingName(drId));
    }

    String^ Native::getDrawingMark(long long drId)
    {
        return WSTR(::getDrawingMark(drId));
    }

    array<String^>^ Native::getDrawingMarks(array<long long>^ drIds)
    {
        if (drIds == nullptr)
            return nullptr;

        std::vector<long long> nativeIds;
        nativeIds.reserve(drIds->Length);
        for (int i = 0; i < drIds->Length; ++i) {
            nativeIds.push_back((long long)drIds[i]);
        }
        std::vector<std::wstring> nativeMarks = ::getDrawingMarks(nativeIds);
        if (nativeMarks.empty())
            return nullptr;

        array<String^>^ resMarks = gcnew array<String^>(nativeMarks.size());
        for (int i = 0; i < (int)nativeMarks.size(); ++i) {
            resMarks[i] = WSTR(nativeMarks[i]);
        }

        return resMarks;
    }

    String^ Native::getDrawingNumber(long long drId)
    {
        return WSTR(::getDrawingNumber(drId));
    }

    LoongCAD::Geometry::Vec2 Native::getDrawingSize(long long drId)
    {
        return wrUtil::wrap(::getDrawingSize(drId));
    }

}