#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderScene/Group.h"
#include "sketch/View/RenderScene/Mesh/Mesh.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewManager.h"

namespace sketch
{
RenderScene::OBJ_INST_FUNC* RenderScene::m_instFuncs = nullptr;

RenderScene::RenderScene(SceneDoc* pDoc)
{
    m_objChangeCallback = nullptr;
    if(!m_instFuncs)
        this->_initInstFuncs();

    m_pDoc = pDoc;
    pDoc->setRenderScene(this);
    pDoc->addListener(this);
}

RenderScene::~RenderScene() {}

RenderObject* RenderScene::_createObject(const Define::ObjectType type, long long id)
{
    auto instFunc = m_instFuncs[type];
    if(!instFunc)
        return nullptr;

    RenderObject* pObj = instFunc(this, type);
    m_objects[id] = pObj;
    return pObj;
}

//-----------------------------------------------------------------------
void RenderScene::_deleteObject(long long id)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    auto it = m_objects.find(id);
    if(it == m_objects.end())
        return;

    RenderObject* pObj = (RenderObject*)it->second;
    m_objects.erase(it);
    m_viewDirtyObjs.erase(pObj->id());

    for(auto& it: pViewMgr->views())
    {
        auto pView = it.second;
        if(pView->scene() == this)
        {
            pView->removeRenderObject(pObj);
            pView->delete_mesh(id);
        }
    }

    delete pObj;
}

bool RenderScene::onDocLoaded(const Engine::Path& projPath, const std::wstring& progName, float progRange)
{
    auto pViewMgr = STCore::inst->viewMgr();

    for(auto it: pViewMgr->views())
    {
        View* pView = it.second;
        if(pView && pView->doc()->scene() == m_pDoc)
            pView->dirty(View::DIRTY_BIT_VIEW_CLIPBOX | View::DIRTY_BIT_CAM_DEPTH | View::DIRTY_BIT_RT | View::DIRTY_BIT_VIEW_BBOX);

        if(!progName.empty())
        {
            STCore::inst->moveUIProgress(progName, progRange / pViewMgr->views().size());
            STCore::inst->updateUIProgress(progName);
        }
    }

    m_viewDirtyObjs.clear();

    return true;
}

void RenderScene::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker)
{
    switch(method)
    {
    case Define::MSG_OBJ_CREATE: {
        long long id = params->get("obj_id")._long();
        ObjectDoc* pObjDoc = (ObjectDoc*)m_pDoc->getObject(id);
        RenderObject* pRenderObj = this->_createObject(pObjDoc->type(), id);
        pObjDoc->_tfObject()->setRenderObject(pRenderObj);
        if(pRenderObj)
        {
            pRenderObj->setDoc(pObjDoc);
        }
    }
    break;
    case Define::MSG_OBJ_DELETE: {
        long long id = params->get("obj_id")._long();
        if(m_objChangeCallback)
            m_objChangeCallback(method, this->getObject(id));
        this->_deleteObject(id);
    }
    break;
    case Define::MSG_OBJ_UPDATE: {
        long long id = params->get("obj_id")._long();
        if(m_objChangeCallback)
            m_objChangeCallback(method, this->getObject(id));
    }
    break;
    case Define::MSG_SCENE_DELETE: {
        long long id = params->get("scene_id")._long();
        if(id != m_pDoc->id())
            return;

        for(auto& it: m_objects)
        {
            auto pObj = it.second;
            delete pObj;
        }

        m_objects.clear();
        m_viewDirtyObjs.clear();
    }
    break;
    }
}

void RenderScene::updateObjInViews(RenderObject* pObj) { m_viewDirtyObjs.insert(pObj->id()); }

void RenderScene::update(unsigned long long tm)
{
    auto pViewMgr = STCore::inst->viewMgr();

    if(!m_viewDirtyObjs.empty())
    {
        std::vector<RenderObject*> updatedObjs;
        auto tmpObjs = m_viewDirtyObjs;// 防止组件更新中有对象增删导致m_viewDirtyObjs变动
        for(auto id: tmpObjs)
        {
            auto pObj = m_pDoc->getTFObject(id);
            if(!pObj)
                continue;

            // 强制组件、构件等对象更新
            // 否则，pView->updateRenderObjects会用到visFrame， 而对象实时更新也会用到，发生冲突
            pObj->bbox();
            updatedObjs.push_back(pObj->renderObject());
        }

        for(auto& it: pViewMgr->layout()->cells())
        {
            if(it.second->cellType() != Define::CELL_VIEW)
                continue;

            auto pView = it.second->_view();
            if(pView->scene() == this)
                pView->updateRenderObjects(updatedObjs);
        }

        m_viewDirtyObjs.clear();
    }
}

void RenderScene::_initInstFuncs()
{
    m_instFuncs = new OBJ_INST_FUNC[Define::OBJ_TYPE_END];
    memset(m_instFuncs, 0, sizeof(OBJ_INST_FUNC) * Define::OBJ_TYPE_END);

    m_instFuncs[Define::OBJ_D5_GROUP] = Group::newInstance;
    m_instFuncs[Define::OBJ_D5_MESH] = Mesh::newInstance;
    m_instFuncs[Define::OBJ_D5_NURBS] = Mesh::newInstance;
}
}// namespace sketch