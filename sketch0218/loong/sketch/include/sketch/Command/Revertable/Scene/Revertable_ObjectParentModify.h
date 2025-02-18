#pragma once

#include "..\..\..\Core.h"
#include "../Revertable.h"
#include "Engine/Math/Mat43.h"
#include "Engine/Util/ObjectMemPool.h"
#include <set>

namespace sketch
{
	class Revertable_ObjectParentModify : public Revertable
	{
	public:
		Revertable_ObjectParentModify(long long objId, long long parBefore, long long parAfter);
		virtual ~Revertable_ObjectParentModify(void);

		OBJ_MEM_POOL_DECL;

		virtual bool undo();
		virtual bool redo();

	protected:
		long long m_objId;
		long long m_parBefore;
		long long m_parAfter;
	};

}


