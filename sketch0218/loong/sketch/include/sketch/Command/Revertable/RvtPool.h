#pragma once

#include "../../Core.h"
#include "../../Define.h"
#include <deque>
#include <string>

namespace sketch
{
class Revertable;

class RvtPool
{
public:
    RvtPool();
    ~RvtPool() = default;

    inline bool isRvtFreezed() const { return m_freezeRvt > 0; }
    inline void freezeRvt() { m_freezeRvt++; }
    inline void unfreezeRvt()
    {
        if(m_freezeRvt > 0)
        {
            m_freezeRvt--;
        }
    }

    void clearRvts();

    void beginRvtQueue();
    void continueRvtQueue();
    void pushRvt(Revertable* pRvt);
    void endRvtQueue();

    inline bool isRvtWorking() const { return m_rvtWorking; }

    void undo();
    void redo();

    void pop_undo();
    void pop_redo();

protected:
    int m_freezeRvt;
    int m_rvtQueueDepth;
    int m_maxRvtQueue;
    int m_rvtQueueNum;
    bool m_rvtWorking;
    std::deque<Revertable*> m_undo;
    std::deque<Revertable*> m_redo;
};
}// namespace sketch
