#pragma once

#include "Engine/Math/PolygonShape.h"

#include <memory>

#include "../../../Core.h"
#include "../../../Define.h"
#include "../../../Serializer/SketchSerializer.h"
#include "../../../View/RenderRes/IRenderable.h"
#include "../TFObjectDoc.h"

namespace sketch
{
class GeoData;
class MeshDoc : public TFObjectDoc
{
public:
    friend class GeoData;

public:
    MeshDoc(SceneDoc* pScene, long long id);
    virtual ~MeshDoc();
    virtual void dispose();

    virtual void dirty();

    static ObjectDoc* newInstance(SceneDoc* pScene, long long id) { return new MeshDoc(pScene, id); }
    virtual void record();

    virtual std::shared_ptr<IRenderable> geometry(bool record = false);

    std::shared_ptr<GeoData> geodata(bool record = false);

    virtual void get_props(SketchParamMap& props) override;
    virtual void set_props(const SketchParamMap& props) override;

    virtual void copy_from(TFObjectDoc* obj);
    virtual bool merge(TFObjectDoc* obj);
    virtual void merge_without_check(TFObjectDoc* obj);

protected:
    virtual void _updateBBox() override;

protected:
    std::shared_ptr<GeoData> m_geo;
};
}// namespace sketch