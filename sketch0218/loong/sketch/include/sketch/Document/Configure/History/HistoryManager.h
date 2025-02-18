#pragma once

#include "..\..\..\Core.h"
#include <string>
#include <unordered_map>

namespace Engine
{
	class Path;
	class DObject;
}

namespace sketch
{
	class HistoryManager
	{
	public:
		HistoryManager(void);
		~HistoryManager(void);
		void release(void);

		bool init();

		bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
		bool onDocNew(const Engine::Path& projPath); //初始化新场景
		bool onDocLoad(const Engine::Path& projPath); //加载场景
		bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
		bool onDocClose(void);
		bool onAppClose(void);

		inline const Engine::DObject* historyGlobal(const std::wstring& name) const {
			auto it = m_historyGlobal.find(name);
			if (it == m_historyGlobal.end())
				return nullptr;

			return it->second;
		}

		inline const Engine::DObject* history(const std::wstring& name) const {
			auto it = m_history.find(name);
			if (it == m_history.end())
				return nullptr;

			return it->second;
		}

		void setGlobal(const std::wstring& name, const Engine::DObject& props);
		void set(const std::wstring& name, const Engine::DObject& props);

	protected:
		bool _loadGlobalConfigs();
		bool _loadDocumentConfigs(const Engine::Path& projPath);
		void _processDirties();

	protected:
		std::unordered_map<std::wstring, Engine::DObject*> m_historyGlobal; //global histories, such as project base dir 
		std::unordered_map<std::wstring, Engine::DObject*> m_history; //document histories

		std::unordered_map<std::wstring, int> m_globalDirties; //<1: modify, 2: delete>
		std::unordered_map<std::wstring, int> m_documentDirties; //<1: modify, 2: delete>
	};

}


