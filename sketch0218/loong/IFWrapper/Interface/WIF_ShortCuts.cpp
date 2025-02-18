#include <Windows.h>
#include <string>

#include "../Native.h"
#include "../StrUtil.h"
#include "sketch/Define.h"
#include "sketch/Interface/IF_ShortCuts.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/STCore.h"

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace sketch;
using namespace LoongCAD::Geometry;
using namespace CLRUtils;

namespace IFWrapper
{
bool Native::queryShortCuts([Out] array<String ^> ^ % names, [Out] array<unsigned int> ^ % scs)
{
    std::vector<std::wstring> eNames;
    std::vector<unsigned int> eScs;
    ::queryShortCuts(eNames, eScs);

    names = gcnew array<String ^>((int)eNames.size());
    scs = gcnew array<unsigned int>((int)eScs.size());

    for(int i = 0; i < (int)eNames.size(); ++i)
        names[i] = WSTR(eNames[i]);

    if(!eScs.empty())
    {
        pin_ptr<unsigned int> pArr = &scs[0];
        memcpy(pArr, eScs.data(), sizeof(unsigned int) * eScs.size());
    }

    return true;
}

bool Native::queryBuiltInShortCuts([Out] array<String ^> ^ % names, [Out] array<unsigned int> ^ % scs)
{
    std::vector<std::wstring> eNames;
    std::vector<unsigned int> eScs;
    ::queryBuiltInShortCuts(eNames, eScs);

    names = gcnew array<String ^>((int)eNames.size());
    scs = gcnew array<unsigned int>((int)eScs.size());

    for(int i = 0; i < (int)eNames.size(); ++i)
        names[i] = WSTR(eNames[i]);

    if(!eScs.empty())
    {
        pin_ptr<unsigned int> pArr = &scs[0];
        memcpy(pArr, eScs.data(), sizeof(unsigned int) * eScs.size());
    }

    return true;
}

bool Native::importShortCuts(String ^ path, [Out] array<String ^> ^ % fus, [Out] array<unsigned int> ^ % scs)
{
    std::vector<std::wstring> efus;
    std::vector<uint32_t> escs;
    if(!::importShortCuts(WSTR(path), efus, escs))
        return false;

    fus = gcnew array<String ^>((int)efus.size());
    scs = gcnew array<unsigned int>((int)escs.size());
    for(int i = 0; i < (int)efus.size(); ++i)
        fus[i] = WSTR(efus[i]);

    if(!escs.empty())
    {
        pin_ptr<unsigned int> pArr = &scs[0];
        memcpy(pArr, escs.data(), sizeof(unsigned int) * escs.size());
    }

    return true;
}

bool Native::exportShortCuts(String ^ path, array<String ^> ^ fus, array<unsigned int> ^ scs)
{
    if(path == nullptr || fus == nullptr || scs == nullptr)
        return false;

    std::vector<std::wstring> efus;
    std::vector<uint32_t> escs;
    for(int i = 0; i < fus->Length; ++i)
    {
        efus.push_back(WSTR(fus[i]));
        escs.push_back((uint32_t)scs[i]);
    }
    return ::exportShortCuts(WSTR(path), efus, escs);
}

bool Native::setShortCuts(array<String ^> ^ fus, array<unsigned int> ^ scs)
{
    //if(fus == nullptr || scs == nullptr)
    //    return nullptr;

    //std::vector<std::wstring> eFus;
    //std::vector<uint32_t> eScs;
    //int nSC = std::min(fus->Length, scs->Length);
    //for(int i = 0; i < nSC; ++i)
    //{
    //    eFus.push_back(WSTR(fus[i]));
    //    eScs.push_back((uint32_t)scs[i]);
    //}

    std::vector<std::wstring> eFus;
    std::vector<uint32_t> eScs;
    int nSC;// = std::min(fus->Length, scs->Length);
    for(int i = 0; i < nSC; ++i)
    {
        eFus.push_back(WSTR(fus[i]));
        eScs.push_back((uint32_t)scs[i]);
    }

    return false;
}

}// namespace IFWrapper
