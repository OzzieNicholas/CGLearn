#include "sketch/Document/Document.h"

#include <Windows.h>
#include <ppl.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Engine/Math/AxisPlane.h"
#include "Engine/Math/OBB.h"
#include "Engine/Util/DObject.h"
#include "Engine/Util/FileUtil.h"
#include "Engine/Util/Guid.h"
#include "Engine/Util/Log.h"
#include "Engine/Util/StrUtil.h"
#include "nous_base/nous_base.h"

#include "sketch/Command/CmdManager.h"
#include "sketch/Command/Command.h"
#include "sketch/STCore.h"
#include "sketch/View/RenderView/View.h"
// #include "Scene/PreBuilt/AssemblyDoc.h"
#include "sketch/Document/Scene/SceneDoc.h"
#include "sketch/Document/View/ViewDoc.h"
// #include "Scene/CAD/CADRefDoc.h" // for CAD Reference
#include "sketch/Document/Configure/ConfigureManager.h"
#include "sketch/Document/Configure/Options.h"
#include "sketch/Document/DocManager.h"
#include "sketch/Document/Scene/TFObjectDoc.h"

using namespace Engine;

namespace sketch
{
class DocumentImpl : public Document
{
public:
    DocumentImpl()
        : m_guid(Guid::EMPTY), m_env(L"cn"), m_readOnly(false), m_d2vMatrix(0.01), m_v2dMatrix(100.0), m_projNo(), m_projName(), m_author(),
          m_target(), m_address(), m_designer(), m_dateBegin(0u), m_dateComplete(0u), m_remarks{ L"", L"" }, m_comment(), m_usrFields{}, m_fullDesc(),
          m_shortDesc(), m_totalWeight(0.0), m_manufacturer()
    {
        m_pScene = this->createScene(0);
        m_pCompEditScene = this->createScene(1);
    }

    virtual ~DocumentImpl()
    {
        while(!m_views.empty())
        {
            this->deleteView(m_views.begin()->first);
        }

        for(auto& it: m_scenes)
        {
            delete it.second;
        }
    }

    void init() override {}

    void save(SketchSerializer& serializer, const std::wstring& progName, float progRange) override
    {
        const std::string guid_str = m_guid.str();
        nous::String guid_nous_str(guid_str.c_str());
        serializer.write(guid_nous_str, "guid");
        serializer.begin_write_array("views");
        for(auto& view: m_views)
        {
            if(view.second->scene() == m_pScene)
            {
                serializer.begin_write_object();
                view.second->save(serializer);
                serializer.end_write_object();
            }

            if(!progName.empty())
            {
                STCore::inst->moveUIProgress(progName, progRange * 0.1f / m_views.size());
                STCore::inst->updateUIProgress(progName);
            }
        }
        serializer.end_write_array();
        serializer.begin_write_array("active_views");
        for(const long long& act_view_idx: m_activeViews)
        {
            serializer.write(act_view_idx);
        }
        serializer.end_write_array();

        serializer.begin_write_object("scene");
        m_pScene->save(serializer, progName, progRange);
        serializer.end_write_object();
    }

    void getProjectProps(DObject& props) override
    {
        props.set("guid", m_guid);
        props.set("env", m_env);
        props.set("proj_no", m_projNo);
        props.set("proj_name", m_projName);
        props.set("author", m_author);
        props.set("target", m_target);
        props.set("address", m_address);
        props.set("designer", m_designer);
        props.set("date_begin", m_dateBegin);
        props.set("date_end", m_dateComplete);
        props.set("remark0", m_remarks[0]);
        props.set("remark1", m_remarks[1]);
        props.set("comment", m_comment);
        props.set("full_desc", m_fullDesc);
        props.set("short_desc", m_shortDesc);
        props.set("total_weight", m_totalWeight);
        props.set("manufacturer", m_manufacturer);
        props.set("usr_field0", m_usrFields[0]);
        props.set("usr_field1", m_usrFields[1]);
        props.set("usr_field2", m_usrFields[2]);
        props.set("usr_field3", m_usrFields[3]);
        props.set("usr_field4", m_usrFields[4]);
        props.set("usr_field5", m_usrFields[5]);
        props.set("usr_field6", m_usrFields[6]);
        props.set("usr_field7", m_usrFields[7]);
    }

    virtual void modify(const DObject& props) override
    {
        STCore::inst->docMgr()->updateModifyFrame();

        for(auto& it: props)
        {
            auto& pname = it.first;
            auto& pval = it.second;

            if(pname == "guid")
            {
                if(pval.type() == DValue::TYPE_GUID)
                {
                    m_guid = pval._guid();
                }
                else if(pval.type() == DValue::TYPE_WSTR)
                {
                    m_guid = Guid(pval._wstr());
                }
            }
            else if(pname == "env")
            {
                m_env = pval._wstr().empty() ? L"cn" : pval._wstr();
            }
            else if(pname == "proj_no")
            {
                m_projNo = pval._wstr();
            }
            else if(pname == "proj_name")
            {
                m_projName = pval._wstr();
            }
            else if(pname == "author")
            {
                m_author = pval._wstr();
            }
            else if(pname == "target")
            {
                m_target = pval._wstr();
            }
            else if(pname == "address")
            {
                m_address = pval._wstr();
            }
            else if(pname == "designer")
            {
                m_designer = pval._wstr();
            }
            else if(pname == "date_begin")
            {
                if(pval.type() == DValue::TYPE_LONG)
                {
                    m_dateBegin = pval._long();
                }
            }
            else if(pname == "date_end")
            {
                if(pval.type() == DValue::TYPE_LONG)
                {
                    m_dateComplete = pval._long();
                }
            }
            else if(pname == "remark0")
            {
                m_remarks[0] = pval._wstr();
            }
            else if(pname == "remark1")
            {
                m_remarks[1] = pval._wstr();
            }
            else if(pname == "comment")
            {
                m_comment = pval._wstr();
            }
            else if(pname == "full_desc")
            {
                m_fullDesc = pval._wstr();
            }
            else if(pname == "short_desc")
            {
                m_shortDesc = pval._wstr();
            }
            else if(pname == "total_weight")
            {
                m_totalWeight = pval._double();
            }
            else if(pname == "manufacturer")
            {
                m_manufacturer = pval._wstr();
            }
            else if(pname == "usr_field0")
            {
                m_usrFields[0] = pval._wstr();
            }
            else if(pname == "usr_field1")
            {
                m_usrFields[1] = pval._wstr();
            }
            else if(pname == "usr_field2")
            {
                m_usrFields[2] = pval._wstr();
            }
            else if(pname == "usr_field3")
            {
                m_usrFields[3] = pval._wstr();
            }
            else if(pname == "usr_field4")
            {
                m_usrFields[4] = pval._wstr();
            }
            else if(pname == "usr_field5")
            {
                m_usrFields[5] = pval._wstr();
            }
            else if(pname == "usr_field6")
            {
                m_usrFields[6] = pval._wstr();
            }
            else if(pname == "usr_field7")
            {
                m_usrFields[7] = pval._wstr();
            }
        }
    }

    virtual bool onDocPrepare(const Path& projPath) override
    {
        this->setPath(projPath);
        return true;
    }

    virtual bool onDocNew(const Path& projPath) override
    {
        m_guid = Guid::create();

        m_pScene->onDocNew(projPath);

        // create default view
        ViewDoc* pView = this->createView(-1, m_pScene->id());
        nous::mat4 default_view_plane = nous::mat4::Identity();
        pView->modify({ { "name", L"D5-Test" },
                        { "type", static_cast<char>(Define::VIEW_3D) },
                        { "proj", static_cast<char>(Define::VIEWPROJ_PERSPECTIVE) },
                        { "depth_up", 100.0 },
                        { "depth_down", -100.0 },
                        { "rdmode_part", static_cast<char>(Define::RENDERMODE_EDGEFACE) },
                        { "rdquality_part", static_cast<char>(Define::ViewRenderQuality::RENDERQUALITY_DETAIL) },
                        { "rotH", 135.0 },
                        { "rotV", 45.0 },
                        { "reset_view", true },
                        { "work_area_min", nous::vec2(-100.0, -100.0) },
                        { "work_area_max", nous::vec2(100.0, 100.0) },// why?
                        { "viewplane", default_view_plane } });

        pView->setCamPivot(Vec3(m_pScene->bbox().center().xy, 0.0));
        pView->resetCameraPose();
        this->addActiveView(pView->id());

        return true;
    }

    void load(const Engine::Path& projPath, SketchSerializer* serializer, const std::wstring& progName, float progRange,
              std::function<void(bool)> onFinish)
    {
        nous::String guid_string;
        serializer->read(guid_string, "guid");

        if(guid_string == "")
        {
            m_guid = Guid::create();
        }
        else
        {
            m_guid = Guid(guid_string.data());
        }

        serializer->begin_read_object("scene");
        m_pScene->load(*serializer);
        // m_pScene->load_async(*serializer, progName, progRange, [=](bool success) {
        serializer->end_read_object();
        size_t view_count = 0;
        serializer->begin_read_array("views", view_count);
        for(int i = 0; i < view_count; ++i)
        {
            serializer->begin_read_object();
            SketchParamMap view_param_map;
            serializer->begin_read_object("props");
            view_param_map.load(*serializer);
            serializer->end_read_object();

            long long view_id;
            view_param_map.get_value("view_id", view_id);
            long long scene_id;
            view_param_map.get_value("scene_id", scene_id);
            ViewDoc* pView = this->createView(view_id, scene_id);
            pView->load(*serializer);
            serializer->end_read_object();
        }
        serializer->end_read_array();

        size_t active_view_count;
        serializer->begin_read_array("active_views", active_view_count);
        for(int i = 0; i < active_view_count; ++i)
        {
            long long view_id;
            serializer->read(view_id);
            if(this->getView(view_id))
            {
                this->addActiveView(view_id);
            }
        }
        serializer->end_read_array();

        if(onFinish)
        {
            // onFinish(success);
            onFinish(true);
        }
        //});
    }

    bool onDocLoaded(const Path& projPath) override
    {
        m_pScene->onDocLoaded(projPath);
        return true;
    }

    bool onDocWillClose() override { return true; }

    bool onDocClose() override { return true; }

    void createLockFile(const Path& projPath) override
    {
        auto path = STCore::inst->appDataPath() / "model_locked";
        if(!FU::hasDir(path))
        {
            FU::createDir(path);
        }

        auto file = path / SU::encodeURI(projPath.wstr()) + ".locked";
        m_hLockFile = FU::createExclusiveFile(file, "");
    }

    void releaseLockFile() override
    {
        if(!m_hLockFile)
        {
            return;
        }

        FU::closeFile(m_hLockFile);
        m_hLockFile = NULL;

        auto file = STCore::inst->appDataPath() / "model_locked" / SU::encodeURI(m_path.wstr()) + ".locked";
        FileUtil::deleteFile(file);
    }

    const Engine::Guid& guid() const override { return m_guid; }

    const std::wstring& env() const override { return m_env; }

    void setEnv(const std::wstring& env) override { m_env = env; }

    void setReadOnly(bool readOnly) override { m_readOnly = readOnly; }

    bool readOnly() const override { return m_readOnly; }

    SceneDoc* createScene(long long id) override
    {
        if(m_scenes.find(id) != m_scenes.end())
        {
            return nullptr;
        }

        SceneDoc* pScene = SceneDoc::create(id);
        m_scenes[pScene->id()] = pScene;
        return pScene;
    }

    SceneDoc* getScene(long long id) const override
    {
        auto it = m_scenes.find(id);
        if(it == m_scenes.end())
        {
            return nullptr;
        }
        return it->second;
    }

    SceneDoc* getSceneByObj(long long objId) const override
    {
        long long sceneId = SceneDoc::sceneId(objId);
        auto it = m_scenes.find(sceneId);
        if(it == m_scenes.end())
        {
            return nullptr;
        }
        return it->second;
    }

    ObjectDoc* getObject(long long id) const override
    {
        long long sceneId = SceneDoc::sceneId(id);
        auto it = m_scenes.find(sceneId);
        if(it == m_scenes.end())
        {
            return nullptr;
        }
        return it->second->getObject(id);
    }

    TFObjectDoc* getTFObject(long long objId) const override
    {
        long long sceneId = SceneDoc::sceneId(objId);
        auto it = m_scenes.find(sceneId);
        if(it == m_scenes.end())
        {
            return nullptr;
        }
        return it->second->getTFObject(objId);
    }

    void deleteObject(long long id) override
    {
        long long sceneId = SceneDoc::sceneId(id);
        auto it = m_scenes.find(sceneId);
        if(it == m_scenes.end())
        {
            return;
        }
        it->second->deleteObject(id);
    }

    void deleteObjects(const std::unordered_set<long long> ids) override
    {
        for(auto id: ids)
        {
            deleteObject(id);
        }
    }

    // main scene
    SceneDoc* scene() const override { return m_pScene; }

    SceneDoc* compEditScene() const override { return m_pCompEditScene; }

    // doc unnit to view unit
    float d2v(double val) const override { return (float)(val * 0.01); }

    Engine::Vec3f d2v(const Engine::Vec3& val) const override { return Engine::Vec3f(val * 0.01); }
    Engine::Vec2f d2v(const Engine::Vec2& val) const override { return Engine::Vec2f(val * 0.01); }
    Engine::Mat43f d2v(const Engine::Mat43& val) const override { return Engine::Mat43f(val.x, val.y, val.z, val.pos * 0.01); }
    Engine::AABBf d2v(const Engine::AABB& bbox) const override
    {
        Engine::AABBf ret;
        ret.setMin(bbox.getMin() * 0.01);
        ret.setMax(bbox.getMax() * 0.01);
        return ret;
    }
    Engine::AxisPlanef d2v(const Engine::AxisPlane& val) const override { return Engine::AxisPlanef(this->d2v(val.o()), val.axisX(), val.axisY()); }
    Engine::OBB d2v(const Engine::OBB& val) const override { return { val.m_min, val.m_max, Engine::Mat43::S(0.01, 0.01, 0.01) * val.m_matrix }; }
    const Engine::Mat43& d2vMatrix() const override { return m_d2vMatrix; }

    const Engine::Mat43& v2dMatrix() const override { return m_v2dMatrix; }

    double v2d(float val) const override { return (double)val * 100.0; }

    Engine::Vec3 v2d(const Engine::Vec3f& val) const override
    {
        return Engine::Vec3((double)val.x * 100.0, (double)val.y * 100.0, (double)val.z * 100.0);
    }
    Engine::Mat43 v2d(const Engine::Mat43f& val) const override { return Engine::Mat43(val.x, val.y, val.z, (Vec3)val.pos * 100.0); }
    Engine::AABB v2d(const Engine::AABBf& val) const override
    {
        Engine::AABB ret;
        ret.setMin((Vec3)val.getMin() * 100.0);
        ret.setMax((Vec3)val.getMax() * 100.0);
        return ret;
    }
    Engine::OBB v2d(const Engine::OBB& val) const override { return { val.m_min, val.m_max, Engine::Mat43::S(100.0, 100.0, 100.0) * val.m_matrix }; }

    ViewDoc* createView(long long id, long long sceneId) override
    {
        SceneDoc* pScene = this->getScene(sceneId);
        if(!pScene || m_views.find(id) != m_views.end())
        {
            STASSERT(0);
            return nullptr;
        }

        STCore::inst->docMgr()->updateModifyFrame();

        ViewDoc* pView = ViewDoc::create(id, pScene);
        m_views[pView->id()] = pView;

        this->invokeListener(Define::MSG_VIEW_CREATE, DObject::alloc({ { "view", pView } }));
        return pView;
    }

    void deleteView(long long id) override
    {
        auto it = m_views.find(id);
        if(it == m_views.end())
        {
            return;
        }

        STCore::inst->docMgr()->updateModifyFrame();

        // delete dependent objects before delete view
        for(auto& it: m_scenes)
        {
            auto pScene = it.second;
            std::unordered_set<TFObjectDoc*> viewObjs;
            for(auto& itt: pScene->tfObjects())
            {
                auto pObj = itt.second;
                if(pObj->viewId() == id)
                {
                    viewObjs.insert(pObj);
                }
            }

            if(!viewObjs.empty())
            {
                STCore::inst->cmdMgr()->freezeRvt(pScene->rvtId());
                pScene->deleteObjects(viewObjs);
                STCore::inst->cmdMgr()->unfreezeRvt(pScene->rvtId());
            }
        }

        ViewDoc* pView = it->second;
        m_views.erase(id);
        m_activeViews.erase(id);

        Engine::DObject* paras = DObject::alloc({ { "view", pView } });
        this->invokeListener(Define::MSG_VIEW_DELETE, paras);

        delete pView;
    }

    std::wstring getValidViewName(const std::wstring& prefix) const override
    {
        std::wstring name = prefix;
        int n = 1;
        bool bValid;
        do
        {
            bValid = true;
            for(auto& it: m_views)
            {
                if(it.second->name() == name)
                {
                    name = prefix + L" " + std::to_wstring(n++);
                    bValid = false;
                    break;
                }
            }
        } while(!bValid);

        return name;
    }

    ViewDoc* getView(long long id) const override
    {
        auto it = m_views.find(id);
        if(it != m_views.end())
        {
            return it->second;
        }

        return nullptr;
    }

    ViewDoc* getView(const std::wstring& name) const override
    {
        for(auto& it: m_views)
        {
            if(it.second->name() == name)
            {
                return it.second;
            }
        }

        return nullptr;
    }

    ViewDoc* GetFirstView() const
    {
        auto it = m_views.begin();
        if(it == m_views.end())
            return nullptr;
        return it->second;
    }

    const std::unordered_map<long long, ViewDoc*>& views() const override { return m_views; }
    void addActiveView(long long id) override { m_activeViews.insert(id); }
    void removeActiveView(long long id) override { m_activeViews.erase(id); }
    const std::unordered_set<long long>& activeViews() const override { return m_activeViews; }
    void setActiveViews(const std::vector<long long>& views) override
    {
        m_activeViews.clear();
        for(auto& view: views)
        {
            m_activeViews.insert(view);
        }
    }
    void setActiveViews(long long* views, size_t num) override
    {
        m_activeViews.clear();
        for(size_t i = 0; i < num; ++i)
        {
            m_activeViews.insert(views[i]);
        }
    }
    void addActiveViews(long long* views, size_t num) override
    {
        for(size_t i = 0; i < num; ++i)
        {
            m_activeViews.insert(views[i]);
        }
    }
    void removeActiveViews(long long* views, size_t num) override
    {
        for(size_t i = 0; i < num; ++i)
        {
            m_activeViews.erase(views[i]);
        }
    }
    const Engine::Path& path() const override { return m_path; }
    void setPath(const Engine::Path& path) override { m_path = path; }
    const std::wstring& projNo() const override { return m_projNo; }
    const std::wstring& projName() const override { return m_projName; }
    const std::wstring& author() const override { return m_author; }
    const std::wstring& target() const override { return m_target; }
    const std::wstring& address() const override { return m_address; }
    const std::wstring& designer() const override { return m_designer; }
    long long dateBegin() const override { return m_dateBegin; }
    long long dateComplete() const override { return m_dateComplete; }
    const std::wstring& remark(int idx) const override { return m_remarks[idx]; }
    const std::wstring& comment() const override { return m_comment; }
    const std::wstring& usrField(int idx) const override { return m_usrFields[idx]; }
    const std::wstring& fullDesc() const override { return m_fullDesc; }
    const std::wstring& shortDesc() const override { return m_shortDesc; }
    double totalWeight() const override { return m_totalWeight; }
    const std::wstring& manufacturer() const override { return m_manufacturer; }
    void package() override
    {
        if(m_path.empty())
        {
            return;
        }

        for(auto& it: m_pScene->objects())
        {
            it.second->package();
        }
    }
    void update(unsigned long long tm) override
    {
        if(m_pScene)
        {
            // m_pScene->update(tm);
        }
    }

protected:
    bool m_readOnly;
    Engine::Path m_path;

    Engine::Guid m_guid;
    std::wstring m_env;
    std::wstring m_projNo;
    std::wstring m_projName;
    std::wstring m_author;
    std::wstring m_target;
    std::wstring m_address;
    std::wstring m_designer;
    long long m_dateBegin;
    long long m_dateComplete;
    std::wstring m_remarks[2];
    std::wstring m_comment;
    std::wstring m_usrFields[8];
    std::wstring m_fullDesc;
    std::wstring m_shortDesc;
    double m_totalWeight;
    std::wstring m_manufacturer;

    std::unordered_map<long long, ViewDoc*> m_views;
    std::unordered_set<long long> m_activeViews;

    std::unordered_map<long long, SceneDoc*> m_scenes;
    SceneDoc* m_pScene;// main scene
    SceneDoc* m_pCompEditScene;

    Mat43 m_d2vMatrix;
    Mat43 m_v2dMatrix;

    HANDLE m_hLockFile;
};

Document* Document::create() { return new DocumentImpl(); }

Document::~Document() {}

}// namespace sketch
