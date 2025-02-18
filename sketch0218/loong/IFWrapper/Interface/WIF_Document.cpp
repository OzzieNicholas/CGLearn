#include <Windows.h>
#include <string>

#include "../Native.h"
#include "sketch/STCore.h"
#include "sketch/Define.h"
#include "sketch/Interface/Main/IF_Scene.h"
#include "sketch/Interface/Main/IF_View.h"
#include "sketch/Interface/IF_Document.h"
#include "../StrUtil.h"



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
    void Native::queryValidDocumentEnv([Out] array<String^>^% envs)
    {
        std::vector<std::wstring> eEnvs;
        ::queryValidDocumentEnv(eEnvs);

        envs = gcnew array<String^>(eEnvs.size());
        for (int i = 0; i < eEnvs.size(); ++i)
            envs[i] = WSTR(eEnvs[i]);
    }

    String^ Native::documentName()
    {
        return WSTR(::documentName());
    }

    String^ Native::documentPath()
    {
        return WSTR(::documentPath().wstr());
    }

    LoongCAD::Util::DObject^ Native::documentProjProps()
    {
        Engine::DObject eProps;
        if (!::queryDocumentProjProps(eProps))
            return nullptr;

        LoongCAD::Util::DObject^ res = gcnew LoongCAD::Util::DObject();
        wrUtil::copyDObj(res, &eProps);
        return res;
    }

    bool Native::modifyDocument(LoongCAD::Util::DObject^ props)
    {
        if (props == nullptr)
            return false;

        Engine::DObject eProps;
        wrUtil::copyDObj(&eProps, props);
        ::modifyDocument(eProps);
        return true;
    }

    bool Native::getDocumentBaseInfosFromBasePath(String^ basePath,
        [Out] array<String^>^% paths,
        [Out] array<long long>^% modifyTms,
        [Out] array<String^>^% designers,
        [Out] array<String^>^% comments)
    {
        std::vector<std::wstring> tmpPaths;
        std::vector<long long> tmpModifyTms;
        std::vector<std::wstring> tmpDesigners;
        std::vector<std::wstring> tmpCmts;

        if (!::getDocumentBaseInfosFromBasePath(WSTR(basePath), tmpPaths, tmpModifyTms, tmpDesigners, tmpCmts)) {
            paths = gcnew array<String^>(0);
            modifyTms = gcnew array<long long>(0);
            designers = gcnew array<String^>(0);
            comments = gcnew array<String^>(0);
            return false;
        }

        paths = gcnew array<String^>((int)tmpPaths.size());
        modifyTms = gcnew array<long long>((int)tmpModifyTms.size());
        designers = gcnew array<String^>((int)tmpDesigners.size());
        comments = gcnew array<String^>((int)tmpCmts.size());

        for (int i = 0; i < tmpPaths.size(); ++i) {
            paths[i] = WSTR(tmpPaths[i]);
            modifyTms[i] = tmpModifyTms[i];
            designers[i] = WSTR(tmpDesigners[i]);
            comments[i] = WSTR(tmpCmts[i]);
        }
            
        return true;
    }

    bool Native::runCommonScript(String^ script, bool debug, [Out] String^% result, [Out] String^% errMsg)
    {
        std::wstring eResult, eErrMsg;
        bool bSucc = ::runCommonScript(WSTR(script), debug, eResult, eErrMsg);
        if (debug) {
            result = WSTR(eResult);
            errMsg = WSTR(eErrMsg);
        }
        return bSucc;
    }

    Object^ Native::evalJSExpr(String^ expr) {
        return wrUtil::wrap(::evalJSExpr(WSTR(expr)));
    }

}
