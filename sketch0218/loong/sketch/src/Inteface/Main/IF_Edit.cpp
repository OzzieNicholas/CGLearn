#include "sketch/STCore.h"

#include <Windows.h>
#include <ppl.h>

#include "Engine/Math/GeoUtil.h"
#include "Engine/Math/Math.h"
#include "Engine/Math/Plane.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/View/Revertable_View_Hide.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/UIFuncs.h"
#include "sketch/Util/HangUpJob.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

using namespace sketch;
using namespace Engine;

CORE_DLL_DECLSPEC Mat43 getMat43ByLineStrideInWorkSpace(double dx, double dy, double dz)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return Mat43::IDENTITY;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& workAxis = pScene->workAxis();
    //
    // Vec3 move_vec = { dx, dy, dz };
    // move_vec = workAxis * move_vec - workAxis.pos;
    // return Mat43::T(move_vec);
    return Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC void getMat43sByLineStrideInWorkSpace(std::vector<Mat43>& tfs, const std::vector<double>& dx, const std::vector<double>& dy,
                                                        const std::vector<double>& dz, int count)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& workAxis = pScene->workAxis();
    //
    // if(count <= 0)
    //     return;
    //
    // std::vector<double> stepx = dx;
    // std::vector<double> stepy = dy;
    // std::vector<double> stepz = dz;
    //
    // if(stepx.empty())
    //     stepx.push_back(0.0);
    //
    // if(stepy.empty())
    //     stepy.push_back(0.0);
    //
    // if(stepz.empty())
    //     stepz.push_back(0.0);
    //
    // if(stepx.size() == 1 && stepy.size() == 1 && stepz.size() == 1)
    //{
    //     // 单步长，复制数量由count决定
    //     Vec3 vec = { stepx[0], stepy[0], stepz[0] };
    //     for(int i = 0; i < count; ++i)
    //     {
    //         tfs.push_back(Mat43::T(workAxis * (vec * (i + 1)) - workAxis.col[3]));
    //     }
    // }
    // else
    //{
    //     // 步长数组，复制数量由数组决定
    //     int cpcount = (int)STMAX3(stepx.size(), stepy.size(), stepz.size());
    //     Vec3 vec = Vec3::ZERO;
    //     for(int i = 0; i < cpcount; ++i)
    //     {
    //         vec.x += (i < stepx.size()) ? stepx[i] : stepx.back();
    //         vec.y += (i < stepy.size()) ? stepy[i] : stepy.back();
    //         vec.z += (i < stepz.size()) ? stepz[i] : stepz.back();
    //         tfs.push_back(Mat43::T(workAxis * vec - workAxis.col[3]));
    //     }
    // }
}

CORE_DLL_DECLSPEC Mat43 getMat43ByRotateInWorkSpace(const Engine::Vec3& target, Define::SpaceAxis axis, double angl)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return Mat43::IDENTITY;

    SceneDoc* pScene = pView->doc()->scene();
    return Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC void getMat43sByRotateInWorkSpace(std::vector<Mat43>& tfs, const Engine::Vec3& target, Define::SpaceAxis axis, double* angls,
                                                    int nAngls, double zoff, int count)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    if(!pView)
        return;

    SceneDoc* pScene = pView->doc()->scene();
    // auto& workAxis = pScene->workAxis();
    //
    // Vec3 origin = workAxis * target;
    //
    // if(nAngls == 0)
    //     return;
    //
    // auto& vRotVec = workAxis.col[axis];
    // if(nAngls == 1)
    //{
    //     // 单角度，复制数量由count决定
    //     double rad = angl2Rad(angls[0]);
    //     for(int i = 0; i < count; ++i)
    //     {
    //         Mat43 matRot = Mat43::R(origin, vRotVec, rad * (i + 1));
    //         if(zoff != 0.0)
    //             matRot.translate(workAxis.col[2] * zoff * (i + 1));
    //         tfs.push_back(matRot);
    //     }
    // }
    // else
    //{
    //     // 步长数组，复制数量由数组决定
    //     double rad = 0.0;
    //     for(int i = 0; i < nAngls; ++i)
    //     {
    //         rad += angl2Rad(angls[i]);
    //
    //         Mat43 matRot = Mat43::R(origin, vRotVec, rad);
    //         if(zoff != 0.0)
    //             matRot.translate(workAxis.col[2] * (zoff * (i + 1)));
    //         tfs.push_back(matRot);
    //     }
    // }
}

CORE_DLL_DECLSPEC Mat43 getMat43ByMirrorInWorkSpace(double x0, double y0, double angl)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = pViewMgr->layout()->focusedView();
    if(pView == nullptr)
        return Mat43::IDENTITY;

    return Mat43::IDENTITY;
}

CORE_DLL_DECLSPEC Define::MoveResult moveCurObjs(const Engine::Mat43& tf)
{
    Settings* pSets = STCore::inst->docMgr()->cfgMgr()->settings();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return Define::MOVE_RESULT_ENV_FAILED;

    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    SceneDoc* pScene = pView->doc()->scene();
    if(!pView)
        return Define::MOVE_RESULT_ENV_FAILED;

    bool bAllGrid = true;
    // for(auto pObj: pScene->curObjs())
    //{
    //     bAllGrid = false;
    //     break;
    // }

    std::unordered_set<long long> validIds;
    // for(auto pObj: pScene->curObjs())
    //{
    //     if(!pObj || !pObj->isUserMovable())
    //         continue;

    //    validIds.insert(pObj->id());
    //}

    if(validIds.empty())
        return Define::MOVE_RESULT_SUCCESS;

    RvtBlock blk(pScene->rvtId());

    // 磁性物体
    std::unordered_set<ObjectDoc*> validObjs;
    for(auto id: validIds)
    {
        ObjectDoc* pObj = pScene->getObject(id);
        if(!pObj)
            continue;

        validObjs.insert(pObj);
    }

    // 移动
    // pScene->moveObjs(validObjs, tf);

    if(pView)
        pView->doc()->askIfClipped(validObjs);

    // for(auto pObj: validObjs)
    //{
    //     if(pObj->isTF())
    //         pViewMgr->layout()->tempShowInDepthVisViews(pObj->_tfObject(), true);
    // }

    if(UIFuncs::setWarning)
    {
        UIFuncs::setWarning(L"");
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return Define::MOVE_RESULT_SUCCESS;
}

CORE_DLL_DECLSPEC Define::MoveResult moveObjs(const std::vector<long long>& ids, const Engine::Mat43& tf)
{
    Settings* pSets = STCore::inst->docMgr()->cfgMgr()->settings();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return Define::MOVE_RESULT_ENV_FAILED;

    ViewManager* pViewMgr = STCore::inst->viewMgr();
    View* pView = pViewMgr->layout()->focusedView();
    SceneDoc* pScene = pView->doc()->scene();
    if(!pView)
        return Define::MOVE_RESULT_ENV_FAILED;

    std::unordered_set<long long> validIds;
    for(auto id: ids)
    {
        auto pObj = pScene->getObject(id);
        if(!pObj || pScene != pObj->scene() || !pObj->isUserMovable())
            continue;

        validIds.insert(id);
    }

    if(validIds.empty())
        return Define::MOVE_RESULT_SUCCESS;

    RvtBlock blk(pScene->rvtId());

    std::unordered_set<ObjectDoc*> validObjs;
    for(auto id: validIds)
    {
        ObjectDoc* pObj = pScene->getObject(id);
        if(!pObj)
            continue;

        validObjs.insert(pObj);
    }

    // pScene->moveObjs(validObjs, tf);

    if(pView)
        pView->doc()->askIfClipped(validObjs);

    // for(auto pObj: validObjs)
    //{
    //     if(pObj->isTF())
    //         pViewMgr->layout()->tempShowInDepthVisViews(pObj->_tfObject(), true);
    // }

    if(UIFuncs::setWarning)
    {
        UIFuncs::setWarning(L"");
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return Define::MOVE_RESULT_SUCCESS;
}

CORE_DLL_DECLSPEC Define::MoveResult moveFocusedViewsPlane(const Engine::Mat43& tf)
{
    Settings* pSets = STCore::inst->docMgr()->cfgMgr()->settings();
    Document* pDoc = STCore::inst->docMgr()->document();
    if(pDoc == nullptr)
        return Define::MOVE_RESULT_ENV_FAILED;

    auto pLayout = STCore::inst->viewMgr()->layout();
    if(!pLayout->focusedView())
        return Define::MOVE_RESULT_ENV_FAILED;

    for(auto cid: pLayout->focusedCells())
    {
        auto pCell = pLayout->getCell(cid);
        if(!pCell || pCell->cellType() != Define::CELL_VIEW)
            continue;

        auto pView = pCell->_view();
        auto viewPlane = pView->doc()->viewPlane();
        viewPlane = tf * viewPlane;
        viewPlane = viewPlane.matrix().normalize();
        pView->doc()->modify({ { "viewplane", NousConvert::convert(viewPlane.matrix()) } });
        pView->dirtyRT();
    }

    STCore::inst->viewMgr()->layout()->dirtyCells();

    return Define::MOVE_RESULT_SUCCESS;
}

extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveFocusedViewsPlaneByLineStrideInWorkSpace(double dx, double dy, double dz)
{
    return moveFocusedViewsPlane(getMat43ByLineStrideInWorkSpace(dx, dy, dz));
}

extern "C" CORE_DLL_DECLSPEC sketch::Define::MoveResult moveFocusedViewsPlaneByRotateInWorkSpace(double x0, double y0, double z0,
                                                                                                 sketch::Define::SpaceAxis axis, double angl)
{
    return moveFocusedViewsPlane(getMat43ByRotateInWorkSpace({ x0, y0, z0 }, axis, angl));
}

extern "C" CORE_DLL_DECLSPEC Define::MoveResult moveCurObjsByLineStrideInWorkSpace(double dx, double dy, double dz)
{
    return moveCurObjs(getMat43ByLineStrideInWorkSpace(dx, dy, dz));
}

extern "C" CORE_DLL_DECLSPEC Define::MoveResult moveCurObjsByRotateInWorkSpace(double x0, double y0, double z0, Define::SpaceAxis axis, double angl)
{
    return moveCurObjs(getMat43ByRotateInWorkSpace({ x0, y0, z0 }, axis, angl));
}

extern "C" CORE_DLL_DECLSPEC Define::MoveResult moveCurObjsByMirrorInWorkSpace(double x0, double y0, double angl)
{
    return moveCurObjs(getMat43ByMirrorInWorkSpace(x0, y0, angl));
}

CORE_DLL_DECLSPEC Define::CopyResult copyCurObjs(const std::vector<Engine::Mat43>& tfs) { return Define::COPY_RESULT_SUCCESS; }

CORE_DLL_DECLSPEC Define::CopyResult copyObjs(const std::vector<long long>& ids, const Engine::Mat43& tf) { return Define::COPY_RESULT_SUCCESS; }

extern "C" CORE_DLL_DECLSPEC Define::CopyResult copyCurObjsByLineStrideInWorkSpace(const std::vector<double>& dx, const std::vector<double>& dy,
                                                                                   const std::vector<double>& dz, int count)
{
    std::vector<Mat43> tfs;
    getMat43sByLineStrideInWorkSpace(tfs, dx, dy, dz, count);
    return copyCurObjs(tfs);
}

extern "C" CORE_DLL_DECLSPEC Define::CopyResult copyCurObjsByRotateInWorkSpace(double x0, double y0, double z0, Define::SpaceAxis axis, double* angls,
                                                                               int nAngls, double zoff, int count)
{
    std::vector<Mat43> tfs;
    getMat43sByRotateInWorkSpace(tfs, { x0, y0, z0 }, axis, angls, nAngls, zoff, count);
    return copyCurObjs(tfs);
}

extern "C" CORE_DLL_DECLSPEC Define::CopyResult copyCurObjsByMirrorInWorkSpace(double x0, double y0, double angl)
{
    return copyCurObjs({ getMat43ByMirrorInWorkSpace(x0, y0, angl) });
}

class CopyObjsFromModelByPhaseHangUp : public HangUpJob
{
public:
    CopyObjsFromModelByPhaseHangUp(ViewDoc* pView, const Engine::Path& path, const std::unordered_set<int>& phaseNos, const std::wstring& progName,
                                   std::function<void(Define::CopyResult)> onFinish)
        : m_pView(pView), m_path(path), m_phaseNos(phaseNos), m_progName(progName), m_onFinish(onFinish), m_result(Define::COPY_RESULT_NONE)
    {
    }

    virtual ~CopyObjsFromModelByPhaseHangUp() {}

    virtual void run() override
    {
        m_progress = 0.0f;

        if(m_phaseNos.empty())
        {
            m_result = Define::COPY_RESULT_SUCCESS;
            return;
        }

        m_task = concurrency::create_task([&] {
            SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

            RvtBlock blk(m_pView->scene()->rvtId());

            STCore::inst->viewMgr()->layout()->dirtyCells();
            m_result = Define::COPY_RESULT_SUCCESS;
        });
    }

    virtual void updateOnMainThread() override
    {
        if(!m_progName.empty())
            STCore::inst->updateUIProgress(m_progName, m_progress);

        Sleep(45);
    }

    virtual void onFinish() override
    {
        if(m_onFinish)
            m_onFinish(m_result);
    }

    inline Define::CopyResult result() const { return m_result; }

protected:
    Engine::Path m_path;
    std::unordered_set<int> m_phaseNos;
    ViewDoc* m_pView;
    std::wstring m_progName;
    std::atomic<float> m_progress;
    Define::CopyResult m_result;
    std::function<void(Define::CopyResult)> m_onFinish;
};

CORE_DLL_DECLSPEC void copyObjsFromModelByPhase(const std::wstring& path, const std::unordered_set<int>& phaseNos, const std::wstring& progressName,
                                                void (*onFinish)(sketch::Define::CopyResult))
{
}

CORE_DLL_DECLSPEC BOOL combineParts(const std::unordered_set<long long> partIds) { return true; }
