#pragma once

#include "..\Core.h"
#include <string>

namespace Engine
{
	class DObject;
}

namespace sketch
{
	class RPCProcessor
	{
	public:
		virtual ~RPCProcessor(void);

		virtual bool onRPCCommand(const std::string& cmd, Engine::DObject& ret) = 0;
	};

}