#pragma once

#include "../../../Core.h"
#include "../Revertable.h"
#include "sketch/Object/SketchParamMap.h"

namespace Engine
{
class DObject;
}

namespace sketch
{
class Revertable_ObjectDelete : public Revertable
{
public:
    Revertable_ObjectDelete(Define::ObjectType objType, long long objId);
    virtual ~Revertable_ObjectDelete(void);

    OBJ_MEM_POOL_DECL;

    virtual bool undo();
    virtual bool redo();

protected:
    long long m_objId;
    Define::ObjectType m_objType;

    SketchParamMap m_undo_props;
};

}// namespace sketch
