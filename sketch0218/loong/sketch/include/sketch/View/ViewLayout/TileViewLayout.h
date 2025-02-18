#pragma once

#include <queue>
#include <string>

#include "../../Core.h"
#include "Engine/Math/Vec4i.h"
#include "ViewLayout.h"
#include "sketch/NousEngine/NousConvert.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace sketch
{
class TFObjectDoc;

class TileViewLayout : public ViewLayout
{
public:
    ~TileViewLayout(void) override;
    virtual void release(void) override;

    static TileViewLayout* instance();

    virtual bool init() override;

    virtual bool onDocClose(void) override;
    virtual void save(Engine::DObject* pSavedObj, const std::wstring& progName, float progRange) override;
    void save_spec(SketchSerializer& serializer, const std::wstring& progName, float progRange);
    void load_spec(SketchSerializer& serializer);

    virtual bool isBusy() override;
    virtual void modify(const Engine::DObject& props) override;

    virtual void onRender(long long lElapsedTm) override;
    virtual void onResize(int width, int height) override;

    virtual void addCell(LayoutCell* pCell) override;
    virtual void addCells(const std::unordered_set<LayoutCell*>& cells) override;
    virtual bool removeCell(LayoutCell* pCell) override;
    virtual void removeCells(const std::unordered_set<LayoutCell*>& cells) override;
    inline const std::list<long long>& sortedCells() const { return m_sortedCells; }
    bool haveDrawingCell();

    virtual void setFocusedCell(LayoutCell* pCell) override;
    virtual void addFocusedCell(LayoutCell* pCell) override;
    virtual void switchFocusedCell(LayoutCell* pCell) override;

    virtual Engine::Vec4i initCellRect();

    // top-left coordinates to Cartesian coordinates
    Engine::Vec4i canvas2Window(const Engine::Vec4i& rect);

    virtual void setCellFullScr(long long id, bool b) override;
    virtual void setCellTopMost(long long id, bool b) override;

    virtual LayoutCell* topCell() const override;

    void dirtyCells() const;
    bool hasDirtyCells() const;

    // input
    virtual void onKeyDown(int key, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseDown(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseMove(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseUp(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseDoubleClick(int button, int x, int y, bool bCtrl, bool bAlt, bool bShift) override;
    virtual void onMouseWheel(int button, int x, int y, int delta, bool bCtrl, bool bAlt, bool bShift) override;

    void updateUISelInfo();

protected:
    TileViewLayout(void);
    virtual void _updateProps() override;

    int _newRenderId();
    void _freeRenderId(int rdId);

    void _resortCells();
    void _snapCell(LayoutCell* pCell, int state, int left, int top, int width, int height);

    void _evalManuState(int x, int y);
    Define::MouseCursor _evalMouseCursor(int x, int y);
    Define::MouseCursor _mergeMouseCursor(Define::MouseCursor src, Define::MouseCursor dst);
    bool _isWindowPosCovered(std::list<long long>::iterator it, int x, int y);
    bool _isCellVisible(std::list<long long>::iterator it, LayoutCell* pIgnore = nullptr);

protected:
    const int PICK_RANGE = 8;
    const int SNAP_RANGE = 15;
    const int MIN_VIEW_SIZE = 50;

    static TileViewLayout* m_pInstance;

    std::list<long long> m_sortedCells;
    std::unordered_set<int> m_usedRenderIds;
    std::queue<int> m_unusedRenderIds;

    // 0:none, 1:move, 2:resize left, 3:resize right, 4:resize bottom, 5:resize top
    // 6:resize left-bottom, 7:resize left-top, 8:resize right-bottom, 9:resize right-top
    // 10: scroll-v, 11: scroll-h
    std::unordered_map<long long, std::pair<int, Engine::Vec4i>> m_manuState;

    bool m_sysMouseSession;
};

}// namespace sketch
