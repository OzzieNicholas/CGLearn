#pragma once

#include <string>

namespace IFWrapper {
	//��std::string�ĺ����������ڶ���CLR Dll�б�¶Ϊ�����ӿڣ�����޷���װ��CLRUtils DLL��
	std::string STR(System::String ^str);
	System::String^ STR(const std::string &str);

	std::wstring WSTR(System::String ^str);
	System::String^ WSTR(const std::wstring &str);
}
