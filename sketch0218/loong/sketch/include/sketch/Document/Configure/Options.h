#include "../../Core.h"
#include <string>
#include <unordered_map>

#include "Engine/Math/Vec3.h"
#include "Engine/Util/DObject.h"
#include "sketch/Document/Document.h"

namespace sketch
{
class View;

class Options
{
public:
    Options();
    ~Options() = default;

    bool init();

    bool onDocPrepare(const Engine::Path& projPath);// 场景初始化/加载前调用
    bool onDocNew(const Engine::Path& projPath);    // 初始化新场景
    bool onDocLoad(const Engine::Path& projPath);   // 加载场景
    bool onDocLoaded(const Engine::Path& projPath); // 场景初始化/加载完毕后调用
    bool onDocClose(void);
    bool onAppClose(void);

    void dirty();

    const Engine::DObject& props()
    {
        this->_updateProps();
        return m_props;
    }

    void modify(const Engine::DObject& props);

    inline bool viewRotEnable() const { return m_viewRotEnable; }

    // Selector
    void setPickMode(Define::SelectorType selType, Define::PickMode mode);
    void setPickFlag(Define::SelectorType selType, uint32_t flag);
    void updatePickFlag(Define::SelectorType selType, uint32_t flag, bool b);
    bool checkPickFlag(Define::SelectorType selType, uint32_t flag) const;
    Define::PickMode pickMode(Define::SelectorType selType) const;
    uint32_t pickFlag(Define::SelectorType selType) const;
    // const std::wstring& pickObjFilterCfg(Define::SelectorType selType) const;
    // ObjectFilter* pickObjFilter(Define::SelectorType selType) const;

    void setSnapMode(Define::SelectorType selType, Define::SnapMode mode);
    Define::SnapMode snapMode(View* pView) const;
    Define::SnapMode snapMode(Define::SelectorType selType) const;

    void setSnapFlagPt(Define::SelectorType selType, uint32_t flag);
    void updateSnapFlagPt(Define::SelectorType selType, uint32_t flag, bool b);
    bool checkSnapFlagPt(Define::SelectorType selType, uint32_t flag) const;
    void setSnapFlagPl(Define::SelectorType selType, uint32_t flag);
    void updateSnapFlagPl(Define::SelectorType selType, uint32_t flag, bool b);
    bool checkSnapFlagPl(Define::SelectorType selType, uint32_t flag) const;
    uint32_t snapFlagPt(Define::SelectorType selType) const;
    uint32_t snapFlagPl(Define::SelectorType selType) const;

    Define::SnapRefPlane snapRefPlane(Define::SelectorType selType) const;
    void setSnapRefPlane(Define::SelectorType selType, Define::SnapRefPlane plane);
    const Engine::AxisPlane& snapRefPlane(View* pView) const;

    inline const std::string& unit_model_dist() const { return m_unit_model_dist; }
    inline const std::string& unit_model_angl() const { return m_unit_model_angl; }

    inline int enob_model_dist() const { return m_enob_model_dist; }

protected:
    bool _loadSysConfigs();
    bool _loadUserConfigs(const Engine::Path& projPath);
    void _saveUserConfigs(const Engine::Path& projPath);

    void _updateProps();
    bool _parseProp(const std::string& pname, const Engine::DValue& pval);

protected:
    bool m_dirty;
    Engine::DObject m_props;

    // Selector3D
    Define::PickMode m_sel3d_pickMode;
    uint32_t m_sel3d_pickFlag;

    Define::SnapMode m_sel3d_snapMode;
    uint32_t m_sel3d_snapFlagPt;
    uint32_t m_sel3d_snapFlagPl;
    Define::SnapRefPlane m_sel3d_snapRefPlane;

    std::string m_unit_model_dist;
    std::string m_unit_model_angl;

    int m_enob_model_dist;

    // view
    bool m_viewRotEnable;
};

}// namespace sketch