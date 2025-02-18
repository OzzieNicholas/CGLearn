#include "sketch/Common/EventPool.h"
#include "sketch/Command/Command.h"
#include "sketch/UIFuncs.h"

namespace sketch
{
EventPool* EventPool::instance()
{
    static EventPool instance;
    return &instance;
}

void EventPool::subscribe(const std::string& event, Callback func) { m_listeners.try_emplace(event, func); }
void EventPool::unsubscribe(const std::string& event) { m_listeners.erase(event); }

void EventPool::push_event(const std::string& event, const SketchParamMap& param) { m_events.push_back(Event{ event, param }); }
void EventPool::trigger_all()
{
    while(!m_events.empty())
    {
        auto& [name, params] = m_events.front();

        if(UIFuncs::executeUICmd)// this is used to compatible with sketch C# ui
        {
            const auto cmd = Command::alloc(name);
            cmd->copyFrom(params.to_dobject());

            if(const auto ret = UIFuncs::executeUICmd(cmd))
            {
                if(auto iter = m_listeners.find(name); iter != m_listeners.end())
                {
                    auto func = iter->second;
                    func(SketchParamMap::alloc(*ret));
                }
                Engine::DObject::free(ret);
            }

            Command::free(cmd);
        }
        else
        {
            if(auto iter = m_listeners.find(name); iter != m_listeners.end())
            {
                auto func = iter->second;
                func(params);
            }
        }

        m_events.pop_front();
    }
}
}// namespace sketch