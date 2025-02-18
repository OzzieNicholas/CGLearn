#pragma once

#include "../Core.h"
#include <functional>
#include <string>

namespace Engine
{
class DObject;

}

namespace sketch
{
class Command;
class RvtPool;
class Revertable;

class CORE_DLL_DECLSPEC CmdManager
{
public:
    static CmdManager* create();
    virtual ~CmdManager() = default;

    static void regCmd(const std::string& cmd, std::function<Engine::DObject*(const Engine::DObject*)>);
    static void unRegCmd(const std::string& cmd);

    virtual void init() = 0;
    virtual void onAppClose() = 0;

    virtual void pushCmd(Command* pCmd) = 0;
    virtual void pushCmd(const std::string& cmd, const Engine::DObject* paras) = 0;
    virtual void execute(Command* pCmd) = 0;
    virtual void update() = 0;

    virtual bool isRvtFreezed(long long pid) const = 0;
    virtual void freezeRvt(long long pid) = 0;
    virtual void unfreezeRvt(long long pid) = 0;

    virtual long long createRvtPool() = 0;
    virtual void deleteRvtPool(long long pid) = 0;

    // virtual void clearRvts(long long pid) = 0;

    virtual void beginRvtQueue(long long pid) = 0;
    virtual void endRvtQueue(long long pid) = 0;
    virtual void continueRvtQueue(long long pid) = 0;
    virtual void pushRvt(long long pid, Revertable* pRvt) = 0;

    // virtual bool isRvtWorking(long long pid) const = 0;

    virtual void undo(long long pid) = 0;
    virtual void redo(long long pid) = 0;

    virtual void pop_undo(long long pid) = 0;
    virtual void pop_redo(long long pid) = 0;
};

}// namespace sketch