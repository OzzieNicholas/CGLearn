#include "sketch/Define.h"
#include <unordered_map>

namespace sketch
{
void Define::init() {}

const double Define::COMP_UI_DEF_DOUBLE = DBL_MAX;
const long long Define::COMP_UI_DEF_LONG = LLONG_MIN;
const int Define::COMP_UI_DEF_INT = INT_MIN;

std::unordered_map<Define::RevertableType, std::wstring> g_RevertableType2Str = {
    { Define::RVT_NONE, L"none" },
    { Define::RVT_BEGIN, L"begin" },
    { Define::RVT_END, L"end" },
    { Define::RVT_OBJ_CREATE, L"obj_create" },
    { Define::RVT_OBJ_DELETE, L"obj_delete" },
    { Define::RVT_OBJ_MODIFY, L"obj_modify" },
    { Define::RVT_OBJ_MATRIX_MODIFY, L"obj_matrix_modify" },
    { Define::RVT_OBJ_PARENT_MODIFY, L"obj_parent_modify" },
    { Define::RVT_CTRLPT_POS_MODIFY, L"cpt_pos_modify" },
    { Define::RVT_CTRLPT_OWNER_MODIFY, L"cpt_owner_modify" },
    { Define::RVT_PART_ASSEMLBY_MODIFY, L"part_ass_modify" },
    { Define::RVT_COMPEDIT_VARS_MODIFY, L"compedit_vars_modify" },
    { Define::RVT_DRAWING_CREATE, L"dr_create" },
    { Define::RVT_DRAWING_DELETE, L"dr_create" },
    { Define::RVT_DRAWING_MODIFY, L"dr_modify" },
    { Define::RVT_DRELE_CREATE, L"drele_create" },
    { Define::RVT_DRELE_DELETE, L"drele_delete" },
    { Define::RVT_DRELE_MODIFY, L"drele_modify" },
    { Define::RVT_DRELE_MATRIX_MODIFY, L"drele_matrix_modify" },
    { Define::RVT_DRELE_MATRIX_LOCAL_MODIFY, L"drele_matrix_local_modify" },
    { Define::RVT_DRELE_PARENT_MODIFY, L"drele_parent_modify" },
};

const std::wstring& Define::toStr(RevertableType type) { return g_RevertableType2Str[type]; }

std::unordered_map<Define::ObjectType, std::wstring> g_ObjectType2Str = {
    { Define::OBJ_UNKNOWN, L"unknown" },
};
static std::map<std::wstring, Define::ObjectType> g_Str2ObjectType;

const std::wstring& Define::toStr(ObjectType type) { return g_ObjectType2Str[type]; }

Define::ObjectType Define::toObjectType(const std::wstring& str)
{
    if(g_Str2ObjectType.empty())
    {
        for(auto it: g_ObjectType2Str)
        {
            g_Str2ObjectType[it.second] = it.first;
        }
    }

    auto it = g_Str2ObjectType.find(str);
    if(it == g_Str2ObjectType.end())
        return Define::OBJ_UNKNOWN;

    return it->second;
}

static std::map<Define::ObjectSide, std::wstring> g_ObjectSide2Str = {
    { Define::SIDE_LEFT, L"left" },     { Define::SIDE_RIGHT, L"right" }, { Define::SIDE_TOP, L"top" },   { Define::SIDE_BOTTOM, L"bottom" },
    { Define::SIDE_FRONT, L"front" },   { Define::SIDE_BACK, L"back" },   { Define::SIDE_UP, L"up" },     { Define::SIDE_DOWN, L"down" },
    { Define::SIDE_MIDDLE, L"middle" }, { Define::SIDE_HEAD, L"head" },   { Define::SIDE_TAIL, L"tail" }, { Define::SIDE_NONE, L"none" }
};
static std::map<std::wstring, Define::ObjectSide> g_Str2ObjectSide;

const std::wstring& Define::toStr(ObjectSide side) { return g_ObjectSide2Str[side]; }

Define::ObjectSide Define::toObjectSide(const std::wstring& str)
{
    if(g_Str2ObjectSide.empty())
    {
        for(auto it: g_ObjectSide2Str)
        {
            g_Str2ObjectSide[it.second] = it.first;
        }
    }

    auto it = g_Str2ObjectSide.find(str);
    if(it == g_Str2ObjectSide.end())
        return Define::SIDE_NONE;

    return it->second;
}

static std::map<Define::Unit, std::wstring> g_Unit2Str = {
    { Define::UNIT_AUTO, L"auto" }, { Define::UNIT_MM, L"mm" },   { Define::UNIT_CM, L"cm" }, { Define::UNIT_M, L"m" },
    { Define::UNIT_CM_M, L"cm/m" }, { Define::UNIT_INCH, L"in" }, { Define::UNIT_FT, L"ft" }, { Define::UNIT_FT_IN, L"ft-in" },
};
static std::map<std::wstring, Define::Unit> g_Str2Unit;

const std::wstring& Define::toStr(Unit type) { return g_Unit2Str[type]; }

Define::Unit Define::toUnit(const std::wstring& str)
{
    if(g_Str2Unit.empty())
    {
        for(auto it: g_Unit2Str)
        {
            g_Str2Unit[it.second] = it.first;
        }
    }

    auto it = g_Str2Unit.find(str);
    if(it == g_Str2Unit.end())
        return Define::UNIT_AUTO;

    return it->second;
}

static std::map<Define::DrawingType, std::wstring> g_DrawingType2Str = {
    { Define::DRAW_UNKNOWN, L"U" },  { Define::DRAW_SINGLE_PART, L"W" }, { Define::DRAW_ASSEMBLY, L"A" },
    { Define::DRAW_MULTIPLE, L"M" }, { Define::DRAW_ARRANGEMENT, L"G" }, { Define::DRAW_TEMPLATE, L"Drawing Template" },
};
static std::map<std::wstring, Define::DrawingType> g_Str2DrawingType;

const std::wstring& Define::toStr(DrawingType type) { return g_DrawingType2Str[type]; }

Define::DrawingType Define::toDrawingType(const std::wstring& str)
{
    if(g_Str2DrawingType.empty())
    {
        for(auto it: g_DrawingType2Str)
        {
            g_Str2DrawingType[it.second] = it.first;
        }
    }

    auto it = g_Str2DrawingType.find(str);
    if(it == g_Str2DrawingType.end())
        return Define::DRAW_UNKNOWN;

    return it->second;
}

//////////////////////////////////////////////////////////////////////////
/// MasterDrawingType

static std::map<Define::MasterDrawingType, std::wstring> g_MasterDrawingType2Str = {
    { Define::MASTER_DRAWING_TYPE_UNKNOWN, L"Unknown" },
    { Define::MASTER_DRAWING_TYPE_SAVED, L"Saved" },
    { Define::MASTER_DRAWING_TYPE_CLONE, L"Clone" },
    { Define::MASTER_DRAWING_TYPE_RULE_SET, L"RuleSet" },
};
static std::map<std::wstring, Define::MasterDrawingType> g_Str2MasterDrawingType;

const std::wstring& Define::toStr(MasterDrawingType type) { return g_MasterDrawingType2Str[type]; }

Define::MasterDrawingType Define::toMasterDrawingType(const std::wstring& str)
{
    if(g_Str2MasterDrawingType.empty())
    {
        for(auto itt: g_MasterDrawingType2Str)
        {
            g_Str2MasterDrawingType[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2MasterDrawingType.find(str);
    if(itt != g_Str2MasterDrawingType.end())
    {
        return itt->second;
    }
    return Define::MASTER_DRAWING_TYPE_UNKNOWN;
}

static std::map<Define::DRViewType, std::wstring> g_DRViewType2Str = {
    { Define::DRVIEW_NONE, L"none" },
    { Define::DRVIEW_2D, L"2d" },
    { Define::DRVIEW_3D, L"3d" },
};
static std::map<std::wstring, Define::DRViewType> g_Str2DRViewType;

const std::wstring& Define::toStr(DRViewType type) { return g_DRViewType2Str[type]; }

Define::DRViewType Define::toDRViewType(const std::wstring& str)
{
    if(g_Str2DRViewType.empty())
    {
        for(auto it: g_DRViewType2Str)
        {
            g_Str2DRViewType[it.second] = it.first;
        }
    }

    auto it = g_Str2DRViewType.find(str);
    if(it == g_Str2DRViewType.end())
        return Define::DRVIEW_NONE;

    return it->second;
}

static std::map<Define::DRViewSide, std::wstring> g_DRViewSide2Str = {
    { Define::DRVIEW_SIDE_NONE, L"none" },   { Define::DRVIEW_SIDE_TOP, L"top" },         { Define::DRVIEW_SIDE_BOTTOM, L"bottom" },
    { Define::DRVIEW_SIDE_FRONT, L"front" }, { Define::DRVIEW_SIDE_BACK, L"back" },       { Define::DRVIEW_SIDE_HEAD, L"head" },
    { Define::DRVIEW_SIDE_END, L"end" },     { Define::DRVIEW_SIDE_SECTION, L"section" },
};
static std::map<std::wstring, Define::DRViewSide> g_Str2DRViewSide;

const std::wstring& Define::toStr(DRViewSide type) { return g_DRViewSide2Str[type]; }

Define::DRViewSide Define::toDRViewSide(const std::wstring& str)
{
    if(g_Str2DRViewSide.empty())
    {
        for(auto it: g_DRViewSide2Str)
        {
            g_Str2DRViewSide[it.second] = it.first;
        }
    }

    auto it = g_Str2DRViewSide.find(str);
    if(it == g_Str2DRViewSide.end())
        return Define::DRVIEW_SIDE_NONE;

    return it->second;
}

static std::map<Define::DRBoltStyle, std::wstring> g_DRBoltStyle2Str = {
    { Define::DRBOLT_NONE, L"none" },       { Define::DRBOLT_SOLID, L"solid" },     { Define::DRBOLT_EXACT, L"exact" },
    { Define::DRBOLT_SYMBOL1, L"symbol1" }, { Define::DRBOLT_SYMBOL2, L"symbol2" }, { Define::DRBOLT_SYMBOL3, L"symbol3" },
};
static std::map<std::wstring, Define::DRBoltStyle> g_Str2DRBoltStyle;

const std::wstring& Define::toStr(DRBoltStyle type) { return g_DRBoltStyle2Str[type]; }

Define::DRBoltStyle Define::toDRBoltStyle(const std::wstring& str)
{
    if(g_Str2DRBoltStyle.empty())
    {
        for(auto it: g_DRBoltStyle2Str)
        {
            g_Str2DRBoltStyle[it.second] = it.first;
        }
    }

    auto it = g_Str2DRBoltStyle.find(str);
    if(it == g_Str2DRBoltStyle.end())
        return Define::DRBOLT_NONE;

    return it->second;
}

static std::map<Define::DRMarkVisible, std::wstring> g_DRMarkVisible2Str = {
    { Define::DRMARK_VISIBLE_NONE, L"none" },
    { Define::DRMARK_VISIBLE_DISTRIBUTED, L"distributed" },
    { Define::DRMARK_VISIBLE_PREFERRED, L"preferred" },
    { Define::DRMARK_VISIBLE_ALWAYS, L"always" },
};
static std::map<std::wstring, Define::DRMarkVisible> g_Str2DRMarkVisible;

const std::wstring Define::toStr(Define::DRMarkVisible type) { return g_DRMarkVisible2Str[type]; }

Define::DRMarkVisible Define::toDRMarkVisible(const std::wstring& str)
{
    if(g_Str2DRMarkVisible.empty())
    {
        for(auto it: g_DRMarkVisible2Str)
        {
            g_Str2DRMarkVisible[it.second] = it.first;
        }
    }

    auto it = g_Str2DRMarkVisible.find(str);
    if(it == g_Str2DRMarkVisible.end())
    {
        return Define::DRMarkVisible::DRMARK_VISIBLE_NONE;
    }
    return it->second;
}

static std::map<Define::DRLineType, std::wstring> g_DRLineType2Str = {
    { Define::DRLINE_TYPE0, L"type0" }, { Define::DRLINE_TYPE1, L"type1" }, { Define::DRLINE_TYPE2, L"type2" },
    { Define::DRLINE_TYPE3, L"type3" }, { Define::DRLINE_TYPE4, L"type4" },
};
static std::map<std::wstring, Define::DRLineType> g_Str2DRLineType;

const std::wstring& Define::toStr(DRLineType type) { return g_DRLineType2Str[type]; }

Define::DRLineType Define::toDRLineType(const std::wstring& str)
{
    if(g_Str2DRLineType.empty())
    {
        for(auto it: g_DRLineType2Str)
        {
            g_Str2DRLineType[it.second] = it.first;
        }
    }

    auto it = g_Str2DRLineType.find(str);
    if(it == g_Str2DRLineType.end())
        return Define::DRLINE_TYPE0;

    return it->second;
}

static std::map<Define::DRElementType, std::wstring> g_DRElementType2Str = { { Define::DRELE_UNKNOWN, L"unknown" },
                                                                             { Define::DRELE_ELEMENT, L"element" },
                                                                             { Define::DRELE_VIEW_BEGIN, L"view_begin" },
                                                                             { Define::DRELE_VIEW_PART, L"view_part" },
                                                                             { Define::DRELE_VIEW_ASSEMBLY, L"view_assembly" },
                                                                             { Define::DRELE_VIEW_ARRANGEMENT, L"view_arrangement" },
                                                                             { Define::DRELE_VIEW_END, L"view_end" },
                                                                             { Define::DRELE_PART, L"part" },
                                                                             { Define::DRELE_DETAIL_BEGIN, L"detail_begin" },
                                                                             { Define::DRELE_BOLT_GROUP, L"bolt_group" },
                                                                             { Define::DRELE_DETAIL_END, L"detail_end" },
                                                                             { Define::DRELE_DIM_DEF_POINT, L"dim_def_pt" },
                                                                             { Define::DRELE_DIM_BEGIN, L"dim_begin" },
                                                                             { Define::DRELE_DIM_NORMAL, L"dim_normal" },
                                                                             { Define::DRELE_DIM_ANGL, L"dim_angl" },
                                                                             { Define::DRELE_DIM_ARC_RADIUS, L"dim_arc_radius" },
                                                                             { Define::DRELE_DIM_ARC_LEN, L"dim_arc_len" },
                                                                             { Define::DRELE_DIM_ARC_LEN_RADIATE, L"dim_arc_len_radiate" },
                                                                             { Define::DRELE_DIM_END, L"dim_end" },
                                                                             { Define::DRELE_SHAPE_BEGIN, L"shape_begin" },
                                                                             { Define::DRELE_SHAPE_LINE, L"line" },
                                                                             { Define::DRELE_SHAPE_RECT, L"rect" },
                                                                             { Define::DRELE_SHAPE_CIRCLE, L"circle" },
                                                                             { Define::DRELE_SHAPE_ARC, L"arc" },
                                                                             { Define::DRELE_SHAPE_POLY_LINE, L"poly_line" },
                                                                             { Define::DRELE_SHAPE_POLYGON, L"polygon" },
                                                                             { Define::DRELE_SHAPE_CLOUD, L"cloud" },
                                                                             { Define::DRELE_SHAPE_END, L"shape_end" },
                                                                             { Define::DRELE_TEXT, L"text" },
                                                                             { Define::DRELE_SYMBOL_BEGIN, L"symbol_begin" },
                                                                             { Define::DRELE_SYMBOL_SECTION, L"symbol_section" },
                                                                             { Define::DRELE_SYMBOL_DETAIL, L"symbol_detail" },
                                                                             { Define::DRELE_SYMBOL_END, L"symbol_end" },
                                                                             { Define::DRELE_MARK_BEGIN, L"mark_begin" },
                                                                             { Define::DRELE_PART_MARK, L"part_mark" },
                                                                             { Define::DRELE_BOLT_MARK, L"bolt_mark" },
                                                                             { Define::DRELE_SURF_TREATMENT_MARK, L"surf_treatment_mark" },
                                                                             { Define::DRELE_CONNECTION_MARK, L"connection_mark" },
                                                                             { Define::DRELE_MARK_END, L"mark_end" },
                                                                             { Define::DRELE_TABLE, L"table" } };

static std::map<std::wstring, Define::DRElementType> g_Str2DRElementType;

const std::wstring& Define::toStr(DRElementType type) { return g_DRElementType2Str[type]; }

Define::DRElementType Define::toDRElementType(const std::wstring& str)
{
    if(g_Str2DRElementType.empty())
    {
        for(auto it: g_DRElementType2Str)
        {
            g_Str2DRElementType[it.second] = it.first;
        }
    }

    auto it = g_Str2DRElementType.find(str);
    if(it == g_Str2DRElementType.end())
        return Define::DRELE_UNKNOWN;

    return it->second;
}

static std::map<Define::DRDimLinearStyle, std::wstring> g_DRDimLinearStyle2Str = {
    { Define::DRDIM_LINEAR_STYLE0, L"style0" }, { Define::DRDIM_LINEAR_STYLE1, L"style1" }, { Define::DRDIM_LINEAR_STYLE2, L"style2" },
    { Define::DRDIM_LINEAR_STYLE3, L"style3" }, { Define::DRDIM_LINEAR_STYLE4, L"style4" }, { Define::DRDIM_LINEAR_STYLE5, L"style5" },
    { Define::DRDIM_LINEAR_STYLE6, L"style6" }, { Define::DRDIM_LINEAR_STYLE7, L"style7" },
};
static std::map<std::wstring, Define::DRDimLinearStyle> g_Str2DRDimLinearStyle;

const std::wstring& Define::toStr(DRDimLinearStyle type) { return g_DRDimLinearStyle2Str[type]; }

Define::DRDimLinearStyle Define::toDRDimLinearStyle(const std::wstring& str)
{
    if(g_Str2DRDimLinearStyle.empty())
    {
        for(auto it: g_DRDimLinearStyle2Str)
        {
            g_Str2DRDimLinearStyle[it.second] = it.first;
        }
    }

    auto it = g_Str2DRDimLinearStyle.find(str);
    if(it == g_Str2DRDimLinearStyle.end())
        return Define::DRDIM_LINEAR_STYLE0;

    return it->second;
}

static std::map<Define::DRDimAnglStyle, std::wstring> g_DRDimAnglStyle2Str = {
    { Define::DRDIM_ANGL_STYLE_ANGL, L"angl" },
    { Define::DRDIM_ANGL_STYLE_ANGL_BASIC, L"angl_basic" },
    { Define::DRDIM_ANGL_STYLE_TRI, L"tri" },
    { Define::DRDIM_ANGL_STYLE_TRI_ANGL, L"tri_angl" },
};
static std::map<std::wstring, Define::DRDimAnglStyle> g_Str2DRDimAnglStyle;

const std::wstring& Define::toStr(DRDimAnglStyle type) { return g_DRDimAnglStyle2Str[type]; }

Define::DRDimAnglStyle Define::toDRDimAnglStyle(const std::wstring& str)
{
    if(g_Str2DRDimAnglStyle.empty())
    {
        for(auto it: g_DRDimAnglStyle2Str)
        {
            g_Str2DRDimAnglStyle[it.second] = it.first;
        }
    }

    auto it = g_Str2DRDimAnglStyle.find(str);
    if(it == g_Str2DRDimAnglStyle.end())
        return Define::DRDIM_ANGL_STYLE_ANGL;

    return it->second;
}

static std::map<Define::DRTextFrame, std::wstring> g_DRTextFrame2Str = {
    { Define::DRTEXT_FRAME0, L"frame0" }, { Define::DRTEXT_FRAME1, L"frame1" }, { Define::DRTEXT_FRAME2, L"frame2" },
    { Define::DRTEXT_FRAME3, L"frame3" }, { Define::DRTEXT_FRAME4, L"frame4" },
};
static std::map<std::wstring, Define::DRTextFrame> g_Str2DRTextFrame;

const std::wstring& Define::toStr(DRTextFrame type) { return g_DRTextFrame2Str[type]; }

Define::DRTextFrame Define::toDRTextFrame(const std::wstring& str)
{
    if(g_Str2DRTextFrame.empty())
    {
        for(auto it: g_DRTextFrame2Str)
        {
            g_Str2DRTextFrame[it.second] = it.first;
        }
    }

    auto it = g_Str2DRTextFrame.find(str);
    if(it == g_Str2DRTextFrame.end())
        return Define::DRTEXT_FRAME0;

    return it->second;
}

static std::map<Define::DRTextAlignment, std::wstring> g_DRTextAlignment2Str = {
    { Define::DRTEXT_ALIGN_NONE, L"none" },     { Define::DRTEXT_ALIGN_LEFT, L"left" },         { Define::DRTEXT_ALIGN_RIGHT, L"right" },
    { Define::DRTEXT_ALIGN_TOP, L"top" },       { Define::DRTEXT_ALIGN_BASELINE, L"baseline" }, { Define::DRTEXT_ALIGN_BOTTOM, L"bottom" },
    { Define::DRTEXT_ALIGN_MIDDLE, L"middle" },
};
static std::map<std::wstring, Define::DRTextAlignment> g_Str2DRTextAlignment;

const std::wstring& Define::toStr(DRTextAlignment type) { return g_DRTextAlignment2Str[type]; }

Define::DRTextAlignment Define::toDRTextAlignment(const std::wstring& str)
{
    if(g_Str2DRTextAlignment.empty())
    {
        for(auto it: g_DRTextAlignment2Str)
        {
            g_Str2DRTextAlignment[it.second] = it.first;
        }
    }

    auto it = g_Str2DRTextAlignment.find(str);
    if(it == g_Str2DRTextAlignment.end())
        return Define::DRTEXT_ALIGN_MIDDLE;

    return it->second;
}

static std::map<Define::DRArrowShape, std::wstring> g_DRArrowShape2Str = {
    { Define::DRARROW_NONE, L"none" },     { Define::DRARROW_SLASH, L"slash" },
    { Define::DRARROW_ARROW, L"arrow" },   { Define::DRARROW_SOLID_ARROW, L"solid_arrow" },
    { Define::DRARROW_SPHERE, L"sphere" }, { Define::DRARROW_SOLID_SPHERE, L"solid_sphere" },
    { Define::DRARROW_X, L"x" },
};
static std::map<std::wstring, Define::DRArrowShape> g_Str2DRArrowShape;

const std::wstring& Define::toStr(DRArrowShape type) { return g_DRArrowShape2Str[type]; }

Define::DRArrowShape Define::toDRArrowShape(const std::wstring& str)
{
    if(g_Str2DRArrowShape.empty())
    {
        for(auto it: g_DRArrowShape2Str)
        {
            g_Str2DRArrowShape[it.second] = it.first;
        }
    }

    auto it = g_Str2DRArrowShape.find(str);
    if(it == g_Str2DRArrowShape.end())
        return Define::DRARROW_ARROW;

    return it->second;
}

static std::map<Define::DRMarkTagType, std::wstring> g_DRMarkTagType2Str = {
    { Define::DRMARK_TAG_NONE, L"none" },
    { Define::DRMARK_TAG_FRAME_BEGIN, L"frame_begin" },
    { Define::DRMARK_TAG_FRAME_END, L"frame_end" },
    { Define::DRMARK_TAG_PROPS, L"props" },
};
static std::map<std::wstring, Define::DRMarkTagType> g_Str2DRMarkTagType;

const std::wstring& Define::toStr(DRMarkTagType type) { return g_DRMarkTagType2Str[type]; }

Define::DRMarkTagType Define::toDRMarkTagType(const std::wstring& str)
{
    if(g_Str2DRMarkTagType.empty())
    {
        for(auto it: g_DRMarkTagType2Str)
        {
            g_Str2DRMarkTagType[it.second] = it.first;
        }
    }

    auto it = g_Str2DRMarkTagType.find(str);
    if(it == g_Str2DRMarkTagType.end())
        return Define::DRMARK_TAG_NONE;

    return it->second;
}

static std::map<Define::DRMarkSymbolShape, std::wstring> g_DRMarkSymbolShape2Str = {
    { Define::DRMARK_SYMBOL_SHAPE_NONE, L"none" },
    { Define::DRMARK_SYMBOL_SHAPE_CIRCLE, L"circle" },
    { Define::DRMARK_SYMBOL_SHAPE_CIRCLE_LINE, L"circle_line" },
};
static std::map<std::wstring, Define::DRMarkSymbolShape> g_Str2DRMarkSymbolShape;

const std::wstring& Define::toStr(DRMarkSymbolShape type) { return g_DRMarkSymbolShape2Str[type]; }

Define::DRMarkSymbolShape Define::toDRMarkSymbolShape(const std::wstring& str)
{
    if(g_Str2DRMarkSymbolShape.empty())
    {
        for(auto it: g_DRMarkSymbolShape2Str)
        {
            g_Str2DRMarkSymbolShape[it.second] = it.first;
        }
    }

    auto it = g_Str2DRMarkSymbolShape.find(str);
    if(it == g_Str2DRMarkSymbolShape.end())
        return Define::DRMARK_SYMBOL_SHAPE_NONE;

    return it->second;
}

static std::map<Define::DRWeldMarkVisible, std::wstring> g_DRWeldMarkVisible2Str = {
    { Define::DRMARK_WELD_VISIBLE_NONE, L"none" },
    { Define::DRMARK_WELD_VISIBLE_SITE, L"site" },
    { Define::DRMARK_WELD_VISIBLE_WORKSHOP, L"workshop" },
    { Define::DRMARK_WELD_VISIBLE_BOTH, L"both" },
};
static std::map<std::wstring, Define::DRWeldMarkVisible> g_Str2DRWeldMarkVisible;

const std::wstring& Define::toStr(DRWeldMarkVisible type) { return g_DRWeldMarkVisible2Str[type]; }

Define::DRWeldMarkVisible Define::toDRWeldMarkVisible(const std::wstring& str)
{
    if(g_Str2DRWeldMarkVisible.empty())
    {
        for(auto itt: g_DRWeldMarkVisible2Str)
        {
            g_Str2DRWeldMarkVisible[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2DRWeldMarkVisible.find(str);
    if(itt == g_Str2DRWeldMarkVisible.end())
    {
        return Define::DRMARK_WELD_VISIBLE_NONE;
    }
    return itt->second;
}

static std::map<Define::DRUpdateStatus, std::wstring> g_DRUpdateStatus2Str = {
    { Define::DRUPDATE_NONE, L"" },       { Define::DRUPDATE_UPDATED, L"updated" }, { Define::DRUPDATE_CLONE, L"clone" },
    { Define::DRUPDATE_DIRTY, L"dirty" }, { Define::DRUPDATE_DELETE, L"delete" },
};
static std::map<std::wstring, Define::DRUpdateStatus> g_Str2DRUpdateStatus;

const std::wstring& Define::toStr(Define::DRUpdateStatus val) { return g_DRUpdateStatus2Str[val]; }

Define::DRUpdateStatus Define::toDRUpdateStatus(const std::wstring& val)
{
    if(g_Str2DRUpdateStatus.empty())
    {
        for(auto& itt: g_DRUpdateStatus2Str)
        {
            g_Str2DRUpdateStatus[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2DRUpdateStatus.find(val);
    if(itt != g_Str2DRUpdateStatus.end())
    {
        return itt->second;
    }
    return Define::DRUPDATE_NONE;
}

static std::map<Define::DRChangeType, std::wstring> g_DRChangeType2Str = {
    { Define::DRCHANGE_NONE, L"" },
    { Define::DRCHANGE_AMOUNT_INCREASE, L"amount_increase" },
    { Define::DRCHANGE_AMOUNT_DECREASE, L"amount_decrease" },
    { Define::DRCHANGE_PARTS_DELETED, L"parts_deleted" },
    { Define::DRCHANGE_PARTS_MODIFIED, L"parts_modified" },
    { Define::DRCHANGE_LINK_MODIFIED, L"link_modified" },
    { Define::DRCHANGE_COPY_MODIFIED, L"copy_modified" },
    { Define::DRCHANGE_UPDATED, L"updated" },
    { Define::DRCHANGE_CLONE, L"clone" },
};
static std::map<std::wstring, Define::DRChangeType> g_Str2DRChangeType;

const std::wstring& Define::toStr(Define::DRChangeType val) { return g_DRChangeType2Str[val]; }

Define::DRChangeType Define::toDRChangeType(const std::wstring& val)
{
    if(g_Str2DRChangeType.empty())
    {
        for(auto& itt: g_DRChangeType2Str)
        {
            g_Str2DRChangeType[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2DRChangeType.find(val);
    if(itt != g_Str2DRChangeType.end())
    {
        return itt->second;
    }
    return DRCHANGE_NONE;
}

static std::map<Define::RFBarOffsetBase, std::wstring> g_RFBarOffsetBase2Str = {
    { Define::RFBAR_OFF_NONE, L"none" },
    { Define::RFBAR_OFF_COVER_THICKNESS, L"cover_thickness" },
    { Define::RFBAR_OFF_LEG_LENGTH, L"leg_length" },
};
static std::map<std::wstring, Define::RFBarOffsetBase> g_Str2RFBarOffsetBase;

const std::wstring& Define::toStr(RFBarOffsetBase type) { return g_RFBarOffsetBase2Str[type]; }

Define::RFBarOffsetBase Define::toRFBarOffsetBase(const std::wstring& str)
{
    if(g_Str2RFBarOffsetBase.empty())
    {
        for(auto itt: g_RFBarOffsetBase2Str)
        {
            g_Str2RFBarOffsetBase[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2RFBarOffsetBase.find(str);
    if(itt == g_Str2RFBarOffsetBase.end())
    {
        return Define::RFBAR_OFF_NONE;
    }
    return itt->second;
}

static std::map<Define::RFBarMeshType, std::wstring> g_RFBarMeshType2Str = {
    { Define::RFBAR_MESH_NONE, L"none" },
    { Define::RFBAR_MESH_POLYGON, L"polygon" },
    { Define::RFBAR_MESH_RECTANGLE, L"rect" },
    { Define::RFBAR_MESH_BENT, L"bent" },
};
static std::map<std::wstring, Define::RFBarMeshType> g_Str2RFBarMeshType;

const std::wstring& Define::toStr(RFBarMeshType type) { return g_RFBarMeshType2Str[type]; }

Define::RFBarMeshType Define::toRFBarMeshType(const std::wstring& str)
{
    if(g_Str2RFBarMeshType.empty())
    {
        for(auto itt: g_RFBarMeshType2Str)
        {
            g_Str2RFBarMeshType[itt.second] = itt.first;
        }
    }

    auto itt = g_Str2RFBarMeshType.find(str);
    if(itt == g_Str2RFBarMeshType.end())
    {
        return Define::RFBAR_MESH_NONE;
    }
    return itt->second;
}
}// namespace sketch