#pragma once

#include "../../Core.h"

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "../../Document/DocListener.h"
#include "../RenderRes/RenderMesh.h"
#include "../ViewLayout/LayoutCell.h"
#include "Engine/Math/Vec2f.h"
#include "Engine/Util/Recorder.h"
#include "IRender.h"

namespace Engine
{
class MeshData;
class TextTagArray;
class DctLight;
class DrawBatchBlinnPhong;
class DrawBatchFlatText;
class TextTag;
class Texture2D;
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;
}// namespace Engine

namespace sketch
{
class SketchSerializer;
class ViewManager;
class ViewDoc;
class ViewCamera;
class AxisTip;
class RenderObject;
class ViewClipPlane;
class VIEW_BATCH_BLOCK;
class VIEW_TEXT_BLOCK;
class RenderScene;
class ObjectDoc;
class GroupDoc;
class TFObjectDoc;
class GeoData;
class ElementSelector;
class InferenceHelper;
class NurbsDoc;
class MeshDoc;
class SelectInfo;

class View : public LayoutCell
{
public:
    View();
    virtual ~View(void);

    virtual bool init(ViewDoc* pDoc);
    virtual void release() override;

    virtual void redraw(void) override;

    virtual void modifyCellProps(const SketchParamMap& props) override;

    virtual long long cellId() const override;
    virtual const std::wstring cellName() const override;

    long long id() const;

    inline ViewDoc* doc() const { return m_pDoc; }

    inline RenderScene* scene() const { return m_pScene; }

    inline long long redrawFrame() const { return m_redrawFrame; }

    inline void dirty(uint32_t flags) { m_dirtyFlags |= flags; }

    void updateRedrawFrame();

    void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pSrc) override;
    void onRenderCell(long long lElapsedTm) override;
    void onCellAdded(int renderId) override;
    void onCellRemoved() override;
    void onLayoutResize(int width, int height) override;

    void prepare_mesh(RenderMesh mesh);
    void delete_mesh(long long id) const;
    void batchRenderCtrlPoints(int idxMode /*0:first 1:second 2:normal*/, bool isSel, bool isHover, const Engine::Vec3& posW);
    void batchRenderScreenLine(int width, float alpha, const Engine::Vec3f& color, const Engine::Vec3f& v0, const Engine::Vec3f& v1, uint16_t pattern,
                               uint8_t factor, bool occlude);
    /*
     * @param patten 0xffff 实线 0x00ff 虚线 0xf000 点划线 以此类推 f是画 0是不画
     * @param factor 填 1 完事儿了
     * @param clip 是否被相机裁减
     * @param occlude 是否被遮挡
     */
    void batchRenderLine(int width, float alpha, const Engine::Vec3f& color, const Engine::Vec3& v0, const Engine::Vec3& v1, uint16_t pattern,
                         uint8_t factor, bool clip, bool occlude);
    void batchRenderTri(const Engine::Vec3f& color, const Engine::Vec3& v0, const Engine::Vec3& v1, const Engine::Vec3& v2, bool occlude,
                        bool twoSide, bool clip, float alpha);

    inline Engine::DctLight* light() const { return m_pLight; }

    inline long long camFrame() const { return m_camFrame; }

    virtual void onViewTypeChanged();
    void onCameraTransform() override;
    void resetCameraPose() override;
    virtual void updateCameraPose();

    inline const std::unordered_set<RenderObject*>& objects()
    {
        if(m_dirtyFlags & DIRTY_BIT_VIEW_CLIPBOX)
        {
            this->_onReclipObjects();
            m_dirtyFlags &= ~DIRTY_BIT_VIEW_CLIPBOX;
        }

        return m_objects;
    }

    inline const std::unordered_set<RenderObject*>& visObjects()
    {
        if(m_dirtyFlags & (DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER))
        {
            if(m_dirtyFlags & DIRTY_BIT_VIEW_CLIPBOX)
                this->_onReclipObjects();

            this->_onRefilterObjs();

            m_dirtyFlags &= ~(DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER);
        }

        return m_visObjects;
    }

    inline const Engine::AABBf& bbox()
    {
        if(m_dirtyFlags & (DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_VIEW_BBOX))
        {
            if(m_dirtyFlags & DIRTY_BIT_VIEW_CLIPBOX)
                this->_onReclipObjects();

            if(m_dirtyFlags & (DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER))
                this->_onRefilterObjs();

            this->_updateBBox();
            m_dirtyFlags &= ~(DIRTY_BIT_VIEW_CLIPBOX | DIRTY_BIT_VIEW_FILTER | DIRTY_BIT_VIEW_BBOX);
        }

        return m_bbox;
    }

    bool isInDepthRange(const Engine::OBB& obb) const;

    void updateRenderObjects(const std::vector<RenderObject*>& objs);
    void removeRenderObject(RenderObject* pObj);

    inline const std::unordered_set<RenderObject*> getHideObjects() const { return m_hideObjects; }

    void clearHideObjects();
    void hideObjects(const std::unordered_set<ObjectDoc*>& objs);
    void setHideObjects(const std::unordered_set<ObjectDoc*>& objs);

    void set_working_group_area(TFObjectDoc* area) { m_group_area = area; }
    TFObjectDoc* working_group_area() { return m_group_area; }
    bool obj_in_working_group(TFObjectDoc* obj);
    TFObjectDoc* get_obj_highest_parent_in_working_group_area(TFObjectDoc* obj);
    //-1不在工作区，0为当前工作区，123...为在工作区的层级
    int get_obj_in_working_group_area_layer(TFObjectDoc* obj);

    void weld_lines_from_select();
    void disperse_weld_line_from_select();
    void delete_obj_from_select();
    void delete_obj_and_try_delete_relation(TFObjectDoc* obj);
    void convert_nurbs_item_into_geodata(NurbsDoc* nurbs_doc, MeshDoc* target, const SelectInfo mesh_select, nous::mat4& group_transform);
    void convert_to_geodata_from_select();
    GroupDoc* create_group_from_select_elemnt();
    TFObjectDoc* create_obj_in_working_group(Define::ObjectType type, GroupDoc* working_group = nullptr);
    void disperse_group_from_select();
    void copy_group(GroupDoc* org, GroupDoc* target);
    // void copy_select_to_clipbord();
    // void paste_clipbord();
    // void clear_clipbord();
    void check_intersection_and_merge_meshes(std::unordered_set<TFObjectDoc*>& src_meshes, std::unordered_set<TFObjectDoc*>& target_meshes);

    void serialize_view_list(SketchSerializer* serializer);
    void serialize_tfobj(TFObjectDoc* obj, SketchSerializer* serializer);

    inline ElementSelector* selector() { return m_selector; }
    inline InferenceHelper* inferHelper() { return m_inferenceHelper; }

    inline void set_render(IRender* render) { m_render = render; }
    inline IRender* render() const { return m_render; }

protected:
    virtual void _updateCellProps() override;
    virtual void _resetBatchBlocks() override;
    virtual void _prepareBatchBlocks() override;

    void sceneChangedCallback(Define::Message method, RenderObject* pObj);

    void _refreshRT();

    void _onReclipObjects();
    void _onRefilterObjs();

    void _updateBBox();
    void _updateCamDepth();

    void _resetCameraPose();
    void _initCamPivotVBuf(float* pVBuf, float r, int segs);

protected:
    ViewDoc* m_pDoc;
    RenderScene* m_pScene;
    Engine::DctLight* m_pLight;

    // Engine::Texture2D* m_pRT;

    ViewCamera* m_pAxisTipCam;
    AxisTip* m_pAxisTip;

    /*Engine::DrawBatchBlinnPhong* m_pWorldAxisBatch;
    Engine::DrawBatchBlinnPhong* m_pSkyBoxBatch;*/

    // render objects
    std::unordered_set<RenderObject*> m_objects;
    std::unordered_set<RenderObject*> m_visObjects;
    std::unordered_set<RenderObject*> m_hideObjects;

    TFObjectDoc* m_group_area;

    // reset
    long long m_redrawFrame;

    Engine::AABBf m_bbox;

    long long m_camFrame;

    Engine::Recorder m_recorder;

    // picker
    ElementSelector* m_selector;
    InferenceHelper* m_inferenceHelper;

    // render
    IRender* m_render;
};

}// namespace sketch
