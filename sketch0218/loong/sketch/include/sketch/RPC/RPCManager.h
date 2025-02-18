#pragma once
#include <string>

namespace Engine
{
class Path;
}

namespace sketch
{
class RPCProcessor;

class RPCManager
{
public:
    static RPCManager* create();
    virtual ~RPCManager();

    virtual bool init() = 0;

    virtual bool onDocPrepare(const Engine::Path& projPath) = 0;
    virtual bool onDocNew(const Engine::Path& projPath) = 0;
    virtual bool onDocLoad(const Engine::Path& projPath) = 0;
    virtual bool onDocLoaded(const Engine::Path& projPath) = 0;
    virtual bool onDocClose() = 0;
    virtual bool onAppClose() = 0;
    virtual void update(unsigned long long tm) = 0;

    virtual void addProcessor(const std::string& cmd, RPCProcessor* pProc) = 0;
    virtual void removeProcessor(const std::string& cmd, RPCProcessor* pProc) = 0;
};
}// namespace sketch