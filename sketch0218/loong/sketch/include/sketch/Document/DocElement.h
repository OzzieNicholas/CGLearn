#pragma once

#include "sketch/Define.h"
#include "Engine/Util/DObject.h"
#include <string>
#include <unordered_set>

namespace sketch
{
class DocListener;

class DocElement
{
public:
    DocElement();
    virtual ~DocElement();

    virtual void addListener(DocListener* listener);
    virtual void removeListener(DocListener* listener);
    virtual void invokeListener(Define::Message method, Engine::DObject* params = nullptr, DocElement* pSrc = nullptr);

protected:
    std::unordered_set<DocListener*> m_listeners;
    std::unordered_set<DocListener*> m_delListeners;// used for delaying the removal of a listener during the dispatch process
    int m_dispatching;                              // is diapatching message
};
}// namespace sketch