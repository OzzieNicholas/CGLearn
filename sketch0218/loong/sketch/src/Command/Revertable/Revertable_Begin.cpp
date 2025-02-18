#include "sketch/Command/Revertable/Revertable_Begin.h"

using namespace Engine;

namespace sketch
{
    OBJ_MEM_POOL_IMPL(Revertable_Begin, 500)

    Revertable_Begin::Revertable_Begin() : Revertable(Define::RVT_BEGIN) {}
}

