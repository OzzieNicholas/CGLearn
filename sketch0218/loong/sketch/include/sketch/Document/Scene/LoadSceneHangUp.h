#pragma once

#include <atomic>
#include <functional>
#include <ppl.h>
#include <ppltasks.h>
#include <string>
#include <vector>

#include "Engine/Util/DObject.h"

#include "../../Core.h"
#include "../../Util/HangUpJob.h"
#include "SceneDoc.h"
#include "sketch/Serializer/SketchSerializer.h"

namespace sketch
{
class LoadSceneHangUp : public HangUpJob
{
public:
    LoadSceneHangUp(SceneDoc* pScene, SketchSerializer& serializer, const std::wstring& progName, float progRange,
                    std::function<void(bool)> onFinish);
    virtual ~LoadSceneHangUp();

    virtual void run() override;
    virtual void updateOnMainThread() override;
    virtual void onFinish() override;

    inline bool isSucc() const { return m_succ; }

protected:
    long long m_sceneId;
    SketchSerializer& m_serializer;// NOTE: 调用者须保证在线程运行期间不释放此对象，同时需保证在Job完成后释放此对象
    std::wstring m_progName;
    float m_progStart;
    float m_progRange;
    std::atomic<float> m_progress;
    bool m_succ;
    std::function<void(bool)> m_onFinish;
};

}// namespace sketch
