#include "sketch/View/ViewLayout/ViewCamera.h"

#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/AssetMesh.h"
#include "Engine/Asset/AssetMaterial.h"
#include "Engine/Asset/AssetData/MeshData.h"
#include "Engine/Asset/AssetData/MaterialBlinnPhong.h"

#include "sketch/Document/Document.h"

using namespace Engine;

namespace sketch
{

void ViewCamera::setDirByEuler(float anglH, float anglV)
{
    //TODO
}

void ViewCamera::getDocPickRay(Document* pDoc, float x, float y, Engine::Vec3* pOrg, Engine::Vec3* pDir)
{
    Vec3f origin, dir;
    this->getPickRay(x, y, &origin, &dir);

    *pOrg = pDoc->v2d(origin);
    *pDir = dir;
}

Engine::Vec3 ViewCamera::getDocPickNearPoint(Document* pDoc, float x, float y)
{
    return pDoc->v2d(getPickNearPoint(x, y));
}

Engine::Vec3 ViewCamera::getDocPickFarPoint(Document* pDoc, float x, float y)
{
    return pDoc->v2d(getPickFarPoint(x, y));
}





}