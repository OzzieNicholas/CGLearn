#pragma once

#include "../Core.h"
#include <set>
#include <string>
#include <unordered_map>

namespace Engine
{
	class Vec3;
	class Vec4;
	class DValue;
}// namespace Engine

namespace sketch
{
	class Document;
	class ViewManager;
	class ScriptContext;

	class ScriptManager
	{
	public:
		ScriptManager();
		~ScriptManager();

		Engine::DValue evalExpr(const std::wstring& expr, bool bOperOverload);

		bool init();
	};
}
