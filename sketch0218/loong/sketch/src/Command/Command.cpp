#include "sketch/Command/Command.h"
#include <vector>

#include "Engine/Util/ObjectMemPool.h"

namespace sketch
{
class CommandImpl : public Command
{
public:
    CommandImpl(const std::string& cmd) : m_cmd(cmd) {}

    virtual ~CommandImpl() {}

    OBJ_MEM_POOL_DECL;

    const std::string& cmd() const override { return m_cmd; }

protected:
    std::string m_cmd;
};

OBJ_MEM_POOL_IMPL(CommandImpl, 1000);

Command::Command() : cbFunc(nullptr) {}

Command::~Command() {}

Command* Command::alloc(const std::string& cmd) { return new CommandImpl(cmd); }

Command* Command::alloc(const std::string& cmd, const Engine::DObject& paras)
{
    CommandImpl* pCmd = new CommandImpl(cmd);
    pCmd->copyFrom(paras);
    return pCmd;
}

void Command::free(Command* pCmd) { delete pCmd; }

}// namespace sketch