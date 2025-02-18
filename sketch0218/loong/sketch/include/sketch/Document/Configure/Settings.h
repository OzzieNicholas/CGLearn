#pragma once

#include "../../Core.h"
#include <string>
#include <unordered_map>

#include "Engine/Util/Path.h"
#include "Engine/Util/DObject.h"

#include "../../Define.h"

namespace Engine
{
	class AxisPlane;
}

namespace sketch
{

	class Settings
	{
	public:
		Settings();
		~Settings();

		bool init();
		void release();

		bool onDocPrepare(const Engine::Path& projPath);
		bool onDocNew(const Engine::Path& projPath);
		bool onDocLoad(const Engine::Path& projPath);
		bool onDocLoaded(const Engine::Path& projPath);
		bool onDocClose();
		bool onAppClose();

		void dirty();

		const Engine::DObject& props() {
			this->_updateProps();
			return m_props;
		}

		void modify(const Engine::DObject& props);

		inline int autoSaveInterval() const { return m_autoSaveInterval; }

		inline const bool preloadSdfTexts() const {
			return m_bPreloadSdfTexts;
		}

		inline bool geoCache() const {
			return m_geoCache;
		}

		inline const Engine::Path& sysCfgPath() const {
			return m_sysCfgPath;
		}

		inline const Engine::Path& sysCfgFullPath() const {
			return m_sysCfgFullPath;
		}

		inline bool geoTriVis() const {
			return m_geoTriVis;
		}

		inline const std::wstring& activePal() const {
			return m_activePal;
		}

		inline Define::DisplayRatio displayRatio() const {
			return m_displayRatio;
		}

		inline bool viewDebugInfo() const {
			return m_viewDebugInfo;
		}

		inline bool vsync() const {
			return m_vsync;
		}

		inline bool viewAA() const {
			return m_viewAA;
		}

		inline bool drAA() const {
			return m_drAA;
		}

		inline bool cutColor() const {
			return m_cutColor;
		}

		inline bool hoverHighlight() const {
			return m_hoverHighlight;
		}

		inline const std::wstring& customPara(const std::string& group, const std::string& name) const {
			return m_props.getWStr(group + '$' + name);
		}

		inline double snapRangeFactor() const {
			return m_snapRangeFactor;
		}

	protected:
		void _updateProps();
		bool _parseProp(const std::string& pname, const Engine::DValue& pval);

		void _loadSysConfigs();
		void _loadUserConfigs();
		void _saveUserConfigs();

	protected:
		bool m_dirty;
		Engine::DObject m_props;

		int m_autoSaveInterval;

		// Assets
		bool m_bPreloadSdfTexts;

		//document
		Engine::Path m_sysCfgPath;
		Engine::Path m_sysCfgFullPath;
		std::wstring m_activePal;

		//for debug
		bool m_viewDebugInfo;
		bool m_geoCache;
		bool m_console;
		bool m_geoTriVis;
		bool m_geoVIdxVis;
		bool m_geoSegVis;
		bool m_reloadCompPluginBeforeRefresh;
		bool m_auxQuadNormalVis;

		//render
		Define::DisplayRatio m_displayRatio;
		bool m_vsync;
		bool m_viewAA; //view anti-alias
		bool m_drAA; //drawing anti-alias
		bool m_cutColor;

		//selector
		bool m_hoverHighlight;
		bool m_snapSymbolVis;
		double m_snapRangeFactor;
	};
}// namespace sketch
