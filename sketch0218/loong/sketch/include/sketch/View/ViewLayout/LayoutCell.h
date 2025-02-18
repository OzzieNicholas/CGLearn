#pragma once

#include "..\..\Core.h"

#include <map>
#include <set>

#include "../../Define.h"
#include "..\..\Document\DocListener.h"
#include "Engine/Util/DObject.h"
#include "Engine\Math\Vec2i.h"
#include "Engine\Math\Vec4i.h"
#include "ViewCamera.h"
#include "sketch/Serializer/SketchSerializer.h"
#include <sketch/Object/SketchParamMap.h>

namespace Engine
{
class FontData;
}

namespace sketch
{
class Selector;
class View;
class Drawing;
class CELL_BATCH_BLOCK;
class CELL_TEXT_BLOCK;

class LayoutCell : public DocListener
{
public:
    static const uint32_t DIRTY_BIT_RT = 1;
    static const uint32_t DIRTY_BIT_CAM_POSE = 1 << 1;
    static const uint32_t DIRTY_BIT_CAM_DEPTH = 1 << 2;
    static const uint32_t DIRTY_BIT_VIEW_CLIPBOX = 1 << 3;
    static const uint32_t DIRTY_BIT_VIEW_FILTER = 1 << 4;
    static const uint32_t DIRTY_BIT_VIEW_BBOX = 1 << 5;

public:
    LayoutCell(Define::LayoutCellType type, uint32_t dirtyFlags);
    virtual ~LayoutCell(void);

    virtual void release();

    inline Define::LayoutCellType cellType() const { return m_cellType; }

    virtual long long cellId() const;
    virtual const std::wstring cellName() const;

    inline int renderId() const { return m_renderId; }

    View* _view() const;

    inline ViewCamera* camera() const { return m_pCam; }

    virtual void redraw(void);

    inline const SketchParamMap& cellProps()
    {
        this->_updateCellProps();
        return m_cellProps;
    }

    void save(SketchSerializer& serializer);
    void load(SketchSerializer& serializer);

    inline void dirtyRT() { m_dirtyFlags |= DIRTY_BIT_RT; }

    inline bool isDirtyRT() const { return m_dirtyFlags & DIRTY_BIT_RT; }

    virtual void modifyCellProps(const SketchParamMap& props);

    virtual void resetCameraPose();
    virtual void onCameraTransform();

    virtual void onRenderCell(long long lElapsedTm) = 0;
    virtual void onLayoutResize(int width, int height);
    virtual void onCellResize();
    virtual void onCellAdded(int renderId);
    virtual bool onCellRemoving();// ret: true, ignore; false, remove;
    virtual void onCellRemoved();

    // cell area in layout space
    virtual void setLayoutRect(const Engine::Vec4i& rect);
    virtual void setLayoutRect(int x, int y, int width, int height);
    virtual void setLayoutPos(int x, int y);
    virtual void setLayoutSize(int width, int height);

    inline const Engine::Vec4i& layoutRect() const { return m_layoutRect; }

    inline const Engine::Vec2i& layoutPos() const { return m_layoutRect.xy; }

    inline const Engine::Vec2i& layoutSize() const { return m_layoutRect.zw; }

    inline int layoutWidth() const { return m_layoutRect.z; }

    inline int layoutHeight() const { return m_layoutRect.w; }

    virtual void setFullScr(bool b);
    inline bool isFullScr() const { return m_fullScr; }

    virtual void setTopMost(bool b);
    inline bool isTopMost() const { return m_topMost; }

    virtual bool isWindowPosIn(int x, int y) const;
    virtual bool isCover(LayoutCell* pCell) const;
    virtual bool isIntersect(LayoutCell* pCell) const;
    virtual bool isMouseOnTitleBar(int x, int y) const;
    virtual bool isMouseOnControlBtn(int x, int y, int idx /*right to left*/) const;

protected:
    virtual void _updateCellProps();
    virtual void _updateCamOrtho();
    virtual void _resetBatchBlocks();
    virtual void _prepareBatchBlocks();
    virtual void _renderShapeCircles(double width, CELL_BATCH_BLOCK* pBlock, bool clip);
    virtual void _renderPost(const std::wstring& title, const Engine::Vec4f& titleColor);

protected:
    static const float SYS_BUTTON_SIZE;
    static const float SYS_BUTTON_ICON_SIZE;
    static const float TITLE_BAR_HEIGHT;

    int m_renderId;

    Define::LayoutCellType m_cellType;
    ViewCamera* m_pCam;

    SketchParamMap m_cellProps;

    bool m_fullScr;
    bool m_topMost;
    Engine::Vec4i m_layoutRect;

    unsigned int m_dirtyFlags;

    // batch render
    union BATCH_LINE_KEY {
        uint64_t key;

        struct
        {
            uint16_t pattern;
            uint8_t factor;
            uint8_t width;
            uint8_t bits;// 0:clip planes, 1:screen space
        };
    };

    // std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_solidTriBatches;
    // std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_transTriBatches;
    std::unordered_map<uint64_t, CELL_BATCH_BLOCK*> m_scrUniTriBatches;
    std::unordered_map<std::string, CELL_BATCH_BLOCK*> m_colorStampLineBatches;//<<width, stamp>, data>
    CELL_BATCH_BLOCK* m_circleBatches[16];                                     // b0-2:width, b3:clip

    // batch render texts
    std::unordered_map<Engine::FontData*, CELL_TEXT_BLOCK*> m_textBatches;
};

}// namespace sketch
