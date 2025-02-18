#pragma once

#include "../../View/RenderRes/IRenderable.h"
#include "Engine/Math/OBB.h"
#include "Engine/Util/Guid.h"
// #include "nous_base/nous_base.h"

#include "ObjectDoc.h"

namespace sketch
{
class RenderObject;
class Revertable_ObjectMatrixModify;
class ViewDoc;

class TFObjectDoc : public ObjectDoc
{
public:
    TFObjectDoc(SceneDoc* pScene, Define::ObjectType type, long long id);
    virtual ~TFObjectDoc();
    virtual void dirty();

    inline virtual std::shared_ptr<IRenderable> geometry(bool record = false) { return nullptr; }

    virtual void record();
    virtual void get_props(SketchParamMap& props) override;
    virtual void set_props(const SketchParamMap& props) override;

    virtual void copy_from(TFObjectDoc* obj);

    virtual void set_matrix_from_local(const Engine::Mat43& m);
    virtual void set_matrix_from_world(const Engine::Mat43& m);
    inline const Engine::Mat43& local_matrix() const { return m_matrix; }
    virtual Engine::Mat43 world_matrix();
    virtual Engine::Mat43 world_matrix_inv();

    const Engine::Mat43& parent_matrix();
    const Engine::Mat43& parent_matrix_inv();

    inline long long viewId() const { return m_viewId; }
    inline void setRenderObject(RenderObject* pRenderObj) { m_pRenderObj = pRenderObj; }
    inline RenderObject* renderObject() const
    {
        if(this->isDisposed())
        {
            return nullptr;
        }
        return m_pRenderObj;
    }

    inline TFObjectDoc* parent() const { return m_parent; }

    Engine::OBB& bbox();

    virtual bool contains(long long id, bool recursive) const;
    virtual bool contains(ObjectDoc* pObj, bool recursive) const;

    virtual void bbox_change();
    virtual void transform_change();

    void set_parent(TFObjectDoc* pPar);

    virtual bool merge(TFObjectDoc* obj) { return false; }

protected:
    virtual void _updateBBox();
    virtual void _update_matrix();

protected:
    long long m_viewId;// for view owned objs, scene obj set to -1

    TFObjectDoc* m_parent;
    std::unordered_set<TFObjectDoc*> m_children;

    Engine::Mat43 m_matrix;
    Engine::Mat43 m_world_matrix;
    Engine::Mat43 m_world_matrix_inv;
    Engine::Mat43 m_parent_matrix;
    Engine::Mat43 m_parent_matrix_inv;

    Engine::OBB m_bbox;

    RenderObject* m_pRenderObj;
};

}// namespace sketch
