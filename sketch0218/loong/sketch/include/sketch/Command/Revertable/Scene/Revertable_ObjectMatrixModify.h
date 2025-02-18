#pragma once

#include "../../../Core.h"
#include "../Revertable.h"
#include "Engine/Math/Mat43.h"
#include "Engine/Util/ObjectMemPool.h"

namespace sketch
{
class Revertable_ObjectMatrixModify : public Revertable
{
public:
    Revertable_ObjectMatrixModify(long long objId);
    virtual ~Revertable_ObjectMatrixModify();

    OBJ_MEM_POOL_DECL;

    virtual bool isValid() override;
    virtual bool undo() override;
    virtual bool redo() override;

    void setMatBefore(const Engine::Mat43& pos);
    void setMatAfter(const Engine::Mat43& pos);

protected:
    long long m_objId;
    Engine::Mat43 m_matBefore;
    Engine::Mat43 m_matAfter;
};
}
