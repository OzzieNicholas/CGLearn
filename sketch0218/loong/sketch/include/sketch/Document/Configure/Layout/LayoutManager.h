#pragma once

#include "..\..\..\Core.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace Engine
{
	class TeklaLis;
	class DObject;
	class Path;
}

namespace sketch
{
	class LayoutManager
	{
	public:
		LayoutManager(void);
		~LayoutManager(void);
		void release(void);

		bool init();

		bool loadUserLayouts();
		bool saveUserLayouts();

		bool loadUserBars();
		bool saveUserBars();

        bool loadUserMiniBars();
        bool saveUserMiniBars();

		bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
		bool onDocNew(const Engine::Path& projPath); //初始化新场景
		bool onDocLoad(const Engine::Path& projPath); //加载场景
		bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
		bool onDocWillClose(void);
		bool onDocClose(void);
		bool onAppClose();

		const Engine::DObject* createLayout(const Engine::DObject& props);
		const Engine::DObject* getLayout(const std::wstring& name) const;
		const bool provisionLayout(const std::wstring& layoutName, const std::wstring& tip, const Engine::DObject* subNodes);
		const bool provisionBar(const std::wstring& name, bool builtIn, const std::wstring& tip, bool provisioned, const std::wstring& location, int x, int y, const std::vector<std::wstring>& funcUnits);
		void removeLayout(const std::wstring& name);

		const Engine::DObject* createBar(const Engine::DObject& props);
		const Engine::DObject* getBar(const std::wstring& name) const;
		void hideBar(const std::wstring& name, const bool isHidden);
		void removeBar(const std::wstring& name);

        const Engine::DObject* getMiniBar(const std::wstring& name) const;
        const Engine::DObject* getMiniBars() const;
        void updateMiniBars(const Engine::DObject* rootConfig);

	protected:
		void _loadSysBars();
        void _loadSysMiniBars();
		void _loadSysLayouts();
        void _loadMiniBars(const Engine::DObject &rootConfig);
        void _clearMiniBars();

	protected:
		std::unordered_map<std::wstring, Engine::DObject*> m_layouts;
		std::unordered_map<std::wstring, Engine::DObject*> m_bars;
        std::unordered_map<std::wstring, Engine::DObject*> m_miniBars;
        Engine::DObject* m_rawMiniBars;
	};

}


