#pragma once

#include "..\..\..\Core.h"
#include <map>
#include <string>

#include "..\..\..\Define.h"
#include "..\..\DocElement.h"
#include "sketch/Object/SketchParamMap.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace sketch
{
class TFObjectDoc;

class ViewDisplaySetting
{
public:
    ViewDisplaySetting(void);
    virtual ~ViewDisplaySetting(void);

    SketchParamMap& props()
    {
        if(m_propsDirty)
        {
            this->_updateProps();
            m_propsDirty = false;
        }

        return m_props;
    }

    void modify(SketchParamMap& props);
    bool filter(TFObjectDoc* pObj) const;

    bool save(SketchSerializer& serializer);
    bool load(SketchSerializer& serializer);

    // render quality
    inline Define::ViewRenderQuality partQuality() const { return m_partQuality; }

    inline Define::ViewRenderQuality boltQuality() const { return m_boltQuality; }

    inline Define::ViewRenderQuality holeQuality() const { return m_holeQuality; }

    inline Define::ViewRenderQuality weldQuality() const { return m_weldQuality; }

    inline Define::ViewRenderQuality constructPlQuality() const { return m_constructPlQuality; }

    inline Define::ViewRenderQuality rfbarQuality() const { return m_rfbarQuality; }

    // render mode
    inline Define::ViewRenderMode partMode() const { return m_partMode; }

    inline Define::ViewRenderMode partModeComp() const { return m_partModeComp; }

    // part label
    inline const std::vector<std::wstring>& partLabelProps() const { return m_partLabelProps; }

    // point
    inline double ptSize() const { return m_ptSize; }

    inline int ptMode() const
    {// 0: model 1: view
        return m_ptMode;
    }

protected:
    void _updateProps();

protected:
    SketchParamMap m_props;
    bool m_propsDirty;

    // object visibility

    // render quality
    Define::ViewRenderQuality m_partQuality;
    Define::ViewRenderQuality m_boltQuality;
    Define::ViewRenderQuality m_holeQuality;
    Define::ViewRenderQuality m_weldQuality;
    Define::ViewRenderQuality m_constructPlQuality;
    Define::ViewRenderQuality m_rfbarQuality;

    // render mode
    Define::ViewRenderMode m_partMode;
    Define::ViewRenderMode m_partModeComp;

    // part label
    std::vector<std::wstring> m_partLabelProps;

    // point
    double m_ptSize;
    int m_ptMode;// 0: model 1: view
};

}// namespace sketch
