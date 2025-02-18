#include "sketch/Document/View/ViewDisPlaySetting/ViewDisplaySetting.h"

#include "Engine/Util/StrUtil.h"

#include "sketch/Define.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
ViewDisplaySetting::ViewDisplaySetting(void)
{
    // render quality
    m_partQuality = Define::RENDERQUALITY_EXACT;
    m_boltQuality = Define::RENDERQUALITY_EXACT;
    m_holeQuality = Define::RENDERQUALITY_EXACT;
    m_weldQuality = Define::RENDERQUALITY_FAST;
    m_constructPlQuality = Define::RENDERQUALITY_EXACT;
    m_rfbarQuality = Define::RENDERQUALITY_FAST;

    // render mode
    m_partMode = Define::RENDERMODE_EDGEFACE;
    m_partModeComp = Define::RENDERMODE_EDGEFACE;

    // part label
    m_partLabelProps = {};

    // point
    m_ptSize = 30.0;
    m_ptMode = 1;// 0: model 1: view

    m_propsDirty = true;
}

ViewDisplaySetting::~ViewDisplaySetting(void) {}

bool ViewDisplaySetting::save(SketchSerializer& serializer)
{
    // serializer.write(static_cast<char>(m_partQuality), "part_quality");
    // serializer.write(static_cast<char>(m_boltQuality), "bolt_quality");
    // serializer.write(static_cast<char>(m_holeQuality), "hole_quality");
    // serializer.write(static_cast<char>(m_weldQuality), "weld_quality");
    // serializer.write(static_cast<char>(m_constructPlQuality), "construct_pl_quality");
    // serializer.write(static_cast<char>(m_rfbarQuality), "rfbar_quality");
    // serializer.write(static_cast<char>(m_partMode), "part_mode");
    // serializer.write(static_cast<char>(m_partModeComp), "part_mode_comp");

    // serializer.begin_write_array("part_label_attrib");
    // for(const auto& attrib_wstr: m_partLabelProps)
    //{
    //     std::string attrib_str = StrUtil::w2s(attrib_wstr);
    //     serializer.write(attrib_str.c_str());
    // }
    // serializer.end_write_array();

    // serializer.write(m_ptSize, "pt_size");
    // serializer.write(m_ptMode, "m_ptMode");

    _updateProps();
    serializer.begin_write_object("props_map");
    m_props.save(serializer);
    serializer.end_write_object();

    return true;
}

bool ViewDisplaySetting::load(SketchSerializer& serializer)
{
    serializer.begin_read_object("props_map");
    m_props.load(serializer);
    serializer.end_read_object();
    modify(m_props);

    // char quality;
    // serializer.read(quality, "part_quality");
    // m_partQuality = static_cast<Define::ViewRenderQuality>(quality);
    // serializer.read(quality, "bolt_quality");
    // m_boltQuality = static_cast<Define::ViewRenderQuality>(quality);
    // serializer.read(quality, "hole_quality");
    // m_holeQuality = static_cast<Define::ViewRenderQuality>(quality);
    // serializer.read(quality, "weld_quality");
    // m_weldQuality = static_cast<Define::ViewRenderQuality>(quality);
    // serializer.read(quality, "construct_pl_quality");
    // m_constructPlQuality = static_cast<Define::ViewRenderQuality>(quality);
    // serializer.read(quality, "rfbar_quality");
    // m_rfbarQuality = static_cast<Define::ViewRenderQuality>(quality);

    // serializer.read(quality, "part_mode");
    // m_partMode = static_cast<Define::ViewRenderMode>(quality);
    // serializer.read(quality, "part_mode_comp");
    // m_partModeComp = static_cast<Define::ViewRenderMode>(quality);

    // size_t part_label_attrib_count = 0;
    // serializer.begin_read_array("part_label_props", part_label_attrib_count);
    // for(int i = 0; i < part_label_attrib_count; ++i)
    //{
    //     nous::String str;
    //     serializer.read(str);
    //     m_partLabelProps.emplace_back(StrUtil::s2w(str.data()));
    // }
    // serializer.end_read_array();

    // serializer.read(m_ptSize, "pt_size");
    // serializer.read(m_ptMode, "pt_size_mode");

    m_propsDirty = true;
    return true;
}

void ViewDisplaySetting::modify(SketchParamMap& props)
{
    char quality;
    if(props.has_field("part_quality") && props.get_value_type("part_quality") == ESketchParamValueType::Char)
    {
        props.get_value("part_quality", quality);
        m_partQuality = static_cast<Define::ViewRenderQuality>(quality);
    }
    if(props.has_field("bolt_quality") && props.get_value_type("bolt_quality") == ESketchParamValueType::Char)
    {
        props.get_value("bolt_quality", quality);
        m_boltQuality = static_cast<Define::ViewRenderQuality>(quality);
    }
    if(props.has_field("hole_quality") && props.get_value_type("hole_quality") == ESketchParamValueType::Char)
    {
        props.get_value("hole_quality", quality);
        m_holeQuality = static_cast<Define::ViewRenderQuality>(quality);
    }
    if(props.has_field("weld_quality") && props.get_value_type("weld_quality") == ESketchParamValueType::Char)
    {
        props.get_value("weld_quality", quality);
        m_weldQuality = static_cast<Define::ViewRenderQuality>(quality);
    }
    if(props.has_field("construct_pl_quality") && props.get_value_type("construct_pl_quality") == ESketchParamValueType::Char)
    {
        props.get_value("construct_pl_quality", quality);
        m_constructPlQuality = static_cast<Define::ViewRenderQuality>(quality);
    }
    if(props.has_field("rfbar_quality") && props.get_value_type("rfbar_quality") == ESketchParamValueType::Char)
    {
        props.get_value("rfbar_quality", quality);
        m_rfbarQuality = static_cast<Define::ViewRenderQuality>(quality);
    }

    if(props.has_field("part_mode") && props.get_value_type("part_mode") == ESketchParamValueType::Char)
    {
        props.get_value("part_mode", quality);
        m_partMode = static_cast<Define::ViewRenderMode>(quality);
    }
    if(props.has_field("part_mode_comp") && props.get_value_type("part_mode_comp") == ESketchParamValueType::Char)
    {
        props.get_value("part_mode_comp", quality);
        m_partModeComp = static_cast<Define::ViewRenderMode>(quality);
    }

    if(props.has_field("part_label_props") && props.get_value_type("part_label_props") == ESketchParamValueType::WildStringArray)
    {
        props.get_value("part_label_props", m_partLabelProps);
    }

    if(props.has_field("pt_size") && props.get_value_type("pt_size") == ESketchParamValueType::Double)
    {
        props.get_value("pt_size", m_ptSize);
    }
    if(props.has_field("pt_size_mode") && props.get_value_type("pt_size_mode") == ESketchParamValueType::Integer)
    {
        props.get_value("part_mode_comp", m_ptMode);
    }

    m_propsDirty = true;
}

void ViewDisplaySetting::_updateProps()
{
    // render quality
    m_props.set_value("part_quality", static_cast<char>(m_partQuality));
    m_props.set_value("bolt_quality", static_cast<char>(m_boltQuality));
    m_props.set_value("hole_quality", static_cast<char>(m_holeQuality));
    m_props.set_value("weld_quality", static_cast<char>(m_weldQuality));
    m_props.set_value("construct_pl_quality", static_cast<char>(m_constructPlQuality));
    m_props.set_value("rfbar_quality", static_cast<char>(m_rfbarQuality));
    // render mode
    m_props.set_value("part_mode", static_cast<char>(m_partMode));
    m_props.set_value("part_mode_comp", static_cast<char>(m_partModeComp));
    // part label
    m_props.set_value("part_label_props", m_partLabelProps);
    // point
    m_props.set_value("pt_size", m_partQuality);
    m_props.set_value("pt_size_mode", m_partQuality);
}

bool ViewDisplaySetting::filter(TFObjectDoc* pObj) const
{
    if(!pObj)
        return false;

    return true;
}
}// namespace sketch