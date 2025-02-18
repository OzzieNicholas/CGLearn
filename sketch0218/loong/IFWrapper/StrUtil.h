#pragma once

#include <string>

namespace IFWrapper {
	//带std::string的函数，不能在独立CLR Dll中暴露为公共接口，因此无法封装在CLRUtils DLL中
	std::string STR(System::String ^str);
	System::String^ STR(const std::string &str);

	std::wstring WSTR(System::String ^str);
	System::String^ WSTR(const std::wstring &str);
}
