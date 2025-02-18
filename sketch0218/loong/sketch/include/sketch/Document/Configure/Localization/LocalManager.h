#pragma once
class LocalManager
{
};

#pragma once

#include "..\..\..\Core.h"
#include <string>
#include <unordered_map>
#include "Engine/Util/DObject.h"

namespace Engine
{
	class Path;
}

namespace sketch
{
	class LocalManager
	{
	public:
		LocalManager(void);
		~LocalManager(void);
		void release(void);

		bool init();

		bool onDocPrepare(const Engine::Path& projPath); //场景初始化/加载前调用
		bool onDocNew(const Engine::Path& projPath); //初始化新场景
		bool onDocLoad(const Engine::Path& projPath); //加载场景
		bool onDocLoaded(const Engine::Path& projPath); //场景初始化/加载完毕后调用
		bool onDocClose(void);
		bool onAppClose(void);

		inline const Engine::DObject& props() const {
			return m_props;
		}

		const std::wstring& local(const std::string& text, const std::string& group = "default") const;
		const std::wstring& local(const std::wstring& textw, const std::string& group = "default") const;

	protected:
		void _loadSysConfigs();

	protected:
		Engine::DObject m_props;
		std::unordered_map<std::string, std::unordered_map<std::string, std::wstring>> m_dict;
	};

}


