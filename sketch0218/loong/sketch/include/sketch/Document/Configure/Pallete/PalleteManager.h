#pragma once

#include "..\..\..\Core.h"
#include <string>

#include "Pallete.h"

namespace Engine
{
	class Path;
}

namespace sketch
{
	class PalleteManager
	{
	public:
		PalleteManager(void);
		~PalleteManager(void);
		void release(void);

		bool init();

		void loadUserConfigs();
		bool saveUserConfig(const std::wstring& name);
		bool deleteUserConfig(const std::wstring& name);
		bool loadSysPallete(const std::wstring& name, Engine::DObject& retProps);

		inline const std::unordered_map<std::wstring, Pallete*>& palletes() const {
			return m_palletes;
		}

		inline Pallete* pallate(const std::wstring& name) {
			auto it = m_palletes.find(name);
			if (it == m_palletes.end())
				return nullptr;

			return it->second;
		}

		inline bool hasPallete(const std::wstring& name) const {
			return m_palletes.find(name) != m_palletes.end();
		}

		Pallete* newPallete(const std::wstring& name, const Engine::DObject& props);
		bool removePallete(const std::wstring& name);
		bool modifyPallete(const std::wstring& name, const Engine::DObject& props);

		inline Pallete* activePallete() const {
			return m_pActivePal;
		}

		inline void setActivePallete(const std::wstring& name) {
			auto it = m_palletes.find(name);
			m_pActivePal = (it != m_palletes.end())? it->second : m_palletes.begin()->second;
		}

		inline Pallete* defPallete() const {
			auto it = m_palletes.find(L"dark");
			if (it != m_palletes.end())
				return it->second;

			it = m_palletes.find(L"light");
			if (it != m_palletes.end())
				return it->second;

			if (!m_palletes.empty())
				return m_palletes.begin()->second;

			return nullptr;
		}

	protected:
		void _loadSysConfigs();
		
	protected:
		std::unordered_map<std::wstring, Pallete*> m_palletes;
		Pallete* m_pActivePal;
	};

}


