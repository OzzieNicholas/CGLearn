#pragma once

#include "..\..\Core.h"
#include "..\..\Define.h"
#include <string>
#include <unordered_set>

#include "Engine\Math\Mat4.h"
#include "Engine\Math\OBB.h"
#include "TFObjectDoc.h"

namespace sketch
{
class Document;
class RenderObject;

class GroupDoc : public TFObjectDoc
{
public:
    GroupDoc(SceneDoc* pScene, long long id);
    virtual ~GroupDoc();
    virtual void dirty();

    static ObjectDoc* newInstance(SceneDoc* pScene, long long id) { return new GroupDoc(pScene, id); }
    virtual void record();

    virtual void get_props(SketchParamMap& props) override;
    virtual void set_props(const SketchParamMap& props) override;

    virtual void copy_from(TFObjectDoc* obj);

    virtual const std::unordered_set<TFObjectDoc*>& children() { return m_children; }

    virtual void addChild(TFObjectDoc* pObj);
    virtual void removeChild(TFObjectDoc* pObj);

    virtual void addChildren(const std::unordered_set<long long>& ids);
    virtual void removeChildren(const std::unordered_set<long long>& ids);

    virtual void removeAllChildren();

protected:
    virtual void _updateBBox();
};
}// namespace sketch