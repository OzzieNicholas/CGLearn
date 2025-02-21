#pragma once

#include "../../../Core.h"
#include "../Revertable.h"
#include "sketch/Object/SketchParamMap.h"

#include <memory>

namespace Engine
{
class DObject;
}

namespace sketch
{
class GeoData;
class ObjectDoc;
class Revertable_ObjectModify : public Revertable
{
public:
    Revertable_ObjectModify(ObjectDoc* obj_doc);
    virtual ~Revertable_ObjectModify();

    OBJ_MEM_POOL_DECL;

    bool undo() override;
    bool redo() override;

protected:
    long long m_objId;

    SketchParamMap m_undo_props;
    SketchParamMap m_redo_props;
};
}// namespace sketch
