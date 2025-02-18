#include <Windows.h>
#include <string>

#include "../../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/Main/IF_SceneObject.h"
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
	System::Guid Native::objGuid(long long sceneId, long long objId)
	{
		auto& eGuid = ::objGuid(sceneId, objId);
		return wrUtil::wrap(eGuid);
	}

	//array<long long>^ Native::objCtrlPointIds(long long sceneId, long long objId)
	//{
	//    auto eCpts = ::objCtrlPoints(sceneId, objId);
	//    auto arr = gcnew array<long long>((int)eCpts.size());
	//    for (int i = 0; i < eCpts.size(); ++i)
	//        arr[i] = eCpts[i]->id();
	//    return arr;
	//}

	//array<long long>^ Native::objChildIds(long long sceneId, long long objId)
	//{
	//    auto eChildren = ::objChildren(sceneId, objId);
	//    auto arr = gcnew array<long long>((int)eChildren.size());
	//    int n = 0;
	//    for (auto pChild : eChildren)
	//        arr[n++] = pChild->id();
	//    return arr;
	//}

	LoongCAD::Geometry::Mat43 Native::objMatrixLocal(long long sceneId, long long objId)
	{
		auto& eMatW = ::objMatrixLocal(sceneId, objId);
		return wrUtil::wrap(eMatW);
	}

	LoongCAD::Geometry::Mat43 Native::objMatrixObj(long long sceneId, long long objId)
	{
		auto& eMatW = ::objMatrixObj(sceneId, objId);
		return wrUtil::wrap(eMatW);
	}

	LoongCAD::Geometry::Mat43 Native::objMatrixObjW(long long sceneId, long long objId)
	{
		auto& eMatW = ::objMatrixObjW(sceneId, objId);
		return wrUtil::wrap(eMatW);
	}

	LoongCAD::Geometry::Mat43 Native::objMatrixWorld(long long sceneId, long long objId)
	{
		auto& eMatW = ::objMatrixWorld(sceneId, objId);
		return wrUtil::wrap(eMatW);
	}

	LoongCAD::Geometry::Mat43 Native::objMatrixParent(long long sceneId, long long objId)
	{
		auto& eMatW = ::objMatrixParent(sceneId, objId);
		return wrUtil::wrap(eMatW);
	}

	LoongCAD::Util::DObject^ Native::objViewProps(long long objId)
	{
		Engine::DObject viewProps;
		if (!::objViewProps(objId, viewProps))
			return nullptr;

		LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(res, &viewProps);
		return res;
	}

	LoongCAD::Util::DObject^ Native::objProps(long long objId)
	{
		Engine::DObject eProps;
		if (!::queryObjProps(objId, eProps))
			return nullptr;

		LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(res, &eProps);
		return res;
	}

	//LoongCAD::Util::DObject^ Native::objPropsWithCtrlPoints(long long objId)
	//{
	//    Engine::DObject eProps;
	//    if (!::queryObjPropsWithCtrlPoints(eProps, objId))
	//        return nullptr;

	//    LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
	//    wrUtil::copyDObj(res, &eProps);
	//    return res;
	//}

	//LoongCAD::Util::DObject^ Native::objWorkPropsWithCtrlPoints(long long objId, LoongCAD::Geometry::Mat43 workAxis)
	//{
	//    Engine::DObject eProps;
	//    if (!::queryObjWorkPropsWithCtrlPoints(eProps, objId, wrUtil::unwrap(workAxis)))
	//        return nullptr;

	//    LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
	//    wrUtil::copyDObj(res, &eProps);
	//    return res;
	//}

	LoongCAD::Util::DObject^ Native::curObjViewProps()
	{
		Engine::DObject viewProps;
		if (!::curObjViewProps(viewProps))
			return nullptr;

		LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(res, &viewProps);
		return res;
	}

	LoongCAD::Util::DObject^ Native::curObjProps()
	{
		Engine::DObject eProps;
		if (!::queryCurObjProps(eProps))
			return nullptr;

		LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
		wrUtil::copyDObj(res, &eProps);
		return res;
	}

	//array<LoongCAD::Geometry::Vec3>^ Native::objCtrlPointsW(long long objId)
	//{
	//    std::vector<Engine::Vec3> cpts;
	//    if (!::objCtrlPointsW(objId, cpts))
	//        return gcnew array<LoongCAD::Geometry::Vec3>(0);

	//    array<LoongCAD::Geometry::Vec3>^ res = gcnew array<LoongCAD::Geometry::Vec3>((int)cpts.size());
	//    for (int i = 0; i < cpts.size(); ++i)
	//        res[i] = wrUtil::wrap(cpts[i]);
	//    return res;
	//}

	bool Native::setObjectGUID(long long objId, System::Guid guid)
	{
		return ::setObjectGUID(objId, wrUtil::unwrap(guid));
	}

	bool Native::setCurObjectGUID(System::Guid guid)
	{
		return ::setCurObjectGUID(wrUtil::unwrap(guid));
	}

	bool Native::modifyObject(long long objId, LoongCAD::Util::DObject^ props)
	{
		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);
		return ::modifyObject(objId, eProps);
	}

	bool Native::modifyObjects(array<long long>^ objIds, LoongCAD::Util::DObject^ props)
	{
		std::vector<long long> eObjIds;
		for (int i = 0; i < objIds->Length; ++i)
		{
			long long id = objIds[i];
			eObjIds.push_back(id);
		}

		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);

		return ::modifyObjects(eObjIds, eProps);
	}

	bool Native::modifyCurObjects(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objType)
	{
		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);

		return ::modifyCurObjects(eProps, (sketch::Define::ObjectType)objType);
	}

	bool Native::modifyCurObjects(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objType0,
	                              LoongCAD::STC::Define::ObjectType objType1)
	{
		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);

		return ::modifyCurObjects(eProps, (sketch::Define::ObjectType)objType0, (sketch::Define::ObjectType)objType1);
	}

	//bool Native::modifyCurObjectsByBaseType(LoongCAD::Util::DObject^ props, LoongCAD::STC::Define::ObjectType objTypeBegin, LoongCAD::STC::Define::ObjectType objTypeEnd)
	//{
	//    Engine::DObject eProps;
	//    wrUtil::copyDObj(&eProps, props);

	//    return ::modifyCurObjectsByBaseType(eProps, (sketch::Define::ObjectType)objTypeBegin, (sketch::Define::ObjectType)objTypeEnd);
	//}

	//LoongCAD::Util::DObject^ Native::queryCurTweenViewProps()
	//{
	//    Engine::DObject eProps;
	//    if (!::queryCurTweenViewProps(eProps))
	//        return nullptr;

	//    LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
	//    wrUtil::copyDObj(res, &eProps);
	//    return res;
	//}

	//bool Native::modifyCurSPTweenBeams(LoongCAD::Util::DObject^ props, [Out] long long% idL, [Out] long long% idR)
	//{
	//    Engine::DObject eProps;
	//    wrUtil::copyDObj(&eProps, props);

	//    long long eIdL = -1, eIdR = -1;
	//    bool bSucc = ::modifyCurSPTweenBeams(eProps, eIdL, eIdR);
	//    idL = eIdL;
	//    idR = eIdR;
	//    return bSucc;
	//}

	long long Native::createObject(long long sceneId, LoongCAD::STC::Define::ObjectType objType,
	                               LoongCAD::Util::DObject^ props)
	{
		if (props == nullptr)
			return -1;

		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);
		return ::createObject(sceneId, (sketch::Define::ObjectType)objType, eProps);
	}

	//LoongCAD::Geometry::Plane^ Native::getBoundPlane(long long objId, LoongCAD::STC::Define::ObjectSide objSide)
	//{
	//    Engine::AxisPlane sidePlane = ::getBoundPlane(objId, (sketch::Define::ObjectSide)objSide);
	//    return wrUtil::wrap(sidePlane);
	//}

	//array<LoongCAD::Geometry::Plane^>^ Native::pickPartPlaneBySideFlags(long long objId, uint32_t flagsAND, uint32_t flagsOR, uint32_t flagsNOT)
	//{
	//    std::vector<Engine::AxisPlane> ePicked;
	//    if (!::pickPartPlaneBySideFlags(ePicked, objId, flagsAND, flagsOR, flagsNOT))
	//        return gcnew array<LoongCAD::Geometry::Plane^>(0);

	//    auto res = gcnew array<LoongCAD::Geometry::Plane^>((int)ePicked.size());
	//    for (int i = 0; i < ePicked.size(); ++i)
	//        res[i] = wrUtil::wrap(ePicked[i]);
	//    return res;
	//}

	bool Native::modifyObjHandler(long long objId, long long hid, LoongCAD::Util::DObject^ props)
	{
		if (props == nullptr)
			return ::modifyObjHandler(objId, hid, {});

		Engine::DObject eProps;
		wrUtil::copyDObj(&eProps, props);
		return ::modifyObjHandler(objId, hid, eProps);
	}
}
