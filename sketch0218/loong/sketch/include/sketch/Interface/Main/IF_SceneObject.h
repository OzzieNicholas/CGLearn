#pragma once

#include <vector>
#include <unordered_set>

#include <Windows.h>
#include "../../Core.h"
#include "../../Define.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/Guid.h"
#include "Engine/Math/Mat43.h"

namespace Engine
{
	class DObject;
	class Vec3;
	class AxisPlane;
}

namespace sketch
{
	class ObjectDoc;
	class TFObjectDoc;
	class CtrlPointDoc;
}

extern "C" CORE_DLL_DECLSPEC long long objModifyCounter(long long sceneId, long long objId);
extern "C" CORE_DLL_DECLSPEC BOOL objDisposed(long long sceneId, long long objId);
extern "C" CORE_DLL_DECLSPEC long long objScene(long long objId);
extern "C" CORE_DLL_DECLSPEC sketch::Define::ObjectType objType(long long sceneId, long long objId);
extern "C" CORE_DLL_DECLSPEC uint16_t objFlags(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Guid& objGuid(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixLocal(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixObj(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Mat43 objMatrixObjW(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixWorld(long long sceneId, long long objId);
CORE_DLL_DECLSPEC const Engine::Mat43& objMatrixParent(long long sceneId, long long objId);
CORE_DLL_DECLSPEC BOOL queryObjProps(long long objId, Engine::DObject& objProps);
CORE_DLL_DECLSPEC BOOL queryCurObjProps(Engine::DObject& objProps);
CORE_DLL_DECLSPEC bool objViewProps(long long objId, Engine::DObject& viewProps);
CORE_DLL_DECLSPEC bool curObjViewProps(Engine::DObject& viewProps);
CORE_DLL_DECLSPEC BOOL setCurObjectGUID(const Engine::Guid& guid);
CORE_DLL_DECLSPEC BOOL setObjectGUID(long long objId, const Engine::Guid& guid);
CORE_DLL_DECLSPEC bool modifyObject(long long objId, const Engine::DObject& props);
CORE_DLL_DECLSPEC bool modifyObjects(const std::vector<long long>& objIds, const Engine::DObject& props);
CORE_DLL_DECLSPEC bool modifyCurObjects(const Engine::DObject& props, sketch::Define::ObjectType objType);
CORE_DLL_DECLSPEC bool modifyCurObjects(const Engine::DObject& props, sketch::Define::ObjectType objType0,
                                        sketch::Define::ObjectType objType1);
CORE_DLL_DECLSPEC bool modifyCurObjectsByBaseType(const Engine::DObject& props, sketch::Define::ObjectType objTypeBegin,
                                                  sketch::Define::ObjectType objTypeEnd);

CORE_DLL_DECLSPEC long long createObject(long long sceneId, sketch::Define::ObjectType objType,
                                         const Engine::DObject& props);

CORE_DLL_DECLSPEC BOOL modifyObjHandler(long long objId, long long hid, const Engine::DObject& props);
