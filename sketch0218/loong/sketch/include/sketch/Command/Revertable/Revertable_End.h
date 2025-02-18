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
class Revertable_End : public Revertable
{
public:
    Revertable_End();
    virtual ~Revertable_End() = default;

    OBJ_MEM_POOL_DECL;

protected:
};
}// namespace sketch