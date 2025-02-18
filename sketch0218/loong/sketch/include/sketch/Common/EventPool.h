#pragma once

#include "../Object/SketchParamMap.h"
#include <string>

namespace sketch
{
struct Event
{
    std::string name;
    SketchParamMap params;
};

// singleton
class EventPool
{
public:
    using Callback = std::function<void(const SketchParamMap&)>;

    static EventPool* instance();

    void subscribe(const std::string& event, Callback func);
    void unsubscribe(const std::string& event);

    void push_event(const std::string& event, const sketch::SketchParamMap& param);
    void trigger_all();

private:
    EventPool() = default;

    std::deque<Event> m_events;
    std::unordered_map<std::string, Callback> m_listeners;
};
}// namespace sketch
