#include "sketch/Document/Configure/Color/ColorManager.h"
#include "sketch/STCore.h"
#include <Windows.h>
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/SysUtil.h"
#include "Engine/Libs/rapidJson/include/rapidjson/document.h"
#include "Engine/Libs/rapidJson/include/rapidjson/error/en.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/Path.h"

#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Settings.h"

using namespace Engine;

namespace sketch
{
	ColorManager::ColorManager(void):
		m_dirty(false)
	{
		memset(m_colors, 0, sizeof(uint32_t) * 256);
		m_colors[0] = 0x808080;
	}

	ColorManager::~ColorManager(void)
	{
		
	}

	bool ColorManager::init()
	{
		m_dirty = false;
		return true;
	}

	void ColorManager::dirty()
	{
		m_dirty = true;
	}

	void ColorManager::setColors(unsigned int* colors, int nColors)
	{
		if (!colors || nColors <= 1)
			return;

		//NOTE: class0保留，所以不允许修改，从1开始读取
		memcpy(m_colors + 1, colors + 1, sizeof(uint32_t) * STMIN(255, nColors - 1));
	}

	int ColorManager::findSimilarClass(unsigned int srcColor)
	{
		int r0 = (int)((srcColor >> 16) & 0xFF);
		int g0 = (int)((srcColor >> 8) & 0xFF);
		int b0 = (int)(srcColor & 0xFF);
		
		int minDeltaColor = 256 * 3;
		int retClass = -1;
		for (int i = 0; i < 256; ++i) {
            int r1 = (int)((m_colors[i] >> 16) & 0xFF);
            int g1 = (int)((m_colors[i] >> 8) & 0xFF);
            int b1 = (int)(m_colors[i] & 0xFF);
			int curDelta = abs(r1 - r0) + abs(g1 - g0) + abs(b1 - b0);
			if (curDelta < minDeltaColor) {
				minDeltaColor = curDelta;
				retClass = i;
			}
		}

		return retClass;
	}

	bool ColorManager::onDocPrepare(const Engine::Path& projPath)
	{
		return true;
	}

	bool ColorManager::onDocNew(const Engine::Path& projPath)
	{
		this->_loadSysConfigs();
		m_dirty = false;
		return true;
	}

	bool ColorManager::onDocLoad(const Engine::Path& projPath)
	{
		this->_loadSysConfigs();
		this->_loadUserConfigs(projPath);
		m_dirty = false;
		return true;
	}

	bool ColorManager::onDocLoaded(const Engine::Path& projPath)
	{
		return true;
	}

	bool ColorManager::onDocClose(void)
	{
		if (m_dirty
			&& !STCore::inst->docMgr()->document()->readOnly()) {
			auto& projPath = STCore::inst->docMgr()->document()->path();
			if (!projPath.empty())
				this->_saveUserConfigs(projPath);

			m_dirty = false;
		}

		return true;
	}

	bool ColorManager::onAppClose(void)
	{
		return this->onDocClose();
	}

	bool ColorManager::_loadUserConfigs(const Engine::Path& projPath)
	{
		auto cfgPath = projPath / L"config\\color\\class.lgst";

		FU::wait(cfgPath);

		FILE* fp = _wfopen(cfgPath.wstr().c_str(), L"rb");
		if (fp == nullptr)
			return false;

		if (!FileUtil::parseMagic(fp, "STCLR")) {
			fclose(fp);
			return false;
		}

		DObject cfg;
		cfg.fromFile(fp);
		fclose(fp);

		if (cfg.has("color_arr")) {
			auto arrLen = cfg["color_arr"].len();
			auto pArr = cfg["color_arr"]._uintArr();
			memcpy(m_colors, pArr, sizeof(uint32_t) * STMIN(arrLen, 256));
		}
		else {
			for (auto& it : cfg) {
				int cls = std::stoi(it.first);
				if (cls < 0 || cls > 255)
					continue;

				m_colors[cls] = it.second._uint();
			}
		}

		return true;
	}

	bool ColorManager::_saveUserConfigs(const Engine::Path& projPath)
	{
		auto basePath = projPath / "config\\color";
		if (!FileUtil::hasDir(basePath))
			FileUtil::createDir(basePath);

		DObject* pCfg = DObject::alloc();
		(*pCfg)["color_arr"].set(m_colors, 256);

		auto filePath = basePath / "class.lgst";
		FU::saveAsync(filePath, "STCLR", pCfg, "");

		return true;
	}

	bool ColorManager::_loadSysConfigs()
	{
		auto& env = STCore::inst->docMgr()->document()->env();
		auto cfgPath = STCore::inst->docMgr()->cfgMgr()->settings()->sysCfgFullPath() / "env" / env / "color";

		{ //json configs
			auto cfgFile = cfgPath / "class.json";
			std::string text = FileUtil::loadTextFile(cfgFile);
			
			do {
				if (text.empty())
					break;
				
				rapidjson::Document doc;
				doc.Parse<rapidjson::kParseTrailingCommasFlag | rapidjson::kParseCommentsFlag>(text.c_str());

				if (doc.HasParseError()) {
					Log::log("", "[Error] Color config parse error: %s\n", cfgFile.str().c_str());
					Log::log("", "[Error] Color config parse error: [%s] %s \n",
						std::to_string(StrUtil::getLineNumber(text, doc.GetErrorOffset())).c_str(),
						rapidjson::GetParseError_En(doc.GetParseError()));
					break;
				}

				if (!doc.IsObject()) {
					break;
				}

				for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
					int cls = std::stoi((it->name.GetString()));
					if (cls < 0 || cls > 255)
						continue;

					std::string color = StrUtil::trim(it->value.GetString());
					if (color.find("rgb") == 0) {
						size_t p0 = color.find('(');
						size_t p1 = color.find(')');
						if (p0 != color.npos && p1 != color.npos) {
							std::vector<std::string> rgb;
							StrUtil::split(rgb, color.substr(p0 + 1, p1 - p0 - 1), ',');
							if (rgb.size() == 3) {
								int r = StrUtil::toInt(rgb[0]);
								int g = StrUtil::toInt(rgb[1]);
								int b = StrUtil::toInt(rgb[2]);
								m_colors[cls] = (r << 16) | (g << 8) | b;
							}
						}
					}
					else {
						m_colors[cls] = std::stoul(color, nullptr, 16);
					}
				}
			} while (false);
		}

		{//binary configs
			auto cfgFile = cfgPath / "class.lgst";

			do {
				FILE* fp = _wfopen(cfgFile.wstr().c_str(), L"rb");
				if (fp == nullptr)
					break;

				if (!FileUtil::parseMagic(fp, "STCLR")) {
					fclose(fp);
					break;
				}

				DObject cfg;
				cfg.fromFile(fp);
				fclose(fp);

				if (cfg.has("color_arr")) {
					auto arrLen = cfg["color_arr"].len();
					auto pArr = cfg["color_arr"]._uintArr();
					memcpy(m_colors, pArr, sizeof(uint32_t) * STMIN(arrLen, 256));
				}
				else {
					for (auto& it : cfg) {
						int cls = std::stoi(it.first);
						if (cls < 0 || cls > 255)
							continue;

						m_colors[cls] = it.second._uint();
					}
				}
			} while (false);
		}

		return true;
	}
}