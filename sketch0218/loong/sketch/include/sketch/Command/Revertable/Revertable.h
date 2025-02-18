#pragma once

#include <string>

#include "../../Core.h"
#include "../../Define.h"

namespace sketch
{
class Revertable
{
public:
    Revertable(Define::RevertableType type);
    virtual ~Revertable() = default;

    inline Define::RevertableType type() const { return m_type; }

    virtual bool isValid();
    virtual bool undo();
    virtual bool redo();

protected:
    Define::RevertableType m_type;
};
}// namespace sketch
