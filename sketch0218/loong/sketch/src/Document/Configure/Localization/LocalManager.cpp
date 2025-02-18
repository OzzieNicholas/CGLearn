#include "sketch/Document/Configure/Localization/LocalManager.h"
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
	LocalManager::LocalManager(void)
	{

	}

	LocalManager::~LocalManager(void)
	{
		this->release();
	}

	bool LocalManager::init()
	{
		this->release();
		this->_loadSysConfigs();
		return true;
	}

	bool LocalManager::onDocPrepare(const Engine::Path& projPath)
	{
		return true;
	}

	bool LocalManager::onDocNew(const Engine::Path& projPath)
	{
		return true;
	}

	bool LocalManager::onDocLoad(const Engine::Path& projPath)
	{
		return true;
	}

	bool LocalManager::onDocLoaded(const Engine::Path& projPath)
	{
		return true;
	}

	bool LocalManager::onDocClose(void)
	{
		return true;
	}

	bool LocalManager::onAppClose(void)
	{
		return true;
	}

	void LocalManager::release(void)
	{
		m_props.clear();
		m_dict.clear();
	}

	void LocalManager::_loadSysConfigs()
	{
		m_props.clear();
		m_dict.clear();

		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "localization/lang";
		auto file = cfgPath / "chinese.json";
		std::string text = FileUtil::loadTextFile(file);
		if (text.empty())
			return;

		rapidjson::Document doc;
		doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

		if (doc.HasParseError()) {
			Log::log("", "[Error] Localization config parse error: %s\n", file.str().c_str());
			Log::log("", "[Error] Localization config parse error: [%s] %s \n",
				std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
				rapidjson::GetParseError_En(doc.GetParseError()));
			return;
		}

		if (!doc.IsObject()) {
			return;
		}

		if (!m_props.fromJson(&doc, "utf8"))
			return;

		for (auto& it : m_props) {
			m_dict[it.first] = {};
			auto& cur = m_dict[it.first];
			for (auto& itt : *it.second._dobj()) {
				if (itt.second.type() != DValue::TYPE_WSTR)
					continue;

				cur[itt.first] = itt.second._wstr();
			}
		}
	}

	const std::wstring& LocalManager::local(const std::string& text, const std::string& group) const {
		static const std::wstring s_err = L"???";

		auto it = m_dict.find(group);
		if (it == m_dict.end())
			return s_err;

		auto itt = it->second.find(text);
		if (itt == it->second.end())
			return s_err;

		return itt->second;
	}

	const std::wstring& LocalManager::local(const std::wstring& textw, const std::string& group) const {
		static const std::wstring s_err = L"???";
		auto text = SU::w2s(textw);

		auto it = m_dict.find(group);
		if (it == m_dict.end())
			return s_err;

		auto itt = it->second.find(text);
		if (itt == it->second.end())
			return s_err;

		return itt->second;
	}
}