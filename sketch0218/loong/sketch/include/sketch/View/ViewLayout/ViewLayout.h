#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Engine/Util/DObject.h"
#include "Engine/Util/Path.h"

#include "../../Define.h"

namespace sketch
{
class LayoutCell;
class View;

class ViewLayout
{
public:
    virtual ~ViewLayout() = default;

    virtual bool init();
    virtual void release();

    virtual void onRender(long long lElapsedTm);

    virtual bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
    virtual bool onDocNew(const Engine::Path& projPath); //初始化新场景
    virtual bool onDocLoad(const Engine::Path& projPath, Engine::DObject* pRootObj); //加载场景
    virtual bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
    virtual bool onDocWillClose();
    virtual bool onDocClose();
    virtual bool onAppClose();

    inline Define::ViewLayoutType type() const { return m_type; }

    virtual const Engine::DObject& props();
    virtual void modify(const Engine::DObject& props);
    virtual void save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange);

    virtual const Engine::DObject* cellProps(long long id);
    virtual void setCellProps(long long id, const Engine::DObject* pProps);

    virtual void onResize(int width, int height);

    virtual void addCell(LayoutCell* pCell);
    virtual void addCells(const std::unordered_set<LayoutCell*>& cells);
    virtual bool removeCell(LayoutCell* pCell);
    virtual void removeCells(const std::unordered_set<LayoutCell*>& cells);
    inline const std::unordered_map<long long, LayoutCell*>& cells() const
    { return m_cells;
    }
    virtual bool haveCell(LayoutCell* pCell);
    virtual bool haveCell(long long cellId);

    virtual void setCellFullScr(long long id, bool b);
    virtual void setCellTopMost(long long id, bool b);

    inline LayoutCell* getCell(long long id) const
    {
        auto it = m_cells.find(id);
        if(it != m_cells.end())
        {
            return it->second;
        }
        return nullptr;
    }

    // utility
    virtual void setFocusedCell(LayoutCell* pCell);
    virtual void addFocusedCell(LayoutCell* pCell);
    virtual void switchFocusedCell(LayoutCell* pCell);
    inline bool isFocusedCell(long long id) const { return std::find(m_focusedCells.begin(), m_focusedCells.end(), id) != m_focusedCells.end(); }
    inline const std::list<long long>& focusedCells() const { return m_focusedCells; }

    inline LayoutCell* focusedCell() const
    {
        if(m_focusedCells.empty())
        {
            return nullptr;
        }
        return this->getCell(*m_focusedCells.begin());
    }
    View* focusedView() const;

    virtual LayoutCell* topCell() const;

    //input 
    virtual void onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift);
    virtual void onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift);

    virtual bool isBusy();
    virtual void lock(bool b);
    virtual bool isLock();

protected:
    virtual void _updateProps();

    virtual void _onFocusedCellChanged();

protected:
    ViewLayout(Define::ViewLayoutType type);

    Define::ViewLayoutType m_type;

    Engine::DObject m_props;
    bool m_propsDirty;

    bool m_lock;

    std::unordered_map<long long, LayoutCell*> m_cells;
    std::list<long long> m_focusedCells;
};
}// namespace sketch
