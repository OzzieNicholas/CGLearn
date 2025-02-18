#include "sketch/STCore.h"

#include <Windows.h>
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Math/Plane.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/View/ViewManager.h"
#include "sketch/View/ViewLayout/TileViewLayout.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/Scene/SceneDoc.h"

using namespace sketch;
using namespace Engine;

extern "C" CORE_DLL_DECLSPEC long long focusedCellId()
{
	auto pLayout = STCore::inst->viewMgr()->layout();
	if (pLayout->focusedCell()) {
		return pLayout->focusedCell()->cellId();
	}
	return -1LL;
}

extern "C" CORE_DLL_DECLSPEC sketch::Define::LayoutCellType focusedCellType()
{
	auto pLayout = STCore::inst->viewMgr()->layout();
	if (!pLayout->focusedCell())
		return Define::CELL_NONE;

	return pLayout->focusedCell()->cellType();
}

extern "C" CORE_DLL_DECLSPEC BOOL isFocusedCellsAllTopMost()
{
	auto pLayout = STCore::inst->viewMgr()->layout();
	if (pLayout->focusedCells().empty())
		return 0;

	for (auto id : pLayout->focusedCells()) {
		auto pCell = pLayout->getCell(id);
		if (!pCell)
			continue;

		if (!pCell->isTopMost())
			return 0;
	}
	return 1;
}

extern "C" CORE_DLL_DECLSPEC void setFocusedCellTopMost(BOOL topMost)
{
	auto pLayout = STCore::inst->viewMgr()->layout();
	for (auto id : pLayout->focusedCells()) {
		pLayout->setCellTopMost(id, topMost);
	}
}

extern "C" CORE_DLL_DECLSPEC void switchCellFullScr(long long id)
{
	auto pLayout = (TileViewLayout*)STCore::inst->viewMgr()->layout();

	auto pCell = pLayout->getCell(id);
	if (!pCell)
		return;

	pLayout->setCellFullScr(id, !pCell->isFullScr());
	pCell->dirtyRT();
}

extern "C" CORE_DLL_DECLSPEC void setFocusedCell(long long id)
{
	ViewManager* pViewMgr = STCore::inst->viewMgr();
	ViewLayout* pLayout = pViewMgr->layout();

	LayoutCell* pCell = pViewMgr->getCell(id);
	if (!pCell)
		return;
	
	//更新redraw frame防止显示关闭时期新创建的物体也被默认显示
	if (pCell->cellType() == Define::CELL_VIEW)
		pCell->_view()->updateRedrawFrame();

	pLayout->addCell(pCell);
	pLayout->setFocusedCell(pCell);
}

extern "C" CORE_DLL_DECLSPEC void resetFocusedCellPos()
{
	Document* pDoc = STCore::inst->docMgr()->document();
	ViewManager* pViewMgr = STCore::inst->viewMgr();
	auto pCell = pViewMgr->layout()->focusedCell();
	if (pCell == nullptr)
		return;

	auto cellSize = pCell->layoutSize();
	if (cellSize.x > pViewMgr->canvasLogicWidth())
		cellSize.x = pViewMgr->canvasLogicWidth() / 2;
	if (cellSize.y > pViewMgr->canvasLogicHeight())
		cellSize.y = pViewMgr->canvasLogicHeight() / 2;
	pCell->setLayoutRect(0, 0, cellSize.x, cellSize.y);
}

extern "C" CORE_DLL_DECLSPEC BOOL removeCellFromLayout(long long id)
{
	ViewManager* pViewMgr = STCore::inst->viewMgr();
		
	LayoutCell* pCell = pViewMgr->layout()->getCell(id);
	if (!pCell)
		return false;

	return pViewMgr->layout()->removeCell(pCell);
}

CORE_DLL_DECLSPEC std::unordered_set<long long> removeCellsFromLayout(long long* ids, int nIds)
{
	Document* pDoc = STCore::inst->docMgr()->document();
	ViewManager* pViewMgr = STCore::inst->viewMgr();

	std::unordered_set<long long> failIds;
	for (int i = 0; i < nIds; ++i) {
		LayoutCell* pCell = pViewMgr->layout()->getCell(ids[i]);
		if (!pCell) {
			failIds.insert(ids[i]);
			continue;
		}
				
		if (!pViewMgr->layout()->removeCell(pCell))
			failIds.insert(ids[i]);
	}

	return failIds;
}


