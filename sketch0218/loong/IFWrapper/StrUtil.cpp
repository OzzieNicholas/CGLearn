#include "StrUtil.h"
#include <stdlib.h>
#include <vcclr.h>

//using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace IFWrapper {
	std::string STR(System::String ^str)
	{
		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(str)).ToPointer();
		return std::string(chars);
	}

	std::wstring WSTR(System::String ^str)
	{
		if (String::IsNullOrEmpty(str))
			return L"";

		const wchar_t* chars = (const wchar_t*)(Marshal::StringToHGlobalUni(str)).ToPointer();
		return std::wstring(chars);
	}

	System::String^ STR(const std::string &str)
	{
		return gcnew String(str.c_str());
	}

	System::String^ WSTR(const std::wstring &str)
	{
		return gcnew String(str.c_str());
	}
}
