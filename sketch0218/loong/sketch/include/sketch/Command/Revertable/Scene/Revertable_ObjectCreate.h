#pragma once

#include "../../../Core.h"
#include "../Revertable.h"
#include "sketch/Object/SketchParamMap.h"

namespace Engine
{
class DObject;
class Guid;
}// namespace Engine

namespace sketch
{
class Revertable_ObjectCreate : public Revertable
{
public:
    Revertable_ObjectCreate(long long objId, Define::ObjectType objType);
    virtual ~Revertable_ObjectCreate(void);

    OBJ_MEM_POOL_DECL;

    virtual bool undo();
    virtual bool redo();

protected:
    long long m_objId;
    Define::ObjectType m_objType;

    SketchParamMap m_redo_props;
};

}// namespace sketch
