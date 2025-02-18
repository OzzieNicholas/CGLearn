#pragma once

#include "../Core.h"

#include <string>
#include <functional>

#include "Engine/Util/DObject.h"

namespace sketch
{
class CORE_DLL_DECLSPEC Command : public Engine::DObject
{
public:
    virtual ~Command();

    static Command* alloc(const std::string& cmd);
    static Command* alloc(const std::string& cmd, const Engine::DObject& paras);
    static void free(Command* pCmd);

    virtual const std::string& cmd() const = 0;

protected:
    Command();

public:
    std::function<void(Command*, Engine::DObject*)> cbFunc;

};
}