#pragma once

#include <vector>

#include "../../Core.h"
#include "../../Define.h"

#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC BOOL getAllViewIdAndNames(std::vector<long long>& ids, std::vector<std::wstring>& names, BOOL incTemp);
CORE_DLL_DECLSPEC BOOL getActiveViewIdAndNames(std::vector<long long>& ids, std::vector<std::wstring>& names, BOOL incTemp);
CORE_DLL_DECLSPEC std::vector<long long> focusedViewIds();
CORE_DLL_DECLSPEC const Engine::DObject* focusedViewProps();
CORE_DLL_DECLSPEC const Engine::DObject* viewProps(long long id);
CORE_DLL_DECLSPEC BOOL modifyView(long long id, const Engine::DObject& props);
CORE_DLL_DECLSPEC BOOL modifyViews(long long* ids, int nIds, const Engine::DObject& props);
CORE_DLL_DECLSPEC BOOL modifyFocusedViews(const Engine::DObject& props);
CORE_DLL_DECLSPEC BOOL modifyDisplaySettingInFocusedViews(const Engine::DObject& props);
CORE_DLL_DECLSPEC BOOL modifyDisplayFilterInFocusedViews(const Engine::DObject& props);

extern "C" CORE_DLL_DECLSPEC BOOL modifyPartRenderModeInFocusedViews(sketch::Define::ViewRenderMode rdMode);
extern "C" CORE_DLL_DECLSPEC BOOL modifyPartRenderModeCompInFocusedViews(sketch::Define::ViewRenderMode rdMode);
extern "C" CORE_DLL_DECLSPEC BOOL modifyPointSizeInAllViews(double ptSize);
extern "C" CORE_DLL_DECLSPEC long long createView3DForCurObjs(const wchar_t* viewName, BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createViewOrthoForCurObjs(const wchar_t* viewName, BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createObjView3D(long long objId, const WCHAR* viewName, BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createObjViewOrtho(long long objId, const WCHAR* viewName, BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createObjViewFromModelSide(long long objId, const WCHAR* viewName, sketch::Define::ObjectSide objSide,
                                                                  BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createObjViewFromObjSide(long long objId, const WCHAR* viewName, sketch::Define::ObjectSide objSide,
                                                                BOOL temp);
extern "C" CORE_DLL_DECLSPEC long long createModelViewFromObjSide(long long objId, const WCHAR* viewName, sketch::Define::ObjectSide objSide,
                                                                  BOOL temp);
extern "C" CORE_DLL_DECLSPEC void hideCurObjsInFocusedView();
extern "C" CORE_DLL_DECLSPEC void hideObjInFocusedView(long long objId);
CORE_DLL_DECLSPEC void hideObjsInFocusedView(const std::unordered_set<long long>& ids);
extern "C" CORE_DLL_DECLSPEC void setWorkAreaToModelInFocusedViews();
extern "C" CORE_DLL_DECLSPEC void setWorkAreaToVisibleAreaInFocusedViews();
extern "C" CORE_DLL_DECLSPEC void setWorkAreaToCurObjsInFocusedViews();
extern "C" CORE_DLL_DECLSPEC void setWorkAreaToCurObjsInActiveViews();
extern "C" CORE_DLL_DECLSPEC long long createModelViewBase(sketch::Define::SpacePlane plane, double height);
extern "C" CORE_DLL_DECLSPEC long long createModelViewByFocusedWorkPlane();
extern "C" CORE_DLL_DECLSPEC void addActiveViews(long long* ids, int nIds, BOOL addLayout);
extern "C" CORE_DLL_DECLSPEC void removeActiveViews(long long* ids, int nIds, BOOL removeLayout);
extern "C" CORE_DLL_DECLSPEC void redrawFocusedViews();
extern "C" CORE_DLL_DECLSPEC void redrawActiveViews();
extern "C" CORE_DLL_DECLSPEC void dirtyFocusedViews();
extern "C" CORE_DLL_DECLSPEC void dirtyActiveViews();
extern "C" CORE_DLL_DECLSPEC void deleteViews(long long* ids, int nIds);
extern "C" CORE_DLL_DECLSPEC void resetFocusedViewCamPose();
extern "C" CORE_DLL_DECLSPEC void switchFocusedViewType();
extern "C" CORE_DLL_DECLSPEC BOOL isFocusedViewWorkBoxVis();
extern "C" CORE_DLL_DECLSPEC BOOL setFocusedViewWorkBoxVis(BOOL b);
extern "C" CORE_DLL_DECLSPEC void sketch_set_camera(const std::array<float, 16>& matrix, float fov);
