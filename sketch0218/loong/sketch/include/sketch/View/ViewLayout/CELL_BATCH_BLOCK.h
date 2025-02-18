#pragma once

#include "../../Core.h"

#include <stdio.h>
#include <string>

#include "Engine/Math/Math.h"
#include "Engine/Util/ObjectMemPool.h"

namespace Engine {
	class VertexBuffer;
}

namespace sketch
{
	//batch render datas
	class CELL_BATCH_BLOCK {
	public:
		CELL_BATCH_BLOCK():
			frame(-1LL),
			pData(new float[256]),
			pVBuf(nullptr),
			size(0),
			poolSize(256)
		{
		
		}

		~CELL_BATCH_BLOCK();

		OBJ_MEM_POOL_DECL;

		inline void reset() {
			size = 0;
		}

		void assure(int need);
		void prepareDraw();

	public:
		long long frame;
		float* pData;
		Engine::VertexBuffer* pVBuf;
		size_t size;
		size_t poolSize;
	};
};



