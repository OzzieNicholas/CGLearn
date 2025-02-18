#pragma once

#include "../..\..\Core.h"
#include "../Revertable.h"
#include <unordered_set>

namespace Engine
{
	class DObject;
}

namespace sketch
{
	class RenderObject;

	class Revertable_View_Hide : public Revertable
	{
	public:
		Revertable_View_Hide(long long viewId);
		virtual ~Revertable_View_Hide(void);

		virtual bool undo();
		virtual bool redo();

		void setHideBefore(const std::unordered_set<RenderObject*>& hideObjsBefore);
		void setHideAfter(const std::unordered_set<RenderObject*>& hideObjsAfter);

	protected:
		long long m_viewId;
		std::unordered_set<long long> m_hideIdsBefore;
		std::unordered_set<long long> m_hideIdsAfter;
	};

}


