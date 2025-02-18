#include "wrUtil.h"
#include <math.h>
#include <vcclr.h>

#include "Engine/Math/AxisPlane.h"
#include "Engine/Math/Mat4.h"
#include "Engine/Math/Mat43.h"
#include "Engine/Math/OBB.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Math/Vec2i.h"
#include "Engine/Math/Vec3.h"
#include "Engine/Math/Vec3i.h"
#include "Engine/Math/Vec4.h"
#include "Engine/Math/Vec4i.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/Guid.h"
#include "Engine/Util/Log.h"

#include "StrUtil.h"

using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;
using namespace Engine;

namespace CLRUtils
{
void wrUtil::copyDObj(LoongCAD::Util::DObject ^ pDst, const Engine::DObject* pSrc)
{
    if(pSrc == nullptr)
        return;

    auto& eObj = *pSrc;
    for(auto& it: eObj)
    {
        String ^ key = STR(it.first);
        pDst->Add(key, wrap(it.second));
    }
}

void wrUtil::copyDObj(Engine::DObject* pDst, LoongCAD::Util::DObject ^ pSrc)
{
    if(pSrc == nullptr)
        return;

    auto& eObj = *pDst;
    for each(auto& it in pSrc)
    {
        std::string key = STR(it.Key);
        Object ^ pObj = it.Value;
        if(pObj == nullptr)
            continue;

        auto& dval = eObj[key];
        unwrap(&dval, pObj);
    }
}

LoongCAD::Util::DObject ^ wrUtil::wrap(const Engine::DObject* v)
{
    if(v == nullptr)
        return nullptr;

    LoongCAD::Util::DObject ^ res = gcnew LoongCAD::Util::DObject();
    copyDObj(res, v);
    return res;
}

System::Object ^ wrUtil::wrap(const Engine::DValue& v)
{
    Engine::DValue::DataType type = v.type();

    switch(type)
    {
    case Engine::DValue::TYPE_DOBJECT: {
        LoongCAD::Util::DObject ^ dstObj = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(dstObj, v._dobj());
        return dstObj;
    }
    case Engine::DValue::TYPE_INT: return v._int();
    case Engine::DValue::TYPE_UINT: return v._uint();
    case Engine::DValue::TYPE_LONG: return v._long();
    case Engine::DValue::TYPE_ULONG: return v._ulong();
    case Engine::DValue::TYPE_DOUBLE: return v._double();
    case Engine::DValue::TYPE_SHORT: return v._short();
    case Engine::DValue::TYPE_USHORT: return v._ushort();
    case Engine::DValue::TYPE_BOOL: return v._bool();
    case Engine::DValue::TYPE_CHAR: return v._char();
    case Engine::DValue::TYPE_VEC2: return wrap(v._vec2());
    case Engine::DValue::TYPE_VEC3: return wrap(v._vec3());
    case Engine::DValue::TYPE_VEC4: return wrap(v._vec4());
    case Engine::DValue::TYPE_VEC2I: return wrap(v._vec2i());
    case Engine::DValue::TYPE_VEC3I: return wrap(v._vec3i());
    case Engine::DValue::TYPE_VEC4I: return wrap(v._vec4i());
    case Engine::DValue::TYPE_LINE: return wrap(v._line());
    case Engine::DValue::TYPE_MAT43: return wrap(v._mat43());
    case Engine::DValue::TYPE_MAT4: return wrap(v._mat4());
    case Engine::DValue::TYPE_AABB: return wrap(v._aabb());
    case Engine::DValue::TYPE_OBB: return wrap(v._obb());
    case Engine::DValue::TYPE_AXIS_PLANE: return wrap(v._axisPlane());
    case Engine::DValue::TYPE_GUID: return wrap(v._guid());
    case Engine::DValue::TYPE_WSTR: return WSTR(v._wstr());
    case Engine::DValue::TYPE_BOOL_ARRAY: {
        size_t count = v.len();
        bool* pArrSrc = v._boolArr();
        array<bool> ^ pArr = gcnew array<bool>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = pArrSrc[i];
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_CHAR_ARRAY: {
        size_t nArr = v.len();
        char* eArr = v._charArr();
        array<char> ^ pArr = gcnew array<char>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<char> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(char) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_SHORT_ARRAY: {
        size_t nArr = v.len();
        short* eArr = v._shortArr();
        array<short> ^ pArr = gcnew array<short>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<short> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(short) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_USHORT_ARRAY: {
        size_t nArr = v.len();
        uint16_t* eArr = v._ushortArr();
        array<uint16_t> ^ pArr = gcnew array<uint16_t>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<uint16_t> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(uint16_t) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_INT_ARRAY: {
        size_t nArr = v.len();
        int* eArr = v._intArr();
        array<int> ^ pArr = gcnew array<int>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<int> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(int) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_UINT_ARRAY: {
        size_t nArr = v.len();
        uint32_t* eArr = v._uintArr();
        array<uint32_t> ^ pArr = gcnew array<uint32_t>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<uint32_t> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(uint32_t) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_LONG_ARRAY: {
        size_t nArr = v.len();
        int64_t* eArr = v._longArr();
        array<int64_t> ^ pArr = gcnew array<int64_t>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<int64_t> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(int64_t) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_DOUBLE_ARRAY: {
        size_t nArr = v.len();
        double* eArr = v._doubleArr();
        array<double> ^ pArr = gcnew array<double>((int)nArr);
        if(nArr > 0)
        {
            pin_ptr<double> ptr = &pArr[0];
            memcpy(ptr, eArr, sizeof(double) * nArr);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_WSTR_ARRAY: {
        size_t count = v.len();
        std::wstring* pArrSrc = v._wstrArr();
        array<String ^> ^ pArr = gcnew array<String ^>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = WSTR(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC2_ARRAY: {
        size_t count = v.len();
        Engine::Vec2* pArrSrc = v._vec2Arr();
        array<LoongCAD::Geometry::Vec2> ^ pArr = gcnew array<LoongCAD::Geometry::Vec2>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC3_ARRAY: {
        size_t count = v.len();
        Engine::Vec3* pArrSrc = v._vec3Arr();
        array<LoongCAD::Geometry::Vec3> ^ pArr = gcnew array<LoongCAD::Geometry::Vec3>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC4_ARRAY: {
        size_t count = v.len();
        Engine::Vec4* pArrSrc = v._vec4Arr();
        array<LoongCAD::Geometry::Vec4> ^ pArr = gcnew array<LoongCAD::Geometry::Vec4>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC2I_ARRAY: {
        size_t count = v.len();
        Engine::Vec2i* pArrSrc = v._vec2iArr();
        array<LoongCAD::Geometry::Vec2i> ^ pArr = gcnew array<LoongCAD::Geometry::Vec2i>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC3I_ARRAY: {
        size_t count = v.len();
        Engine::Vec3i* pArrSrc = v._vec3iArr();
        array<LoongCAD::Geometry::Vec3i> ^ pArr = gcnew array<LoongCAD::Geometry::Vec3i>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_VEC4I_ARRAY: {
        size_t count = v.len();
        Engine::Vec4i* pArrSrc = v._vec4iArr();
        array<LoongCAD::Geometry::Vec4i> ^ pArr = gcnew array<LoongCAD::Geometry::Vec4i>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_MAT43_ARRAY: {
        size_t count = v.len();
        Engine::Mat43* pArrSrc = v._mat43Arr();
        array<LoongCAD::Geometry::Mat43> ^ pArr = gcnew array<LoongCAD::Geometry::Mat43>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_MAT4_ARRAY: {
        size_t count = v.len();
        Engine::Mat4* pArrSrc = v._mat4Arr();
        array<LoongCAD::Geometry::Mat4> ^ pArr = gcnew array<LoongCAD::Geometry::Mat4>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_LINE_ARRAY: {
        size_t count = v.len();
        Engine::Line* pArrSrc = v._lineArr();
        array<LoongCAD::Geometry::Line> ^ pArr = gcnew array<LoongCAD::Geometry::Line>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_OBB_ARRAY: {
        size_t count = v.len();
        Engine::OBB* pArrSrc = v._obbArr();
        array<LoongCAD::Geometry::OBB> ^ pArr = gcnew array<LoongCAD::Geometry::OBB>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_AXIS_PLANE_ARRAY: {
        size_t count = v.len();
        Engine::AxisPlane* pArrSrc = v._axisPlaneArr();
        array<LoongCAD::Geometry::Plane ^> ^ pArr = gcnew array<LoongCAD::Geometry::Plane ^>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_DOBJECT_ARRAY: {
        size_t count = v.len();
        Engine::DObject** pArrSrc = v._dobjArr();
        array<LoongCAD::Util::DObject ^> ^ pArr = gcnew array<LoongCAD::Util::DObject ^>((int)count);
        for(int i = 0; i < count; ++i)
        {
            LoongCAD::Util::DObject ^ dstObj = gcnew LoongCAD::Util::DObject();
            wrUtil::copyDObj(dstObj, pArrSrc[i]);
            pArr[i] = dstObj;
        }
        return pArr;
    }
    break;
    case Engine::DValue::TYPE_DVALUE_ARRAY: {
        size_t count = v.len();
        Engine::DValue* pArrSrc = v._dvalArr();
        array<System::Object ^> ^ pArr = gcnew array<System::Object ^>((int)count);
        for(int i = 0; i < count; ++i)
        {
            pArr[i] = wrap(pArrSrc[i]);
        }
        return pArr;
    }
    break;
    }

    return nullptr;
}

LoongCAD::Geometry::Vec2 wrUtil::wrap(const Engine::Vec2& v) { return LoongCAD::Geometry::Vec2(v.x, v.y); }

LoongCAD::Geometry::Vec3 wrUtil::wrap(const Engine::Vec3& v) { return LoongCAD::Geometry::Vec3(v.x, v.y, v.z); }

LoongCAD::Geometry::Vec4 wrUtil::wrap(const Engine::Vec4& v) { return LoongCAD::Geometry::Vec4(v.x, v.y, v.z, v.w); }

LoongCAD::Geometry::Vec2i wrUtil::wrap(const Engine::Vec2i& v) { return LoongCAD::Geometry::Vec2i(v.x, v.y); }

LoongCAD::Geometry::Vec3i wrUtil::wrap(const Engine::Vec3i& v) { return LoongCAD::Geometry::Vec3i(v.x, v.y, v.z); }

LoongCAD::Geometry::Vec4i wrUtil::wrap(const Engine::Vec4i& v) { return LoongCAD::Geometry::Vec4i(v.x, v.y, v.z, v.w); }

LoongCAD::Geometry::Mat43 wrUtil::wrap(const Engine::Mat43& v)
{
    return LoongCAD::Geometry::Mat43(v.x.x, v.x.y, v.x.z, v.y.x, v.y.y, v.y.z, v.z.x, v.z.y, v.z.z, v.pos.x, v.pos.y, v.pos.z);
}

LoongCAD::Geometry::Mat4 wrUtil::wrap(const Engine::Mat4& v)
{
    return LoongCAD::Geometry::Mat4(v.x.x, v.x.y, v.x.z, v.x.w, v.y.x, v.y.y, v.y.z, v.y.w, v.z.x, v.z.y, v.z.z, v.z.w, v.pos.x, v.pos.y, v.pos.z,
                                    v.pos.w);
}

LoongCAD::Geometry::Line wrUtil::wrap(const Engine::Line& v)
{
    return LoongCAD::Geometry::Line(LoongCAD::Geometry::Vec3(v.v0.x, v.v0.y, v.v0.z), LoongCAD::Geometry::Vec3(v.v1.x, v.v1.y, v.v1.z));
}

LoongCAD::Geometry::Plane ^ wrUtil::wrap(const Engine::AxisPlane& v)
{
    auto& m = v.matrix();
#if 0
		return LoongCAD::Geometry::Plane(m.x.x, m.x.y, m.x.z,
									m.y.x, m.y.y, m.y.z,
									m.z.x, m.z.y, m.z.z,
									m.pos.x, m.pos.y, m.pos.z);
#else
    if(m != Mat43::ZERO)
    {
        return gcnew LoongCAD::Geometry::Plane(m.x.x, m.x.y, m.x.z, m.y.x, m.y.y, m.y.z, m.z.x, m.z.y, m.z.z, m.pos.x, m.pos.y, m.pos.z);
    }
    return nullptr;
#endif
}

System::Guid wrUtil::wrap(const Engine::Guid& v)
{
    return System::Guid(v.A, v.B, v.C, v.D[0], v.D[1], v.D[2], v.D[3], v.D[4], v.D[5], v.D[6], v.D[7]);
}

LoongCAD::Geometry::OBB wrUtil::wrap(const Engine::OBB& v)
{
    auto& vMin = v.m_min;
    auto& vMax = v.m_max;
    auto& vMat = v.matrix();

    return LoongCAD::Geometry::OBB(LoongCAD::Geometry::Vec3(vMin.x, vMin.y, vMin.z), LoongCAD::Geometry::Vec3(vMax.x, vMax.y, vMax.z),
                                   LoongCAD::Geometry::Mat43(vMat.x.x, vMat.x.y, vMat.x.z, vMat.y.x, vMat.y.y, vMat.y.z, vMat.z.x, vMat.z.y, vMat.z.z,
                                                             vMat.pos.x, vMat.pos.y, vMat.pos.z));
}

LoongCAD::Geometry::AABB wrUtil::wrap(const Engine::AABB& v)
{
    auto& vMin = v.m_min;
    auto& vMax = v.m_max;
    return LoongCAD::Geometry::AABB(LoongCAD::Geometry::Vec3(vMin.x, vMin.y, vMin.z), LoongCAD::Geometry::Vec3(vMax.x, vMax.y, vMax.z));
}

bool wrUtil::unwrap(Engine::DValue* pDst, System::Object ^ pObj)
{
    System::Type ^ objType = pObj->GetType();

    if(objType == Int32::typeid)
    {
        (*pDst) = (Int32)pObj;
        return true;
    }
    else if(objType == Single::typeid)
    {
        (*pDst) = (Single)pObj;
        return true;
    }
    else if(objType == UInt32::typeid)
    {
        (*pDst) = (UInt32)pObj;
        return true;
    }
    else if(objType == Int64::typeid)
    {
        (*pDst) = (Int64)pObj;
        return true;
    }
    else if(objType == UInt64::typeid)
    {
        (*pDst) = (UInt64)pObj;
        return true;
    }
    else if(objType == Double::typeid)
    {
        (*pDst) = (Double)pObj;
        return true;
    }
    else if(objType == Boolean::typeid)
    {
        (*pDst) = (Boolean)pObj;
        return true;
    }
    else if(objType == Char::typeid)
    {
        (*pDst) = (Char)pObj;
        return true;
    }
    else if(objType == SByte::typeid)
    {
        (*pDst) = (SByte)pObj;
        return true;
    }
    else if(objType == Byte::typeid)
    {
        (*pDst) = (Byte)pObj;
        return true;
    }
    else if(objType == Int16::typeid)
    {
        (*pDst) = (Int16)pObj;
        return true;
    }
    else if(objType == UInt16::typeid)
    {
        (*pDst) = (UInt16)pObj;
        return true;
    }
    else if(objType == String::typeid)
    {
        (*pDst) = WSTR((String ^) pObj);
        return true;
    }
    else if(objType == LoongCAD::Util::DObject::typeid)
    {
        LoongCAD::Util::DObject ^ pSrc = (LoongCAD::Util::DObject ^) pObj;
        Engine::DObject* pDObj = Engine::DObject::alloc();
        copyDObj(pDObj, pSrc);

        pDst->set(pDObj, true);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec2::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec2)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec3::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec3)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec4::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec4)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec2i::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec2i)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec3i::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec3i)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Vec4i::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Vec4i)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Line::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Line)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Mat43::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Mat43)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Mat4::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Mat4)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::AABB::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::AABB)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::OBB::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::OBB)pObj);
        return true;
    }
    else if(objType == LoongCAD::Geometry::Plane::typeid)
    {
        (*pDst) = unwrap((LoongCAD::Geometry::Plane ^) pObj);
        return true;
    }
    else if(objType == System::Guid::typeid)
    {
        (*pDst) = unwrap((System::Guid)pObj);
        return true;
    }
    else if(objType == array<bool>::typeid)
    {
        array<bool> ^ pArrObj = (array<bool> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            bool* pArr = new bool[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((bool*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<char>::typeid)
    {
        array<char> ^ pArrObj = (array<char> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            char* pArr = new char[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((char*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<short>::typeid)
    {
        array<short> ^ pArrObj = (array<short> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            short* pArr = new short[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((short*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<uint16_t>::typeid)
    {
        array<uint16_t> ^ pArrObj = (array<uint16_t> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            uint16_t* pArr = new uint16_t[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((uint16_t*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<int>::typeid)
    {
        array<int> ^ pArrObj = (array<int> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            int* pArr = new int[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((int*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == List<int>::typeid)
    {
        List<int> ^ pArrObj = (List<int> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            int* pArr = new int[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((int*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<uint32_t>::typeid)
    {
        array<uint32_t> ^ pArrObj = (array<uint32_t> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            uint32_t* pArr = new uint32_t[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((uint32_t*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == List<uint32_t>::typeid)
    {
        List<uint32_t> ^ pArrObj = (List<uint32_t> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            uint32_t* pArr = new uint32_t[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((uint32_t*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<int64_t>::typeid)
    {
        array<int64_t> ^ pArrObj = (array<int64_t> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            int64_t* pArr = new int64_t[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((int64_t*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == List<int64_t>::typeid)
    {
        List<int64_t> ^ pArrObj = (List<int64_t> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            int64_t* pArr = new int64_t[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((int64_t*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<double>::typeid)
    {
        array<double> ^ pArrObj = (array<double> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            double* pArr = new double[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((double*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == List<double>::typeid)
    {
        List<double> ^ pArrObj = (List<double> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            double* pArr = new double[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = pArrObj[i];
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((double*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<String ^>::typeid)
    {
        array<String ^> ^ pArrObj = (array<String ^> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            std::wstring* pArr = new std::wstring[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = WSTR(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((std::wstring*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == List<String ^>::typeid)
    {
        List<String ^> ^ pArrObj = (List<String ^> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            std::wstring* pArr = new std::wstring[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = WSTR(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((std::wstring*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == SortedSet<String ^>::typeid)
    {
        SortedSet<String ^> ^ pArrObj = (SortedSet<String ^> ^) pObj;
        int len = pArrObj->Count;
        if(len > 0)
        {
            std::wstring* pArr = new std::wstring[pArrObj->Count];
            int n = 0;
            for each(String ^ s in pArrObj)
            {
                pArr[n++] = WSTR(s);
            }
            pDst->set(pArr, pArrObj->Count, true);
        }
        else
        {
            pDst->set((std::wstring*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec2>::typeid)
    {
        array<LoongCAD::Geometry::Vec2> ^ pArrObj = (array<LoongCAD::Geometry::Vec2> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec2* pArr = new Vec2[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec2*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec3>::typeid)
    {
        array<LoongCAD::Geometry::Vec3> ^ pArrObj = (array<LoongCAD::Geometry::Vec3> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec3* pArr = new Vec3[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec3*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec4>::typeid)
    {
        array<LoongCAD::Geometry::Vec4> ^ pArrObj = (array<LoongCAD::Geometry::Vec4> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec4* pArr = new Vec4[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec4*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec2i>::typeid)
    {
        array<LoongCAD::Geometry::Vec2i> ^ pArrObj = (array<LoongCAD::Geometry::Vec2i> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec2i* pArr = new Vec2i[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec2i*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec3i>::typeid)
    {
        array<LoongCAD::Geometry::Vec3i> ^ pArrObj = (array<LoongCAD::Geometry::Vec3i> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec3i* pArr = new Vec3i[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec3i*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Vec4i>::typeid)
    {
        array<LoongCAD::Geometry::Vec4i> ^ pArrObj = (array<LoongCAD::Geometry::Vec4i> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Vec4i* pArr = new Vec4i[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Vec4i*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Mat43>::typeid)
    {
        array<LoongCAD::Geometry::Mat43> ^ pArrObj = (array<LoongCAD::Geometry::Mat43> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Mat43* pArr = new Mat43[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Mat43*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Mat4>::typeid)
    {
        array<LoongCAD::Geometry::Mat4> ^ pArrObj = (array<LoongCAD::Geometry::Mat4> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Mat4* pArr = new Mat4[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Mat4*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Line>::typeid)
    {
        array<LoongCAD::Geometry::Line> ^ pArrObj = (array<LoongCAD::Geometry::Line> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            Line* pArr = new Line[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((Line*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::Plane ^>::typeid)
    {
        array<LoongCAD::Geometry::Plane ^> ^ pArrObj = (array<LoongCAD::Geometry::Plane ^> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            AxisPlane* pArr = new AxisPlane[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((AxisPlane*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Geometry::OBB>::typeid)
    {
        array<LoongCAD::Geometry::OBB> ^ pArrObj = (array<LoongCAD::Geometry::OBB> ^) pObj;
        int len = pArrObj->Length;
        if(len > 0)
        {
            OBB* pArr = new OBB[len];
            for(int i = 0; i < len; ++i)
            {
                pArr[i] = unwrap(pArrObj[i]);
            }
            pDst->set(pArr, len, true);
        }
        else
        {
            pDst->set((OBB*)nullptr, len, true);
        }
        return true;
    }
    else if(objType == array<LoongCAD::Util::DObject ^>::typeid)
    {
        array<LoongCAD::Util::DObject ^> ^ pArrSrc = (array<LoongCAD::Util::DObject ^> ^) pObj;
        int len = pArrSrc->Length;
        std::vector<Engine::DObject*> vArr;
        for(int i = 0; i < len; ++i)
        {
            if(pArrSrc[i])
            {
                Engine::DObject* pDObj = Engine::DObject::alloc();
                copyDObj(pDObj, pArrSrc[i]);
                vArr.push_back(pDObj);
            }
            else
            {
                vArr.push_back(nullptr);
            }
        }
        pDst->set(vArr, true);
        return true;
    }
    else if(objType == List<LoongCAD::Util::DObject ^>::typeid)
    {
        List<LoongCAD::Util::DObject ^> ^ pArrSrc = (List<LoongCAD::Util::DObject ^> ^) pObj;
        int len = pArrSrc->Count;
        std::vector<Engine::DObject*> vArr;
        for(int i = 0; i < len; ++i)
        {
            if(pArrSrc[i])
            {
                Engine::DObject* pDObj = Engine::DObject::alloc();
                copyDObj(pDObj, pArrSrc[i]);
                vArr.push_back(pDObj);
            }
            else
            {
                vArr.push_back(nullptr);
            }
        }
        pDst->set(vArr, true);
        return true;
    }
    else if(objType == Dictionary<String ^, LoongCAD::Util::DObject ^>::typeid)
    {
        Dictionary<String ^, LoongCAD::Util::DObject ^> ^ pArrSrc = (Dictionary<String ^, LoongCAD::Util::DObject ^> ^) pObj;
        int len = pArrSrc->Count;
        std::vector<Engine::DObject*> vArr;
        for each(auto it in pArrSrc)
        {
            if(it.Value == nullptr)
            {
                Engine::DObject* pDObj = Engine::DObject::alloc();
                copyDObj(pDObj, it.Value);
                vArr.push_back(pDObj);
            }
            else
            {
                vArr.push_back(nullptr);
            }
        }

        pDst->set(vArr, true);
        return true;
    }
    else if(objType == array<System::Object ^>::typeid)
    {
        array<System::Object ^> ^ pArrSrc = (array<System::Object ^> ^) pObj;
        int len = pArrSrc->Length;
        Engine::DValue* vArr = (len > 0) ? new Engine::DValue[len] : nullptr;
        for(int i = 0; i < len; ++i)
        {
            if(!pArrSrc[i])
                continue;

            unwrap(vArr + i, pArrSrc[i]);
        }
        pDst->set(vArr, len, true);
        return true;
    }
    else if(objType == List<System::Object ^>::typeid)
    {
        List<System::Object ^> ^ pArrSrc = (List<System::Object ^> ^) pObj;
        int len = pArrSrc->Count;
        Engine::DValue* vArr = (len > 0) ? new Engine::DValue[len] : nullptr;
        for(int i = 0; i < len; ++i)
        {
            if(!pArrSrc[i])
                continue;

            unwrap(vArr + i, pArrSrc[i]);
        }
        pDst->set(vArr, len, true);
        return true;
    }

    // Common.Definition
    if(objType->FullName->StartsWith("LoongCAD.STC.Define"))
    {
        System::Type ^ dataType = objType->GetEnumUnderlyingType();
        if(dataType == Int32::typeid)
        {
            if(objType->Name->LastIndexOf("[]") < 0)
            {
                (*pDst) = (int)pObj;
            }
            else
            {
                Array ^ pArrObj = (Array ^) pObj;
                int len = pArrObj->Length;

                if(len > 0)
                {
                    int* pArr = new int[len];
                    for(int i = 0; i < len; ++i)
                    {
                        pArr[i] = (int)pArrObj->GetValue(i);
                    }
                    pDst->set(pArr, len, true);
                }
                else
                {
                    pDst->set((int*)nullptr, len, true);
                }
            }
        }
        else if(dataType == Int16::typeid)
        {
            if(objType->Name->LastIndexOf("[]") < 0)
            {
                (*pDst) = (short)pObj;
            }
            else
            {
                Array ^ pArrObj = (Array ^) pObj;
                int len = pArrObj->Length;

                if(len > 0)
                {
                    short* pArr = new short[len];
                    for(int i = 0; i < len; ++i)
                    {
                        pArr[i] = (short)pArrObj->GetValue(i);
                    }
                    pDst->set(pArr, len, true);
                }
                else
                {
                    pDst->set((short*)nullptr, len, true);
                }
            }
        }
        else if(dataType == SByte::typeid)
        {
            if(objType->Name->LastIndexOf("[]") < 0)
            {
                (*pDst) = (char)pObj;
            }
            else
            {
                Array ^ pArrObj = (Array ^) pObj;
                int len = pArrObj->Length;

                if(len > 0)
                {
                    char* pArr = new char[len];
                    for(int i = 0; i < len; ++i)
                    {
                        pArr[i] = (char)pArrObj->GetValue(i);
                    }
                    pDst->set(pArr, len, true);
                }
                else
                {
                    pDst->set((char*)nullptr, len, true);
                }
            }
        }

        return true;
    }

    // Non-builtin class Array
    if(objType->Name->LastIndexOf("[]") >= 0)
    {
        Array ^ pArrObj = (Array ^) pObj;
        int len = pArrObj->Length;

        if(len > 0)
        {// pick first element to evaluate type
            System::Type ^ eleType = pArrObj->GetValue(0)->GetType();
            if(eleType == LoongCAD::Geometry::Vec2::typeid)
            {
                Vec2* pArr = new Vec2[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec2)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Vec3::typeid)
            {
                Vec3* pArr = new Vec3[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec3)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Vec4::typeid)
            {
                Vec4* pArr = new Vec4[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec4)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Vec2i::typeid)
            {
                Vec2i* pArr = new Vec2i[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec2i)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Vec3i::typeid)
            {
                Vec3i* pArr = new Vec3i[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec3i)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Vec4i::typeid)
            {
                Vec4i* pArr = new Vec4i[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Vec4i)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Mat43::typeid)
            {
                Mat43* pArr = new Mat43[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Mat43)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Mat4::typeid)
            {
                Mat4* pArr = new Mat4[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Mat4)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Line::typeid)
            {
                Line* pArr = new Line[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Line)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::OBB::typeid)
            {
                OBB* pArr = new OBB[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::OBB)pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
            else if(eleType == LoongCAD::Geometry::Plane::typeid)
            {
                AxisPlane* pArr = new AxisPlane[len];
                for(int i = 0; i < len; ++i)
                {
                    pArr[i] = unwrap((LoongCAD::Geometry::Plane ^) pArrObj->GetValue(i));
                }
                pDst->set(pArr, len, true);
                return true;
            }
        }
        else
        {// empty array, no element to evaluate type
            String ^ eleType = objType->Name->Substring(0, objType->Name->IndexOf("[]"));
            if(eleType == "Vec2")
            {
                pDst->set((Vec2*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Vec3")
            {
                pDst->set((Vec3*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Vec4")
            {
                pDst->set((Vec4*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Vec2i")
            {
                pDst->set((Vec2i*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Vec3i")
            {
                pDst->set((Vec3i*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Vec4i")
            {
                pDst->set((Vec4i*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Mat43")
            {
                pDst->set((Mat43*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Mat4")
            {
                pDst->set((Mat4*)nullptr, len, true);
                return true;
            }
            else if(eleType == "Line")
            {
                pDst->set((Line*)nullptr, len, true);
                return true;
            }
            else if(eleType == "OBB")
            {
                pDst->set((OBB*)nullptr, len, true);
                return true;
            }
            else if(eleType == "AxisPlane")
            {
                pDst->set((AxisPlane*)nullptr, len, true);
                return true;
            }
        }
    }

    Engine::Log::log("", "wrUtil::copyDObj(DObject*, DObject^): unknown type >> %s\n", STR(objType->FullName).c_str());
    return false;
}

Engine::Vec2 wrUtil::unwrap(const LoongCAD::Geometry::Vec2 ^ v) { return { v->X, v->Y }; }

Engine::Vec3 wrUtil::unwrap(const LoongCAD::Geometry::Vec3 ^ v) { return { v->X, v->Y, v->Z }; }

Engine::Vec4 wrUtil::unwrap(const LoongCAD::Geometry::Vec4 ^ v) { return { v->X, v->Y, v->Z, v->W }; }

Engine::Vec2i wrUtil::unwrap(const LoongCAD::Geometry::Vec2i ^ v) { return { v->X, v->Y }; }

Engine::Vec3i wrUtil::unwrap(const LoongCAD::Geometry::Vec3i ^ v) { return { v->X, v->Y, v->Z }; }

Engine::Vec4i wrUtil::unwrap(const LoongCAD::Geometry::Vec4i ^ v) { return { v->X, v->Y, v->Z, v->W }; }

Engine::Mat43 wrUtil::unwrap(const LoongCAD::Geometry::Mat43 ^ v)
{
    return { v->X.X, v->X.Y, v->X.Z, v->Y.X, v->Y.Y, v->Y.Z, v->Z.X, v->Z.Y, v->Z.Z, v->Pos.X, v->Pos.Y, v->Pos.Z };
}

Engine::Mat4 wrUtil::unwrap(const LoongCAD::Geometry::Mat4 ^ v)
{
    return { v->X.X, v->X.Y, v->X.Z, v->X.W, v->Y.X, v->Y.Y, v->Y.Z, v->Y.W, v->Z.X, v->Z.Y, v->Z.Z, v->Z.W, v->Pos.X, v->Pos.Y, v->Pos.Z, v->Pos.W };
}

Engine::Line wrUtil::unwrap(const LoongCAD::Geometry::Line ^ v) { return { { v->V0.X, v->V0.Y, v->V0.Z }, { v->V1.X, v->V1.Y, v->V1.Z } }; }

Engine::AxisPlane wrUtil::unwrap(LoongCAD::Geometry::Plane ^ v)
{
    return { { v->Matrix.X.X, v->Matrix.X.Y, v->Matrix.X.Z, v->Matrix.Y.X, v->Matrix.Y.Y, v->Matrix.Y.Z, v->Matrix.Z.X, v->Matrix.Z.Y, v->Matrix.Z.Z,
               v->Matrix.Pos.X, v->Matrix.Pos.Y, v->Matrix.Pos.Z } };
}

Engine::Guid wrUtil::unwrap(System::Guid ^ v)
{
    auto bytes = v->ToByteArray();
    if(bytes->Length == 0)
        return Engine::Guid::EMPTY;

    pin_ptr<unsigned char> pArr = &bytes[0];
    return Engine::Guid((uint64_t*)pArr);
}

Engine::OBB wrUtil::unwrap(LoongCAD::Geometry::OBB ^ v)
{
    return { { v->Min.X, v->Min.Y, v->Min.Z },
             { v->Max.X, v->Max.Y, v->Max.Z },
             { v->Matrix.X.X, v->Matrix.X.Y, v->Matrix.X.Z, v->Matrix.Y.X, v->Matrix.Y.Y, v->Matrix.Y.Z, v->Matrix.Z.X, v->Matrix.Z.Y, v->Matrix.Z.Z,
               v->Matrix.Pos.X, v->Matrix.Pos.Y, v->Matrix.Pos.Z } };
}

Engine::AABB wrUtil::unwrap(LoongCAD::Geometry::AABB ^ v) { return { { v->Min.X, v->Min.Y, v->Min.Z }, { v->Max.X, v->Max.Y, v->Max.Z } }; }
}// namespace CLRUtils