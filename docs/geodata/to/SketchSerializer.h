#pragma once

#include "nous_mesh/mesh.h"
#include "sketch/Object/SketchObject.h"
#include "sketch/View/RenderRes/Element.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace sketch
{

class SketchSerializer : public nous::Serializer
{
public:
    SketchSerializer();

    void advance_save(const std::string& path);

    void advance_load(const std::string& path);

    std::string get_sketch_obj_type_name(const ESketchObjectType type);

    ESketchObjectType get_sketch_obj_type_name(const std::string& type_name);

    std::string get_sketch_nurbs_item_type_name(const EElementType type);

    EElementType get_sketch_nurbs_item_type_name(const std::string& type_name);

    void write_sketch_object_ref(const SketchObjectPtr& skt_obj, std::string name = "");

    void write_sketch_nurbs_point_ref(const SketchNurbsPointPtr& skt_nurbs_pt, std::string name = "");

    void write_sketch_nurbs_item_ref(const std::shared_ptr<INurbsBase>& skt_nurbs_item, std::string name = "");

    SketchObjectPtr read_sketch_object_ref(std::string name = "");

    SketchNurbsPointPtr read_sketch_nurbs_point_ref(std::string name = "");

    std::shared_ptr<INurbsBase> read_sketch_nurbs_item_ref(std::string name = "");

    void advance_write(const nous::vec2& value, const std::string& name = "");

    void advance_write(const nous::vec3& value, const std::string& name = "");

    void advance_write(const nous::vec4& value, const std::string& name = "");

    void advance_write(const nous::mat3& value, const std::string& name = "");

    void advance_write(const nous::mat4& value, const std::string& name = "");

    void advance_read(nous::vec2& value, const std::string& name = "");

    void advance_read(nous::vec3& value, const std::string& name = "");

    void advance_read(nous::vec4& value, const std::string& name = "");

    void advance_read(nous::mat3& value, const std::string& name = "");

    void advance_read(nous::mat4& value, const std::string& name = "");

private:
    unsigned int try_get_sketch_object_reference(const SketchObjectPtr& skobj);

    unsigned int try_get_sketch_nurbs_point_reference(const SketchNurbsPointPtr& skobj);

    unsigned int try_get_sketch_nurbs_item_reference(const std::shared_ptr<INurbsBase>& skobj);

    SketchObjectPtr try_get_sketch_object(const unsigned int idx);

    SketchNurbsPointPtr try_get_sketch_nurbs_point(const unsigned int idx);

    std::shared_ptr<INurbsBase> try_get_sketch_nurbs_item(const unsigned int idx);

    void parse_all_sketch_objects();

    void parse_all_sketch_nurbs_point();

    void parse_all_sketch_nurbs_items();

private:
    std::unordered_map<SketchObjectPtr, unsigned int> m_sketch_obj_map;

    std::unordered_map<SketchNurbsPointPtr, unsigned int> m_sketch_nurbs_point_map;

    std::unordered_map<std::shared_ptr<INurbsBase>, unsigned int> m_sketch_nurbs_item_map;

    std::vector<std::shared_ptr<INurbsBase>> m_sketch_nurbs_item_arr;

    std::unordered_map<unsigned int, SketchObjectPtr> m_revert_sketch_obj_map;

    std::unordered_map<unsigned int, SketchNurbsPointPtr> m_revert_sketch_nurbs_point_map;

    std::unordered_map<unsigned int, std::shared_ptr<INurbsBase>> m_revert_sketch_nurbs_item_map;
};

};// namespace sketch