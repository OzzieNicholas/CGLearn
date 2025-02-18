#include "sketch/Command/Revertable/RvtBlock.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/STCore.h"

namespace sketch
{
sketch::RvtBlock::RvtBlock(long long pid, bool continue_last_queue) : m_pid(pid)
{
    if(continue_last_queue)
    {
        STCore::inst->cmdMgr()->continueRvtQueue(pid);
    }
    else
    {
        STCore::inst->cmdMgr()->beginRvtQueue(pid);
    }
}
RvtBlock::~RvtBlock() { STCore::inst->cmdMgr()->endRvtQueue(m_pid); }

bool RvtBlock::lock(long long pid)
{
    if(m_pid >= 0)
    {
        return false;
    }

    m_pid = pid;
    STCore::inst->cmdMgr()->beginRvtQueue(pid);
    return true;
}

}// namespace sketch