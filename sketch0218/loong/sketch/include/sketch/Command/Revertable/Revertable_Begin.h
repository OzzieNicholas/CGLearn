#pragma once

#include "../../Core.h"
#include "Revertable.h"

#include "Engine/Util/ObjectMemPool.h"

namespace Engine
{
class DObject;
}

namespace sketch
{
class Revertable_Begin : public Revertable
{
public:
    Revertable_Begin();
    virtual ~Revertable_Begin() = default;

    OBJ_MEM_POOL_DECL;

protected:
};
}// namespace sketch
