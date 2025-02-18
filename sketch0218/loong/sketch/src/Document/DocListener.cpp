#include "sketch/Document/DocListener.h"
#include "sketch/Document/DocElement.h"

#include "Engine/Util/DObject.h"
#include "Engine/Util/Assert.h"

namespace sketch
{
	DocListener::DocListener(void)
	{

	}

	DocListener::~DocListener(void)
	{
		std::unordered_set<DocElement*> targets = m_listenTargets;
		for (auto it = targets.begin(); it != targets.end(); ++it) {
			(*it)->removeListener(this);
		}
		STASSERT(m_listenTargets.empty());
	}

	void DocListener::onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pSrc)
	{

	}

}