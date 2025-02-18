#include <Windows.h>
#include <string>

#include "../../Native.h"

#include "Engine/Math/Mat43.h"
#include "Engine/Util/Guid.h"

#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "../../StrUtil.h"


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
    array<long long>^ Native::curObjIds(long long sceneId) {
        std::vector<long long> ids;
        getCurObjIds(sceneId, ids);

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> pArr = &arr[0];
            memcpy(pArr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    array<long long>^ Native::curObjIds() {
        std::vector<long long> ids;
        getCurObjIds(ids);

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> pArr = &arr[0];
            memcpy(pArr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    array<long long>^ Native::curObjIds(LoongCAD::STC::Define::ObjectType objType) {
        std::vector<long long> ids;
        getCurObjIds(ids, (::sketch::Define::ObjectType)objType);

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> ptr = &arr[0];
            memcpy(ptr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    array<long long>^ Native::curObjIds(LoongCAD::STC::Define::ObjectType objType0, LoongCAD::STC::Define::ObjectType objType1) {
        std::vector<long long> ids;
        getCurObjIds(ids, (::sketch::Define::ObjectType)objType0, (::sketch::Define::ObjectType)objType1);

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> ptr = &arr[0];
            memcpy(ptr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    array<long long>^ Native::curObjIdsByBaseType(LoongCAD::STC::Define::ObjectType typeBegin, LoongCAD::STC::Define::ObjectType typeEnd) {
        std::vector<long long> ids;
        getCurObjIdsByBaseType(ids, (::sketch::Define::ObjectType)typeBegin, (::sketch::Define::ObjectType)typeEnd);

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> ptr = &arr[0];
            memcpy(ptr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    LoongCAD::Geometry::Mat43 Native::focusedWorkAxis() {
        auto m = ::focusedWorkAxis();
        return wrUtil::wrap(m);
    }

    LoongCAD::Geometry::Mat43 Native::workAxis(long long sceneId) {
        auto m = ::workAxis(sceneId);
        return wrUtil::wrap(m);
    }

    void Native::setFocusedWorkAxis(LoongCAD::Geometry::Mat43 m)
    {
        ::setFocusedWorkAxis(wrUtil::unwrap(m));
    }

    void Native::setWorkAxis(long long sceneId, LoongCAD::Geometry::Mat43 m)
    {
        ::setWorkAxis(sceneId, wrUtil::unwrap(m));
    }

    //bool Native::queryCurObjBaseInfos([Out] array<long long>^% resIds, [Out] array<LoongCAD::STC::Define::ObjectType>^% resTypes, [Out] array<bool>^% resCutters)
    //{
    //    std::vector<long long> ids;
    //    std::vector<sketch::Define::ObjectType> types;
    //    std::vector<bool> cutters;
    //    if (!::queryCurObjBaseInfos(ids, types, cutters))
    //        return false;

    //    resIds = gcnew array<long long>((int)ids.size());
    //    resTypes = gcnew array<LoongCAD::STC::Define::ObjectType>((int)types.size());
    //    resCutters = gcnew array<bool>((int)cutters.size());

    //    if (!ids.empty()) {
    //        pin_ptr<long long> ptrId = &resIds[0];
    //        memcpy(ptrId, ids.data(), sizeof(long long) * ids.size());

    //        //enum, bool不确定C++/C#层内部存储是否一致，所以不直接内存拷贝
    //        for (int i = 0; i < ids.size(); ++i) {
    //            resTypes[i] = (LoongCAD::STC::Define::ObjectType)types[i];
    //            resCutters[i] = cutters[i];
    //        }
    //    }

    //    return true;
    //}

    //bool Native::queryCurObjBaseInfos([Out] array<long long>^% resIds, [Out] array<LoongCAD::STC::Define::ObjectType>^% resTypes)
    //{
    //    std::vector<long long> ids;
    //    std::vector<sketch::Define::ObjectType> types;
    //    if (!::queryCurObjBaseInfos(ids, types))
    //        return false;

    //    resIds = gcnew array<long long>((int)ids.size());
    //    resTypes = gcnew array<LoongCAD::STC::Define::ObjectType>((int)types.size());

    //    if (!ids.empty()) {
    //        pin_ptr<long long> ptrId = &resIds[0];
    //        memcpy(ptrId, ids.data(), sizeof(long long) * ids.size());

    //        //enum, bool不确定C++/C#层内部存储是否一致，所以不直接内存拷贝
    //        for (int i = 0; i < ids.size(); ++i) {
    //            resTypes[i] = (LoongCAD::STC::Define::ObjectType)types[i];
    //        }
    //    }

    //    return true;
    //}

    //bool Native::queryNoableBaseInfos(
    //    array<long long>^% resObjIds, array<LoongCAD::STC::Define::ObjectType>^% resObjTypes,
    //    array<String^>^% resNos, array<long long>^% resAssIds)
    //{
    //    std::vector<long long> objIds;
    //    std::vector<sketch::Define::ObjectType> objTypes;
    //    std::vector<std::wstring> noList;
    //    std::vector<long long> assIds;
    //    if (!::queryNoableBaseInfos(objIds, objTypes, noList, assIds))
    //        return false;

    //    resObjIds = gcnew array<long long>((int)objIds.size());
    //    resObjTypes = gcnew array<LoongCAD::STC::Define::ObjectType>((int)objTypes.size());
    //    resNos = gcnew array<String^>((int)noList.size());
    //    resAssIds = gcnew array<long long>((int)assIds.size());
    //    
    //    if (!objIds.empty()) {
    //        // id
    //        {
    //            pin_ptr<long long> ptrId = &resObjIds[0];
    //            memcpy(ptrId, objIds.data(), sizeof(long long) * objIds.size());
    //        }
    //        //enum, bool不确定C++/C#层内部存储是否一致，所以不直接内存拷贝
    //        for (int i = 0; i < objTypes.size(); ++i) {
    //            resObjTypes[i] = (LoongCAD::STC::Define::ObjectType)objTypes[i];
    //        }
    //        // noStr
    //        for (int i = 0; i < noList.size(); ++i) {
    //            resNos[i] = WSTR(noList[i]);
    //        }
    //        // assId
    //        {
    //            pin_ptr<long long> ptrId = &resAssIds[0];
    //            memcpy(ptrId, assIds.data(), sizeof(long long) * assIds.size());
    //        }
    //    }

    //    return true;
    //}

    //bool Native::queryCADRefBaseInfos([Out] array<long long>^% objIds, [Out] array<String^>^% refPaths, [Out] array<bool>^% visibles)
    //{
    //    std::vector<long long> eObjIds;
    //    std::vector<std::wstring> eRefPaths;
    //    std::vector<bool> eVisibles;

    //    if (!::queryCADRefBaseInfos(eObjIds, eRefPaths, eVisibles)) {
    //        objIds = gcnew array<long long>(0);
    //        refPaths = gcnew array<String^>(0);
    //        visibles = gcnew array<bool>(0);
    //        return false;
    //    }
    //        
    //    objIds = gcnew array<long long>((int)eObjIds.size());
    //    refPaths = gcnew array<String^>((int)eRefPaths.size());
    //    visibles = gcnew array<bool>((int)eVisibles.size());
    //    for (int i = 0; i < eObjIds.size(); ++i) {
    //        objIds[i] = eObjIds[i];
    //        refPaths[i] = WSTR(eRefPaths[i]);
    //        visibles[i] = eVisibles[i];
    //    }

    //    return true;
    //}

    //String^ Native::querySelectGroupInfo(array<long long>^ objIds)
    //{
    //    if (objIds == nullptr)
    //        return nullptr;

    //    std::vector<long long> eIds;
    //    if (objIds->Length > 0) {
    //        pin_ptr<long long> pArr = &objIds[0];
    //        for (int i = 0; i < objIds->Length; ++i)
    //            eIds.push_back(pArr[i]);
    //    }
    //    
    //    return WSTR(::querySelectGroupInfo(eIds));
    //}

    bool Native::setCurObjs(array<long long>^ objIds)
    {
        if (objIds == nullptr)
            return false;

        std::vector<long long> eObjIds;
        if (objIds->Length > 0) {
            pin_ptr<long long> pArr = &objIds[0];
            eObjIds = { (long long*)pArr, (long long*)pArr + objIds->Length };
        }

        return ::setCurObjs(eObjIds);
    }

    //bool Native::setCurObjsByPhaseIds(array<int>^ phaseIds)
    //{
    //    if (phaseIds == nullptr)
    //        return false;

    //    if (phaseIds->Length == 0) 
    //        return ::setCurObjsByPhaseIds({});
    //    
    //    std::unordered_set<int> ePhaseIds;
    //    if (phaseIds->Length > 0) {
    //        pin_ptr<int> pArr = &phaseIds[0];
    //        ePhaseIds = { (int*)pArr, (int*)pArr + phaseIds->Length };
    //    }

    //    return ::setCurObjsByPhaseIds(ePhaseIds);
    //}

    bool Native::deleteObjs(array<long long>^ objIds)
    {
        if (objIds == nullptr)
            return false;

        std::vector<long long> nativeIds;
        if (objIds->Length > 0) {
            pin_ptr<long long> pArr = &objIds[0];
            nativeIds = { (long long*)pArr, (long long*)pArr + objIds->Length };
        }

        return ::deleteObjs(nativeIds);
    }

}
