#include "sketch/View/Controller/Main/Camera/Controller_Camera_TRS.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
Controller_Camera_TRS::Controller_Camera_TRS(const std::wstring& name) : Controller_Main(name) {}

Controller_Camera_TRS::~Controller_Camera_TRS() {}

void Controller_Camera_TRS::active(const SketchParamMap& pProps)
{
    Controller_Main::active(pProps);

    m_cur_mode = 0;
}

void Controller_Camera_TRS::deactive()
{
    m_cur_mode = 0;

    Controller_Main::deactive();
}

void Controller_Camera_TRS::key_down(int key, bool ctrl, bool alt, bool shift) {}

void Controller_Camera_TRS::mouse_down(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Device* pDev = STE::device();
    Document* pDoc = STCore::inst->docMgr()->document();

    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(pView == nullptr)
        return;

    ViewDoc* pViewDoc = pView->doc();
    auto& clipBox = pViewDoc->clipBox();
    auto pCam = pView->camera();
    auto& vp = pCam->viewport().zw;

    if(button == 1)
    {
        m_mouse_begin = { x, y };
        m_mat_cam_begin = pCam->matrix().mat43();
        pCam->getDocPickRay(pDoc, (float)x, (float)y, &m_pick_origin_begin, &m_pick_dir_begin);

        if(shift)
        {// translate
            m_cur_mode = 2;

            if(pCam->projType() == Camera::PROJ_PERSPECTIVE)
            {
                Vec3f camPivotProj = pCam->dvpMatrix() * pViewDoc->camPivot();

                if(pViewDoc->isCamPivotValid() && RANGE_IN(camPivotProj.x, -1.0f, 1.0f) && RANGE_IN(camPivotProj.y, -1.0f, 1.0f) &&
                   RANGE_IN(camPivotProj.z, -1.0f, 1.0f))
                {

                    m_trans_plane = { pViewDoc->camPivot(), pDoc->v2d(pCam->pos()) - pViewDoc->camPivot() };

                    double retT = DBL_MAX;
                    m_trans_plane.intersectRay(m_pick_origin_begin, m_pick_dir_begin, &retT);
                    m_trans_begin_pos = m_pick_origin_begin + m_pick_dir_begin * retT;
                }
                else
                {
                    double retT = DBL_MAX;
                    pViewDoc->viewPlane().intersectRay(m_pick_origin_begin, m_pick_dir_begin, &retT);
                    Vec3 hitPt = m_pick_origin_begin + m_pick_dir_begin * retT;

                    if(pViewDoc->clipBox().contains(hitPt))
                    {
                        m_trans_plane = { hitPt, pDoc->v2d(pCam->pos()) - hitPt };
                        m_trans_begin_pos = m_trans_plane.o();
                    }
                    else
                    {
                        double t = pViewDoc->pickClipBoxFacingFaces(m_pick_origin_begin, m_pick_dir_begin);
                        if(t < DBL_MAX && t > 0.0)
                        {
                            m_trans_plane.setO(m_pick_origin_begin + m_pick_dir_begin * t);
                            m_trans_plane.setN(pDoc->v2d(pCam->pos()) - m_trans_plane.o());
                        }
                        else
                        {
                            m_trans_plane.setO(pDoc->v2d(pCam->pos() + pCam->forwardDir() * 1000.0f));
                            m_trans_plane.setN(pCam->forwardDir());
                        }

                        m_trans_begin_pos = m_trans_plane.o();
                    }
                }
            }
            else
            {// ortho
                m_trans_begin_pos.xy = Vec2(x, y);
            }
        }
        else
        {// rotate
            m_cur_mode = 1;

            m_rot_target = Vec3::MAX;

            if(pViewDoc->isCamPivotValid())
            {
                m_rot_target = pViewDoc->camPivot();
            }

            if(m_rot_target.x == DBL_MAX)
            {
                // 获取屏幕中心探测射线（非鼠标点）
                Vec3 vOriginCenter, vDirCenter;
                pCam->getDocPickRay(pDoc, pCam->viewportX() + pCam->viewportWidth() * 0.5f, pCam->viewportY() + pCam->viewportHeight() * 0.5f,
                                    &vOriginCenter, &vDirCenter);

                double t = DBL_MAX;
                pViewDoc->viewPlane().intersectRay(vOriginCenter, vDirCenter, &t);

                Vec3 hitPt = vOriginCenter + vDirCenter * t;
                if(clipBox.contains(hitPt))
                {
                    m_rot_target = hitPt;
                }
                else
                {
                    double t = pViewDoc->pickClipBoxFacingFaces(vOriginCenter, vDirCenter);
                    if(t < DBL_MAX)
                        m_rot_target = vOriginCenter + vDirCenter * t;
                }
            }

            if(m_rot_target.x == DBL_MAX)
                m_rot_target = pViewDoc->viewPlane().fromVertex2D((pViewDoc->workAreaMin() + pViewDoc->workAreaMax()) * 0.5);
        }
    }
}

void Controller_Camera_TRS::mouse_move(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(pView == nullptr)
        return;

    ViewDoc* pViewDoc = pView->doc();

    ViewCamera* pCam = pView->camera();
    if(pCam == nullptr)
        return;

    auto& vp = pCam->viewport().zw;

    if(button == 1)
    {
        if(shift)
        {// translate
            if(m_cur_mode != 2)
                return;

            if(pCam->projType() == Camera::PROJ_PERSPECTIVE)
            {
                // 获取当前鼠标探测射线
                Vec3f curPos = pCam->pos();
                pCam->setPos(m_mat_cam_begin.pos);

                Vec3 pickPos, pickDir;
                pCam->getDocPickRay(pDoc, (float)x, (float)y, &pickPos, &pickDir);

                double retT = 0.0;
                if(m_trans_plane.intersectRay(pickPos, pickDir, &retT))
                {
                    Vec3 pickResult = pickPos + pickDir * retT;
                    Vec3 moveVec = pickResult - m_trans_begin_pos;
                    pCam->setPos(pDoc->d2v(pDoc->v2d(m_mat_cam_begin.pos) - moveVec));
                }
                else
                {
                    pCam->setPos(curPos);
                }
            }
            else
            {
                Vec2f moveVec = (Vec2(x, y) - m_trans_begin_pos.xy) * Vec2f(pCam->orthoWidth(), pCam->orthoHeight()) / vp;
                pCam->setPos(Vec3f(m_mat_cam_begin.pos) - pCam->rightDir() * moveVec.x - pCam->upDir() * moveVec.y);
            }

            pView->onCameraTransform();
        }
        else if(x != m_mouse_begin.x || y != m_mouse_begin.y)
        {// rotate
            if(m_cur_mode != 1)
                return;

            Mat43 matCam = m_mat_cam_begin;
            Vec3 rotO = pDoc->d2v(m_rot_target);

            {// vertical
                double radSweep = double(y - m_mouse_begin.y) / STCore::inst->viewMgr()->canvasLogicHeight() * DOUBLE_2PI * 0.75;
                Mat43 matRot = Mat43::R(rotO, m_mat_cam_begin.x, radSweep);
                matCam = matRot * matCam;
            }

            {// horizontal
                double radSweep = double(x - m_mouse_begin.x) / STCore::inst->viewMgr()->canvasLogicWidth() * DOUBLE_2PI * 1.5;
                double rad_0 = matCam.x.xy.clockRad();
                double rad_1 = DOUBLE_SNAP(rad_0 - radSweep, DOUBLE_PI / 720.0);// 旋转最小单位0.25度
                Mat43 matRot = Mat43::R(rotO, Vec3::AXIS_Z, rad_1 - rad_0);
                matCam = matRot * matCam;
            }

            pCam->setMatrix(matCam);

            if(pViewDoc->type() != Define::VIEW_3D)
                pViewDoc->setViewType(Define::VIEW_3D);

            auto euler = pCam->eulerAngl();
            pViewDoc->setEuler(euler.y, euler.x);
            pView->onCameraTransform();
        }
    }
}

void Controller_Camera_TRS::mouse_up(int button, int x, int y, bool ctrl, bool alt, bool shift)
{
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(pView == nullptr)
        return;

    ViewDoc* pViewDoc = pView->doc();

    if(m_cur_mode == 1)
    {// rotate, invoke UI to update anglH, anglV
        if(pViewDoc->type() == Define::VIEW_2D)
        {
            // set view type to 3D
            pViewDoc->modify({ { "type", Define::VIEW_3D } });
        }
    }

    m_cur_mode = 0;
}

void Controller_Camera_TRS::mouse_wheel(int button, int x, int y, int delta, bool ctrl, bool alt, bool shift)
{
    Document* pDoc = STCore::inst->docMgr()->document();
    View* pView = STCore::inst->viewMgr()->layout()->focusedView();
    if(pView == nullptr || button >= 0 || shift)// 与selector层级滚动冲突
        return;

    Camera* pCam = pView->camera();
    ViewDoc* pViewDoc = pView->doc();

    if(pViewDoc->projMode() == Define::VIEWPROJ_PERSPECTIVE)
    {
        Vec3f pickPos, pickDir;
        pCam->getPickRay((float)x, (float)y, &pickPos, &pickDir);

        Vec3 vOrigin = pDoc->v2d(pickPos);
        Vec3 vDir = pickDir;

        Vec3f camPivot = pDoc->d2v(pViewDoc->camPivot());
        Vec3f camPivotProj = pCam->viewProjMatrix() * camPivot;

        Planef pl0;
        if(pViewDoc->isCamPivotValid() && RANGE_IN(camPivotProj.x, -1.0f, 1.0f) && RANGE_IN(camPivotProj.y, -1.0f, 1.0f) &&
           RANGE_IN(camPivotProj.z, 0.0f, 1.0f))
        {
            pl0.o = pDoc->d2v(pViewDoc->camPivot());
            pl0.n = pickDir;
        }
        else
        {
            double t0 = pViewDoc->pickClipBoxFacingFaces(vOrigin, vDir);
            double t1 = DBL_MAX;
            pViewDoc->viewPlane().intersectRay(vOrigin, vDir, &t1);
            double t = DBL_MAX;
            if(t0 >= 0.0 && t1 >= 0.0)
                t = STMIN(t0, t1);
            else if(t0 >= 0.0)
                t = t0;
            else
                t = t1;

            if(t <= 0.0)
            {
                pl0.o = pDoc->d2v(vOrigin + vDir.normalize() * 5.0);
                pl0.n = pickDir;
            }
            else if(t < DBL_MAX)
            {
                pl0.o = pDoc->d2v(vOrigin + vDir * t);
                pl0.n = pickDir;
            }
            else
                return;
        }

        Vec3f scaleTarget;

        float retT;
        if(pl0.intersectRay(pickPos, pickDir, &retT))
        {
            scaleTarget = pickPos + pickDir * retT;
        }
        else
        {
            return;
        }

        Vec3f scaleRay = scaleTarget - pCam->pos();
        if(scaleRay.len() < 0.005f)
            scaleRay = scaleRay.normalize() * 0.005f;

        pCam->setPos(pCam->pos() + scaleRay * (delta / 1000.0f));
        pView->onCameraTransform();
    }
    else
    {
        float scale = STMAX(1.0f - delta / 1000.0f, 0.01f);// 防止delta过大(部分鼠标滚轮有问题)，出现负值

        float xOrtho = pCam->orthoLeft() + (x - pCam->viewportX()) / pCam->viewportWidth() * pCam->orthoWidth();
        float yOrtho = pCam->orthoBottom() + (y - pCam->viewportY()) / pCam->viewportHeight() * pCam->orthoHeight();

        // 防止精度不足导致放大之后宽高比错误的问题
        if((pCam->orthoWidth() < 1e-2 || pCam->orthoHeight() < 1e-2) && scale < 1.0f)
        {
            return;
        }
        if((pCam->orthoWidth() > 2048 || pCam->orthoHeight() > 2048) && scale > 1.0f)
        {
            return;
        }

        float left = (pCam->orthoLeft() - xOrtho) * scale + xOrtho;
        float right = (pCam->orthoRight() - xOrtho) * scale + xOrtho;
        float bottom = (pCam->orthoBottom() - yOrtho) * scale + yOrtho;
        float top = (pCam->orthoTop() - yOrtho) * scale + yOrtho;
        float w = right - left;
        float h = top - bottom;
        float cx = (left + right) * 0.5f;
        float cy = (bottom + top) * 0.5f;
        Engine::Vec3f upDir = pCam->rightDir().cross(pCam->forwardDir()).normalize();
        pCam->translate(pCam->rightDir() * cx + upDir * cy);
        pCam->setOrthoView(-w * 0.5f, w * 0.5f, -h * 0.5f, h * 0.5f);
        pView->onCameraTransform();
    }
}

void Controller_Camera_TRS::mouse_out(int button, int x, int y, bool ctrl, bool alt, bool shift) {}

}// namespace sketch