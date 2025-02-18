#include "sketch/Document/Configure/Layout/LayoutManager.h"
#include "sketch/STCore.h"
#include <Windows.h>
#include <stack>

#include "Engine/Util/StrUtil.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Util/TeklaLis.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"

using namespace Engine;

namespace sketch
{
	LayoutManager::LayoutManager(void):
		m_rawMiniBars(new DObject())
	{
		
	}

	LayoutManager::~LayoutManager(void)
	{
		this->release();
        DObject::free(m_rawMiniBars);
        m_rawMiniBars = nullptr;
	}

	bool LayoutManager::init()
	{
		this->release();

		this->_loadSysBars();
        this->_loadSysMiniBars();
		this->_loadSysLayouts();

		this->loadUserBars();
		this->loadUserLayouts();

		return true;
	}

	void LayoutManager::release(void)
	{
		for (auto& it : m_layouts)
			DObject::free(it.second);
		m_layouts.clear();

		for (auto& it : m_bars)
			DObject::free(it.second);
		m_bars.clear();

        _clearMiniBars();
	}

	bool LayoutManager::loadUserLayouts()
	{
		auto cfgPath = STCore::inst->appDataPath() / "config/ui/layouts.lgst";

		FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
		if (fp == nullptr)
			return false;

		if (!FileUtil::parseMagic(fp, "STLAYOUT")) {
			fclose(fp);
			return false;
		}

		DObject cfg;
		cfg.fromFile(fp);
		fclose(fp);

		if (!cfg.has("version"))
			return true;

		if (cfg["version"]._int() < 1)
			return true;

		cfg.remove("version");

		for (auto& it : cfg) {
			/*DObject& itcfg = *it.second._dobj();
			Log::log("", "loadUserLayouts: %s \n", it.first.c_str());

			for (auto& ot : itcfg) {
				Log::log("", "      @@@@ %s \n", ot.first.c_str());
			}
			
			if (itcfg.has("top_bar"))
			{
				DObject* topcfg = itcfg.getDObject("top_bar");
				for (auto& rowit : *topcfg) {
					Log::log("", "      @@@@ %s \n", rowit.first.c_str());
					for (auto& barit : rowit.second._wstrList()) {
						Log::log("", "            @@@@ %s \n", StrUtil::w2s(barit).c_str());
					}
				}
			}

			if (itcfg.has("bottom_bar"))
			{
				DObject* bcfg = itcfg.getDObject("bottom_bar");
				for (auto& rowit : *bcfg) {
					Log::log("", "      @@@@ %s \n", rowit.first.c_str());
					for (auto& barit : rowit.second._wstrList()) {
						Log::log("", "            @@@@ %s \n", StrUtil::w2s(barit).c_str());
					}
				}
			}

			if (itcfg.has("left_bar"))
			{
				DObject* lcfg = itcfg.getDObject("left_bar");
					for (auto& rowit : *lcfg) {
						Log::log("", "    @@@@ %s \n", rowit.first.c_str());
						for (auto& barit : rowit.second._wstrList()) {
							Log::log("", "            @@@@ %s \n", StrUtil::w2s(barit).c_str());
						}
					}
			}

			if (itcfg.has("right_bar"))
			{
				DObject* rcfg = itcfg.getDObject("right_bar");
				for (auto& rowit : *rcfg) {
					Log::log("", "    @@@@ %s \n", rowit.first.c_str());
					for (auto& barit : rowit.second._wstrList()) {
						Log::log("", "            @@@@ %s \n", StrUtil::w2s(barit).c_str());
					}
				}
			}

			if (itcfg.has("floaty_bar"))
			{
				DObject* rcfg = itcfg.getDObject("floaty_bar");
				for (auto& rowit : *rcfg) {
					Log::log("", "    @@@@ %s \n", rowit.first.c_str());
					for (auto& barit : rowit.second._wstrList()) {
						Log::log("", "            @@@@ %s \n", StrUtil::w2s(barit).c_str());
					}
				}
			}*/

			this->createLayout(*it.second._dobj());
		}

		return true;
	}

	bool LayoutManager::saveUserLayouts()
	{
		//Log::log("", "    %%%% saveUserLayouts\n");

		auto basePath = STCore::inst->appDataPath() / "config/ui";
		if (!FileUtil::hasDir(basePath))
			FileUtil::createDir(basePath);

		DObject cfg;
		cfg["version"] = 1;
		for (auto it : m_layouts) {
			auto pProps = it.second;
			cfg[StrUtil::w2s(it.first)] = pProps;

			//if ((*pProps).has("top_bar"))
			//{
			//	DObject* rcfg = (*pProps).getDObject("top_bar");
			//	for (auto& rowit : *rcfg) {
			//		Log::log("", "    %%%% saveUserLayouts - %s \n", rowit.first.c_str());
			//		for (auto& barit : rowit.second._wstrList()) {
			//			Log::log("", "            %%%% saveUserLayouts - %s \n", StrUtil::w2s(barit).c_str());
			//		}
			//	}
			//}
		}

		auto cfgPath = basePath / "layouts.lgst";
		if (!FU::save(cfgPath, "STLAYOUT", cfg, "", true)) {
			Log::log("", "[Error] Failed to save layout user config: %s \n", cfgPath.str().c_str());
			return false;
		}

		return true;
	}

	bool LayoutManager::loadUserBars()
	{
		auto cfgPath = STCore::inst->appDataPath() / "config/ui/bars.lgst";

		FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
		if (fp == nullptr)
			return false;

		if (!FileUtil::parseMagic(fp, "STBARS")) {
			fclose(fp);
			return false;
		}

		DObject cfg;
		bool bSucc = cfg.fromFile(fp);
		fclose(fp);

		if (!bSucc || !cfg.has("version"))
			return true;

		if (cfg["version"]._int() < 1)
			return true;
		
		cfg.remove("version");

		for (auto& it : cfg) {
			/*DObject& itcfg = *it.second._dobj();
			Log::log("", "loadUserBars: %s \n", it.first.c_str());

			for (auto& ot : itcfg) {
				Log::log("", "    @@@@ %s \n", ot.first.c_str());
				if (ot.second.refer_type() == DValue::TYPE_WSTR_ARRAY)
					for (auto& at : (ot.second._wstrList())) {
						Log::log("", "        @@@@ %s \n", StrUtil::w2s(at).c_str());
					}
				if (ot.second.refer_type() == DValue::TYPE_BOOL)
					Log::log("", "        @@@@ %d \n", ot.second._bool());
				if (ot.second.refer_type() == DValue::TYPE_WSTR)
					Log::log("", "        @@@@ %s \n", StrUtil::w2s(ot.second._wstr()).c_str());
				if (ot.second.refer_type() == DValue::TYPE_INT)
					Log::log("", "        @@@@ %d \n", ot.second._int());
				if (ot.second.refer_type() == DValue::TYPE_LONG)
					Log::log("", "        @@@@ %d \n", ot.second._long());
			}
			Log::log("", "      @@ %d \n", itcfg["x"]._int());
			Log::log("", "      @@ %d \n", itcfg["y"]._int());

			std::list<wstring>& topcfg = itcfg["func_units"]._wstrList();
			for (auto& ot : topcfg) {
				Log::log("", "        @@@@ %s \n", StrUtil::w2s(ot).c_str());
			}*/

			this->createBar(*it.second._dobj());
		}

		return true;
	}

	bool LayoutManager::saveUserBars()
	{
		//Log::log("", "    &&&& saveUserBars \n");
		auto basePath = STCore::inst->appDataPath() / "config/ui";
		if (!FileUtil::hasDir(basePath))
			FileUtil::createDir(basePath);

		DObject cfg;
		cfg["version"] = 1;
		for (auto it : m_bars) {
			auto pProps = it.second;
			cfg[StrUtil::w2s(it.first)] = pProps;
		}

		/*for (auto& it : m_bars) {
			Log::log("", "    &&&& %s \n", StrUtil::w2s(it.first).c_str());
			std::list<wstring>& topcfg = (*it.second)["func_units"]._wstrList();
			for (auto& ot : topcfg) {
				Log::log("", "        &&&& %s \n", StrUtil::w2s(ot).c_str());
			}
		}*/

		auto cfgPath = basePath / "bars.lgst";
		if (!FU::save(cfgPath, "STBARS", cfg, "", true)) {
			Log::log("", "[Error] Failed to save toolbar user config: %s \n", cfgPath.str().c_str());
			return false;
		}

		return true;
	}

    bool LayoutManager::loadUserMiniBars()
    {
		auto cfgPath = STCore::inst->appDataPath() / "config/ui/mini_bars.lgst";
		FILE *fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
		if (fp == nullptr) {
			return false;
		}
		if (!FileUtil::parseMagic(fp, "STMINIBARS")) {
			fclose(fp);
			return false;
		}
		
		DObject rootConfig;
		rootConfig.fromFile(fp);
		fclose(fp);

		this->_loadMiniBars(rootConfig);
        return true;
    }

	bool LayoutManager::saveUserMiniBars()
	{
		return false;
	}

	bool LayoutManager::onDocPrepare(const Engine::Path& projPath) {
		return true;
	}

	bool LayoutManager::onDocNew(const Engine::Path& projPath) {
		return true;
	}

	bool LayoutManager::onDocLoad(const Engine::Path& projPath) {
		return true;
	}

	bool LayoutManager::onDocLoaded(const Engine::Path& projPath) {
		return true;
	}

	bool LayoutManager::onDocWillClose(void) {
		return true;
	}

	bool LayoutManager::onDocClose(void) {
		return true;
	}

	bool LayoutManager::onAppClose()
	{
		//FU_File_Exit中已经调用过save了
		//saveUserLayouts();
		//saveUserBars();
		return true;
	}

	void LayoutManager::_loadSysLayouts()
	{
		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "ui";

		if (FileUtil::hasFile(cfgPath / "layouts.lgst")) { //binary
			auto file = cfgPath / "layouts.lgst";

			FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
			if (fp == nullptr)
				return;

			if (!FileUtil::parseMagic(fp, "STLAYOUT")) {
				fclose(fp);
				return;
			}

			DObject cfg;
			cfg.fromFile(fp);
			fclose(fp);

			for (auto& it : cfg) {
				this->createLayout(*it.second._dobj());
			}
		}
		else if (FileUtil::hasFile(cfgPath / "layouts.json")) { //json
			auto file = cfgPath / "layouts.json";
			std::string text = FileUtil::loadTextFile(file);

			rapidjson::Document doc;
			doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

			if (doc.HasParseError()) {
				Log::log("", "[Error] Layout sys config parse error: %s\n", file.str().c_str());
				Log::log("", "[Error] Layout sys config parse error: [%s] %s \n",
					std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
					rapidjson::GetParseError_En(doc.GetParseError()));
				return;
			}

			if (!doc.IsObject()) {
				return;
			}

			for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
				std::wstring name = StrUtil::s2w(StrUtil::trim(it->name.GetString()));

				DObject props;
				props.fromJson(&it->value, "utf8");
				props["name"] = name;
				this->createLayout(props);
			}
		}
	}

    void LayoutManager::_loadSysMiniBars()
    {
        DObject rootConfig;
		auto cfgDir = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "ui";

        if (FileUtil::hasFile(cfgDir / "mini_bars.lgst")) {
            auto cfgPath = cfgDir / "mini_bars.lgst";
            FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
            if (fp == nullptr) {
                return;
            }
            if (!FileUtil::parseMagic(fp, "STMINIBARS")) {
                fclose(fp);
                return;
            }
            
            rootConfig.fromFile(fp);
            fclose(fp);
        }
		else if(FileUtil::hasFile(cfgDir / "mini_bars.json")) {
			auto cfgPath = cfgDir / "mini_bars.json";
			std::string text = FileUtil::loadTextFile(cfgPath);
			rapidjson::Document doc;
			doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());
			if (doc.HasParseError()) {
				Log::log("", "[Error] Bar sys config parse error: %s\n", cfgPath.str().c_str());
				Log::log("", "[Error] Bar sys config parse error: [%s] %s \n",
					std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
					rapidjson::GetParseError_En(doc.GetParseError()));
				return;
			}
			if (!doc.IsObject()) {
				return;
			}

			rootConfig.fromJson(&doc, "utf8");
        }

		this->_loadMiniBars(rootConfig);
    }

	void LayoutManager::_loadSysBars()
	{
		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "ui";

		if (FileUtil::hasFile(cfgPath / "bars.lgst")) { //binary
			auto file = cfgPath / "bars.lgst";

			FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
			if (fp == nullptr)
				return;

			if (!FileUtil::parseMagic(fp, "STBARS")) {
				fclose(fp);
				return;
			}

			DObject cfg;
			cfg.fromFile(fp);
			fclose(fp);

			for (auto& it : cfg) {
				this->createBar(*it.second._dobj());
			}
		}
		else if (FileUtil::hasFile(cfgPath / "bars.json")) { //json
			auto file = cfgPath / "bars.json";
			std::string text = FileUtil::loadTextFile(file);

			rapidjson::Document doc;
			doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

			if (doc.HasParseError()) {
				Log::log("", "[Error] Bar sys config parse error: %s\n", file.str().c_str());
				Log::log("", "[Error] Bar sys config parse error: [%s] %s \n",
					std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
					rapidjson::GetParseError_En(doc.GetParseError()));
				return;
			}

			if (!doc.IsObject()) {
				return;
			}

			for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
				std::wstring name = StrUtil::s2w(StrUtil::trim(it->name.GetString()));

				DObject props;
				props.fromJson(&it->value, "utf8");
				props["name"] = name;
				this->createBar(props);
			}
		}
	}

	void LayoutManager::_loadMiniBars(const Engine::DObject& rootConfig)
	{
        m_rawMiniBars->copyFrom(rootConfig);
        if (rootConfig.has("model_objects")) {
            DObject* modelConfigs = rootConfig.getDObject("model_objects");
            for (auto itt : (*modelConfigs)) {
                DObject* cfg = DObject::alloc();
                cfg->copyFrom(modelConfigs->getDObject(itt.first));
                m_miniBars[StrUtil::s2w(itt.first)] = cfg;
            }
        }
	}

    void LayoutManager::_clearMiniBars()
    {
        for (auto& itt : m_miniBars) {
            DObject::free(itt.second);
        }
        m_miniBars.clear();
        m_rawMiniBars->clear();
    }

	const Engine::DObject* LayoutManager::createLayout(const Engine::DObject& props)
	{
		if (!props.has("name")) {
			Log::log("", "[Error] Layout miss parameter: name\n");
			return nullptr;
		}

		auto& name = props["name"]._wstr();
		if (m_layouts.find(name) != m_layouts.end()) {
			DObject::free(m_layouts[name]);
		}

		DObject* pCfg = DObject::alloc(props);
		m_layouts[name] = pCfg;
		return pCfg;
	}

	const Engine::DObject* LayoutManager::getLayout(const std::wstring& name) const
	{
		auto it = m_layouts.find(name);
		if (it != m_layouts.end())
			return it->second;

		return nullptr;
	}

	void LayoutManager::removeLayout(const std::wstring& name)
	{
		auto it = m_layouts.find(name);
		if (it == m_layouts.end())
			return;

		delete it->second;
		m_layouts.erase(it);
	}

	const Engine::DObject* LayoutManager::createBar(const Engine::DObject& props)
	{
		if (!props.has("name")) {
			Log::log("", "[Error] Bar miss parameter: name\n");
			return nullptr;
		}

		auto& name = props["name"]._wstr();
		if (m_bars.find(name) != m_bars.end()) {
			DObject::free(m_bars[name]);
		}

		DObject* pCfg = DObject::alloc(props);

		if (!(*pCfg).has("provisioned"))
		{
			(*pCfg)["provisioned"] = true;
		}

		if (!(*pCfg).has("tip"))
		{
			(*pCfg)["tip"] = name;
		}

		m_bars[name] = pCfg;

		return pCfg;
	}

	const Engine::DObject* LayoutManager::getBar(const std::wstring& name) const
	{
		auto it = m_bars.find(name);
		if (it != m_bars.end())
			return it->second;

		return nullptr;
	}

	const bool LayoutManager::provisionBar(const std::wstring& name, bool builtIn, const std::wstring& tip, bool provisioned, const std::wstring& location, int x, int y, const std::vector<std::wstring>& funcUnits)
	{
		//Log::log("", "  >>>> provisionBar - new bar\n");
		DObject* pBarCfg;

		auto barIt = m_bars.find(name);
		if (barIt == m_bars.end()) {
			//Log::log("", "  >>>> provisionBar - new bar : %s\n", StrUtil::w2s((*paras)["name"]._wstr()).c_str());
			//Log::log("", "  >>>> new bar : 00\n");
			pBarCfg = DObject::alloc({
				{"name", name},
				{"builtin", builtIn},
				{"tip", tip},
				{"provisioned", provisioned},
				{"location", location},
				{"x", x},
				{"y", y},
				{"func_units", funcUnits},
			});
			//Log::log("", "  >>>> new bar : 2\n");
			m_bars[name] = pBarCfg;
		}
		else {
			/*Log::log("", "  >>>> found existed bar : %s\n", StrUtil::w2s((*paras)["name"]._wstr()).c_str());
			Log::log("", "  >>>> found existed bar : provisioned %d\n", StrUtil::w2s((*paras)["provisioned"]._bool()));
			Log::log("", "  >>>> found existed bar : builtin %d\n", StrUtil::w2s((*paras)["builtin"]._bool()));
			Log::log("", "  >>>> found existed bar : tip %s\n", StrUtil::w2s((*paras)["tip"]._wstr()).c_str());
			Log::log("", "  >>>> found existed bar : x %d\n", (*paras)["x"]._int());
			Log::log("", "  >>>> found existed bar : y %d\n", (*paras)["y"]._int());*/
			pBarCfg = barIt->second;
			pBarCfg->set("provisioned", provisioned);
			pBarCfg->set("builtin", builtIn);
			pBarCfg->set("tip", tip);
			pBarCfg->set("location", location);
			pBarCfg->set("x", x);
			pBarCfg->set("y", y);
			pBarCfg->set("func_units", funcUnits);
		}

		return true;
	}

	const bool LayoutManager::provisionLayout(const std::wstring& layoutName, const std::wstring& tip, const Engine::DObject* subNodes)
	{
		/*Log::log("", "#### provisionLayout - % s: % s - provisioned \n", StrUtil::w2s(layoutName).c_str(), StrUtil::w2s(tip).c_str());

		for (auto& it : *subNodes) {
			Log::log("", "    #### %s \n", it.first.c_str());
			for (auto& rowit : *(it.second._dobj())) {
				Log::log("", "        #### %s \n", rowit.first.c_str());
				for (auto& barit : rowit.second._wstrList()) {
					Log::log("", "            #### %s \n", StrUtil::w2s(barit).c_str());
				}
			}
		}*/
		DObject* pLayoutCfg;

		auto layoutIt = m_layouts.find(layoutName);
		if (layoutIt == m_layouts.end())
		{
			//Log::log("", "  #### new layout \n");
			pLayoutCfg = DObject::alloc();
			(*pLayoutCfg)["name"] = layoutName;

			this->createLayout(*pLayoutCfg);
		}
		else
		{
			//Log::log("", "  #### found existed layout \n");
			pLayoutCfg = layoutIt->second;
		}

		if (pLayoutCfg->has("top_bar")) pLayoutCfg->remove("top_bar");
		if (pLayoutCfg->has("bottom_bar")) pLayoutCfg->remove("bottom_bar");
		if (pLayoutCfg->has("left_bar")) pLayoutCfg->remove("left_bar");
		if (pLayoutCfg->has("right_bar")) pLayoutCfg->remove("right_bar");
		if (pLayoutCfg->has("floaty_bar")) pLayoutCfg->remove("floaty_bar");

		if (subNodes->has("top_bar")) pLayoutCfg->set("top_bar", (*subNodes)["top_bar"]);
		if (subNodes->has("bottom_bar")) pLayoutCfg->set("bottom_bar", (*subNodes)["bottom_bar"]); 
		if (subNodes->has("left_bar")) pLayoutCfg->set("left_bar", (*subNodes)["left_bar"]);
		if (subNodes->has("right_bar")) pLayoutCfg->set("right_bar", (*subNodes)["right_bar"]);
		if (subNodes->has("floaty_bar")) pLayoutCfg->set("floaty_bar", (*subNodes)["floaty_bar"]);

		(*pLayoutCfg)["tip"] = tip;

		return true;
	}

	void LayoutManager::hideBar(const std::wstring& name, const bool isHidden)
	{
		auto it = m_bars.find(name);
		if (it == m_bars.end())
			return;
		DObject* pCfg = it->second;
		(*pCfg)["provisioned"] = isHidden;
	}

	void LayoutManager::removeBar(const std::wstring& name)
	{
		auto it = m_bars.find(name);
		if (it == m_bars.end())
			return;

		delete it->second;
		m_bars.erase(it);
	}

    const Engine::DObject* LayoutManager::getMiniBar(const std::wstring& name) const
    {
        auto itt = m_miniBars.find(name);
        if (itt != m_miniBars.end()) {
            return itt->second;
        }
        return nullptr;
    }

    const Engine::DObject* LayoutManager::getMiniBars() const
    {
        return m_rawMiniBars;
    }

    void LayoutManager::updateMiniBars(const Engine::DObject* miniBars)
    {
        if (!miniBars) {
            return;
        }
        this->_clearMiniBars();
        this->_loadMiniBars(*miniBars);
    }
}