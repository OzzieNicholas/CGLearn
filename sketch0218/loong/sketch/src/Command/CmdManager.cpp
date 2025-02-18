#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Command.h"
#include "sketch/Command/Revertable/RvtPool.h"

#include <unordered_map>

namespace sketch
{
class CmdManagerImpl : public CmdManager
{
public:
    CmdManagerImpl() = default;
    virtual ~CmdManagerImpl() = default;

    static void register_cmd(const std::string& cmd, std::function<Engine::DObject*(const Engine::DObject*)> cb_func) { m_cmdFuncs[cmd] = cb_func; }

    static void unregister_cmd(const std::string& cmd) { m_cmdFuncs.erase(cmd); }

    void freezeRvt(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
        {
            return;
        }

        it->second->freezeRvt();
    }

    virtual void unfreezeRvt(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
        {
            return;
        }

        it->second->unfreezeRvt();
    }

    void init() override {}

    void pushRvt(long long pid, Revertable* pRvt) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
        {
            return;
        }

        it->second->pushRvt(pRvt);
    }

    bool isRvtFreezed(long long pid) const override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
        {
            return false;
        }

        return it->second->isRvtFreezed();
    }

    long long createRvtPool() override
    {
        long long pid = m_pidCounter++;
        m_rvtPools[pid] = new RvtPool();
        return pid;
    }

    void deleteRvtPool(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
        {
            return;
        }

        delete it->second;
        m_rvtPools.erase(it);
    }

    void onAppClose() override
    {
        // 结束掉所有未执行的command
        this->update();
    }

    void pushCmd(Command* pCmd) override { m_cmdExe.push_back(pCmd); }

    void pushCmd(const std::string& cmd, const Engine::DObject* paras) override
    {
        Command* pCmd = Command::alloc(cmd);
        pCmd->copyFrom(paras);
        m_cmdExe.push_back(pCmd);
    }

    virtual void execute(Command* pCmd) override
    {
        auto cmd_pos = m_cmdFuncs.find(pCmd->cmd());
        if(cmd_pos == m_cmdFuncs.end())
        {
            return;
        }

        Engine::DObject* pResult = m_cmdFuncs[pCmd->cmd()](pCmd);

        if(pCmd->cbFunc)
            pCmd->cbFunc(pCmd, pResult);

        if(pResult)
            Engine::DObject::free(pResult);

        Command::free(pCmd);
    }

    void update() override
    {
        while(!m_cmdExe.empty())
        {
            sketch::Command* pCmd = m_cmdExe.front();
            m_cmdExe.pop_front();

            Engine::DObject* pResult = m_cmdFuncs[pCmd->cmd()](pCmd);
            if(pCmd->cbFunc)
                pCmd->cbFunc(pCmd, pResult);

            if(pResult)
                Engine::DObject::free(pResult);

            sketch::Command::free(pCmd);
        }
    }

    virtual void beginRvtQueue(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->beginRvtQueue();
    }

    virtual void endRvtQueue(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->endRvtQueue();
    }

    virtual void continueRvtQueue(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->continueRvtQueue();
    }

    virtual void undo(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->undo();
    }

    virtual void redo(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->redo();
    }

    virtual void pop_undo(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->pop_undo();
    }

    virtual void pop_redo(long long pid) override
    {
        auto it = m_rvtPools.find(pid);
        if(it == m_rvtPools.end())
            return;

        it->second->pop_redo();
    }

protected:
    std::unordered_map<long long, RvtPool*> m_rvtPools;

    std::list<Command*> m_cmdExe;// waiting to execute

    long long m_pidCounter;

    static bool m_cmdFuncsInited;
    static std::unordered_map<std::string, std::function<Engine::DObject*(const Engine::DObject*)>> m_cmdFuncs;
};

bool CmdManagerImpl::m_cmdFuncsInited = false;
std::unordered_map<std::string, std::function<Engine::DObject*(const Engine::DObject*)>> CmdManagerImpl::m_cmdFuncs;

CmdManager* CmdManager::create() { return new CmdManagerImpl(); }

void CmdManager::regCmd(const std::string& cmd, std::function<Engine::DObject*(const Engine::DObject*)> cbFunc)
{
    CmdManagerImpl::register_cmd(cmd, cbFunc);
}

void CmdManager::unRegCmd(const std::string& cmd) { CmdManagerImpl::unregister_cmd(cmd); }

}// namespace sketch