#pragma once

#include <vector>
#include <string>

#include "../../Core.h"
#include "../../Define.h"
#include "Engine/Util/DObject.h"

CORE_DLL_DECLSPEC Engine::Mat43 getMat43ByLineStrideInWorkSpace(double dx, double dy, double dz);
CORE_DLL_DECLSPEC void getMat43sByLineStrideInWorkSpace(std::vector<Engine::Mat43>& tfs, const std::vector<double>& dx, const std::vector<double>& dy, const std::vector<double>& dz, int count);
CORE_DLL_DECLSPEC Engine::Mat43 getMat43ByRotateInWorkSpace(const Engine::Vec3& target, sketch::Define::SpaceAxis axis, double angl);
CORE_DLL_DECLSPEC void getMat43sByRotateInWorkSpace(std::vector<Engine::Mat43>& tfs, const Engine::Vec3& target, sketch::Define::SpaceAxis axis, double* angls, int nAngls, double zoff, int count);
CORE_DLL_DECLSPEC Engine::Mat43 getMat43ByMirrorInWorkSpace(double x0, double y0, double angl);
CORE_DLL_DECLSPEC sketch::Define::MoveResult moveCurObjs(const Engine::Mat43& tf);
CORE_DLL_DECLSPEC sketch::Define::MoveResult moveObjs(const std::vector<long long>& ids, const Engine::Mat43& tf);
extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveCurObjsByLineStrideInWorkSpace(double dx, double dy, double dz);
extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveCurObjsByRotateInWorkSpace(double x0, double y0, double z0, sketch::Define::SpaceAxis axis, double angl);
extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveCurObjsByMirrorInWorkSpace(double x0, double y0, double angl);
CORE_DLL_DECLSPEC sketch::Define::CopyResult copyCurObjs(const std::vector<Engine::Mat43>& tfs);
CORE_DLL_DECLSPEC sketch::Define::CopyResult copyObjs(const std::vector<long long>& ids, const Engine::Mat43& tf);
extern "C" CORE_DLL_DECLSPEC sketch::Define::CopyResult copyCurObjsByLineStrideInWorkSpace(const std::vector<double>&dx, const std::vector<double>&dy, const std::vector<double>&dz, int count);
extern "C" CORE_DLL_DECLSPEC sketch::Define::CopyResult copyCurObjsByRotateInWorkSpace(double x0, double y0, double z0, sketch::Define::SpaceAxis axis, double* angls, int nAngls, double zoff, int count);
extern "C" CORE_DLL_DECLSPEC sketch::Define::CopyResult copyCurObjsByMirrorInWorkSpace(double x0, double y0, double angl);
CORE_DLL_DECLSPEC void copyObjsFromModelByPhase(const std::wstring& path, const std::unordered_set<int>& phaseNos, const std::wstring& progressName, void(*onFinish)(sketch::Define::CopyResult));

CORE_DLL_DECLSPEC sketch::Define::MoveResult moveFocusedViewsPlane(const Engine::Mat43& tf);
extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveFocusedViewsPlaneByLineStrideInWorkSpace(double dx, double dy, double dz);
extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveFocusedViewsPlaneByRotateInWorkSpace(double x0, double y0, double z0, sketch::Define::SpaceAxis axis, double angl);

CORE_DLL_DECLSPEC BOOL combineParts(const std::unordered_set<long long> partIds);