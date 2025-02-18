#pragma once

#include <string>

#include "../../Core.h"
#include "../../Define.h"

namespace sketch
{
class RvtBlock
{
public:
    RvtBlock(long long pid, bool continue_last_queue = false);
    ~RvtBlock();

    bool lock(long long pid);

protected:
    long long m_pid;
};
}// namespace sketch
