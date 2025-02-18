#include "sketch/Command/Revertable/Revertable_End.h"

using namespace Engine;

namespace sketch
{
OBJ_MEM_POOL_IMPL(Revertable_End, 500)

Revertable_End::Revertable_End() : Revertable(Define::RVT_END) {}
}// namespace sketch