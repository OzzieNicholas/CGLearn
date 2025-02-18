#include "sketch/Document/View/ViewDoc.h"

#include <ppl.h>

#include "Engine/Math/AABB.h"
#include "Engine/Math/AxisPlane.h"
#include "Engine/Math/Mat43.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDisplaySetting/ViewDisplaySetting.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/LayoutCell.h"
#include "sketch/View/ViewLayout/ViewCamera.h"

using namespace Engine;

namespace sketch
{
class ViewDocImpl : public ViewDoc
{
public:
    ViewDocImpl(long long id, SceneDoc* pScene)
        :// NOTE: 此处新分配id必须用全局uniqueId
         // 因不能与Drawing id重复，否则Layout中Cell id会有冲突
          m_id(id < 0 ? STCore::inst->uniqueId() : id), m_scene(pScene), m_temp(false), m_type(Define::VIEW_2D),
          m_proj_mode(Define::VIEWPROJ_PERSPECTIVE), m_depth_up(15000.0), m_depth_down(500.0), m_rot_hor(0.0), m_rot_ver(0.0),
          m_disp_setting(new ViewDisplaySetting()), m_view_plane(Engine::Vec3::ZERO, Engine::Vec3::AXIS_X, Engine::Vec3::AXIS_Y),
          m_work_area_min(Vec2::ZERO), m_work_area_max(Vec2::ZERO), m_clip_box(), m_cam_pivot(Vec3::MAX), m_props_dirty(true), m_render_view(nullptr)
    {
    }

    virtual ~ViewDocImpl(void)
    {
        if(m_disp_setting)
        {
            delete m_disp_setting;
        }

        if(m_disp_filter)
        {
            delete m_disp_filter;
        }
    }

    virtual void modify(const SketchParamMap& props) override
    {
        DocManager* pDocMgr = STCore::inst->docMgr();
        Document* pDoc = pDocMgr->document();

        pDocMgr->updateModifyFrame();

        SketchParamMap flags;
        _parseProp(flags, props);

        bool type_dirty_flag = false;
        bool proj_dirty_flag = false;
        bool clipbox_dirty_flag = false;
        bool viewplane_dirty_flag = false;

        if(flags.has_field("type_dirty"))
        {
            flags.get_value("type_dirty", type_dirty_flag);
        }

        if(flags.has_field("proj_dirty"))
        {
            flags.get_value("proj_dirty", proj_dirty_flag);
        }

        if(flags.has_field("clipbox_dirty"))
        {
            flags.get_value("clipbox_dirty", clipbox_dirty_flag);
        }

        if(flags.has_field("viewplane_dirty"))
        {
            flags.get_value("viewplane_dirty", viewplane_dirty_flag);
        }

        if(clipbox_dirty_flag || viewplane_dirty_flag)
        {
            this->_updateClipBox();
        }
        m_props_dirty = true;

        auto pCam = m_render_view->camera();

        bool reset_view_flag = false;
        if(props.has_field("reset_view"))
        {
            props.get_value("reset_view", reset_view_flag);
        }

        if((!pDocMgr->isLoading() || reset_view_flag) && type_dirty_flag)
        {
            m_render_view->onViewTypeChanged();
        }

        if(proj_dirty_flag)
        {
            pCam->setProjType(static_cast<Camera::ProjType>(m_proj_mode));
        }

        if(!type_dirty_flag && m_type == Define::VIEW_3D)
        {
            pCam->setByEuler((float)angl2Rad(m_rot_ver), (float)angl2Rad(m_rot_hor), 0.0);
        }

        if(!pDocMgr->isLoading() && proj_dirty_flag)
        {
            pCam->refineToSee(pDoc->d2v(this->clipBox()));
        }

        if(clipbox_dirty_flag || viewplane_dirty_flag)
        {
            m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX);
        }

        if(props.has_field("reset_cam_pose") && props.get_value_type("reset_cam_pose") == ESketchParamValueType::Boolean)
        {
            bool reset_cam_pose_flag = false;
            props.get_value("reset_cam_pose", reset_cam_pose_flag);
            if(reset_cam_pose_flag)
            {
                m_render_view->dirty(View::DIRTY_BIT_CAM_POSE);
            }
        }

        m_render_view->dirty(View::DIRTY_BIT_VIEW_FILTER | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT | View::DIRTY_BIT_VIEW_BBOX);
    }

    virtual const SketchParamMap& props() override
    {
        if(m_props_dirty)
        {
            this->_updateProps();
            m_props_dirty = false;
        }

        return m_props;
    }

    virtual void redraw() override
    {
        std::vector<long long> delObjs;

        for(auto pRdObj: m_render_view->objects())
        {
            TFObjectDoc* pObj = pRdObj->doc();
            if(pObj->viewId() == m_id && pObj->isTemporary())
            {
                delObjs.push_back(pObj->id());
            }
        }

        for(auto id: delObjs)
        {
            m_scene->deleteObject(id);
        }

        m_render_view->redraw();
    }

    virtual void resetCameraPose() override
    {
        STCore::inst->docMgr()->updateModifyFrame();
        m_render_view->resetCameraPose();
    }

    virtual void setRenderView(View* pView) override { m_render_view = pView; }

    virtual View* renderView() const override { return m_render_view; }

    virtual SceneDoc* scene() const override { return m_scene; }

    virtual long long id() const override { return m_id; }

    virtual const std::wstring& name() const override { return m_name; }

    virtual bool isTemp() const override { return m_temp; }

    virtual Define::ViewType type() const override { return m_type; }

    virtual Define::ViewProjMode projMode() const override { return m_proj_mode; }

    virtual double depthUp() const override { return m_depth_up; }

    virtual double depthDown() const override { return m_depth_down; }

    virtual double rotH() const override { return m_rot_hor; }

    virtual double rotV() const override { return m_rot_ver; }

    virtual ViewDisplaySetting* dispSetting() const override { return m_disp_setting; }

    virtual ObjectFilter* dispFilter() const override { return m_disp_filter; }

    virtual const Engine::AxisPlane& viewPlane() const override { return m_view_plane; }

    virtual const Engine::Vec2& workAreaMin() const override { return m_work_area_min; }

    virtual const Engine::Vec2& workAreaMax() const override { return m_work_area_max; }

    virtual const Engine::OBB& clipBox() const override { return m_clip_box; }

    virtual bool isCamPivotValid() const override { return m_cam_pivot.x < DBL_MAX; }

    virtual const Engine::Vec3& camPivot() const override { return m_cam_pivot; }

    virtual Engine::Vec3 majorAxis() const override
    {
        auto& n = m_view_plane.normal();
        auto nx = fabs(n.x);
        auto ny = fabs(n.y);
        auto nz = fabs(n.z);
        auto nmax = STMAX3(nx, ny, nz);
        if(nmax == nx)
            return (n.x >= 0.0) ? Vec3::AXIS_X : -Vec3::AXIS_X;
        else if(nmax == ny)
            return (n.y >= 0.0) ? Vec3::AXIS_Y : -Vec3::AXIS_Y;
        else
            return (n.z >= 0.0) ? Vec3::AXIS_Z : -Vec3::AXIS_Z;
    }

    virtual void setViewType(Define::ViewType type) override
    {
        if(m_type == type)
            return;

        m_type = type;

        if(m_type == Define::VIEW_2D)
        {
            auto pCam = m_render_view->camera();
            Mat43f camMat = pCam->matrix();
            camMat.x = m_view_plane.axisX();
            camMat.y = m_view_plane.axisY();
            camMat.z = m_view_plane.normal();
            pCam->setMatrix(camMat);
        }

        m_render_view->dirty(View::DIRTY_BIT_RT | View::DIRTY_BIT_CAM_DEPTH);
    }

    virtual void setCamPivot(const Engine::Vec3& pivot) override
    {
        STCore::inst->docMgr()->updateModifyFrame();

        m_cam_pivot = pivot;
        m_props_dirty = true;

        if(m_render_view)
            m_render_view->dirtyRT();
    }

    virtual void resetCamPivot() override
    {
        STCore::inst->docMgr()->updateModifyFrame();

        m_cam_pivot = Vec3::MAX;
        m_props_dirty = true;

        if(m_render_view)
            m_render_view->dirtyRT();
    }

    virtual void setWorkArea(const Engine::Vec2& vMin, const Engine::Vec2& vMax) override
    {
        STCore::inst->docMgr()->updateModifyFrame();

        m_work_area_min = vMin;
        m_work_area_max = vMax;

        this->_updateClipBox();

        m_props_dirty = true;
        m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT);
    }

    virtual void setEuler(double rotH, double rotV) override
    {
        if(m_rot_hor == rotH && m_rot_ver == rotV)
            return;

        m_rot_hor = rotH;
        m_rot_ver = rotV;

        m_props_dirty = true;
        m_render_view->dirty(View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT);
    }

    virtual void askIfClipped(long long objId) override
    {
        TFObjectDoc* pObj = m_scene->getTFObject(objId);
        if(pObj == nullptr)
            return;

        if(!pObj->bbox().isValid())
            return;

        if(m_clip_box.intersect(pObj->bbox()))
            return;

        if(MessageBox(STCore::inst->mainHandle(), L"是否扩大工作区和深度以容纳新对象?", L"提示", MB_YESNO) == IDYES)
        {
            auto matInv = m_view_plane.matrix().inverse();
            AABB bbox = { { m_work_area_min, -m_depth_down }, { m_work_area_max, m_depth_up } };
            for(auto& pt: pObj->bbox().pointsW())
            {
                bbox.add(matInv * pt);
            }

            m_work_area_min = bbox.getMin().xy;
            m_work_area_max = bbox.getMax().xy;
            m_depth_up = bbox.getMax().z;
            m_depth_down = -bbox.getMin().z;
            this->_updateClipBox();

            m_props_dirty = true;
            m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX | View::DIRTY_BIT_VIEW_FILTER | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT);
            STCore::inst->docMgr()->updateModifyFrame();
        }
    }

    virtual void askIfClipped(const std::unordered_set<long long>& ids) override
    {
        std::unordered_set<long long> extIds;
        for(auto id: ids)
        {
            TFObjectDoc* pObj = m_scene->getTFObject(id);
            if(pObj == nullptr)
                continue;

            if(!pObj->bbox().isValid())
                continue;

            if(m_clip_box.intersect(pObj->bbox()))
                continue;

            extIds.insert(id);
        }

        if(extIds.empty())
            return;

        if(MessageBox(STCore::inst->mainHandle(), L"是否扩大工作区和深度以容纳新对象?", L"提示", MB_YESNO) == IDYES)
        {
            auto matInv = m_view_plane.matrix().inverse();
            AABB bbox = { { m_work_area_min, -m_depth_down }, { m_work_area_max, m_depth_up } };
            for(auto id: extIds)
            {
                TFObjectDoc* pObj = m_scene->getTFObject(id);
                for(auto& pt: pObj->bbox().pointsW())
                {
                    bbox.add(matInv * pt);
                }
            }

            m_work_area_min = bbox.getMin().xy;
            m_work_area_max = bbox.getMax().xy;
            m_depth_up = bbox.getMax().z;
            m_depth_down = -bbox.getMin().z;
            this->_updateClipBox();

            m_props_dirty = true;
            m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX | View::DIRTY_BIT_VIEW_FILTER | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT);
            STCore::inst->docMgr()->updateModifyFrame();
        }
    }

    virtual void askIfClipped(const std::unordered_set<ObjectDoc*>& objs) override
    {
        std::unordered_set<long long> extIds;
        for(auto pObj: objs)
        {
            if(!pObj->isTF())
                continue;

            auto& bbox = pObj->_tfObject()->bbox();
            if(!bbox.isValid())
                continue;

            if(m_clip_box.intersect(pObj->_tfObject()->bbox()))
                continue;

            extIds.insert(pObj->id());
        }

        if(extIds.empty())
            return;

        if(MessageBox(STCore::inst->mainHandle(), L"是否扩大工作区来容纳新对象?", L"提示", MB_YESNO) == IDYES)
        {
            auto matInv = m_view_plane.matrix().inverse();
            AABB bbox = { { m_work_area_min, -m_depth_down }, { m_work_area_max, m_depth_up } };
            for(auto id: extIds)
            {
                TFObjectDoc* pObj = m_scene->getTFObject(id);
                for(auto& pt: pObj->bbox().pointsW())
                {
                    bbox.add(matInv * pt);
                }
            }

            m_work_area_min = bbox.getMin().xy;
            m_work_area_max = bbox.getMax().xy;
            m_depth_up = bbox.getMax().z;
            m_depth_down = -bbox.getMin().z;
            this->_updateClipBox();

            m_props_dirty = true;
            m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX | View::DIRTY_BIT_VIEW_FILTER | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT);
            STCore::inst->docMgr()->updateModifyFrame();
        }
    }

    virtual double pickClipBoxFacingFaces(const Engine::Vec3& vOrigin, const Engine::Vec3& vDir) override
    {
        double tMin = DBL_MAX;

        auto matInv = m_clip_box.matrix().inverse();
        auto vOriginL = matInv * vOrigin;
        auto vDirL = matInv * vDir - matInv.pos;
        auto dirLen = vDirL.len();

        if(vDirL.x > 0.3 * dirLen)
        {
            // if (vOriginL.x < m_clip_box.m_min.x)
            //	tMin = STMIN(tMin, (m_clip_box.m_min.x - vOriginL.x) / vDirL.x);
            // else if (vOriginL.x < m_clip_box.m_max.x)
            tMin = STMIN(tMin, (m_clip_box.m_max.x - vOriginL.x) / vDirL.x);
        }
        else if(vDirL.x < -0.3 * dirLen)
        {
            // if (vOriginL.x > m_clip_box.m_max.x)
            //	tMin = STMIN(tMin, (m_clip_box.m_max.x - vOriginL.x) / vDirL.x);
            // else if (vOriginL.x > m_clip_box.m_min.x)
            tMin = STMIN(tMin, (m_clip_box.m_min.x - vOriginL.x) / vDirL.x);
        }

        if(vDirL.y > 0.3 * dirLen)
        {
            // if (vOriginL.y < m_clip_box.m_min.y)
            //	tMin = STMIN(tMin, (m_clip_box.m_min.y - vOriginL.y) / vDirL.y);
            // else if (vOrigin.y < m_clip_box.m_max.y)
            tMin = STMIN(tMin, (m_clip_box.m_max.y - vOriginL.y) / vDirL.y);
        }
        else if(vDirL.y < -0.3 * dirLen)
        {
            // if (vOriginL.y > m_clip_box.m_max.y)
            //	tMin = STMIN(tMin, (m_clip_box.m_max.y - vOriginL.y) / vDirL.y);
            // else if (vOrigin.y > m_clip_box.m_min.y)
            tMin = STMIN(tMin, (m_clip_box.m_min.y - vOriginL.y) / vDirL.y);
        }

        if(vDirL.z > 0.3 * dirLen)
        {
            // if (vOriginL.z < m_clip_box.m_min.z)
            //	tMin = STMIN(tMin, (m_clip_box.m_min.z - vOriginL.z) / vDirL.z);
            // else if (vOriginL.z < m_clip_box.m_max.z)
            tMin = STMIN(tMin, (m_clip_box.m_max.z - vOriginL.z) / vDirL.z);
        }
        else if(vDirL.z < -0.3 * dirLen)
        {
            // if (vOriginL.z > m_clip_box.m_max.z)
            //	tMin = STMIN(tMin, (m_clip_box.m_max.z - vOriginL.z) / vDirL.z);
            // else if (vOriginL.z > m_clip_box.m_min.z)
            tMin = STMIN(tMin, (m_clip_box.m_min.z - vOriginL.z) / vDirL.z);
        }

        return tMin;
    }

    virtual void save(SketchSerializer& serializer) override
    {
        // serializer.write(m_id, "view_id");
        // serializer.write(m_scene->id(), "scene_id");
        // serializer.write(StrUtil::w2s(m_name).c_str(), "name");
        // serializer.write(m_temp, "temp");
        // serializer.write(static_cast<char>(m_type), "type");
        // serializer.write(static_cast<char>(m_proj_mode), "proj");
        // serializer.write(m_depth_up, "depth_up");
        // serializer.write(m_depth_down, "depth_down");
        // serializer.write(m_rot_hor, "rotH");
        // serializer.write(m_rot_ver, "rotV");
        // nous::mat4 view_plane_mat4 = NousConvert::convert(m_view_plane.matrix());
        // serializer.advance_write(view_plane_mat4, "viewplane");
        // serializer.advance_write(NousConvert::convert(m_work_area_min), "work_area_min");
        // serializer.advance_write(NousConvert::convert(m_work_area_max), "work_area_max");
        // serializer.advance_write(NousConvert::convert(m_cam_pivot), "cam_pivot");

        _updateProps();
        m_props.remove_value("disp_setting");
        serializer.begin_write_object("props");
        m_props.save(serializer);
        serializer.end_write_object();
        if(m_disp_setting)
        {
            m_props.set_value("disp_setting", m_disp_setting->props());
            serializer.begin_write_object("disp_setting");
            m_disp_setting->save(serializer);
            serializer.end_write_object();
        }

        if(m_render_view)
        {
            serializer.begin_write_object("layout_props");
            m_render_view->save(serializer);
            serializer.end_write_object();
        }
    }

    virtual void load(SketchSerializer& serializer) override
    {
        serializer.begin_read_object("props");
        m_props.load(serializer);
        serializer.end_read_object();
        modify(m_props);

        if(serializer.has_field("disp_setting") && m_disp_setting)
        {
            serializer.begin_read_object("disp_setting");
            m_disp_setting->load(serializer);
            serializer.end_read_object();
        }

        if(serializer.has_field("layout_props"))
        {
            serializer.begin_read_object("layout_props");
            if(m_render_view)
            {
                m_render_view->load(serializer);
            }
            serializer.end_read_object();
        }

        // SketchParamMap flag_map;
        // serializer.read(m_id, "view_id");
        // nous::String str;
        // serializer.read(str, "name");
        // m_name = StrUtil::s2w(str.data());
        // serializer.read(m_temp, "temp");

        // char value;
        // serializer.read(value, "type");
        // m_type = static_cast<Define::ViewType>(value);
        // flag_map.set_value("type_dirty", true);

        // serializer.read(value, "proj");
        // m_proj_mode = static_cast<Define::ViewProjMode>(value);
        // flag_map.set_value("proj_dirty", true);

        // serializer.read(m_depth_up, "depth_up");
        // serializer.read(m_depth_down, "depth_down");
        // serializer.read(m_rot_hor, "rotH");
        // serializer.read(m_rot_ver, "rotV");
        // nous::mat4 view_plane_mat4;
        // serializer.advance_read(view_plane_mat4, "viewplane");
        // m_view_plane = AxisPlane(NousConvert::convert(view_plane_mat4));
        // flag_map.set_value("viewplane_dirty", true);
        // flag_map.set_value("clipbox_dirty", true);

        // nous::vec2 vec2_v;
        // serializer.advance_read(vec2_v, "work_area_min");
        // m_work_area_min = NousConvert::convert(vec2_v);
        // serializer.advance_read(vec2_v, "work_area_max");
        // m_work_area_max = NousConvert::convert(vec2_v);
        // nous::vec3 vec3_v;
        // serializer.advance_read(vec3_v, "cam_pivot");
        // m_cam_pivot = NousConvert::convert(vec3_v);
        // serializer.begin_read_object("disp_setting");
        // m_disp_setting->load(serializer);
        // serializer.end_read_object();

        // if(serializer.has_field("layout_props"))
        //{
        //     serializer.begin_read_object("layout_props");
        //     if(m_render_view)
        //     {
        //         m_render_view->load(serializer);
        //     }
        //     serializer.end_read_object();
        // }

        // m_props_dirty = true;

        // DocManager* pDocMgr = STCore::inst->docMgr();
        // Document* pDoc = pDocMgr->document();

        // pDocMgr->updateModifyFrame();

        // bool typeDirty, projDirty, clipBoxDirty, viewPlaneDirty;
        // flag_map.get_value("type_dirty", typeDirty);
        // flag_map.get_value("proj_dirty", projDirty);
        // flag_map.get_value("clipbox_dirty", clipBoxDirty);
        // flag_map.get_value("viewplane_dirty", viewPlaneDirty);

        // if(clipBoxDirty || viewPlaneDirty)
        //     this->_updateClipBox();

        // m_props_dirty = true;

        // auto pCam = m_render_view->camera();

        // if(typeDirty)
        //     m_render_view->onViewTypeChanged();

        // if(projDirty)
        //     pCam->setProjType((Camera::ProjType)m_proj_mode);

        // if(!typeDirty && m_type == Define::VIEW_3D)
        //     pCam->setByEuler((float)angl2Rad(m_rot_ver), (float)angl2Rad(m_rot_hor), 0.0);

        // if(!pDocMgr->isLoading() && projDirty)
        //     pCam->refineToSee(pDoc->d2v(this->clipBox()));

        // if(clipBoxDirty || viewPlaneDirty)
        //     m_render_view->dirty(View::DIRTY_BIT_VIEW_CLIPBOX);

        // m_render_view->dirty(View::DIRTY_BIT_CAM_POSE);

        // m_render_view->dirty(View::DIRTY_BIT_VIEW_FILTER | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT | View::DIRTY_BIT_VIEW_BBOX);
    }

protected:
    void _updateProps()
    {
        m_props.set_value("view_id", m_id);
        m_props.set_value("scene_id", m_scene->id());
        m_props.set_value("name", m_name);
        m_props.set_value("temp", m_temp);
        m_props.set_value("type", static_cast<int>(m_type));
        m_props.set_value("proj", static_cast<int>(m_proj_mode));
        m_props.set_value("depth_up", m_depth_up);
        m_props.set_value("depth_down", m_depth_down);
        m_props.set_value("rotH", m_rot_hor);
        m_props.set_value("rotV", m_rot_ver);
        m_props.set_value("viewplane", NousConvert::convert(m_view_plane.matrix()));
        m_props.set_value("work_area_min", NousConvert::convert(m_work_area_min));
        m_props.set_value("work_area_max", NousConvert::convert(m_work_area_max));
        m_props.set_value("cam_pivot", NousConvert::convert(m_cam_pivot));
        m_props.set_value("disp_setting", m_disp_setting->props());
    }

    bool _parseProp(SketchParamMap& flags, const SketchParamMap& props)
    {
        if(props.has_field("name") && props.get_value_type("name") == ESketchParamValueType::String)
        {
            std::string name;
            props.get_value("name", name);
            m_name = StrUtil::s2w(name);
        }

        if(props.has_field("runtime_cfg") && props.get_value_type("runtime_cfg") == ESketchParamValueType::String)
        {
            m_props.set_value_wrapper("runtime_cfg", props.get_value_wrapper("runtime_cfg"));
        }

        if(props.has_field("temp") && props.get_value_type("temp") == ESketchParamValueType::Boolean)
        {
            props.get_value("temp", m_temp);
        }

        if(props.has_field("type") && props.get_value_type("type") == ESketchParamValueType::Integer)
        {
            int type_as_int;
            props.get_value("type", type_as_int);
            Define::ViewType v = static_cast<Define::ViewType>(type_as_int);
            if(m_type != v)
            {
                m_type = v;
                flags.set_value("type_dirty", true);
            }
        }

        if(props.has_field("proj") && props.get_value_type("proj") == ESketchParamValueType::Integer)
        {
            int proj_as_int;
            props.get_value("type", proj_as_int);
            Define::ViewProjMode v = static_cast<Define::ViewProjMode>(proj_as_int);
            if(m_proj_mode != v)
            {
                m_proj_mode = v;
                flags.set_value("proj_dirty", true);
            }
        }

        if(props.has_field("viewplane") && props.get_value_type("viewplane") == ESketchParamValueType::Matrix4x4)
        {
            nous::mat4 view_plane_mat4;
            props.get_value("viewplane", view_plane_mat4);
            AxisPlane viewplane = AxisPlane(NousConvert::convert(view_plane_mat4));
            if(!m_view_plane.matrix().isSimilar(viewplane.matrix()))
            {
                m_view_plane = viewplane;
                flags.set_value("viewplane_dirty", true);
                flags.set_value("clipbox_dirty", true);
            }
        }

        if(props.has_field("depth_up") && props.get_value_type("depth_up") == ESketchParamValueType::Double)
        {
            double depth_up;
            props.get_value("depth_up", depth_up);
            if(fabs(m_depth_up - depth_up) > 1e-4)
            {
                m_depth_up = depth_up;
                flags.set_value("clipbox_dirty", true);
            }
        }

        if(props.has_field("depth_down") && props.get_value_type("depth_down") == ESketchParamValueType::Double)
        {
            double depth_down;
            props.get_value("depth_down", depth_down);
            if(fabs(m_depth_down - depth_down) > 1e-4)
            {
                m_depth_down = depth_down;
                flags.set_value("clipbox_dirty", true);
            }
        }

        if(props.has_field("work_area_min") && props.get_value_type("work_area_min") == ESketchParamValueType::Vector2)
        {
            nous::vec2 work_area_min;
            props.get_value("work_area_min", work_area_min);
            if(work_area_min.x() != m_work_area_min.x || work_area_min.y() != m_work_area_min.y)
            {
                m_work_area_min = NousConvert::convert(work_area_min);
                flags.set_value("clipbox_dirty", true);
            }
        }

        if(props.has_field("work_area_max") && props.get_value_type("work_area_max") == ESketchParamValueType::Vector2)
        {
            nous::vec2 work_area_max;
            props.get_value("work_area_max", work_area_max);
            if(work_area_max.x() != m_work_area_max.x || work_area_max.y() != m_work_area_max.y)
            {
                m_work_area_max = NousConvert::convert(work_area_max);
                flags.set_value("clipbox_dirty", true);
            }
        }

        if(props.has_field("cam_pivot") && props.get_value_type("cam_pivot") == ESketchParamValueType::Vector3)
        {
            nous::vec3 cam_pivot;
            props.get_value("cam_pivot", cam_pivot);
            m_cam_pivot = NousConvert::convert(cam_pivot);
        }

        if(props.has_field("rotH") && props.get_value_type("rotH") == ESketchParamValueType::Double)
        {
            props.get_value("rotH", m_rot_hor);
        }

        if(props.has_field("rotV") && props.get_value_type("rotV") == ESketchParamValueType::Double)
        {
            props.get_value("rotV", m_rot_ver);
        }

        if(props.has_field("disp_setting") && props.get_value_type("disp_setting") == ESketchParamValueType::SketchParamMap)
        {
            SketchParamMapPtr disp_setting_map;
            props.get_value("disp_setting", disp_setting_map);
            m_disp_setting->modify(*disp_setting_map);
        }

        return true;
    }

    void _updateClipBox()
    {
        m_clip_box.m_min.xy = m_work_area_min;
        m_clip_box.m_min.z = -m_depth_down;
        m_clip_box.m_max.xy = m_work_area_max;
        m_clip_box.m_max.z = m_depth_up;
        m_clip_box.setMatrix(m_view_plane.matrix());
    }

protected:
    long long m_id;
    SceneDoc* m_scene;
    SketchParamMap m_props;
    std::wstring m_name;
    bool m_temp;

    Define::ViewType m_type;
    Define::ViewProjMode m_proj_mode;

    Engine::AxisPlane m_view_plane;

    double m_rot_hor;
    double m_rot_ver;
    double m_depth_up;
    double m_depth_down;

    Engine::Vec2 m_work_area_min;
    Engine::Vec2 m_work_area_max;
    Engine::OBB m_clip_box;
    Engine::Vec3 m_cam_pivot;

    ViewDisplaySetting* m_disp_setting;
    ObjectFilter* m_disp_filter;

    bool m_props_dirty;

    View* m_render_view;
};

ViewDoc* ViewDoc::create(long long id, SceneDoc* pScene) { return new ViewDocImpl(id, pScene); }

ViewDoc::~ViewDoc(void) {}

}// namespace sketch