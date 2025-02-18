#include "sketch/View/ViewLayout/ViewLayout.h"

#include "sketch/Common/EventPool.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/ViewLayout/LayoutCell.h"
#include "sketch/View/ViewManager.h"
#include <sketch/Serializer/SketchSerializer.h>

namespace sketch
{
ViewLayout::ViewLayout(Define::ViewLayoutType type) : m_type(type), m_propsDirty(true), m_lock(false) {}

bool ViewLayout::init()
{
    this->release();
    return true;
}

void ViewLayout::release()
{
    m_cells.clear();
    m_focusedCells.clear();

    m_lock = false;

    EventPool::instance()->push_event("layout_on_clear", {});
    // STCore::inst->pushUICommand("layout_on_clear", {});
    this->_onFocusedCellChanged();
}

void ViewLayout::onRender(long long lElapsedTm) {}

bool ViewLayout::onDocPrepare(const Engine::Path& projPath) { return true; }
bool ViewLayout::onDocNew(const Engine::Path& projPath) { return true; }
bool ViewLayout::onDocLoad(const Engine::Path& projPath, Engine::DObject* pRootObj)
{
    this->modify(*pRootObj);
    this->setFocusedCell(this->topCell());
    return true;
}
bool ViewLayout::onDocLoaded(const Engine::Path& projPath) { return true; }
bool ViewLayout::onDocWillClose() { return true; }
bool ViewLayout::onDocClose()
{
    m_cells.clear();
    m_focusedCells.clear();
    m_lock = false;

    EventPool::instance()->push_event("layout_on_clear", {});
    // STCore::inst->pushUICommand("layout_on_clear", {});
    this->_onFocusedCellChanged();

    return true;
}

void ViewLayout::save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange) {}

bool ViewLayout::onAppClose() { return this->onDocClose(); }

const Engine::DObject& ViewLayout::props()
{
    if(m_propsDirty)
    {
        this->_updateProps();
        m_propsDirty = false;
    }

    return m_props;
}

void ViewLayout::modify(const Engine::DObject& props) {}

const Engine::DObject* ViewLayout::cellProps(long long id) { return nullptr; }

void ViewLayout::setCellProps(long long id, const Engine::DObject* pProps) {}

void ViewLayout::addCell(LayoutCell* pCell) {}
void ViewLayout::addCells(const std::unordered_set<LayoutCell*>& cells) {}
bool ViewLayout::removeCell(LayoutCell* pCell) { return false; }
void ViewLayout::removeCells(const std::unordered_set<LayoutCell*>& cells) {}
bool ViewLayout::haveCell(LayoutCell* pCell) { return pCell != nullptr && this->haveCell(pCell->cellId()); }
bool ViewLayout::haveCell(long long cellId)
{
    auto it = m_cells.find(cellId);
    return it != m_cells.end();
}

void ViewLayout::setCellFullScr(long long id, bool b) {}
void ViewLayout::setCellTopMost(long long id, bool b) {}

void ViewLayout::onResize(int width, int height) {}
void ViewLayout::setFocusedCell(LayoutCell* pCell) {}
void ViewLayout::addFocusedCell(LayoutCell* pCell) {}
void ViewLayout::switchFocusedCell(LayoutCell* pCell) {}

View* ViewLayout::focusedView() const { return m_focusedCells.empty() ? nullptr : STCore::inst->viewMgr()->getView(m_focusedCells.front()); }

LayoutCell* ViewLayout::topCell() const { return nullptr; }
void ViewLayout::_updateProps() { m_props["type"] = m_type; }
void ViewLayout::_onFocusedCellChanged()
{
    auto pViewMgr = STCore::inst->viewMgr();
    pViewMgr->onFocusedCellChanged();
    if(UIFuncs::updateFocusedCells)
    {
        UIFuncs::updateFocusedCells(m_focusedCells);
    }
}

void ViewLayout::onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) {}

void ViewLayout::onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift) {}

bool ViewLayout::isBusy() { return false; }

void ViewLayout::lock(bool b) { m_lock = b; }

bool ViewLayout::isLock() { return m_lock; }

}// namespace sketch
