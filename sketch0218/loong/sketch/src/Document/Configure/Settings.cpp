#include "sketch/Document/Configure/Settings.h"

#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Render/Device.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"

#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{

	Settings::Settings(void) :
		m_dirty(false),
		m_viewDebugInfo(false),
		m_geoCache(true),
		m_console(false),
		m_geoTriVis(false),
		m_geoVIdxVis(false),
		m_geoSegVis(false),
		m_reloadCompPluginBeforeRefresh(false),
		m_auxQuadNormalVis(false),
		m_sysCfgPath(L"dark"),
		m_hoverHighlight(true),
		m_snapSymbolVis(true),
		m_snapRangeFactor(1.0),
		m_bPreloadSdfTexts(true),
		m_autoSaveInterval(5),
		m_displayRatio(Define::DISP_RATIO_AUTO),
		m_vsync(false),
		m_viewAA(true),
		m_drAA(false),
		m_cutColor(true)
	{
		m_props["model$keyin_global_prefix"] = L"!";
		m_props["model$keyin_absolute_prefix"] = L"$";
		m_props["model$keyin_relative_prefix"] = L"@";
	}

	Settings::~Settings(void)
	{
		this->release();
	}

bool Settings::init() {
	this->_loadSysConfigs();
	this->_loadUserConfigs();
	return true;
}

void Settings::release(void)
{
	m_dirty = false;
	m_props.clear();

	m_viewDebugInfo = false;
	m_geoCache = true;
	m_console = false;
	m_geoTriVis = false;
	m_geoVIdxVis = false;
	m_geoSegVis = false;
	m_reloadCompPluginBeforeRefresh = false;
	m_auxQuadNormalVis = false;
	m_hoverHighlight = true;
	m_snapSymbolVis = true;
	m_sysCfgPath = L"";
	m_cutColor = true;
	m_props["model$keyin_global_prefix"] = L"!";
	m_props["model$keyin_absolute_prefix"] = L"$";
	m_props["model$keyin_relative_prefix"] = L"@";
}

bool Settings::onDocPrepare(const Engine::Path& projPath) { return false; }
bool Settings::onDocNew(const Engine::Path& projPath) { return false; }
bool Settings::onDocLoad(const Engine::Path& projPath) { return false; }
bool Settings::onDocLoaded(const Engine::Path& projPath) { return false; }
bool Settings::onDocClose() { return false; }
bool Settings::onAppClose() { return false; }
void Settings::_updateProps()
{
}
bool Settings::_parseProp(const std::string& pname, const Engine::DValue& pval)
{
	if (pname == "view_debug_info") {
		m_viewDebugInfo = pval._bool();
		return true;
	}
	else if (pname == "geo_cache") {
		m_geoCache = pval._bool();
		return true;
	}
	else if (pname == "console") {
		m_console = pval._bool();
		return true;
	}
	else if (pname == "geo_tri_vis") {
		m_geoTriVis = pval._bool();
		return true;
	}
	else if (pname == "geo_vidx_vis") {
		m_geoVIdxVis = pval._bool();
		return true;
	}
	else if (pname == "geo_seg_vis") {
		m_geoSegVis = pval._bool();
		return true;
	}
	else if (pname == "reload_comp_plugin_before_refresh") {
		m_reloadCompPluginBeforeRefresh = pval._bool();
		return true;
	}
	else if (pname == "aux_quad_normal_vis") {
		m_auxQuadNormalVis = pval._bool();
		return true;
	}
	else if (pname == "hover_highlight") {
		m_hoverHighlight = pval._bool();
		return true;
	}
	else if (pname == "snap_symbol_vis") {
		m_snapSymbolVis = pval._bool();
		return true;
	}
	else if (pname == "snap_range_factor") {
		m_snapRangeFactor = STMIN(STMAX(pval._double(), 1.0), 9999.0);
		return true;
	}
	else if (pname == "preload_sdf_texts") {
		m_bPreloadSdfTexts = pval._bool();
		return true;
	}
	else if (pname == "autosave_interval") {
		m_autoSaveInterval = pval._int();
		return true;
	}
	else if (pname == "sys_cfg_path") {
		m_sysCfgPath = pval._wstr();
		m_sysCfgFullPath = SysUtil::exeFilePath() / "config" / m_sysCfgPath;
		return true;
	}
	else if (pname == "active_pal") {
		m_activePal = pval._wstr();
		return true;
	}
	else if (pname == "disp_ratio") {
		m_displayRatio = (Define::DisplayRatio)pval._int();
		return true;
	}
	else if (pname == "vsync") {
		m_vsync = pval._bool();
		Engine::STE::device()->enableVSync(m_vsync);
		return true;
	}
	else if (pname == "view_aa") {
		m_viewAA = pval._bool();
		return true;
	}
	else if (pname == "dr_aa") {
		m_drAA = pval._bool();
		return true;
	}
	else if (pname == "cut_color") {
		m_cutColor = pval._bool();
		return true;
	}
	else if (pname == "model$keyin_global_prefix") {
		m_props["model$keyin_global_prefix"] = pval._wstr();
		return true;
	}
	else if (pname == "model$keyin_absolute_prefix") {
		m_props["model$keyin_absolute_prefix"] = pval._wstr();
		return true;
	}
	else if (pname == "model$keyin_relative_prefix") {
		m_props["model$keyin_relative_prefix"] = pval._wstr();
		return true;
	}

	return false;
}

void Settings::_loadUserConfigs()
{
	auto file = STCore::inst->appDataPath() / "config/settings.cfg";

	FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
	if (fp == nullptr)
		return;

	if (!FileUtil::parseMagic(fp, "STST")) {
		fclose(fp);
		return;
	}

	m_props.fromFile(fp);
	fclose(fp);

	for (auto& it : m_props) {
		this->_parseProp(it.first, it.second);
	}
}

void Settings::dirty()
{
	m_dirty = true;
}

void Settings::_saveUserConfigs()
{
	this->_updateProps();

	auto file = STCore::inst->appDataPath() / "config/settings.cfg";

	FILE* fp = _wfopen(file.wstr().c_str(), L"wb");
	if (fp == nullptr)
		return;

	DObject props = m_props;

	//下列属性不保存，仅在初始配置中设置
	props.erase("view_debug_info");
	props.erase("geo_cache");
	props.erase("geo_tri_vis");
	props.erase("geo_vidx_vis");
	props.erase("geo_seg_vis");
	props.erase("reload_comp_plugin_before_refresh");
	props.erase("console");
	props.erase("sys_cfg_path");
	props.erase("preload_sdf_texts");

	fwrite("STST", 4, 1, fp);
	props.toFile(fp, "");
	fclose(fp);
}

void Settings::_loadSysConfigs()
{
	auto file = SysUtil::exeFilePath() / "config\\settings.json";

	std::string text = FileUtil::loadTextFile(file);
	if (text.empty()) {
		Log::log("", "[Error] Failed to load config file: %s\n", file.str().c_str());
		return;
	}

	rapidjson::Document doc;
	doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

	if (doc.HasParseError()) {
		Log::log("", "[Error] Config parse error: %s\n", file.str().c_str());
		Log::log("", "[Error] Config parse error: [%s] %s \n",
			std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
			rapidjson::GetParseError_En(doc.GetParseError()));
		return;
	}

	if (!doc.IsObject()) {
		Log::log("", "[Error] Config parse error: %s\n", file.str().c_str());
		return;
	}

	m_props.fromJson(&doc, "utf8");

	for (auto& it : m_props) {
		this->_parseProp(it.first, it.second);
	}

	if (m_console)
		Log::initConsole();
}

void Settings::modify(const Engine::DObject& props)
{
	for (auto& it : props)
		this->_parseProp(it.first, it.second);
}

}// namespace sketch
