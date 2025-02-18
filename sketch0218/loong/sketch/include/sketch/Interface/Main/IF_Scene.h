#pragma once

#include <vector>
#include <unordered_set>

#include "../../Core.h"
#include "../../Define.h"

extern "C" CORE_DLL_DECLSPEC long long focusedScene();
CORE_DLL_DECLSPEC void getCurObjIds(long long sceneId, std::vector<long long>& ids);
CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids);
CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids, sketch::Define::ObjectType objType);
CORE_DLL_DECLSPEC void getCurObjIds(std::vector<long long>& ids, sketch::Define::ObjectType objType0, sketch::Define::ObjectType objType1);
CORE_DLL_DECLSPEC void getCurObjIdsByBaseType(std::vector<long long>& ids, sketch::Define::ObjectType typeBegin, sketch::Define::ObjectType typeEnd);
extern "C" CORE_DLL_DECLSPEC sketch::Define::ObjectType getCurObjsUnifiedType();
CORE_DLL_DECLSPEC const Engine::Mat43& focusedWorkAxis();
CORE_DLL_DECLSPEC const Engine::Mat43& workAxis(long long sceneId);
CORE_DLL_DECLSPEC void setFocusedWorkAxis(const Engine::Mat43& m);
CORE_DLL_DECLSPEC void setWorkAxis(long long sceneId, const Engine::Mat43& m);

CORE_DLL_DECLSPEC BOOL setCurObjs(const std::vector<long long>& objIds);
extern "C" CORE_DLL_DECLSPEC BOOL setCurObjsToFocusedViewVisObjs();
extern "C" CORE_DLL_DECLSPEC BOOL deleteCurObjs();
extern "C" CORE_DLL_DECLSPEC BOOL deleteObjs(const std::vector<long long>&objIds);
extern "C" CORE_DLL_DECLSPEC BOOL undoCurObjs();
extern "C" CORE_DLL_DECLSPEC BOOL redoCurObjs();



