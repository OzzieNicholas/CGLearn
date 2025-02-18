#include "sketch/Document/Configure/Pallete/PalleteManager.h"
#include "sketch/STCore.h"
#include <Windows.h>
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/Log.h"

#include "sketch/Document/Configure/Settings.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"

using namespace Engine;

namespace sketch
{
	PalleteManager::PalleteManager(void)
	{
		m_pActivePal = nullptr;
	}

	PalleteManager::~PalleteManager(void)
	{
		this->release();
	}

	bool PalleteManager::init()
	{
		Settings* pSets = STCore::inst->docMgr()->cfgMgr()->settings();

		this->release();
		this->_loadSysConfigs();
		this->loadUserConfigs();

		auto pActive = this->pallate(pSets->activePal());
		if (pActive)
		{
			this->setActivePallete(pSets->activePal());
		}
		else
		{
			if (auto pDef = this->defPallete())
			{
				this->setActivePallete(pDef->name());
			}
			else
			{
				auto pPal = this->newPallete(L"default", DObject());
				this->setActivePallete(pPal->name());
			}
		}

		return true;
	}

	void PalleteManager::release(void)
	{
		for (auto& it : m_palletes)
			delete it.second;
		m_palletes.clear();
		m_pActivePal = nullptr;
	}

	bool PalleteManager::loadSysPallete(const std::wstring& name, Engine::DObject& retProps)
	{
		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "pallete";

		auto file = cfgPath / Path(name + L".json");
		std::string text = FileUtil::loadTextFile(file);
		
		do {
			if (text.empty())
				break;

			rapidjson::Document doc;
			doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

			if (doc.HasParseError()) {
				Log::log("", "[Error] Pallete config parse error: %s\n", file.str().c_str());
				Log::log("", "[Error] Pallete config parse error: [%s] %s \n",
					std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
					rapidjson::GetParseError_En(doc.GetParseError()));
				break;
			}

			if (!doc.IsObject()) 
				break;

			DObject cfgObj;
			if (!cfgObj.fromJson(&doc, "utf8"))
				break;

			Pallete pal(name);
			pal.modify(cfgObj);
			retProps = pal.props();
			return true;

		} while (false);

		file = cfgPath / Path(name + L".stpal");
		FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
		if (fp == nullptr)
			return false;

		if (!FileUtil::parseMagic(fp, "STPAL")) {
			fclose(fp);
			return false;
		}

		DObject cfgObj;
		if (!cfgObj.fromFile(fp)) {
			fclose(fp);
			return false;
		}
		
		Pallete pal(name);
		pal.modify(cfgObj);
		retProps = pal.props();

		fclose(fp);
		return true;
	}

	void PalleteManager::_loadSysConfigs()
	{
		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "pallete";

		std::vector<Path> vFiles;
		FileUtil::parseFiles(vFiles, cfgPath, "json", false);
		for (auto file : vFiles) {//json configs
			std::string text = FileUtil::loadTextFile(file);
			if (text.empty())
				continue;

			rapidjson::Document doc;
			doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

			if (doc.HasParseError()) {
				Log::log("", "[Error] Pallete config parse error: %s\n", file.str().c_str());
				Log::log("", "[Error] Pallete config parse error: [%s] %s \n",
					std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
					rapidjson::GetParseError_En(doc.GetParseError()));
				return;
			}

			if (!doc.IsObject()) {
				return;
			}

			DObject cfgObj;
			if (!cfgObj.fromJson(&doc, "utf8"))
				continue;

			this->newPallete(file.fileMain().wstr(), cfgObj);
		}

		vFiles.clear();
		FileUtil::parseFiles(vFiles, cfgPath, "lgst", false);
		for (auto file : vFiles) {//binary configs
			FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
			if (fp == nullptr)
				return;

			if (!FileUtil::parseMagic(fp, "STPAL")) {
				fclose(fp);
				return;
			}

			DObject cfgObj;
			cfgObj.fromFile(fp);
			fclose(fp);

			this->newPallete(file.fileMain().wstr(), cfgObj);
		}
	}

	void PalleteManager::loadUserConfigs()
	{
		auto cfgPath = STCore::inst->appDataPath() / "config/pallete";

		std::vector<Path> vFiles;
		FileUtil::parseFiles(vFiles, cfgPath, "lgst", false);
		for (auto file : vFiles) {//binary configs
			FILE* fp = _wfopen(file.wstr().c_str(), L"rb");
			if (fp == nullptr)
				return;

			if (!FileUtil::parseMagic(fp, "STPAL")) {
				fclose(fp);
				return;
			}

			DObject cfgObj;
			cfgObj.fromFile(fp);
			fclose(fp);

			this->newPallete(file.fileMain().wstr(), cfgObj);
		}
	}

	bool PalleteManager::saveUserConfig(const std::wstring& name)
	{
		auto it = m_palletes.find(name);
		if (it == m_palletes.end())
			return false;

		Pallete* pPal = it->second;

		auto basePath = STCore::inst->appDataPath() / "config/pallete";
		if (!FileUtil::hasDir(basePath))
			FileUtil::createDir(basePath);

		auto filePath = basePath / Path(name + L".lgst");

		FILE* fp = _wfopen(filePath.wstr().c_str(), L"wb");
		if (fp == nullptr) {
			Log::log("", "[Error] Failed to save pallete config: %s \n", filePath.str().c_str());
			return false;
		}

		fwrite("STPAL", 5, 1, fp);//magic
		pPal->props().toFile(fp, "");
		fclose(fp);

		return true;
	}

	bool PalleteManager::deleteUserConfig(const std::wstring& name)
	{
		auto filePath = STCore::inst->appDataPath() / "config/pallete" / Path(name + L".lgst");

		if (!FileUtil::hasFile(filePath))
			return false;

		FileUtil::deleteFile(filePath);
		return true;
	}

	Pallete* PalleteManager::newPallete(const std::wstring& name, const Engine::DObject& props)
	{
		auto it = m_palletes.find(name);
		if (it != m_palletes.end()) {
			it->second->modify(props);
			return it->second;
		}

		Pallete* pPal = new Pallete(name);
		pPal->modify(props);
		m_palletes[name] = pPal;
		return pPal;
	}

	bool PalleteManager::removePallete(const std::wstring& name)
	{
		if (m_pActivePal && m_pActivePal->name() == name)
			m_pActivePal = this->defPallete();

		auto it = m_palletes.find(name);
		if (it == m_palletes.end())
			return false;

		delete it->second;
		m_palletes.erase(it);
		return true;
	}

	bool PalleteManager::modifyPallete(const std::wstring& name, const Engine::DObject& props)
	{
		auto it = m_palletes.find(name);
		if (it == m_palletes.end())
			return false;

		it->second->modify(props);
		return true;
	}
}