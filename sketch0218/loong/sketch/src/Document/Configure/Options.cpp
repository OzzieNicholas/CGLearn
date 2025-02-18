#include "sketch/Document/Configure/Options.h"

#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
#include "sketch/STCore.h"
#include "sketch/UIFuncs.h"
#include "sketch/View/RenderView/View.h"

using namespace Engine;

namespace sketch
{
Options::Options()
    : m_dirty(false),

      m_viewRotEnable(true),

      // Selector3D
      m_sel3d_pickMode(Define::PICK_MODE_COMPONENT), m_sel3d_snapMode(Define::SNAP_MODE_AUTO),
      m_sel3d_snapFlagPt(Define::SNAP_PT_ALL & ~(Define::SNAP_PT_LINE_EXT | Define::SNAP_PT_LINE | Define::SNAP_PT_FREE)),
      m_sel3d_snapFlagPl(Define::SNAP_PL_GEO), m_sel3d_snapRefPlane(Define::SNAP_REF_PLANE_VIEW)
{
}
bool Options::init()
{
    m_dirty = false;
    return true;
}

bool Options::onDocPrepare(const Engine::Path& projPath) { return true; }

bool Options::onDocNew(const Engine::Path& projPath)
{
    this->_loadSysConfigs();
    m_dirty = false;
    return true;
}

bool Options::onDocLoad(const Engine::Path& projPath)
{
    this->_loadSysConfigs();
    this->_loadUserConfigs(projPath);
    m_dirty = false;
    return true;
}

bool Options::onDocLoaded(const Engine::Path& projPath) { return true; }

bool Options::onDocClose(void)
{
    if(m_dirty && !STCore::inst->docMgr()->document()->readOnly())
    {
        auto& projPath = STCore::inst->docMgr()->document()->path();
        if(!projPath.empty())
            this->_saveUserConfigs(projPath);

        m_dirty = false;
    }

    m_props.clear();
    return true;
}

bool Options::onAppClose(void) { return this->onDocClose(); }

void Options::modify(const Engine::DObject& props)
{
    for(auto& it: props)
        this->_parseProp(it.first, it.second);
}
// Selector

void Options::setPickMode(Define::SelectorType selType, Define::PickMode mode)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_pickMode = mode; break;
    }
}

void Options::setPickFlag(Define::SelectorType selType, uint32_t flag)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_pickFlag = flag; break;
    }
}

void Options::updatePickFlag(Define::SelectorType selType, uint32_t flag, bool b)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: {
        if(b)
        {
            m_sel3d_pickFlag |= flag;
        }
        else
        {
            m_sel3d_pickFlag &= (~flag);
        }
    }
    break;
    }
}

bool Options::checkPickFlag(Define::SelectorType selType, uint32_t flag) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return (m_sel3d_pickFlag & flag) == flag;
    }
    return false;
}

Define::PickMode Options::pickMode(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_pickMode;
    }
    return Define::PICK_MODE_NONE;
}

uint32_t Options::pickFlag(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_pickFlag;
    }
    return 0u;
}

void Options::setSnapMode(Define::SelectorType selType, Define::SnapMode mode)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_snapMode = mode; break;
    }
}

Define::SnapMode Options::snapMode(View* pView) const
{
    return m_sel3d_snapMode == Define::SNAP_MODE_AUTO ? (pView->doc()->type() == Define::VIEW_2D ? Define::SNAP_MODE_PLANE : Define::SNAP_MODE_3D)
                                                      : m_sel3d_snapMode;
}

Define::SnapMode Options::snapMode(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_snapMode;
    }
    return Define::SNAP_MODE_NONE;
}

void Options::setSnapFlagPl(Define::SelectorType selType, uint32_t flag)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_snapFlagPl = flag; break;
    }
}

void Options::setSnapFlagPt(Define::SelectorType selType, uint32_t flag)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_snapFlagPt = flag; break;
    }
}

void Options::updateSnapFlagPt(Define::SelectorType selType, uint32_t flag, bool b)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: {
        if(b)
        {
            m_sel3d_snapFlagPt |= flag;
        }
        else
        {
            m_sel3d_snapFlagPt &= (~flag);
        }
    }
    break;
    }
}

bool Options::checkSnapFlagPt(Define::SelectorType selType, uint32_t flag) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return (m_sel3d_snapFlagPt & flag) == flag;
    }
    return false;
}

void Options::updateSnapFlagPl(Define::SelectorType selType, uint32_t flag, bool b)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: {
        if(b)
        {
            m_sel3d_snapFlagPl |= flag;
        }
        else
        {
            m_sel3d_snapFlagPl &= (~flag);
        }
    }
    break;
    }
}

bool Options::checkSnapFlagPl(Define::SelectorType selType, uint32_t flag) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return (m_sel3d_snapFlagPl & flag) == flag;
    }
    return false;
}

uint32_t Options::snapFlagPt(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_snapFlagPt;
    }
    return 0u;
}

uint32_t Options::snapFlagPl(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_snapFlagPl;
    }
    return 0u;
}

Define::SnapRefPlane Options::snapRefPlane(Define::SelectorType selType) const
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: return m_sel3d_snapRefPlane;
    }
    return Define::SNAP_REF_PLANE_NONE;
}

void Options::setSnapRefPlane(Define::SelectorType selType, Define::SnapRefPlane plane)
{
    switch(selType)
    {
    case Define::SEL_TYPE_3D: m_sel3d_snapRefPlane = plane; break;
    }
}

const Engine::AxisPlane& Options::snapRefPlane(View* pView) const { return pView->doc()->viewPlane(); }

bool Options::_loadSysConfigs()
{
    auto& env = STCore::inst->docMgr()->document()->env();
    auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "env" / env / "options.json";

    std::string text = FileUtil::loadTextFile(cfgPath);
    if(text.empty())
    {
        Log::log("", "[Error] Failed to load config file: %s\n", cfgPath.str().c_str());
        return false;
    }

    // rapidjson::Document doc;
    // doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

    // if (doc.HasParseError()) {
    //	Log::log("", "[Error] Config parse error: %s\n", cfgPath.str().c_str());
    //	Log::log("", "[Error] Config parse error: [%s] %s \n",
    //		std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
    //		rapidjson::GetParseError_En(doc.GetParseError()));
    //	return false;
    // }

    // if (!doc.IsObject()) {
    //	Log::log("", "[Error] Config parse error: %s\n", cfgPath.str().c_str());
    //	return false;
    // }

    // m_props.fromJson(&doc);

    for(auto& it: m_props)
    {
        this->_parseProp(it.first, it.second);
    }

    return true;
}

bool Options::_loadUserConfigs(const Engine::Path& projPath)
{
    auto cfgPath = projPath / L"config\\options.lgst";

    FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
    if(fp == nullptr)
        return false;

    if(!FileUtil::parseMagic(fp, "STOP"))
    {
        fclose(fp);
        return false;
    }

    m_props.fromFile(fp);
    fclose(fp);

    for(auto& it: m_props)
    {
        this->_parseProp(it.first, it.second);
    }

    return true;
}

void Options::dirty() { m_dirty = true; }

void Options::_saveUserConfigs(const Engine::Path& projPath)
{
    DObject optProps = this->props();
    optProps["sel3d_pick_obj_filter_cfg"] = L"";// Ñ¡ÔñÄ¬ÈÏÅäÖÃ²»±£´æ

    auto cfgPath = projPath / "config";
    if(!FileUtil::hasDir(cfgPath))
        FileUtil::createDir(cfgPath);

    auto file = cfgPath / "options.lgst";

    FILE* fp = _wfopen(file.wstr().c_str(), L"wb");
    if(fp == nullptr)
        return;

    fwrite("STOP", 4, 1, fp);
    optProps.toFile(fp, "");
    fclose(fp);
}

void Options::_updateProps()
{
    // Selector3D
    m_props["sel3d_pick_mode"] = m_sel3d_pickMode;
    m_props["sel3d_pick_flag"] = m_sel3d_pickFlag;
    m_props["sel3d_snap_mode"] = m_sel3d_snapMode;
    m_props["sel3d_snap_flag_pt"] = m_sel3d_snapFlagPt;
    m_props["sel3d_snap_flag_pl"] = m_sel3d_snapFlagPl;
    m_props["sel3d_snap_ref_plane"] = m_sel3d_snapRefPlane;
}

bool Options::_parseProp(const std::string& pname, const Engine::DValue& pval)
{
    if(pname == "sel3d_pick_mode")
    {
        m_sel3d_pickMode = (Define::PickMode)pval._int();
        return true;
    }
    else if(pname == "sel3d_pick_flag")
    {
        m_sel3d_pickFlag = pval._uint();
        return true;
    }
    // else if (pname == "sel3d_pick_obj_filter_cfg") {
    //	this->setPickObjFilterCfg(Define::SEL_TYPE_3D, pval._wstr());
    //	return true;
    // }
    else if(pname == "sel3d_snap_mode")
    {
        m_sel3d_snapMode = (Define::SnapMode)pval._int();
        return true;
    }
    else if(pname == "sel3d_snap_flag_pt")
    {
        m_sel3d_snapFlagPt = pval._uint();
        return true;
    }
    else if(pname == "sel3d_snap_flag_pl")
    {
        m_sel3d_snapFlagPl = pval._uint();
        return true;
    }
    else if(pname == "sel3d_snap_ref_plane")
    {
        m_sel3d_snapRefPlane = (Define::SnapRefPlane)pval._int();
        return true;
    }
    // else if (pname == "seldr_pick_flag") {
    //	m_seldr_pickFlag = pval._uint();
    //	return true;
    // }
    // else if (pname == "seldr_snap_mode") {
    //	m_seldr_snapMode = (Define::SnapMode)pval._int();
    //	return true;
    // }
    // else if (pname == "seldr_snap_flag_pt") {
    //	m_seldr_snapFlagPt = pval._uint();
    //	return true;
    // }
    // else if (pname == "seldr_snap_ref_plane") {
    //	m_seldr_snapRefPlane = (Define::SnapRefPlane)pval._int();
    //	return true;
    // }
    else if(pname == "view_rot_enable")
    {
        m_viewRotEnable = pval._bool();
        return true;
    }
    // else if (pname == "dr_ask_saving_unchecked") {
    //	m_dr_ask_saving_unchecked = pval._bool();
    //	return true;
    // }
    // else if (pname == "dr_dim_min_distance") {
    //	m_drDimMinDistance = pval._double();
    //	return true;
    // }
    // else if (pname == "dr_dim_min_edge_length") {
    //	m_drDimMinEdgeLength = pval._double();
    //	return true;
    // }
    // else if (pname == "dr_dim_exagger_limit") {
    //	m_drDimExaggerLimit = pval._double();
    //	return true;
    // }
    // else if (pname == "dr_dim_exagger_scaling") {
    //	m_drDimExaggerScaling = pval._int();
    //	return true;
    // }
    // else if (pname == "dr_dim_tag_unit") {
    //	m_drDimTagUnit = (Define::Unit)pval._int();
    //	return true;
    // }
    // else if (pname == "dr_dim_tag_format") {
    //	m_drDimTagFormat = pval._wstr();
    //	return true;
    // }
    // else if (pname == "dr_dim_tag_precision") {
    //	m_drDimTagPrecision = pval._int();
    //	return true;
    // }
    // else if (pname == "dr_dim_ext_line") {
    //	m_drDimExtLine = pval._double();
    //	return true;
    // }
    // else if (pname == "dr_chamfer_line_type") {
    //	m_drChamferLineType = (Define::DRLineType)pval._int();
    //	return true;
    // }
    // else if (pname == "dr_chamfer_line_color") {
    //	m_drChamferLineColor = pval._uint();
    //	return true;
    // }
    // else if (pname == "no_sep_char") {
    //	m_noSepChar = pval._wstr();
    //	return true;
    // }
    // else if (pname == "no_rfbar_sep_char") {
    //	m_noRfbarSepChar = pval._wstr();
    //	return true;
    // }
    // else if (pname == "no_part_no_type") {
    //	m_noPartNoType = pval._int();
    //	return true;
    // }
    // else if (pname == "no_part_padding_length") {
    //	m_no_part_padding_length = pval._int();
    //	return true;
    // }
    // else if (pname == "no_ass_padding_length") {
    //	m_no_ass_padding_length = pval._int();
    //	return true;
    // }
    // else if (pname == "no_rfbar_padding_length") {
    //	m_no_rfbar_padding_length = pval._int();
    //	return true;
    // }
    // else if (pname == "ori_north_angle") {
    //	m_oriNorthAngle = pval._double();
    //	return true;
    // }
    // else if (pname == "ori_beam_skew_limit") {
    //	m_oriBeamSkewLimit = pval._double();
    //	return true;
    // }
    // else if (pname == "ori_column_skew_limit") {
    //	m_oriColumnSkewLimit = pval._double();
    //	return true;
    // }
    // else if (pname == "ori_ass_beam_view_dir") {
    //	m_oriAssBeamViewdir = pval._int();
    //	return true;
    // }
    // else if (pname == "ori_ass_column_view_dir") {
    //	m_oriAssCloumnViewDir = pval._int();
    //	return true;
    // }
    // else if (pname == "ori_beam_mark_location") {
    //	m_oriBeamMarkLocation = pval._int();
    //	return true;
    // }
    // else if (pname == "ori_ga_column_mark_center") {
    //	m_oriGAColumnMarkCenter = pval._bool();
    //	return true;
    // }
    // else if (pname == "clash_bolt_d_factor") {
    //	m_clash_bolt_d_factor = pval._double();
    //	return true;
    // }
    // else if (pname == "clash_bolt_ext_factor_head") {
    //	m_clash_bolt_ext_factor_head = pval._double();
    //	return true;
    // }
    // else if (pname == "clash_bolt_ext_factor_tail") {
    //	m_clash_bolt_ext_factor_tail = pval._double();
    //	return true;
    // }
    // else if (pname == "no_def_welded_to_main") {
    //	m_no_def_welded_to_main = pval._wstr();
    //	return true;
    // }
    // else if (pname == "no_def_welded_to_subs") {
    //	m_no_def_welded_to_subs = pval._wstr();
    //	return true;
    // }
    // else if (pname == "no_def_loose_parts") {
    //	m_no_def_loose_parts = pval._wstr();
    //	return true;
    // }
    // else if (pname == "no_def_assembly_loose_parts") {
    //	m_no_def_assembly_loose_parts = pval._wstr();
    //	return true;
    // }
    // else if (pname == "bolt_edge_dist_factor") {
    //	m_bolt_edge_dist_factor = pval._double();
    //	return true;
    // }
    // else if (pname == "bolt_edge_dist_target") {
    //	m_bolt_edge_dist_target = (Define::BoltEdgeDistTarget)pval._int();
    //	return true;
    // }
    // else if (pname == "bolt_def_standard") {
    //	m_bolt_def_standard = pval._wstr();
    //	return true;
    // }
    // else if (pname == "bolt_def_size") {
    //	m_bolt_def_size = pval._double();
    //	return true;
    // }
    // else if (pname == "export_nc_dir") {
    //	m_export_nc_dir = pval._wstr();
    //	return true;
    // }
    // else if (pname == "plot_cad_dir") {
    //	m_plot_cad_dir = pval._wstr();
    //	return true;
    // }
    // else if (pname == "plot_pdf_dir") {
    //	m_plot_pdf_dir = pval._wstr();
    //	return true;
    // }
    // else if (pname == "unit_model_dist") {
    //	m_unit_model_dist = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_angl") {
    //	m_unit_model_angl = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_spconst") {
    //	m_unit_model_spconst = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_rot_spconst") {
    //	m_unit_model_rot_spconst = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_force") {
    //	m_unit_model_force = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_distributed_load") {
    //	m_unit_model_distributed_load = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_surface_load") {
    //	m_unit_model_surface_load = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_moment") {
    //	m_unit_model_moment = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_distributed_moment") {
    //	m_unit_model_distributed_moment = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_temperature") {
    //	m_unit_model_temperature = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_model_deform") {
    //	m_unit_model_deform = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_sect_dim") {
    //	m_unit_prf_sect_dim = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_angl") {
    //	m_unit_prf_angl = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_area") {
    //	m_unit_prf_area = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_sect_modulus") {
    //	m_unit_prf_sect_modulus = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_moment_inertia") {
    //	m_unit_prf_moment_inertia = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_radius_inertia") {
    //	m_unit_prf_radius_inertia = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_torsion_const") {
    //	m_unit_prf_torsion_const = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_warping_const") {
    //	m_unit_prf_warping_const = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_prf_cover_area") {
    //	m_unit_prf_cover_area = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_strength") {
    //	m_unit_mtrl_strength = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_modulus") {
    //	m_unit_mtrl_modulus = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_density") {
    //	m_unit_mtrl_density = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_weight") {
    //	m_unit_mtrl_weight = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_strain") {
    //	m_unit_mtrl_strain = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_thermal_coeff") {
    //	m_unit_mtrl_thermal_coeff = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_ratio") {
    //	m_unit_mtrl_ratio = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    // else if (pname == "unit_mtrl_volume") {
    //	m_unit_mtrl_volume = SU::w2s(pval._wstr());
    //	m_props[pname] = pval._wstr();
    // }
    else if(pname == "enob_model_dist")
    {
        m_enob_model_dist = pval._int();
        m_props[pname] = pval._int();
    }
    // else if (pname == "enob_model_angl") {
    //	m_enob_model_angl = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_spconst") {
    //	m_enob_model_spconst = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_rot_spconst") {
    //	m_enob_model_rot_spconst = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_factor") {
    //	m_enob_model_factor = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_force") {
    //	m_enob_model_force = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_distributed_load") {
    //	m_enob_model_distributed_load = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_surface_load") {
    //	m_enob_model_surface_load = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_moment") {
    //	m_enob_model_moment = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_distributed_moment") {
    //	m_enob_model_distributed_moment = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_temperature") {
    //	m_enob_model_temperature = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_model_deform") {
    //	m_enob_model_deform = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_sect_dim") {
    //	m_enob_prf_sect_dim = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_angl") {
    //	m_enob_prf_angl = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_area") {
    //	m_enob_prf_area = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_sect_modulus") {
    //	m_enob_prf_sect_modulus = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_moment_inertia") {
    //	m_enob_prf_moment_inertia = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_radius_inertia") {
    //	m_enob_prf_radius_inertia = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_torsion_const") {
    //	m_enob_prf_torsion_const = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_warping_const") {
    //	m_enob_prf_warping_const = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_prf_cover_area") {
    //	m_enob_prf_cover_area = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_strength") {
    //	m_enob_mtrl_strength = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_modulus") {
    //	m_enob_mtrl_modulus = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_density") {
    //	m_enob_mtrl_density = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_weight") {
    //	m_enob_mtrl_weight = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_strain") {
    //	m_enob_mtrl_strain = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_thermal_coeff") {
    //	m_enob_mtrl_thermal_coeff = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_ratio") {
    //	m_enob_mtrl_ratio = pval._int();
    //	m_props[pname] = pval._int();
    // }
    // else if (pname == "enob_mtrl_volume") {
    //	m_enob_mtrl_volume = pval._int();
    //	m_props[pname] = pval._int();
    // }
    else if(pname == "part_defs_mtrl")
    {
        if(m_props.has("part_defs_mtrl"))
            m_props.getDObject("part_defs_mtrl")->copyFrom(pval._dobj());
        else
            m_props["part_defs_mtrl"] = pval._dobj();
    }

    return false;

    return true;
}

}// namespace sketch
