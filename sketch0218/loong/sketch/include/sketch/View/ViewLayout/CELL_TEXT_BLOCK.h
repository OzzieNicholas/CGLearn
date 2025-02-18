#pragma once

#include "..\..\Core.h"

#include <vector>

#include "Engine/Render/DrawBatch/DrawBatchFlatText.h"
#include "Engine/Math/Vec4f.h"
#include "Engine/Util/ObjectMemPool.h"

namespace sketch
{
	class CELL_TEXT_BLOCK {
	public:
		CELL_TEXT_BLOCK() {
			frame = -1LL;
			pBatch = Engine::DrawBatchFlatText::alloc();
		}

		~CELL_TEXT_BLOCK() {
			if (pBatch) {
				Engine::DrawBatchFlatText::free(pBatch);
				pBatch = nullptr;
			}
		}

		OBJ_MEM_POOL_DECL;

		inline void reset() {
			data.clear();
		}

	public:
		long long frame;
		std::vector<Engine::Vec4f> data;
		Engine::DrawBatchFlatText* pBatch;
	};
};



