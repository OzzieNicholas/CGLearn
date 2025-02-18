#include "sketch/View/RenderScene/Group.h"

#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/GroupDoc.h"
#include "sketch/Document/Scene/Mesh/MeshDoc.h"
#include "sketch/Document/Scene/Mesh/NurbsDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/Select/ElementSelector.h"

#include "sketch/NousEngine/NousConvert.h"
namespace sketch
{
Group::Group(RenderScene* pScene) : RenderObject(pScene)
{
    m_type = Define::OBJ_D5_GROUP;

    m_matrixDirty = true;
    m_geoDirty = true;
}

Group::~Group() {}

void Group::dispose() { return; }

void Group::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker)
{
    switch(method)
    {
    case Define::MSG_OBJ_UPDATE: {
        m_matrixDirty = true;
        m_geoDirty = true;
        m_pScene->updateObjInViews(this);
    }
    break;
    }
}

void Group::_onUpdateGeometry() {}

void Group::_onUpdateTransform()
{
    if(m_pDoc == nullptr)
        return;

    DocManager* pDocMgr = STCore::inst->docMgr();
    Document* pDoc = pDocMgr->document();

    auto& world_matrix = m_pDoc->world_matrix();

    this->setMatrix(/*pDoc->d2vMatrix() * */ world_matrix);// batchRenderLine不需要转换到视图坐标系
}

void Group::prepareDrawBatch(std::vector<Engine::DrawBatch*>& opaqueBatches, std::vector<Engine::DrawBatch*>& transBatches, View* pView)
{
    N_UNUSED(opaqueBatches);
    N_UNUSED(transBatches);

    prepare_render(pView);
}

void Group::prepare_render(View* view)
{
    RenderObject::prepare_render(view);

    TFObjectDoc* show_group = view->working_group_area();
    if(show_group == this->doc())
    {
        Engine::OBB obb = this->doc()->bbox();
        obb.set(obb.m_min - Engine::Vec3(20.0), obb.m_max + Engine::Vec3(20.0), Engine::Mat3::IDENTITY);
        Engine::Mat43f matrix = this->matrix();
        std::vector<Engine::Line>& linsX = obb.edges('x');
        std::vector<Engine::Line>& linsY = obb.edges('y');
        std::vector<Engine::Line>& linsZ = obb.edges('z');
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsX[0].v0, matrix * linsX[0].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsX[1].v0, matrix * linsX[1].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsX[2].v0, matrix * linsX[2].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsX[3].v0, matrix * linsX[3].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsY[0].v0, matrix * linsY[0].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsY[1].v0, matrix * linsY[1].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsY[2].v0, matrix * linsY[2].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsY[3].v0, matrix * linsY[3].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsZ[0].v0, matrix * linsZ[0].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsZ[1].v0, matrix * linsZ[1].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsZ[2].v0, matrix * linsZ[2].v1, 0x000fu, 1, true, true);
        view->batchRenderLine(1, 0.5f, Engine::Vec3f::BLACK, matrix * linsZ[3].v0, matrix * linsZ[3].v1, 0x000fu, 1, true, true);

        return;
    }

    auto& select_info = view->selector()->get_select_info();
    auto& find_pos = select_info.find(this);

    if(find_pos != select_info.end())
    {
        std::vector<std::pair<Engine::Vec3, Engine::Vec3>> group_lines;

        Engine::OBB obb = this->doc()->bbox();
        Engine::Mat43f matrix = this->matrix();
        std::vector<Engine::Line>& linsX = obb.edges('x');
        std::vector<Engine::Line>& linsY = obb.edges('y');
        std::vector<Engine::Line>& linsZ = obb.edges('z');
        group_lines.emplace_back(matrix * linsX[0].v0, matrix * linsX[0].v1);
        group_lines.emplace_back(matrix * linsX[1].v0, matrix * linsX[1].v1);
        group_lines.emplace_back(matrix * linsX[2].v0, matrix * linsX[2].v1);
        group_lines.emplace_back(matrix * linsX[3].v0, matrix * linsX[3].v1);
        group_lines.emplace_back(matrix * linsY[0].v0, matrix * linsY[0].v1);
        group_lines.emplace_back(matrix * linsY[1].v0, matrix * linsY[1].v1);
        group_lines.emplace_back(matrix * linsY[2].v0, matrix * linsY[2].v1);
        group_lines.emplace_back(matrix * linsY[3].v0, matrix * linsY[3].v1);
        group_lines.emplace_back(matrix * linsZ[0].v0, matrix * linsZ[0].v1);
        group_lines.emplace_back(matrix * linsZ[1].v0, matrix * linsZ[1].v1);
        group_lines.emplace_back(matrix * linsZ[2].v0, matrix * linsZ[2].v1);
        group_lines.emplace_back(matrix * linsZ[3].v0, matrix * linsZ[3].v1);

        // mesh show mode?
        this->_get_group_mesh_lines(this, group_lines);

        // draw lines
        for(auto& line: group_lines)
        {
            view->batchRenderLine(2, 1.0f, Engine::Vec3f::LIGHT_BLUE, line.first, line.second, 0xffffu, 1, false, false);
        }
    }
}

void Group::_get_group_mesh_lines(Group* group, std::vector<std::pair<Engine::Vec3, Engine::Vec3>>& lines)
{
    auto& children = group->doc()->_group()->children();
    for(auto child: children)
    {
        if(child->type() == Define::OBJ_D5_GROUP)
        {
            _get_group_mesh_lines(static_cast<Group*>(child->renderObject()), lines);
        }
        else if(child->type() == Define::OBJ_D5_MESH)
        {
            MeshDoc* mesh_doc = static_cast<MeshDoc*>(child);
            Engine::Mat43 mat = mesh_doc->world_matrix();
            auto geo = mesh_doc->geodata();

            std::unordered_map<SketchPointPtr, Engine::Vec3> point_cache;
            for(auto& line: geo->get_all_lines())
            {
                SketchPointPtr p0 = line.second->p0;
                SketchPointPtr p1 = line.second->p1;
                auto it0 = point_cache.find(p0);
                if(it0 == point_cache.end())
                {
                    point_cache[p0] = mat * NousConvert::convert(geo->get_point_position(p0));
                }
                auto it1 = point_cache.find(p1);
                if(it1 == point_cache.end())
                {
                    point_cache[p1] = mat * NousConvert::convert(geo->get_point_position(p1));
                }
                lines.emplace_back(point_cache[p0], point_cache[p1]);
            }
        }
        else if(child->type() == Define::OBJ_D5_NURBS)
        {
            NurbsDoc* nurbs_doc = static_cast<NurbsDoc*>(child);
            Engine::Mat43 mat = nurbs_doc->world_matrix();
            auto nurbs = nurbs_doc->nurbsdata();
            for(auto& item: nurbs->get_all_nurbs())
            {
                if(item.second->type() == EElementType::Curve)
                {
                    NurbsCurve* curve = item.second->to_nurbs_curve().get();
                    const std::vector<nous::vec3>& points = curve->get_curve_position_arr();
                    for(int i = 1; i < points.size(); ++i)
                    {
                        const nous::vec3& pos_0 = points[i];
                        const nous::vec3& pos_1 = points[i - 1];
                        lines.emplace_back(mat * NousConvert::convert(pos_0), mat * NousConvert::convert(pos_1));
                    }
                    if(curve->is_curve_closed())
                    {
                        const nous::vec3& pos_0 = points.back();
                        const nous::vec3& pos_1 = points.front();
                        lines.emplace_back(mat * NousConvert::convert(pos_0), mat * NousConvert::convert(pos_1));
                    }
                }
                else if(item.second->type() == EElementType::AlgebraSurface)
                {
                    NurbsAlgebraSurface* surface = item.second->to_nurbs_algebra_surface().get();
                    const std::vector<std::vector<nous::vec3>>& points = surface->get_surface_bounding_lines();
                    for(int i = 0; i < points.size(); ++i)
                    {
                        for(int j = 1; j < points[i].size(); ++j)
                        {
                            const nous::vec3& pos_0 = points[i][j];
                            const nous::vec3& pos_1 = points[i][j - 1];
                            lines.emplace_back(mat * NousConvert::convert(pos_0), mat * NousConvert::convert(pos_1));
                        }
                    }
                }
                else if(item.second->type() == EElementType::ClosedSurface)
                {
                    NurbsClosedSurface* curve_brep = item.second->to_nurbs_closed_surface().get();
                    const std::vector<nous::vec3>& points = curve_brep->get_curve_position_arr();
                    for(int i = 1; i < points.size(); ++i)
                    {
                        const nous::vec3& pos_0 = points[i];
                        const nous::vec3& pos_1 = points[i - 1];
                        lines.emplace_back(mat * NousConvert::convert(pos_0), mat * NousConvert::convert(pos_1));
                    }
                    const nous::vec3& pos_0 = points.back();
                    const nous::vec3& pos_1 = points.front();
                    lines.emplace_back(mat * NousConvert::convert(pos_0), mat * NousConvert::convert(pos_1));
                }
            }
        }
    }
}

}// namespace sketch
