#pragma once

#include <memory>

#include "../../../Core.h"
#include "../../../Define.h"
#include "../../../Serializer/SketchSerializer.h"
#include "../../../View/RenderRes/IRenderable.h"
#include "../../Scene/SceneDoc.h"
#include "../TFObjectDoc.h"

namespace sketch
{
class NurbsData;
class NurbsDoc : public TFObjectDoc
{
public:
    friend class NurbsData;

public:
    NurbsDoc(SceneDoc* pScene, long long id);
    virtual ~NurbsDoc();
    virtual void dirty();

    static ObjectDoc* newInstance(SceneDoc* pScene, long long id) { return new NurbsDoc(pScene, id); }
    void record();

    virtual void get_props(SketchParamMap& props) override;
    virtual void set_props(const SketchParamMap& props) override;

    inline virtual std::shared_ptr<IRenderable> geometry(bool record = false) override;

    inline std::shared_ptr<NurbsData> nurbsdata(bool record = false)
    {
        if(record)
        {
            this->record();
        }
        return m_nurbs;
    }

    virtual void copy_from(TFObjectDoc* obj);
    virtual bool merge(TFObjectDoc* obj);

protected:
    virtual void _updateBBox() override;

protected:
    std::shared_ptr<NurbsData> m_nurbs;
};

};// namespace sketch