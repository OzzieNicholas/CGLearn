#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/STCore.h"

#include "Engine/Math/GeoUtil.h"
#include "Engine/Math/Mat4f.h"
#include "Engine/Math/Vec3f.h"
#include "Engine/Render/Device.h"
#include "Engine/Render/DrawBatch/DrawBatchBlinnPhong.h"

#include "sketch/Document/Configure/Color/ColorManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/ObjectDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/View/Controller/Controller.h"
#include "sketch/View/RenderRes/ResManager.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/RenderView/View.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/ViewLayout/ViewCamera.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
const double RenderObject::CTRL_POINT_SIZE = 8.0;

RenderObject::RenderObject(RenderScene* pScene)
{
    m_pScene = pScene;

    m_type = Define::OBJ_UNKNOWN;
    m_pDoc = nullptr;

    m_geoDirty = true;
}

RenderObject::~RenderObject() {}

void RenderObject::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pInvoker) {}

void RenderObject::setDoc(ObjectDoc* pObjDoc)
{
    if(m_pDoc)
        m_pDoc->removeListener(this);

    m_pDoc = (TFObjectDoc*)pObjDoc;
    m_pDoc->addListener(this);
}

void RenderObject::_onUpdateGeometry() {}

void RenderObject::_updateMatrix()
{
    this->_onUpdateTransform();

    Object3D::_updateMatrix();
}

void RenderObject::_onUpdateTransform() {}

bool RenderObject::isViewContain(View* pView)
{
    auto& clipbox = pView->doc()->clipBox();
    auto& objbbox = m_pDoc->bbox();

    // 物体本身在视图包围盒内
    if(objbbox.isValid())
        return true;

    // 物体在工作区外，但在视图深度内，判断refDst是否在包围盒内
    if(pView->isInDepthRange(objbbox))
    {
        return true;
    }

    return false;
}

void RenderObject::prepareDrawBatch(std::vector<Engine::DrawBatch*>& opaqueBatches, std::vector<Engine::DrawBatch*>& transBatches, View* pView)
{
    prepare_render(pView);
}

void RenderObject::prepare_render(View* view)
{
    if(m_matrixDirty)
    {
        this->_updateMatrix();
        m_matrixDirty = false;
    }

    if(m_geoDirty)
    {
        this->_onUpdateGeometry();
        m_geoDirty = false;
    }
}

}// namespace sketch
