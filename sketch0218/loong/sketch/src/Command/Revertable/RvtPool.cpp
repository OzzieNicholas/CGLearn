#include "sketch/Command/Revertable/RvtPool.h"

#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Revertable/Revertable_Begin.h"
#include "sketch/Command/Revertable/Revertable_End.h"
#include "sketch/Document/DocManager.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
RvtPool::RvtPool() : m_maxRvtQueue(128), m_rvtQueueNum(0), m_freezeRvt(0), m_rvtQueueDepth(0), m_rvtWorking(false) {}

void RvtPool::clearRvts()
{
    for(auto pRvt: m_undo)
    {
        delete pRvt;
    }
    m_undo.clear();

    for(auto pRvt: m_redo)
    {
        delete pRvt;
    }
    m_redo.clear();

    m_rvtQueueDepth = 0;
    m_rvtQueueNum = 0;
    m_rvtWorking = false;
}

void RvtPool::beginRvtQueue()
{
    if(this->isRvtFreezed())
    {
        return;
    }

    // check if exceed the max length
    m_rvtQueueDepth += 1;
    if(m_rvtQueueDepth == 1)// top level queue
    {
        m_rvtQueueNum += 1;
    }

    if(m_maxRvtQueue > 0 && m_rvtQueueNum > m_maxRvtQueue)
    {
        int depth = 0;
        while(!m_undo.empty())
        {
            Revertable* pRvt = m_undo.front();
            m_undo.pop_front();

            auto type = pRvt->type();
            if(type == Define::RVT_BEGIN)
            {
                depth += 1;
            }
            else if(type == Define::RVT_END)
            {
                depth -= 1;
                if(depth == 0)// top level queue
                {
                    m_rvtQueueNum -= 1;
                    if(m_rvtQueueNum <= m_maxRvtQueue)
                    {
                        break;
                    }
                }
            }
        }
    }

    m_undo.push_back(new Revertable_Begin());
}

void RvtPool::continueRvtQueue()
{
    if(this->isRvtFreezed())
    {
        return;
    }
    if(m_undo.empty())
    {
        return;
    }
    if(m_undo.back()->type() == Define::RVT_END)
    {
        m_undo.pop_back();
    }
}

void RvtPool::pushRvt(Revertable* pRvt) { m_undo.push_back(pRvt); }

void RvtPool::endRvtQueue()
{
    if(this->isRvtFreezed())
    {
        return;
    }
    bool bValid = false;
    for(auto it = m_undo.rbegin(); it != m_undo.rend(); ++it)
    {
        if((*it)->type() == Define::RVT_BEGIN)
        {
            break;
        }
        else if((*it)->isValid())
        {
            bValid = true;
            break;
        }
    }

    if(bValid)
    {
        for(auto pRvt: m_redo)
        {
            delete pRvt;
        }
        m_redo.clear();

        m_undo.push_back(new Revertable_End());
    }
    else
    {
        while(!m_undo.empty())
        {
            auto pRvt = m_undo.back();
            auto rvtType = pRvt->type();

            m_undo.pop_back();
            delete pRvt;

            if(rvtType == Define::RVT_BEGIN)
            {
                break;
            }
        }
    }

    m_rvtQueueDepth -= 1;
}

void RvtPool::undo()
{
    this->freezeRvt();
    m_rvtWorking = true;

    int m_depth = 0;
    while(!m_undo.empty())
    {
        Revertable* pRvt = m_undo.back();
        m_undo.pop_back();

        auto type = pRvt->type();

        m_redo.push_back(pRvt);

        if(type == Define::RVT_END)
        {
            m_depth += 1;
        }
        else if(type == Define::RVT_BEGIN)
        {
            m_depth -= 1;
            if(m_depth == 0)
            {
                break;
            }
        }
        else
            pRvt->undo();
    }

    m_rvtWorking = false;
    this->unfreezeRvt();
}

void RvtPool::redo()
{
    this->freezeRvt();
    m_rvtWorking = true;

    int m_depth = 0;
    while(!m_redo.empty())
    {
        Revertable* pRvt = m_redo.back();
        m_redo.pop_back();

        auto type = pRvt->type();

        m_undo.push_back(pRvt);

        if(type == Define::RVT_BEGIN)
        {
            m_depth += 1;
        }
        else if(type == Define::RVT_END)
        {
            m_depth -= 1;
            if(m_depth == 0)
            {
                break;
            }
        }
        else
            pRvt->redo();
    }

    m_rvtWorking = false;
    this->unfreezeRvt();
}

void RvtPool::pop_undo()
{
    this->freezeRvt();
    m_rvtWorking = true;

    int m_depth = 0;
    while(!m_undo.empty())
    {
        Revertable* pRvt = m_undo.back();
        m_undo.pop_back();

        auto type = pRvt->type();

        if(type == Define::RVT_END)
        {
            m_depth += 1;
        }
        else if(type == Define::RVT_BEGIN)
        {
            m_depth -= 1;
            if(m_depth == 0)
            {
                break;
            }
        }
        else
            pRvt->undo();
    }

    m_rvtWorking = false;
    this->unfreezeRvt();
}

void RvtPool::pop_redo()
{
    this->freezeRvt();
    m_rvtWorking = true;

    int m_depth = 0;
    while(!m_redo.empty())
    {
        Revertable* rvt = m_redo.back();
        m_redo.pop_back();

        auto type = rvt->type();

        if(type == Define::RVT_BEGIN)
        {
            m_depth += 1;
        }
        else if(type == Define::RVT_END)
        {
            m_depth -= 1;
            if(m_depth == 0)
            {
                break;
            }
        }
        else
            rvt->redo();
    }

    m_rvtWorking = false;
    this->unfreezeRvt();
}

}// namespace sketch