#pragma once

#include "./RenderObject.h"

namespace Engine
{
class DrawBatch;
}
namespace sketch
{
class View;
class Group : public RenderObject
{
public:
    Group(RenderScene* pScene);
    virtual ~Group();
    virtual void dispose();

    static RenderObject* newInstance(RenderScene* pScene, Define::ObjectType type) { return new Group(pScene); }

    virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) override;

    virtual void prepareDrawBatch(std::vector<Engine::DrawBatch*>& opaqueBatches, std::vector<Engine::DrawBatch*>& transBatches,
                                  View* pView) override;

protected:
    virtual void _onUpdateGeometry() override;
    virtual void _onUpdateTransform() override;
    void prepare_render(View* view) override;
    void _get_group_mesh_lines(Group* group, std::vector<std::pair<Engine::Vec3, Engine::Vec3>>& lines);

private:
};
}// namespace sketch