#include "sketch/View/ViewLayout/TileViewLayout.h"

#include <stack>

#include "Engine/Math/Math.h"
#include "Engine/Math/Vec4i.h"
#include "Engine/Render/Device.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Util/Log.h"
#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Command.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderScene/RenderObject.h"
#include "sketch/View/RenderScene/RenderScene.h"
#include "sketch/View/ViewManager.h"

using namespace Engine;

namespace sketch
{
TileViewLayout* TileViewLayout::m_pInstance = nullptr;

TileViewLayout::TileViewLayout(void) : ViewLayout(Define::VIEWLAYOUT_TILED_VIEW), m_sysMouseSession(false) {}

TileViewLayout::~TileViewLayout(void) { this->release(); }

TileViewLayout* TileViewLayout::instance()
{
    if(m_pInstance == nullptr)
    {
        m_pInstance = new TileViewLayout();
    }

    return m_pInstance;
}

bool TileViewLayout::init()
{
    this->release();

    return true;
}

void TileViewLayout::release(void)
{
    m_sortedCells.clear();
    m_manuState.clear();
    m_usedRenderIds.clear();
    m_unusedRenderIds = {};

    __super::release();
}

bool TileViewLayout::onDocClose(void)
{
    m_sortedCells.clear();
    m_manuState.clear();
    m_usedRenderIds.clear();
    m_unusedRenderIds = {};
    return __super::onDocClose();
}

void TileViewLayout::save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    // 只保存View,忽略其它打开的cell
    std::vector<long long> viewIds;
    for(auto id: m_sortedCells)
    {
        if(pViewMgr->getView(id))
        {
            viewIds.push_back(id);
        }

        if(!progName.empty())
        {
            STCore::inst->moveUIProgress(progName, progRange * 0.8f / m_sortedCells.size());
            STCore::inst->updateUIProgress(progName);
        }
    }

    DObject* pLayoutProps = DObject::alloc();
    pLayoutProps->set("sorted_cells", viewIds);
    pSavedObj->set("layout_tiled", pLayoutProps, true);

    if(!progName.empty())
    {
        STCore::inst->moveUIProgress(progName, progRange * 0.2f);
        STCore::inst->updateUIProgress(progName);
    }
}

void TileViewLayout::load_spec(SketchSerializer& serializer)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    m_cells.clear();
    m_sortedCells.clear();
    m_focusedCells.clear();
    m_usedRenderIds.clear();
    m_unusedRenderIds = {};

    size_t sorted_cell_count = 0;
    serializer.begin_read_array("sorted_cells", sorted_cell_count);
    std::vector<Define::LayoutCellType> cellTypes;
    std::vector<std::wstring> cellNames;
    for(int i = 0; i < sorted_cell_count; ++i)
    {
        long long id = 0;
        serializer.read(id);
        LayoutCell* pCell = pViewMgr->getView(id);

        if(!pCell)
            continue;

        m_cells[id] = pCell;
        m_sortedCells.push_back(id);

        pCell->onCellAdded(this->_newRenderId());

        cellTypes.push_back(pCell->cellType());
        cellNames.push_back(pCell->cellName());
    }

    if(UIFuncs::updateLayoutCells)
    {
        UIFuncs::updateLayoutCells(cellTypes, m_sortedCells, cellNames);
    }
    serializer.end_read_array();

    m_propsDirty = true;

    this->_resortCells();
    this->_onFocusedCellChanged();
    this->setFocusedCell(this->topCell());
}

void TileViewLayout::save_spec(SketchSerializer& serializer, const std::wstring& progName, float progRange)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    serializer.begin_write_array("sorted_cells");
    for(auto id: m_sortedCells)
    {
        if(pViewMgr->getView(id))
        {
            serializer.write(id);
        }

        if(!progName.empty())
        {
            STCore::inst->moveUIProgress(progName, progRange * 0.8f / m_sortedCells.size());
            STCore::inst->updateUIProgress(progName);
        }
    }
    serializer.end_write_array();

    if(!progName.empty())
    {
        STCore::inst->moveUIProgress(progName, progRange * 0.2f);
        STCore::inst->updateUIProgress(progName);
    }
}

void TileViewLayout::onRender(long long lElapsedTm)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
    {
        long long id = *it;
        LayoutCell* pCell = m_cells[id];
        if(!pCell || !this->_isCellVisible(it))
            continue;

        pCell->onRenderCell(lElapsedTm);
    }
}

void TileViewLayout::onResize(int width, int height)
{
    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
    {
        LayoutCell* pCell = this->getCell(*it);
        if(pCell)
            pCell->onLayoutResize(width, height);
    }
}

Engine::Vec4i TileViewLayout::initCellRect()
{
    auto& size = STCore::inst->viewMgr()->canvasLogicSize();
    int w = (int)(size.x * 0.5f);
    int h = (int)(size.y * 0.5f);
    int x = Engine::rand(50, size.x - w - 50);
    int y = Engine::rand(50, size.y - h - 50);
    return { x, y, w, h };
}

int TileViewLayout::_newRenderId()
{
    if(!m_unusedRenderIds.empty())
    {
        int rdId = m_unusedRenderIds.front();
        m_unusedRenderIds.pop();
        m_usedRenderIds.insert(rdId);
        return rdId;
    }
    int rdId = (int)m_usedRenderIds.size();
    m_usedRenderIds.insert(rdId);
    return rdId;
}

void TileViewLayout::_freeRenderId(int rdId)
{
    if(rdId < 0)
        return;

    m_unusedRenderIds.push(rdId);
    m_usedRenderIds.erase(rdId);
}

void TileViewLayout::addCell(LayoutCell* pCell)
{
    if(pCell == nullptr)
        return;

    long long id = pCell->cellId();
    if(m_cells.find(id) != m_cells.end())
        return;

    if(pCell->layoutWidth() == 0 && pCell->layoutHeight() == 0)
    {// uninitialized
        pCell->setLayoutRect(initCellRect());
    }

    m_cells[pCell->cellId()] = pCell;
    m_sortedCells.push_back(id);
    m_propsDirty = true;

    this->_resortCells();

    pCell->onCellAdded(this->_newRenderId());

    if(UIFuncs::addLayoutCell)
    {
        UIFuncs::addLayoutCell(pCell->cellType(), pCell->cellId(), pCell->cellName());
    }
}

void TileViewLayout::addCells(const std::unordered_set<LayoutCell*>& cells)
{
    std::vector<Define::LayoutCellType> cellTypes;
    std::vector<long long> cellIds;
    std::vector<std::wstring> cellNames;

    for(auto pCell: cells)
    {
        if(!pCell)
            continue;

        long long id = pCell->cellId();
        if(m_cells.find(id) != m_cells.end())
            continue;

        if(pCell->layoutWidth() == 0 && pCell->layoutHeight() == 0)
        {// uninitialized
            pCell->setLayoutRect(initCellRect());
        }

        m_cells[pCell->cellId()] = pCell;
        m_sortedCells.push_back(id);
        pCell->onCellAdded(this->_newRenderId());

        cellTypes.push_back(pCell->cellType());
        cellIds.push_back(id);
        cellNames.push_back(pCell->cellName());
    }

    if(cellIds.empty())
        return;

    m_propsDirty = true;

    this->_resortCells();

    if(UIFuncs::addLayoutCells)
    {
        UIFuncs::addLayoutCells(cellTypes, cellIds, cellNames);
    }
}

bool TileViewLayout::removeCell(LayoutCell* pCell)
{
    if(pCell == nullptr)
        return false;

    long long id = pCell->cellId();
    auto it = std::find(m_sortedCells.begin(), m_sortedCells.end(), id);
    if(it == m_sortedCells.end())
        return false;
    if(pCell->onCellRemoving())
        return false;

    m_sortedCells.erase(it);
    m_cells.erase(id);

    auto itf = std::find(m_focusedCells.begin(), m_focusedCells.end(), id);
    if(itf != m_focusedCells.end())
    {
        m_focusedCells.erase(itf);
        this->_onFocusedCellChanged();
    }

    m_manuState.erase(id);

    if(UIFuncs::removeLayoutCell)
    {
        UIFuncs::removeLayoutCell(pCell->cellId());
    }

    Define::LayoutCellType curCellType = pCell->cellType();
    if(curCellType == Define::CELL_VIEW && ((View*)pCell)->doc()->isTemp())
    {
        STCore::inst->docMgr()->document()->deleteView(((View*)pCell)->id());
        // STCore::inst->updateUIActiveViewList();
    }
    else
    {
        pCell->onCellRemoved();
        this->_freeRenderId(pCell->renderId());
    }

    return true;
}

void TileViewLayout::removeCells(const std::unordered_set<LayoutCell*>& cells)
{
    std::vector<long long> cellIds;
    bool bFocusedCellChanged = false;
    for(auto pCell: cells)
    {
        if(pCell == nullptr)
            continue;

        long long id = pCell->cellId();
        auto it = std::find(m_sortedCells.begin(), m_sortedCells.end(), id);
        if(it == m_sortedCells.end())
            continue;

        if(pCell->onCellRemoving())
            continue;

        m_sortedCells.erase(it);
        m_cells.erase(id);
        cellIds.push_back(id);

        auto itf = std::find(m_focusedCells.begin(), m_focusedCells.end(), id);
        if(itf != m_focusedCells.end())
        {
            m_focusedCells.erase(itf);
            bFocusedCellChanged = true;
        }

        m_manuState.erase(id);

        if(pCell->cellType() == Define::CELL_VIEW)
        {
            if(((View*)pCell)->doc()->isTemp())
                STCore::inst->docMgr()->document()->deleteView(((View*)pCell)->id());
        }
        else
        {
            pCell->onCellRemoved();
            this->_freeRenderId(pCell->renderId());
        }
    }

    if(UIFuncs::removeLayoutCells)
    {
        UIFuncs::removeLayoutCells(cellIds);
    }
}

bool TileViewLayout::haveDrawingCell()
{
    for(auto itt: m_cells)
    {
        if(itt.second->cellType() == Define::CELL_DRAWING)
            return true;
    }

    return false;
}

void TileViewLayout::setFocusedCell(LayoutCell* pCell)
{
    if(pCell == nullptr)
        return;

    long long id = pCell->cellId();
    if(m_cells.find(id) == m_cells.end())
        return;

    m_focusedCells.clear();
    m_focusedCells.push_back(id);

    // push to front
    m_sortedCells.erase(std::find(m_sortedCells.begin(), m_sortedCells.end(), id));
    m_sortedCells.push_back(id);
    this->_resortCells();

    m_propsDirty = true;

    this->_onFocusedCellChanged();
}

void TileViewLayout::addFocusedCell(LayoutCell* pCell)
{
    if(pCell == nullptr)
        return;

    long long id = pCell->cellId();
    if(m_cells.find(id) == m_cells.end())
        return;

    auto itf = std::find(m_focusedCells.begin(), m_focusedCells.end(), id);
    if(itf != m_focusedCells.end())
        return;

    m_focusedCells.push_back(id);

    if(m_focusedCells.size() == 1)
    {// push to front
        m_sortedCells.erase(std::find(m_sortedCells.begin(), m_sortedCells.end(), id));
        m_sortedCells.push_back(id);
        this->_resortCells();
    }

    m_propsDirty = true;

    this->_onFocusedCellChanged();
}

void TileViewLayout::switchFocusedCell(LayoutCell* pCell)
{
    if(pCell == nullptr)
        return;

    long long id = pCell->cellId();
    if(m_cells.find(id) == m_cells.end())
        return;

    auto itf = std::find(m_focusedCells.begin(), m_focusedCells.end(), id);
    if(itf != m_focusedCells.end())
    {
        m_focusedCells.erase(itf);
    }
    else
    {
        m_focusedCells.push_back(id);

        if(m_focusedCells.size() == 1)
        {// push to front
            m_sortedCells.erase(std::find(m_sortedCells.begin(), m_sortedCells.end(), id));
            m_sortedCells.push_back(id);
            this->_resortCells();
        }
    }

    m_propsDirty = true;

    this->_onFocusedCellChanged();
}

void TileViewLayout::_updateProps()
{
    __super::_updateProps();
    m_props["sorted_cells"] = m_sortedCells;
}

void TileViewLayout::modify(const Engine::DObject& props)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    m_cells.clear();
    m_sortedCells.clear();
    m_focusedCells.clear();
    m_usedRenderIds.clear();
    m_unusedRenderIds = {};

    for(auto& it: props)
    {
        if(it.first == "sorted_cells")
        {
            std::vector<Define::LayoutCellType> cellTypes;
            std::vector<std::wstring> cellNames;

            for(auto id: it.second._longVec())
            {
                LayoutCell* pCell = pViewMgr->getView(id);

                if(!pCell)
                    continue;

                m_cells[id] = pCell;
                m_sortedCells.push_back(id);

                pCell->onCellAdded(this->_newRenderId());

                cellTypes.push_back(pCell->cellType());
                cellNames.push_back(pCell->cellName());
            }

            if(UIFuncs::updateLayoutCells)
            {
                UIFuncs::updateLayoutCells(cellTypes, m_sortedCells, cellNames);
            }
        }
    }

    m_propsDirty = true;

    this->_resortCells();
    this->_onFocusedCellChanged();
    this->setFocusedCell(this->topCell());
}

// input
void TileViewLayout::onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift) {}

void TileViewLayout::dirtyCells() const
{
    for(auto it: m_cells)
    {
        it.second->dirtyRT();
    }
}

bool TileViewLayout::hasDirtyCells() const
{
    return std::any_of(m_cells.begin(), m_cells.end(), [](auto it) { return it.second->isDirtyRT(); });
}

void TileViewLayout::_snapCell(LayoutCell* pCell, int state, int left, int top, int width, int height)
{
    auto& size = STCore::inst->viewMgr()->canvasLogicSize();

    int right = left + width;
    int bottom = top + height;

    int canvasLeft = 0;
    int canvasRight = size.x;
    int canvasTop = 0;
    int canvasBottom = size.y;

    if(state == 1)
    {// moving
        int snapRangeH = SNAP_RANGE;
        int snapRangeV = SNAP_RANGE;

        if(left < canvasLeft)
        {
            left = canvasLeft;
            right = left + width;
        }
        else if(right > canvasRight)
        {
            right = STMAX(canvasRight, canvasLeft + width);
            left = right - width;
        }

        if(top < canvasTop)
        {
            top = canvasTop;
            bottom = top + height;
        }
        else if(bottom > canvasBottom)
        {
            bottom = STMAX(canvasBottom, canvasTop + height);
            top = bottom - height;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);

            // NOTE: 这里_isCellVisible需要排除pCell自身
            // 否则会因为上一帧pCell被Snap而遮挡住当前pOtherCell导致这一帧pOtherCell无法继续snap pCell
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell))
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(left > canvasLeft && abs(left - left1) < snapRangeH)
                {
                    left = left1;
                    right = left + width;
                    snapRangeH = abs(left - left1);
                }
                else if(left > canvasLeft && abs(left - right1) < snapRangeH)
                {
                    left = right1;
                    right = left + width;
                    snapRangeH = abs(left - right1);
                }
                else if(right < canvasRight && abs(right - left1) < snapRangeH)
                {
                    right = left1;
                    left = right - width;
                    snapRangeH = abs(right - left1);
                }
                else if(right < canvasRight && abs(right - right1) < snapRangeH)
                {
                    right = right1;
                    left = right - width;
                    snapRangeH = abs(right - right1);
                }
            }

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(top > canvasTop && abs(top - top1) < snapRangeV)
                {
                    top = top1;
                    bottom = top + height;
                    snapRangeV = abs(top - top1);
                }
                else if(top > canvasTop && abs(top - bottom1) < snapRangeV)
                {
                    top = bottom1;
                    bottom = top + height;
                    snapRangeV = abs(top - bottom1);
                }
                else if(bottom < canvasBottom && abs(bottom - top1) < snapRangeV)
                {
                    bottom = top1;
                    top = bottom - height;
                    snapRangeV = abs(bottom - top1);
                }
                else if(bottom < canvasBottom && abs(bottom - bottom1) < snapRangeV)
                {
                    bottom = bottom1;
                    top = bottom - height;
                    snapRangeV = abs(bottom - bottom1);
                }
            }
        }

        right = left + width;
        bottom = top + height;

        pCell->setLayoutPos(left, top);
    }
    else if(state == 2)
    {// resize left
        int snapRange = SNAP_RANGE;

        if(left < canvasLeft)
        {
            left = canvasLeft;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(left > canvasLeft)
                {
                    if(abs(left - left1) < snapRange)
                    {
                        left = left1;
                        snapRange = abs(left - left1);
                    }

                    if(abs(left - right1) < snapRange)
                    {
                        left = right1;
                        snapRange = abs(left - right1);
                    }
                }
            }
        }

        left = STMIN(left, right - MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 3)
    {// resize right
        int snapRange = SNAP_RANGE;

        if(right > canvasRight)
        {
            right = canvasRight;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(right < canvasRight)
                {
                    if(abs(right - left1) < snapRange)
                    {
                        right = left1;
                        snapRange = abs(right - left1);
                    }

                    if(abs(right - right1) < snapRange)
                    {
                        right = right1;
                        snapRange = abs(right - right1);
                    }
                }
            }
        }

        right = STMAX(right, left + MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 4)
    {// resize bottom
        int snapRange = SNAP_RANGE;

        if(bottom > canvasBottom)
        {
            bottom = canvasBottom;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(bottom < canvasBottom)
                {
                    if(abs(bottom - top1) < snapRange)
                    {
                        bottom = top1;
                        snapRange = abs(bottom - top1);
                    }

                    if(abs(bottom - bottom1) < snapRange)
                    {
                        bottom = bottom1;
                        snapRange = abs(bottom - bottom1);
                    }
                }
            }
        }

        bottom = STMAX(bottom, top + MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 5)
    {// resize top
        int snapRange = SNAP_RANGE;

        if(top < canvasTop)
        {
            top = canvasTop;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(top > canvasTop)
                {
                    if(abs(top - top1) < snapRange)
                    {
                        top = top1;
                        snapRange = abs(top - top1);
                    }

                    if(abs(top - bottom1) < snapRange)
                    {
                        top = bottom1;
                        snapRange = abs(top - bottom1);
                    }
                }
            }
        }

        top = STMIN(top, bottom - MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 6)
    {// resize left-bottom
        int snapRangeH = SNAP_RANGE;
        int snapRangeV = SNAP_RANGE;

        if(left < canvasLeft)
        {
            left = canvasLeft;
        }

        if(bottom > canvasBottom)
        {
            bottom = canvasBottom;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(left > canvasLeft)
                {
                    if(abs(left - left1) < snapRangeH)
                    {
                        left = left1;
                        snapRangeH = abs(left - left1);
                    }

                    if(abs(left - right1) < snapRangeH)
                    {
                        left = right1;
                        snapRangeH = abs(left - right1);
                    }
                }
            }

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(bottom < canvasBottom)
                {
                    if(abs(bottom - top1) < snapRangeV)
                    {
                        bottom = top1;
                        snapRangeV = abs(bottom - top1);
                    }

                    if(abs(bottom - bottom1) < snapRangeV)
                    {
                        bottom = bottom1;
                        snapRangeV = abs(bottom - bottom1);
                    }
                }
            }
        }

        left = STMIN(left, right - MIN_VIEW_SIZE);
        bottom = STMAX(bottom, top + MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 7)
    {// resize left-top
        int snapRangeH = SNAP_RANGE;
        int snapRangeV = SNAP_RANGE;

        if(left < canvasLeft)
        {
            left = canvasLeft;
        }

        if(top < canvasTop)
        {
            top = canvasTop;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(left > canvasLeft)
                {
                    if(abs(left - left1) < snapRangeH)
                    {
                        left = left1;
                        snapRangeH = abs(left - left1);
                    }

                    if(abs(left - right1) < snapRangeH)
                    {
                        left = right1;
                        snapRangeH = abs(left - right1);
                    }
                }
            }

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(top > canvasTop)
                {
                    if(abs(top - top1) < snapRangeV)
                    {
                        top = top1;
                        snapRangeV = abs(top - top1);
                    }

                    if(abs(top - bottom1) < snapRangeV)
                    {
                        top = bottom1;
                        snapRangeV = abs(top - bottom1);
                    }
                }
            }
        }

        left = STMIN(left, right - MIN_VIEW_SIZE);
        top = STMIN(top, bottom - MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 8)
    {// resize right-bottom
        int snapRangeH = SNAP_RANGE;
        int snapRangeV = SNAP_RANGE;

        if(right > canvasRight)
        {
            right = canvasRight;
        }

        if(bottom > canvasBottom)
        {
            bottom = canvasBottom;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(right < canvasRight)
                {
                    if(abs(right - left1) < snapRangeH)
                    {
                        right = left1;
                        snapRangeH = abs(right - left1);
                    }

                    if(abs(right - right1) < snapRangeH)
                    {
                        right = right1;
                        snapRangeH = abs(right - right1);
                    }
                }
            }

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(bottom < canvasBottom)
                {
                    if(abs(bottom - top1) < snapRangeV)
                    {
                        bottom = top1;
                        snapRangeV = abs(bottom - top1);
                    }

                    if(abs(bottom - bottom1) < snapRangeV)
                    {
                        bottom = bottom1;
                        snapRangeV = abs(bottom - bottom1);
                    }
                }
            }
        }

        right = STMAX(right, left + MIN_VIEW_SIZE);
        bottom = STMAX(bottom, top + MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
    else if(state == 9)
    {// resize right-top
        int snapRangeH = SNAP_RANGE;
        int snapRangeV = SNAP_RANGE;

        if(right > canvasRight)
        {
            right = canvasRight;
        }

        if(top < canvasTop)
        {
            top = canvasTop;
        }

        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pOtherCell = this->getCell(id);
            if(pCell == pOtherCell || pOtherCell->isFullScr() || !this->_isCellVisible(it, pCell) || m_manuState.find(id) != m_manuState.end())
                continue;

            auto& rect1 = pOtherCell->layoutRect();
            int left1 = rect1.x;
            int right1 = rect1.x + rect1.z;
            int top1 = rect1.y;
            int bottom1 = rect1.y + rect1.w;

            if(STMAX(top, top1) <= STMIN(bottom, bottom1))
            {// vertial overlap
                if(right < canvasRight)
                {
                    if(abs(right - left1) < snapRangeH)
                    {
                        right = left1;
                        snapRangeH = abs(right - left1);
                    }

                    if(abs(right - right1) < snapRangeH)
                    {
                        right = right1;
                        snapRangeH = abs(right - right1);
                    }
                }
            }

            if(STMAX(left, left1) <= STMIN(right, right1))
            {// horizontal overlap
                if(top > canvasTop)
                {
                    if(abs(top - top1) < snapRangeV)
                    {
                        top = top1;
                        snapRangeV = abs(top - top1);
                    }

                    if(abs(top - bottom1) < snapRangeV)
                    {
                        top = bottom1;
                        snapRangeV = abs(top - bottom1);
                    }
                }
            }
        }

        right = STMAX(right, left + MIN_VIEW_SIZE);
        top = STMIN(top, bottom - MIN_VIEW_SIZE);

        pCell->setLayoutRect(left, top, right - left, bottom - top);
    }
}

void TileViewLayout::onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift)
{
    m_sysMouseSession = false;

    if(button < 0)
        return;

    if(button == 0)
        this->_evalManuState(x, y);

    if(!this->isBusy())
    {
        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pCell = this->getCell(id);
            if(this->_isWindowPosCovered(it, x, y))
                continue;

            if(pCell->isWindowPosIn(x, y))
            {
                /*if (bCtrl && button == 0) {
                    this->switchFocusedCell(pView);
                }
                else */
                if(bShift && button == 0)
                {
                    this->addFocusedCell(pCell);
                }
                else
                {
                    this->setFocusedCell(pCell);
                }

                this->updateUISelInfo();
                break;
            }
        }
    }
    else if(m_manuState.size() == 1 && m_manuState.begin()->second.first == 1)
    {// move时，切换focused cell
        long long id = m_manuState.begin()->first;
        LayoutCell* pCell = this->getCell(id);
        this->setFocusedCell(pCell);
    }
}

void TileViewLayout::onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();
    auto& size = pViewMgr->canvasLogicSize();

    if(m_manuState.empty() && button < 0)
    {// check mouse state
        Define::MouseCursor cursor = this->_evalMouseCursor(x, y);
        if(UIFuncs::setCursor)
        {
            UIFuncs::setCursor(1, cursor);
        }
    }
    else
    {// manipulating
        for(auto it: m_manuState)
        {
            long long id = it.first;
            int state = it.second.first;
            Vec2i& mousePos0 = it.second.second.xy;
            Vec2i& viewPos0 = it.second.second.zw;

            LayoutCell* pCell = this->getCell(id);
            auto& rect = pCell->layoutRect();
            int newLeft = rect.x;
            int newTop = rect.y;
            int newRight = rect.x + rect.z;
            int newBottom = rect.y + rect.w;

            if(state == 1)
            {// moving
                newLeft = viewPos0.x + x - mousePos0.x;
                newTop = viewPos0.y + mousePos0.y - y;
                newRight = newLeft + rect.z;
                newBottom = newTop + rect.w;
            }
            else if(state == 2)
            {// resize left
                newLeft = x;
            }
            else if(state == 3)
            {// resize right
                newRight = x;
            }
            else if(state == 4)
            {// resize bottom
                newBottom = size.y - y;
            }
            else if(state == 5)
            {// resize top
                newTop = size.y - y;
            }
            else if(state == 6)
            {// resize left-bottom
                newLeft = x;
                newBottom = size.y - y;
            }
            else if(state == 7)
            {// resize left-top
                newLeft = x;
                newTop = size.y - y;
            }
            else if(state == 8)
            {// resize right-bottom
                newRight = x;
                newBottom = size.y - y;
            }
            else if(state == 9)
            {// resize right-top
                newRight = x;
                newTop = size.y - y;
            }

            this->_snapCell(pCell, state, newLeft, newTop, newRight - newLeft, newBottom - newTop);
        }
    }
}

void TileViewLayout::onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift)
{
    if(!m_manuState.empty())
    {
        if(m_manuState.size() == 1 && m_manuState.begin()->second.first == 1)
        {// move时，切换focused cell
            long long id = m_manuState.begin()->first;
            LayoutCell* pCell = this->getCell(id);
            this->setFocusedCell(pCell);
            this->updateUISelInfo();
        }

        m_manuState.clear();
    }

    m_sysMouseSession = false;
}

void TileViewLayout::onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift)
{
    if(button == 0)
    {
        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pCell = this->getCell(id);
            if(this->_isWindowPosCovered(it, x, y))
                continue;

            if(pCell->isMouseOnControlBtn(x, y, 0))
            {
                LayoutCell* pNextCell = nullptr;
                if(this->isFocusedCell(pCell->cellId()))
                {
                    auto itn = std::next(it);
                    if(itn == m_sortedCells.end() && m_sortedCells.begin() != it)
                        itn = m_sortedCells.begin();

                    if(std::next(it) != m_sortedCells.end())
                        pNextCell = this->getCell(*std::next(it));
                    else if(it != m_sortedCells.begin())
                        pNextCell = this->getCell(*std::prev(it));
                }

                this->removeCell(pCell);

                if(pNextCell)
                    this->setFocusedCell(pNextCell);

                m_sysMouseSession = true;
                break;
            }
            else if(pCell->isMouseOnControlBtn(x, y, 1))
            {
                if(pCell->isFullScr())
                    this->setCellFullScr(id, false);
                else
                    this->setCellFullScr(id, true);
                m_sysMouseSession = true;
                break;
            }
        }
    }
}

// return: true, this event is resolved
void TileViewLayout::onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift)
{
    if(button == 0)
    {
        for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
        {
            long long id = *it;
            LayoutCell* pCell = this->getCell(id);
            if(!pCell->isMouseOnTitleBar(x, y) || this->_isWindowPosCovered(it, x, y) || pCell->isFullScr())
                continue;

            if(!pCell->isMouseOnControlBtn(x, y, 0) && !pCell->isMouseOnControlBtn(x, y, 1))
            {// maximize view while double clicking title bar
                this->setCellFullScr(id, true);
                m_sysMouseSession = true;
                break;
            }
        }
    }
}

void TileViewLayout::onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift)
{
    if(this->isBusy() || button >= 0)
        return;

    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
    {
        long long id = *it;
        LayoutCell* pCell = this->getCell(id);
        if(this->_isWindowPosCovered(it, x, y))
            continue;

        if(pCell->isWindowPosIn(x, y))
        {
            this->setFocusedCell(pCell);
            break;
        }
    }
}

bool TileViewLayout::isBusy() { return !m_manuState.empty() || m_sysMouseSession; }

void TileViewLayout::setCellFullScr(long long id, bool b)
{
    auto it = m_cells.find(id);
    if(it == m_cells.end())
        return;

    LayoutCell* pCell = it->second;
    pCell->setFullScr(b);
}

void TileViewLayout::setCellTopMost(long long id, bool b)
{
    auto it = m_cells.find(id);
    if(it == m_cells.end())
        return;

    it->second->setTopMost(b);

    // push to front
    m_sortedCells.erase(std::find(m_sortedCells.begin(), m_sortedCells.end(), id));
    m_sortedCells.push_back(id);
    this->_resortCells();
}

LayoutCell* TileViewLayout::topCell() const
{
    if(m_sortedCells.empty())
        return nullptr;

    return this->getCell(m_sortedCells.back());
}

bool TileViewLayout::_isCellVisible(std::list<long long>::iterator it, LayoutCell* pIgnore)
{
    LayoutCell* pCell = this->getCell(*it);
    if(pCell == nullptr)
        pCell = pCell;

    ++it;
    while(it != m_sortedCells.end())
    {
        long long id = *it;
        LayoutCell* pAfterCell = this->getCell(id);
        if(pAfterCell != pIgnore && (pAfterCell->isFullScr() || pAfterCell->isCover(pCell)))
        {
            return false;
        }

        ++it;
    }

    return true;
}

bool TileViewLayout::_isWindowPosCovered(std::list<long long>::iterator it, int x, int y)
{
    it++;
    while(it != m_sortedCells.end())
    {
        LayoutCell* pAfterCell = this->getCell(*it);
        if(pAfterCell->isWindowPosIn(x, y))
        {
            return true;
        }

        ++it;
    }

    return false;
}

Engine::Vec4i TileViewLayout::canvas2Window(const Engine::Vec4i& rect)
{
    auto& size = STCore::inst->viewMgr()->canvasLogicSize();
    int vpX = rect.x;
    int vpY = size.y - rect.y - rect.w;
    return { vpX, vpY, rect.z, rect.w };
}

void TileViewLayout::_evalManuState(int x, int y)
{
    m_manuState.clear();

    int borderRange = PICK_RANGE;

    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
    {
        long long id = *it;
        LayoutCell* pCell = this->getCell(id);
        if(!pCell)
            continue;

        bool fullscr = pCell->isFullScr();
        if(fullscr)
            continue;

        auto rect = this->canvas2Window(pCell->layoutRect());
        auto& pos = rect.xy;
        auto& size = rect.zw;

        int left = (int)pos.x;
        int bottom = (int)pos.y;
        int right = (int)(pos.x + size.x) - 1;
        int top = (int)(pos.y + size.y) - 1;

        if(fabs(left - x) <= borderRange && fabs(bottom - y) <= borderRange)
        {// resize left-bottom
            if(!this->_isWindowPosCovered(it, left, bottom))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 6)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 6, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(left - x) <= borderRange && fabs(top - y) <= borderRange)
        {// resize left-top
            if(!this->_isWindowPosCovered(it, left, top))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 7)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 7, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(right - x) <= borderRange && fabs(bottom - y) <= borderRange)
        {// resize right-bottom
            if(!this->_isWindowPosCovered(it, right, bottom))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 8)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 8, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(right - x) <= borderRange && fabs(top - y) <= borderRange)
        {// resize right-top
            if(!this->_isWindowPosCovered(it, right, top))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 9)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 9, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(left - x) <= borderRange && y > bottom && y < top)
        {// resize left
            if(!this->_isWindowPosCovered(it, left, y))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 2)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 2, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(right - x) <= borderRange && y > bottom && y < top)
        {// resize right
            if(!this->_isWindowPosCovered(it, right, y))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 3)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 3, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(bottom - y) <= borderRange && x > left && x < right)
        {// resize bottom
            if(!this->_isWindowPosCovered(it, x, bottom))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 4)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 4, Vec4i::ZERO };
                continue;
            }
        }

        if(fabs(top - y) <= borderRange && x > left && x < right)
        {// resize top
            if(!this->_isWindowPosCovered(it, x, top))
            {
                for(auto itt = m_manuState.begin(); itt != m_manuState.end();)
                {
                    if(itt->second.first != 5)
                    {
                        ++itt;
                        continue;
                    }

                    if(this->getCell(itt->first)->isIntersect(pCell))
                        itt = m_manuState.erase(itt);
                    else
                        ++itt;
                }

                m_manuState[id] = { 5, Vec4i::ZERO };
                continue;
            }
        }

        if(pCell->isMouseOnTitleBar(x, y))
        {// move
            if(!this->_isWindowPosCovered(it, x, y))
            {
                auto& rect = pCell->layoutRect();
                m_manuState.clear();
                m_manuState[id] = { 1, Vec4i(x, y, rect.x, rect.y) };//<state, <origin mouse xy, origin view pos>>
                break;
            }
        }
    }
}

void TileViewLayout::_resortCells()
{
    std::list<long long> topIds;
    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end();)
    {
        long long id = *it;
        LayoutCell* pCell = this->getCell(id);
        if(!pCell->isFullScr() && pCell->isTopMost())
        {
            topIds.push_back(id);
            it = m_sortedCells.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for(auto id: topIds)
        m_sortedCells.push_back(id);
}

void TileViewLayout::updateUISelInfo()
{
    int nView = 0;
    for(auto id: m_focusedCells)
    {
        auto pCell = this->getCell(id);
        if(!pCell)
            continue;

        if(pCell->cellType() == Define::CELL_VIEW)
            nView++;
    }

    if(nView < 2)
    {
        if(UIFuncs::setCellSelInfo)
        {
            UIFuncs::setCellSelInfo(L"");
        }
        return;
    }

    std::wstring selInfo = L"视图" + std::to_wstring(nView);
    if(UIFuncs::setCellSelInfo)
    {
        UIFuncs::setCellSelInfo(selInfo);
    }
}

Define::MouseCursor TileViewLayout::_mergeMouseCursor(Define::MouseCursor src, Define::MouseCursor dst)
{
    if(src == Define::CURSOR_NONE)
        return dst;
    else if(dst == Define::CURSOR_HAND)
        return Define::CURSOR_HAND;
    else if(src == Define::CURSOR_MOVE || dst == Define::CURSOR_MOVE)
        return Define::CURSOR_MOVE;
    else if(src == dst)
        return src;
    else
        return Define::CURSOR_SIZE_ALL;
}

Define::MouseCursor TileViewLayout::_evalMouseCursor(int x, int y)
{
    ViewManager* pViewMgr = STCore::inst->viewMgr();

    int range = PICK_RANGE;

    Define::MouseCursor cursor = Define::CURSOR_NONE;

    for(auto it = m_sortedCells.begin(); it != m_sortedCells.end(); ++it)
    {
        long long id = *it;
        LayoutCell* pCell = this->getCell(id);
        if(!pCell)
            continue;

        bool fullscr = pCell->isFullScr();

        auto rect = this->canvas2Window(pCell->layoutRect());
        auto& pos = rect.xy;
        auto& size = rect.zw;

        int left = (int)pos.x;
        int bottom = (int)pos.y;
        int right = (int)(pos.x + size.x) - 1;
        int top = (int)(pos.y + size.y) - 1;

        if(pCell->isMouseOnControlBtn(x, y, 0) || pCell->isMouseOnControlBtn(x, y, 1))
        {// sys buttons
            return Define::CURSOR_ARROW;
        }

        if(!fullscr && fabs(left - x) <= range && fabs(bottom - y) <= range)
        {// left-bottom
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NESW);
                continue;
            }
        }

        if(!fullscr && fabs(left - x) <= range && fabs(top - y) <= range)
        {// left-top
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NWSE);
                continue;
            }
        }

        if(!fullscr && fabs(right - x) <= range && fabs(bottom - y) <= range)
        {// right-bottom
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NWSE);
                continue;
            }
        }

        if(!fullscr && fabs(right - x) <= range && fabs(top - y) <= range)
        {// right-top
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NESW);
                continue;
            }
        }

        if(!fullscr && fabs(left - x) <= range && y > bottom && y < top)
        {// left
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_WE);
                continue;
            }
        }

        if(!fullscr && fabs(right - x) <= range && y > bottom && y < top)
        {// right
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_WE);
                continue;
            }
        }

        if(!fullscr && fabs(bottom - y) <= range && x > left && x < right)
        {// bottom
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NS);
                continue;
            }
        }

        if(!fullscr && fabs(top - y) <= range && x > left && x < right)
        {// top
            if(!this->_isWindowPosCovered(it, x, y))
            {
                cursor = this->_mergeMouseCursor(cursor, Define::CURSOR_SIZE_NS);
                continue;
            }
        }
    }

    return cursor;
}
}// namespace sketch