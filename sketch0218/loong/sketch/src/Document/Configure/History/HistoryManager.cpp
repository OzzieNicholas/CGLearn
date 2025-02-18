#include "sketch/Document/Configure/History/HistoryManager.h"

#include <Windows.h>

#include "Engine/Util/StrUtil.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/Path.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"
#include "sketch/STCore.h"
#include "sketch/Document/Document.h"

using namespace Engine;

namespace sketch
{
	HistoryManager::HistoryManager(void)
	{

	}

	HistoryManager::~HistoryManager(void)
	{
		this->release();
	}

	bool HistoryManager::init()
	{
		this->release();
		this->_loadGlobalConfigs();
		return true;
	}

	void HistoryManager::release(void)
	{
		for (auto& it : m_historyGlobal) {
			delete it.second;
		}
		m_historyGlobal.clear();

		for (auto& it : m_history) {
			delete it.second;
		}
		m_history.clear();
	}

	bool HistoryManager::onDocPrepare(const Engine::Path& projPath)
	{
		m_globalDirties.clear();
		m_documentDirties.clear();
		return true;
	}

	bool HistoryManager::onDocNew(const Engine::Path& projPath)
	{
		this->_loadDocumentConfigs(projPath);
		return true;
	}

	bool HistoryManager::onDocLoad(const Engine::Path& projPath)
	{
		this->_loadDocumentConfigs(projPath);
		return true;
	}

	bool HistoryManager::onDocLoaded(const Engine::Path& projPath)
	{
		return true;
	}

	bool HistoryManager::onDocClose(void)
	{
		this->_processDirties();

		for (auto& it : m_history) {
			delete it.second;
		}
		m_history.clear();
		
		return true;
	}

	bool HistoryManager::onAppClose(void)
	{
		return this->onDocClose();
	}

	void HistoryManager::_processDirties()
	{
		do {
			if (m_globalDirties.empty())
				break;

			auto basePath = STCore::inst->appDataPath() / "config/history";
			if (!FileUtil::hasDir(basePath))
				FileUtil::createDir(basePath);

			for (auto& it : m_globalDirties) {
				if (it.second == 1) { //modify
					auto ith = m_historyGlobal.find(it.first);
					if (ith == m_historyGlobal.end())
						continue;

					auto file = basePath / it.first + ".lgst";
					FILE* fp = fopen(file.str().c_str(), "wb");
					if (!fp)
						continue;

					fwrite("STHIS", 5, 1, fp);
					ith->second->toFile(fp, "");
					fclose(fp);
				}
				else if (it.second == 2) { //delete
					auto file = basePath / it.first + ".lgst";
					if (FU::hasFile(file))
						FU::deleteFile(file);
				}
			}

			m_globalDirties.clear();
		} while (false);
			
		
		do {
			if (m_documentDirties.empty())
				break;

			auto& projPath = STCore::inst->docMgr()->document()->path();
			if (projPath.empty())
				break;

			auto basePath = projPath / "config/history";
			if (!FileUtil::hasDir(basePath))
				FileUtil::createDir(basePath);

			for (auto& it : m_documentDirties) {
				if (it.second == 1) { //modify
					auto ith = m_history.find(it.first);
					if (ith == m_history.end())
						continue;

					auto file = basePath / it.first + ".lgst";
					FILE* fp = fopen(file.str().c_str(), "wb");
					if (!fp)
						continue;

					fwrite("STHIS", 5, 1, fp);
					ith->second->toFile(fp, "");
					fclose(fp);
				}
				else if (it.second == 2) { //delete
					auto file = basePath / it.first + ".lgst";
					if (FU::hasFile(file))
						FU::deleteFile(file);
				}
			}

			m_documentDirties.clear();
		} while (false);
	}

	bool HistoryManager::_loadGlobalConfigs()
	{
		auto cfgPath = STCore::inst->appDataPath() / "config/history";
		if (!FU::hasDir(cfgPath))
			return true;

		std::vector<Path> vFiles;
		FileUtil::parseFiles(vFiles, cfgPath, "lgst", false);

		for (auto& cfgPath : vFiles) {
			FU::wait(cfgPath);
			FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
			if (fp == nullptr)
				continue;

			if (!FileUtil::parseMagic(fp, "STHIS")) {
				fclose(fp);
				continue;
			}

			DObject* pCfg = DObject::alloc();
			pCfg->fromFile(fp);
			fclose(fp);

			if (!pCfg->has("$name")) {
				delete pCfg;
				continue;
			}

			auto& name = pCfg->getWStr("$name");
			auto it = m_historyGlobal.find(name);
			if (it != m_historyGlobal.end()) {
				delete it->second;
				it->second = pCfg;
			}
			else
				m_historyGlobal[name] = pCfg;
		}

		return true;
	}

	bool HistoryManager::_loadDocumentConfigs(const Engine::Path& projPath)
	{
		auto cfgPath = projPath / "config/history";

		if (!FU::hasDir(cfgPath))
			return true;

		std::vector<Path> vFiles;
		FileUtil::parseFiles(vFiles, cfgPath, "lgst", false);

		for (auto& cfgPath : vFiles) {
			FU::wait(cfgPath);
			FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
			if (fp == nullptr)
				continue;

			if (!FileUtil::parseMagic(fp, "STHIS")) {
				fclose(fp);
				continue;
			}

			DObject* pCfg = DObject::alloc();
			pCfg->fromFile(fp);
			fclose(fp);

			if (!pCfg->has("$name")) {
				delete pCfg;
				continue;
			}

			auto& name = pCfg->getWStr("$name");
			auto it = m_history.find(name);
			if (it != m_history.end()) {
				delete it->second;
				it->second = pCfg;
			}
			else
				m_history[name] = pCfg;
		}

		return true;
	}

	void HistoryManager::setGlobal(const std::wstring& name, const Engine::DObject& props)
	{
		auto pCfg = DObject::alloc(props);
		pCfg->set("$name", name);

		auto it = m_historyGlobal.find(name);
		if (it != m_historyGlobal.end()) {
			delete it->second;
			it->second = pCfg;
		}
		else
			m_historyGlobal[name] = pCfg;

		m_globalDirties[name] = 1;
	}

	void HistoryManager::set(const std::wstring& name, const Engine::DObject& props)
	{
		auto pCfg = DObject::alloc(props);
		pCfg->set("$name", name);

		auto it = m_history.find(name);
		if (it != m_history.end()) {
			delete it->second;
			it->second = pCfg;
		}
		else
			m_history[name] = pCfg;

		m_documentDirties[name] = 1;
	}
}