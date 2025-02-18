#include "sketch/Document/DocElement.h"
#include "sketch/Document/DocListener.h"
//#include "DocMessage.h"

#include "Engine/Util/DObject.h"

namespace sketch
{
DocElement::DocElement() : m_dispatching(-1) {}

DocElement::~DocElement()
{
    for(auto pListener: m_listeners)
    {
        pListener->m_listenTargets.erase(this);
    }
    m_listeners.clear();
}

void DocElement::addListener(DocListener* listener)
{
    if(m_dispatching >= 0)// dispatching
    {
        if(m_delListeners.find(listener) == m_delListeners.end())
        {
            m_delListeners.insert(listener);
        }
    }

    m_listeners.insert(listener);
    listener->m_listenTargets.insert(this);
}

void DocElement::removeListener(DocListener* pListener)
{
    auto it = m_listeners.find(pListener);
    if(it == m_listeners.end())
    {
        return;
    }
    if(m_dispatching < 0)// not dispatching
    {
        m_listeners.erase(it);
        pListener->m_listenTargets.erase(this);
    }
    else// dispatching, delay the removal
    {
        m_delListeners.insert(pListener);
        pListener->m_listenTargets.erase(this);
        return;
    }
}

void DocElement::invokeListener(Define::Message method, Engine::DObject* params, DocElement* pSrc)
{

    m_dispatching += 1;

    for(auto pListener: m_listeners)
    {
        if(m_delListeners.find(pListener) == m_delListeners.end())
        {
            pListener->onDocMessage(this, method, params, pSrc);
        }
    }

    m_dispatching -= 1;

    if(params)
    {
        Engine::DObject::free(params);
    }

    if(m_dispatching <= 0 && !m_delListeners.empty())// defer the removal
    {
        for(auto pListener: m_delListeners)
        {
            m_listeners.erase(pListener);
        }
        m_delListeners.clear();
    }
}

}// namespace sketch