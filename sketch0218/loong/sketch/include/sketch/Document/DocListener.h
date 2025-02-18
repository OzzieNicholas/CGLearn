#pragma once

#include <string>
#include <map>
#include "Engine/Util/DObject.h"
#include "sketch/Define.h"

namespace sketch
{
	class DocElement;

	class DocListener
	{
		friend class DocElement;
		friend class DocElement;

	public:
		DocListener(void);
		virtual ~DocListener(void);

		virtual void onDocMessage(DocElement* pTarget, Define::Message method, Engine::DObject* params, DocElement* pSrc);

	protected:
		std::unordered_set<DocElement*> m_listenTargets;
	};

}


