#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "Engine/Util/DObject.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/NurbsData.h"

namespace sketch
{

NurbsDoc::NurbsDoc(SceneDoc* pScene, long long id) : TFObjectDoc(pScene, Define::OBJ_D5_NURBS, id) { m_nurbs = std::make_shared<NurbsData>(); }

NurbsDoc::~NurbsDoc() {}

void NurbsDoc::dirty()
{
    m_nurbs->dirty();
    this->transform_change();
    m_pScene->invokeListener(Define::MSG_OBJ_UPDATE, Engine::DObject::alloc({ { "obj_id", this->id() } }));
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
    m_dirtyFlags |= DIRTY_BIT_PROPS_MATLW_BBOX_GEO_MEASURE;
}

void NurbsDoc::record()
{
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    if(!cmd_mgr->isRvtFreezed(m_pScene->rvtId()) && this->isRevertable())
    {
        cmd_mgr->pushRvt(m_pScene->rvtId(), new Revertable_ObjectModify(this));
    }
}

void NurbsDoc::get_props(SketchParamMap& props)
{
    TFObjectDoc::get_props(props);

    const std::shared_ptr<SketchObject> geodata_as_obj = std::make_shared<NurbsData>(*m_nurbs);
    props.set_value("geodata", geodata_as_obj);
    m_nurbs->dirty();
}

void NurbsDoc::set_props(const SketchParamMap& props)
{
    TFObjectDoc::set_props(props);

    std::shared_ptr<SketchObject> skobj;
    props.get_value("geodata", skobj);
    std::shared_ptr<NurbsData> reverted_geodata = std::static_pointer_cast<NurbsData>(skobj);

    m_nurbs = std::make_shared<NurbsData>(*reverted_geodata);
    reverted_geodata->dirty();
    m_nurbs->dirty();

    this->dirty();
}

std::shared_ptr<IRenderable> NurbsDoc::geometry(bool record)
{
    if(record)
    {
        this->record();
    }
    return m_nurbs;
}

void NurbsDoc::copy_from(TFObjectDoc* obj)
{
    NurbsDoc* mesh = dynamic_cast<NurbsDoc*>(obj);
    if(mesh)
    {
        std::unordered_map<SketchNurbsPointPtr, SketchNurbsPointPtr> remaped_map;
        for(auto& item: mesh->nurbsdata()->get_all_nurbs())
        {
            m_nurbs->add_item(item.second->clone(remaped_map));
        }
        m_matrix = obj->local_matrix();
        m_world_matrix = obj->world_matrix();
        m_world_matrix_inv = obj->world_matrix_inv();
        m_parent_matrix = obj->parent_matrix();
        m_parent_matrix_inv = obj->parent_matrix_inv();

        this->dirty();
    }
}

bool NurbsDoc::merge(TFObjectDoc* obj)
{
    NurbsDoc* tmesh = dynamic_cast<NurbsDoc*>(obj);
    if(tmesh)
    {
        Engine::Mat43 tmat = tmesh->world_matrix();
        Engine::Mat43 oimat = this->world_matrix_inv();
        Engine::Mat43 trans_mat = oimat * tmat;

        auto origin_nurbs = m_nurbs;
        auto other_nurbs = tmesh->geometry();

        origin_nurbs->merge(std::dynamic_pointer_cast<NurbsData>(other_nurbs));

        return true;
    }

    return false;
}

void NurbsDoc::_updateBBox()
{
    const nous::AABB3 bounding_box = m_nurbs->get_bounding_box();
    m_bbox.set(NousConvert::convert(bounding_box.min()), NousConvert::convert(bounding_box.max()), this->world_matrix());
}

};// namespace sketch