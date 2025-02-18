#pragma once

#include "..\..\..\Core.h"
#include <string>
#include <unordered_map>

#include "Engine/Math/Math.h"

namespace Engine
{
	class Path;
}

namespace sketch
{
	class ColorManager
	{
	public:
		ColorManager(void);
		~ColorManager(void);

		bool init();

		void dirty();

		bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
		bool onDocNew(const Engine::Path& projPath); //初始化新场景
		bool onDocLoad(const Engine::Path& projPath); //加载场景
		bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
		bool onDocClose(void);
		bool onAppClose(void);

		inline unsigned int color(int cls) const {
			return m_colors[STMIN(255, STMAX(0, cls))];
		}

		void setColors(unsigned int* colors, int nColors);

		int findSimilarClass(unsigned int srcColor);

		inline const uint32_t* colors() const {
			return m_colors;
		}

	protected:
		bool _loadSysConfigs();
		bool _loadUserConfigs(const Engine::Path& projPath);
		bool _saveUserConfigs(const Engine::Path& projPath);

	protected:
		uint32_t m_colors[256];
		bool m_dirty;
	};

}


