#include "sketch/Document/Scene/LoadSceneHangUp.h"
#include <vector>

#include "Engine/Util/DObject.h"
#include "Engine/Util/StrUtil.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Document.h"
#include "sketch/Document/Scene/ObjectDoc.h"
#include "sketch/Document/Scene/TFObjectDoc.h"
#include "sketch/STCore.h"

using namespace Engine;

namespace sketch
{
LoadSceneHangUp::LoadSceneHangUp(SceneDoc* pScene, SketchSerializer& serializer, const std::wstring& progName, float progRange,
                                 std::function<void(bool)> onFinish)
    : m_sceneId(pScene->id()), m_serializer(serializer), m_progName(progName), m_progStart(0), m_progRange(progRange), m_onFinish(onFinish),
      m_succ(false)
{
}

LoadSceneHangUp::~LoadSceneHangUp() {}

void LoadSceneHangUp::run()
{
    m_progStart = m_progName.empty() ? 0.0f : STCore::inst->uiProgress(m_progName);
    m_progress = m_progStart;

    m_task = concurrency::create_task([&] {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

        auto pScene = STCore::inst->docMgr()->document()->getScene(m_sceneId);
        if(!pScene)
        {
            m_succ = false;
            return;
        }

        pScene->load(m_serializer);
        m_succ = true;
    });
}

void LoadSceneHangUp::updateOnMainThread()
{
    if(!m_progName.empty())
        STCore::inst->updateUIProgress(m_progName, m_progress);

    Sleep(45);
}

void LoadSceneHangUp::onFinish()
{
    if(m_onFinish)
        m_onFinish(m_succ);
}
}// namespace sketch