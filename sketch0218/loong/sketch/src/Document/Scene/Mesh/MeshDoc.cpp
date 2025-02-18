#include "sketch/Document/Scene/Mesh/MeshDoc.h"

#include "Engine/Math/CCDT/CCDT.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/SysUtil.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/Util/Util.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/GeoDataUtil.h"
#include "sketch/View/RenderRes/NoBranchLineGroup.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/Revertable.h"
#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectCreate.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectDelete.h"
#include "sketch/Command/Revertable/Scene/Revertable_ObjectModify.h"

#include "sketch/NousEngine/NousConvert.h"
#include "sketch/NousEngine/NousTopoUtil.h"
#include "sketch/NousEngine/SketchMath.h"

#include "nous_mesh/mesh.h"

// #define SKETCH_PERF_ENABLE

using namespace Engine;
namespace sketch
{
MeshDoc::MeshDoc(SceneDoc* pScene, long long id) : TFObjectDoc(pScene, Define::OBJ_D5_MESH, id) { m_geo = std::make_shared<GeoData>(); }

MeshDoc::~MeshDoc() {}

void MeshDoc::dispose() { return; }

void MeshDoc::dirty()
{
    m_geo->dirty();
    this->transform_change();
    m_pScene->invokeListener(Define::MSG_OBJ_UPDATE, DObject::alloc({ { "obj_id", this->id() } }));
    this->invokeListener(Define::MSG_OBJ_UPDATE, nullptr);
}

void MeshDoc::record()
{
    CmdManager* cmd_mgr = STCore::inst->cmdMgr();
    if(!cmd_mgr->isRvtFreezed(m_pScene->rvtId()) && this->isRevertable())
    {
        cmd_mgr->pushRvt(m_pScene->rvtId(), new Revertable_ObjectModify(this));
    }
}

void MeshDoc::get_props(SketchParamMap& props)
{
    TFObjectDoc::get_props(props);

    const std::shared_ptr<SketchObject> geodata_as_obj = std::make_shared<GeoData>(*m_geo);
    props.set_value("geodata", geodata_as_obj);
    m_geo->dirty();
}

void MeshDoc::set_props(const SketchParamMap& props)
{
    TFObjectDoc::set_props(props);

    std::shared_ptr<SketchObject> skobj;
    props.get_value("geodata", skobj);
    std::shared_ptr<GeoData> reverted_geodata = std::static_pointer_cast<GeoData>(skobj);

    m_geo = std::make_shared<GeoData>(*reverted_geodata);
    reverted_geodata->dirty();
    m_geo->dirty();

    this->dirty();
}

std::shared_ptr<IRenderable> MeshDoc::geometry(bool record)
{
    if(record)
    {
        this->record();
    }
    return m_geo;
}

std::shared_ptr<GeoData> MeshDoc::geodata(bool record)
{
    if(record)
    {
        this->record();
    }
    return m_geo;
}

void MeshDoc::copy_from(TFObjectDoc* obj)
{
    MeshDoc* mesh = static_cast<MeshDoc*>(obj);
    m_geo = std::make_shared<GeoData>(*mesh->geodata());
    mesh->dirty();
    m_geo->dirty();

    m_matrix = obj->local_matrix();
    m_world_matrix = obj->world_matrix();
    m_world_matrix_inv = obj->world_matrix_inv();
    m_parent_matrix = obj->parent_matrix();
    m_parent_matrix_inv = obj->parent_matrix_inv();

    this->dirty();
}

void MeshDoc::_updateBBox()
{
    nous::AABB3 naabb = m_geo->get_nous_aabb();
    m_bbox.set(NousConvert::convert(naabb.min()), NousConvert::convert(naabb.max()), this->world_matrix());
}

bool MeshDoc::merge(TFObjectDoc* obj)
{
#ifdef SKETCH_PERF_ENABLE
    std::cout << "merge begin time:";
    long long begin_time = Util::getCurrentTimeMS();
    std::cout << begin_time;
#endif

    MeshDoc* tmesh = static_cast<MeshDoc*>(obj);
    Engine::Mat43 tmat = tmesh->world_matrix();
    Engine::Mat43 oimat = this->world_matrix_inv();
    Engine::Mat43 trans_mat = oimat * tmat;

    auto org_geo = m_geo;
    auto target_renderable = tmesh->geometry();
    auto target_geo = std::dynamic_pointer_cast<GeoData>(target_renderable);

#ifdef SKETCH_PERF_ENABLE
    int org_polygon_num = org_geo->get_ring_relation_polygons().size();
    int target_polygon_num = target_geo->get_ring_relation_polygons().size();
    std::cout << "merge org polygon num:";
    std::cout << std::to_string(org_polygon_num);
    std::cout << "merge target polygon num:";
    std::cout << std::to_string(target_polygon_num);
#endif
    if(target_geo)
    {
        // polygon
        for(auto& polygon: target_geo->get_all_polygons())
        {
            std::vector<std::vector<nous::vec3>> polygon_pt_pos_arr;

            std::vector<std::vector<SketchPointPtr>> polygon_points;
            GeoDataUtils::get().get_polygon_topo(target_geo.get(), polygon, polygon_points);
            for(auto ring: polygon_points)
            {
                std::vector<nous::vec3>& process_ring = polygon_pt_pos_arr.emplace_back();
                for(auto point: ring)
                {
                    process_ring.emplace_back(NousConvert::product_v3(trans_mat, target_geo->get_point_position(point)));
                }
            }

            org_geo->add_polygon(polygon_pt_pos_arr);
        }

        // line&group
        std::unordered_map<NoBranchLineGroupPtr, std::unordered_set<SketchElementPtr>> no_branch_line_map;
        std::unordered_map<SmoothGroupPtr, std::unordered_set<SketchElementPtr>> smooth_group_edge_map;
        for(auto& line: target_geo->get_all_lines())
        {
            auto line_adj_rings = target_geo->get_line_adj_rings(line.second);
            // add_polygon中没有处理分割焊接组的情况，所以在这里将所有的边重新add一次，以此来处理分割焊接组
            // 优化项：先addline，拿到point ptr，再addpolygon，这样可以减少addline的次数
            // if(!line_adj_rings.empty() && !line.second->group())// 没有组的多边形边已经在多边形中添加
            //{
            //    continue;
            //}
            nous::vec3 local_p0 = NousConvert::product_v3(trans_mat, target_geo->get_point_position(line.second->p0));
            nous::vec3 local_p1 = NousConvert::product_v3(trans_mat, target_geo->get_point_position(line.second->p1));
            std::vector<SketchLinePtr> org_new_lines = org_geo->add_line(local_p0, local_p1, false);
            ElementGroupPtr group = line.second->group();
            if(group)
            {
                if(group->type() == EElementType::NoBranchLineGroup)
                {
                    NoBranchLineGroupPtr no_branch_group = std::dynamic_pointer_cast<NoBranchLineGroup>(group);
                    if(org_new_lines.size() > 1)
                    {
                        std::unordered_set<SketchPointPtr> point_cache;
                        // 被分割的线段需要在焊接组中添加分割点，以分割焊接组
                        for(auto& line: org_new_lines)
                        {
                            if(!point_cache.insert(line->p0).second)
                            {
                                no_branch_group->add_force_separate_point(line->p0);
                            }
                            if(!point_cache.insert(line->p1).second)
                            {
                                no_branch_group->add_force_separate_point(line->p1);
                            }
                        }
                    }
                    no_branch_line_map[no_branch_group].insert(org_new_lines.begin(), org_new_lines.end());
                }
                else if(group->type() == EElementType::SmoothGroup)
                {
                    SmoothGroupPtr smooth_group = std::dynamic_pointer_cast<SmoothGroup>(group);
                    smooth_group_edge_map[smooth_group].insert(org_new_lines.begin(), org_new_lines.end());
                }
            }
        }
        // 重新添加焊接组
        for(auto& group: no_branch_line_map)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto& line: group.second)
            {
                new_group->add_element(line);
            }
            new_group->add_force_separate_points(group.first->force_separate_points());
            // 清空原焊接组的分割点，保持原焊接组的原始属性
            group.first->clear_force_separate_points();
            org_geo->add_element_group(new_group);
        }
        GeoDataTopoOperations::get().check_and_recreate_no_branch_line_group(org_geo.get());
        // 重新添加平滑组
        for(auto& group: smooth_group_edge_map)
        {
            GeoDataTopoOperations::get().try_merge_smooth_group(org_geo.get(), group.second, false);
        }
        GeoDataTopoOperations::get().check_and_recreate_smooth_group(org_geo.get());
        this->dirty();

#ifdef SKETCH_PERF_ENABLE
        std::cout << "merge end time:";
        long long end_time = Util::getCurrentTimeMS();
        std::cout << std::to_string((end_time - begin_time) * 0.001);
#endif

        return true;
    }

    return false;
}

void MeshDoc::merge_without_check(TFObjectDoc* obj)
{
    MeshDoc* tmesh = dynamic_cast<MeshDoc*>(obj);
    if(!tmesh)
    {
        return;
    }

    Engine::Mat43 tmat = tmesh->world_matrix();
    Engine::Mat43 oimat = this->world_matrix_inv();
    Engine::Mat43 trans_mat = oimat * tmat;

    auto org_geo = m_geo.get();
    auto target_geo = tmesh->geodata();

    // point cache
    std::unordered_map<SketchPointPtr, SketchPointPtr> point_pos;
    size_t point_num = target_geo->get_point_number();
    for(int i = 0; i < point_num; ++i)
    {
        SketchPointPtr np = target_geo->get_point(i);
        nous::vec3 p0 = NousConvert::product_v3(trans_mat, target_geo->get_point_position(np));
        SketchPointPtr nnp = org_geo->add_point_without_check(p0);

        point_pos[np] = nnp;
    }

    // polygon
    for(auto& polygon: target_geo->get_all_polygons())
    {
        std::vector<std::vector<SketchPointPtr>> polygon_points;
        GeoDataUtils::get().get_polygon_topo(target_geo.get(), polygon, polygon_points);

        std::vector<std::vector<SketchPointPtr>> new_polygon_points;
        for(auto& ring: polygon_points)
        {
            std::vector<SketchPointPtr>& new_ring = new_polygon_points.emplace_back();
            for(auto& point: ring)
            {
                new_ring.push_back(point_pos[point]);
            }
        }
        org_geo->add_polygon_without_check(new_polygon_points);
    }

    // line&group
    std::unordered_map<SketchElementPtr, std::unordered_set<SketchElementPtr>> group_line_map;
    for(auto& line: target_geo->get_all_lines())
    {
        SketchLinePtr org_line = org_geo->add_line_without_check(point_pos[line.second->p0], point_pos[line.second->p1]);
        if(line.second->group())
        {
            group_line_map[line.second->group()].insert(org_line);
        }
    }

    for(auto& group: group_line_map)
    {
        if(group.first->type() == EElementType::NoBranchLineGroup)
        {
            NoBranchLineGroupPtr new_group = std::make_shared<NoBranchLineGroup>();
            for(auto& line: group.second)
            {
                new_group->add_element(line);
            }
            org_geo->add_element_group(new_group);
        }
        else if(group.first->type() == EElementType::SmoothGroup)
        {
            GeoDataTopoOperations::get().try_merge_smooth_group(org_geo, group.second, false);
        }
    }

    this->dirty();
}

}// namespace sketch