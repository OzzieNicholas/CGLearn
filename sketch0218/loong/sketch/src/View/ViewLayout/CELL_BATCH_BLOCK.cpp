#include "sketch/View/ViewLayout/CELL_BATCH_BLOCK.h"

#include <list>
#include <set>
#include <stack>
#include <stdio.h>
#include <string.h>

#include "Engine/Render/Renderer.h"
#include "Engine/Render/Environment.h"
#include "Engine/Render/OGLRes/VertexBuffer.h"
#include "Engine/Math/Math.h"
#include "Engine/Util/Log.h"
#include "Engine/Render/Device.h"

#include "sketch/STCore.h"



//using namespace std;
using namespace Engine;

namespace sketch
{
	OBJ_MEM_POOL_IMPL(CELL_BATCH_BLOCK, 500);

	CELL_BATCH_BLOCK::~CELL_BATCH_BLOCK() {
		if (pData)
			delete[] pData;

		if (pVBuf)
			STE::device()->deleteVertexBuffer(pVBuf);
	}

	void CELL_BATCH_BLOCK::assure(int need) {
		if (need < 0 || poolSize >= size + need)
			return;

		//resize pool
		size_t newPoolSize = STMAX(poolSize * 2, size + need);
		float* pTmpBuf = new float[newPoolSize];
		if (pData) {
			memcpy(pTmpBuf, pData, sizeof(float) * size);
			delete[] pData;
		}

		pData = pTmpBuf;
		poolSize = newPoolSize;
	}

	void CELL_BATCH_BLOCK::prepareDraw()
	{
		if (size == 0)
			return;

		if (!pVBuf) {
			pVBuf = STE::device()->createVertexBuffer((int)poolSize, GL_DYNAMIC_DRAW, pData);
		}
		else if (pVBuf->length() < (int)size) {
			delete pVBuf;
			pVBuf = STE::device()->createVertexBuffer((int)poolSize, GL_DYNAMIC_DRAW, pData);
		}
		else 
			pVBuf->load(0, (int)size, pData);
	}
}