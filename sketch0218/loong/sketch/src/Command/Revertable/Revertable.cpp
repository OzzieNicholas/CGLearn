#include "sketch/Command/Revertable/Revertable.h"

namespace sketch
{
Revertable::Revertable(Define::RevertableType type) : m_type(type) {}

bool Revertable::isValid() { return true; }

bool Revertable::undo() { return true; }

bool Revertable::redo() { return true; }
}// namespace sketch
