#include <Windows.h>
#include <string>

#include "../../Native.h"
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
    bool Native::getAllViewIdAndNames([Out] array<long long>^% ids, [Out] array<String^>^% names, bool incTemp)
    {
        std::vector<long long> eIds;
        std::vector<std::wstring> eNames;
        if (!::getAllViewIdAndNames(eIds, eNames, incTemp)) 
            return false;
        
        ids = gcnew array<long long>((int)eIds.size());
        if (!eIds.empty()) {
            pin_ptr<long long> pArr = &ids[0];
            memcpy(pArr, eIds.data(), sizeof(long long) * eIds.size());
        }
        
        names = gcnew array<String^>((int)eNames.size());
        for (int i = 0; i < eNames.size(); ++i)
            names[i] = WSTR(eNames[i]);

        return true;
    }

    bool Native::getActiveViewIdAndNames([Out] array<long long>^% ids, [Out] array<String^>^% names, bool incTemp)
    {
        std::vector<long long> eIds;
        std::vector<std::wstring> eNames;
        if (!::getActiveViewIdAndNames(eIds, eNames, incTemp))
            return false;

        ids = gcnew array<long long>((int)eIds.size());
        if (!eIds.empty()) {
            pin_ptr<long long> pArr = &ids[0];
            memcpy(pArr, eIds.data(), sizeof(long long) * eIds.size());
        }

        names = gcnew array<String^>((int)eNames.size());
        for (int i = 0; i < eNames.size(); ++i)
            names[i] = WSTR(eNames[i]);

        return true;
    }

    array<long long>^ Native::focusedViewIds() {
        auto ids = ::focusedViewIds();

        array<long long>^ arr = gcnew array<long long>((int)ids.size());
        if (!ids.empty()) {
            pin_ptr<long long> pArr = &arr[0];
            memcpy(pArr, ids.data(), sizeof(long long) * ids.size());
        }
        return arr;
    }

    LoongCAD::Util::DObject^ Native::focusedViewProps() {
        auto pProps = ::focusedViewProps();
        if (!pProps)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pProps);
        return res;
    }

    LoongCAD::Util::DObject^ Native::focusedViewDisplaySettingProps() {
        //auto pProps = ::focusedViewProps();
        //if (!pProps)
        //    return nullptr;
        //
        //auto it = pProps->find("disp_setting");
        //if (it == pProps->end()
        //    || it->second.type() != Engine::DValue::TYPE_DOBJECT)
        //    return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        //wrUtil::copyDObj(res, it->second._dobj());
        return res;
    }

    LoongCAD::Util::DObject^ Native::viewProps(long long id)
    {
        auto pProps = ::viewProps(id);
        if (!pProps)
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, pProps);
        return res;
    }

    bool Native::modifyView(long long id, LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;
        
        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);

        return ::modifyView(id, eProps);
    }

    bool Native::modifyViews(array<long long>^ ids, LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);

        pin_ptr<long long> pArr = &ids[0];
        return ::modifyViews(pArr, ids->Length, eProps);
    }

    bool Native::modifyFocusedViews(LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        return ::modifyFocusedViews(eProps);
    }

    bool Native::modifyDisplaySettingInFocusedViews(LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        return ::modifyDisplaySettingInFocusedViews(eProps);
    }

    bool Native::modifyDisplayFilterInFocusedViews(LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        return ::modifyDisplayFilterInFocusedViews(eProps);
    }

    //array<String^>^ Native::objectFilterPropList(LoongCAD::STC::Define::ObjectFilterType type)
    //{
    //    auto& vec = ::objectFilterPropList((sketch::Define::ObjectFilterType)type);

    //    array<String^>^ res = gcnew array<String^>((int)vec.size());
    //    for (int i = 0; i < vec.size(); ++i)
    //        res[i] = WSTR(vec[i]);
    //    return res;
    //}
    //
    //array<long long>^ Native::createModelViewsByCurGrid(LoongCAD::STC::Define::ViewNoRange modeX, String^ prefixX, String^ propsX,
    //                                                    LoongCAD::STC::Define::ViewNoRange modeY, String^ prefixY, String^ propsY,
    //                                                    LoongCAD::STC::Define::ViewNoRange modeZ, String^ prefixZ, String^ propsZ)
    //{
    //    if (prefixX == nullptr || propsX == nullptr
    //        || prefixY == nullptr || propsY == nullptr
    //        || prefixZ == nullptr || propsZ == nullptr)
    //        return nullptr;

    //    std::vector<long long> ids;
    //    if (!::createModelViewsByCurGrid(ids, (sketch::Define::ViewNoRange)modeX, WSTR(prefixX).c_str(), WSTR(propsX),
    //                                            (sketch::Define::ViewNoRange)modeY, WSTR(prefixY).c_str(), WSTR(propsY),
    //                                            (sketch::Define::ViewNoRange)modeZ, WSTR(prefixZ).c_str(), WSTR(propsZ)))
    //        return nullptr;

    //    array<long long>^ arr = gcnew array<long long>((int)ids.size());
    //    if (!ids.empty()) {
    //        pin_ptr<long long> pArr = &arr[0];
    //        memcpy(pArr, ids.data(), sizeof(long long) * ids.size());
    //    }
    //    return arr;
    //}

    void Native::hideObjsInFocusedView(array<long long>^ ids)
    {
        if (ids == nullptr)
            return;

        std::unordered_set<long long> eIds;
        for (int i = 0; i < ids->Length; ++i) {
            long long eid = ids[i];
            eIds.insert(eid);
        }
            
        ::hideObjsInFocusedView(eIds);
    }
}
