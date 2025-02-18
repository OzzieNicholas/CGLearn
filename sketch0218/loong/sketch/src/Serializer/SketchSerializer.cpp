#include "sketch/Serializer/SketchSerializer.h"
#include "sketch/Object/SketchParamValue.h"
#include "sketch/View/RenderRes/GeoData.h"
#include "sketch/View/RenderRes/NurbsData.h"
#include "sketch/View/RenderRes/NurbsElement/NurbsElements.h"

namespace sketch
{
SketchSerializer::SketchSerializer() : nous::Serializer(nous::SerializerFlag::Bson) {}

unsigned int SketchSerializer::try_get_sketch_object_reference(const SketchObjectPtr& skobj)
{
    if(m_sketch_obj_map.find(skobj) == m_sketch_obj_map.end())
    {
        m_sketch_obj_map.insert({ skobj, m_sketch_obj_map.size() });
    }

    return m_sketch_obj_map[skobj];
}

unsigned int SketchSerializer::try_get_sketch_nurbs_point_reference(const SketchNurbsPointPtr& skobj)
{
    if(m_sketch_nurbs_point_map.find(skobj) == m_sketch_nurbs_point_map.end())
    {
        m_sketch_nurbs_point_map.insert({ skobj, m_sketch_nurbs_point_map.size() });
    }

    return m_sketch_nurbs_point_map[skobj];
}

unsigned int SketchSerializer::try_get_sketch_nurbs_item_reference(const std::shared_ptr<INurbsBase>& skobj)
{
    if(m_sketch_nurbs_item_map.find(skobj) == m_sketch_nurbs_item_map.end())
    {
        m_sketch_nurbs_item_map.insert({ skobj, m_sketch_nurbs_item_map.size() });
        m_sketch_nurbs_item_arr.emplace_back(skobj);
    }

    return m_sketch_nurbs_item_map[skobj];
}

SketchObjectPtr SketchSerializer::try_get_sketch_object(const unsigned int idx)
{
    if(m_revert_sketch_obj_map.find(idx) != m_revert_sketch_obj_map.end())
    {
        return m_revert_sketch_obj_map[idx];
    }

    return nullptr;
}

SketchNurbsPointPtr SketchSerializer::try_get_sketch_nurbs_point(const unsigned int idx)
{
    if(m_revert_sketch_nurbs_point_map.find(idx) != m_revert_sketch_nurbs_point_map.end())
    {
        return m_revert_sketch_nurbs_point_map[idx];
    }

    return nullptr;
}

std::shared_ptr<INurbsBase> SketchSerializer::try_get_sketch_nurbs_item(const unsigned int idx)
{
    if(m_revert_sketch_nurbs_item_map.find(idx) != m_revert_sketch_nurbs_item_map.end())
    {
        return m_revert_sketch_nurbs_item_map[idx];
    }

    return nullptr;
}

// 级联搜索所有存在的 object 类型引用
void SketchSerializer::parse_all_sketch_objects()
{
    // try parse all
    begin_write_array("sketch_obj_dict");
    for(const auto& skobj_pair: m_sketch_obj_map)
    {
        begin_write_object();
        const std::string& obj_type = get_sketch_obj_type_name(skobj_pair.first->get_type());
        nous::String obj_type_nous_str(obj_type.c_str());
        write(obj_type_nous_str, "type");
        write(skobj_pair.second, "id");
        begin_write_object("data");
        skobj_pair.first->save(*this);
        end_write_object();
        end_write_object();
    }
    end_write_array();
}

void SketchSerializer::parse_all_sketch_nurbs_point()
{
    begin_write_array("sketch_nurbs_point_dict");
    for(const auto& skpt_pair: m_sketch_nurbs_point_map)
    {
        begin_write_object();
        SketchNurbsPointPtr nurbs_pt = skpt_pair.first;
        advance_write(nurbs_pt->m_point, "pos");
        write(static_cast<char>(nurbs_pt->m_type), "type");
        write(skpt_pair.second, "id");
        end_write_object();
    }
    end_write_array();
}

void SketchSerializer::parse_all_sketch_nurbs_items()
{
    begin_write_object("sketch_nurbs_item_dict");
    for(int i = 0; i < m_sketch_nurbs_item_arr.size(); ++i)
    {
        begin_write_object(std::to_string(i).c_str());
        std::shared_ptr<INurbsBase> nurbs_item = m_sketch_nurbs_item_arr[i];
        const std::string& obj_type = get_sketch_nurbs_item_type_name(nurbs_item->type());
        nous::String obj_type_nous_str(obj_type.c_str());
        write(obj_type_nous_str, "type");
        write(i, "id");
        begin_write_object("data");
        nurbs_item->save(*this);
        end_write_object();
        end_write_object();
    }
    write(m_sketch_nurbs_item_arr.size(), "total_size");
    end_write_object();
}

void SketchSerializer::advance_save(const std::string& path)
{
    parse_all_sketch_objects();
    parse_all_sketch_nurbs_items();
    parse_all_sketch_nurbs_point();
    save(path.c_str());
}

void SketchSerializer::advance_load(const std::string& path)
{
    load(path.c_str());
    if(has_field("sketch_nurbs_point_dict"))
    {
        size_t array_size = 0;
        begin_read_array("sketch_nurbs_point_dict", array_size);
        for(int i = 0; i < array_size; ++i)
        {
            SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>();
            begin_read_object();
            char pt_type_c;
            read(pt_type_c, "type");
            nurbs_point->m_type = static_cast<tinynurbs::ENurbsPointType>(pt_type_c);
            nous::scalar x, y, z;
            advance_read(nurbs_point->m_point, "pos");
            unsigned int pt_id = 0;
            read(pt_id, "id");
            end_read_object();
            m_revert_sketch_nurbs_point_map.insert({ pt_id, nurbs_point });
        }
        end_read_array();
    }

    if(has_field("sketch_nurbs_point_dict"))
    {
        size_t array_size = 0;
        begin_read_array("sketch_nurbs_point_dict", array_size);
        for(int i = 0; i < array_size; ++i)
        {
            SketchNurbsPointPtr nurbs_point = std::make_shared<SketchNurbsPoint>();
            begin_read_object();
            char pt_type_c;
            read(pt_type_c, "type");
            nurbs_point->m_type = static_cast<tinynurbs::ENurbsPointType>(pt_type_c);
            nous::scalar x, y, z;
            advance_read(nurbs_point->m_point, "pos");
            unsigned int pt_id = 0;
            read(pt_id, "id");
            end_read_object();
            m_revert_sketch_nurbs_point_map.insert({ pt_id, nurbs_point });
        }
        end_read_array();
    }

    if(has_field("sketch_nurbs_item_dict"))
    {
        size_t array_size = 0;
        begin_read_object("sketch_nurbs_item_dict");
        read(array_size, "total_size");
        for(int i = 0; i < array_size; ++i)
        {
            int reverse_id = array_size - 1 - i;
            begin_read_object(std::to_string(reverse_id).c_str());
            unsigned int obj_id = 0;
            read(obj_id, "id");
            nous::String nurbs_item_type_name_nous_str;
            read(nurbs_item_type_name_nous_str, "type");
            std::string nurbs_item_type_name(nurbs_item_type_name_nous_str.data());
            const EElementType nurbs_item_type = get_sketch_nurbs_item_type_name(nurbs_item_type_name);
            if(nurbs_item_type == EElementType::Curve)
            {
                NurbsCurvePtr nurbs_curve = std::make_shared<NurbsCurve>();
                begin_read_object("data");
                nurbs_curve->load(*this);
                end_read_object();
                m_revert_sketch_nurbs_item_map.insert({ obj_id, nurbs_curve });
            }
            else if(nurbs_item_type == EElementType::AlgebraSurface)
            {
                NurbsAlgebraSurfacePtr nurbs_surface = std::make_shared<NurbsAlgebraSurface>();
                begin_read_object("data");
                nurbs_surface->load(*this);
                end_read_object();
                m_revert_sketch_nurbs_item_map.insert({ obj_id, nurbs_surface });
            }
            else if(nurbs_item_type == EElementType::ClosedSurface)
            {
                NurbsClosedSurfacePtr nurbs_curve_brep = std::make_shared<NurbsClosedSurface>();
                begin_read_object("data");
                nurbs_curve_brep->load(*this);
                end_read_object();
                m_revert_sketch_nurbs_item_map.insert({ obj_id, nurbs_curve_brep });
            }
            else if(nurbs_item_type == EElementType::TrimmedSurface)
            {
                NurbsTrimmedSurfacePtr nurbs_trimmed_surface = std::make_shared<NurbsTrimmedSurface>();
                begin_read_object("data");
                nurbs_trimmed_surface->load(*this);
                end_read_object();
                m_revert_sketch_nurbs_item_map.insert({ obj_id, nurbs_trimmed_surface });
            }
            end_read_object();
        }
        end_read_object();
    }

    if(has_field("sketch_obj_dict"))
    {
        size_t array_size = 0;
        begin_read_array("sketch_obj_dict", array_size);
        for(int i = 0; i < array_size; ++i)
        {
            begin_read_object();
            unsigned int obj_id = 0;
            read(obj_id, "id");
            nous::String obj_type_name_nous_str;
            read(obj_type_name_nous_str, "type");
            std::string obj_type_name(obj_type_name_nous_str.data());
            const ESketchObjectType obj_type = get_sketch_obj_type_name(obj_type_name);

            if(obj_type == ESketchObjectType::GeoData)
            {
                std::shared_ptr<GeoData> geodata_obj = std::make_shared<GeoData>();
                begin_read_object("data");
                geodata_obj->load(*this);
                end_read_object();
                m_revert_sketch_obj_map.insert({ obj_id, geodata_obj });
            }
            else if(obj_type == ESketchObjectType::NurbsData)
            {
                std::shared_ptr<NurbsData> nurbsdata_obj = std::make_shared<NurbsData>();
                begin_read_object("data");
                nurbsdata_obj->load(*this);
                end_read_object();
                m_revert_sketch_obj_map.insert({ obj_id, nurbsdata_obj });
            }

            end_read_object();
        }
        end_read_array();
    }
}

std::string SketchSerializer::get_sketch_obj_type_name(const ESketchObjectType type)
{
    switch(type)
    {
    case ESketchObjectType::GeoData: {
        return "geodata";
    }
    case ESketchObjectType::NurbsData: {
        return "nurbsdata";
    }
    default: break;
    }

    return "none";
}

ESketchObjectType SketchSerializer::get_sketch_obj_type_name(const std::string& type_name)
{
    if(type_name == "geodata")
    {
        return ESketchObjectType::GeoData;
    }
    else if(type_name == "nurbsdata")
    {
        return ESketchObjectType::NurbsData;
    }

    return ESketchObjectType::None;
}

std::string SketchSerializer::get_sketch_nurbs_item_type_name(const EElementType type)
{
    switch(type)
    {
    case EElementType::Curve: {
        return "curve";
    }
    case EElementType::AlgebraSurface: {
        return "algebra_surface";
    }
    case EElementType::ClosedSurface: {
        return "closed_surface";
    }
    case EElementType::TrimmedSurface: {
        return "complex_surface";
    }
    default: break;
    }

    return "none";
}

EElementType SketchSerializer::get_sketch_nurbs_item_type_name(const std::string& type_name)
{
    if(type_name == "curve")
    {
        return EElementType::Curve;
    }
    else if(type_name == "surface")
    {
        return EElementType::AlgebraSurface;
    }
    else if(type_name == "curve_brep")
    {
        return EElementType::ClosedSurface;
    }
    else if(type_name == "complex_surface")
    {
        return EElementType::TrimmedSurface;
    }

    return EElementType::None;
}

void SketchSerializer::write_sketch_object_ref(const SketchObjectPtr& skt_obj, std::string name)
{
    const unsigned int ref_idx = try_get_sketch_object_reference(skt_obj);
    write(ref_idx, name.c_str());
}

void SketchSerializer::write_sketch_nurbs_point_ref(const SketchNurbsPointPtr& skt_nurbs_pt, std::string name)
{
    const unsigned int ref_idx = try_get_sketch_nurbs_point_reference(skt_nurbs_pt);
    write(ref_idx, name.c_str());
}

void SketchSerializer::write_sketch_nurbs_item_ref(const std::shared_ptr<INurbsBase>& skt_nurbs_item, std::string name)
{
    const unsigned int ref_idx = try_get_sketch_nurbs_item_reference(skt_nurbs_item);
    write(ref_idx, name.c_str());
}

SketchObjectPtr SketchSerializer::read_sketch_object_ref(std::string name)
{
    unsigned int ref_idx = 0;
    read(ref_idx, name.c_str());
    SketchObjectPtr skobj = try_get_sketch_object(ref_idx);
    return skobj;
}

SketchNurbsPointPtr SketchSerializer::read_sketch_nurbs_point_ref(std::string name)
{
    unsigned int ref_idx = 0;
    read(ref_idx, name.c_str());
    SketchNurbsPointPtr skpt = try_get_sketch_nurbs_point(ref_idx);
    return skpt;
}

std::shared_ptr<INurbsBase> SketchSerializer::read_sketch_nurbs_item_ref(std::string name)
{
    unsigned int ref_idx = 0;
    read(ref_idx, name.c_str());
    std::shared_ptr<INurbsBase> nurbs_base = try_get_sketch_nurbs_item(ref_idx);
    return nurbs_base;
}

void SketchSerializer::advance_write(const nous::vec2& value, const std::string& name)
{
    begin_write_object(name.c_str());
    write(value.x(), "x");
    write(value.y(), "y");
    end_write_object();
}

void SketchSerializer::advance_write(const nous::vec3& value, const std::string& name)
{
    begin_write_object(name.c_str());
    write(value.x(), "x");
    write(value.y(), "y");
    write(value.z(), "z");
    end_write_object();
}

void SketchSerializer::advance_write(const nous::vec4& value, const std::string& name)
{
    begin_write_object(name.c_str());
    write(value.x(), "x");
    write(value.y(), "y");
    write(value.z(), "z");
    write(value.w(), "w");
    end_write_object();
}

void SketchSerializer::advance_write(const nous::mat3& value, const std::string& name)
{
    begin_write_array(name.c_str());
    for(int i = 0; i < nous::mat3::SizeAtCompileTime; ++i)
    {
        write(value.coeff(i));
    }
    end_write_array();
}

void SketchSerializer::advance_write(const nous::mat4& value, const std::string& name)
{
    begin_write_array(name.c_str());
    for(int i = 0; i < nous::mat4::SizeAtCompileTime; ++i)
    {
        write(value.coeff(i));
    }
    end_write_array();
}

void SketchSerializer::advance_read(nous::vec2& value, const std::string& name)
{
    begin_read_object(name.c_str());
    nous::scalar x, y;
    read(x, "x");
    read(y, "y");
    end_read_object();
    value = nous::vec2(x, y);
}

void SketchSerializer::advance_read(nous::vec3& value, const std::string& name)
{
    begin_read_object(name.c_str());
    nous::scalar x, y, z;
    read(x, "x");
    read(y, "y");
    read(z, "z");
    end_read_object();
    value = nous::vec3(x, y, z);
}

void SketchSerializer::advance_read(nous::vec4& value, const std::string& name)
{
    begin_read_object(name.c_str());
    nous::scalar x, y, z, w;
    read(x, "x");
    read(y, "y");
    read(z, "z");
    read(w, "w");
    end_read_object();
    value = nous::vec4(x, y, z, w);
}

void SketchSerializer::advance_read(nous::mat3& value, const std::string& name)
{
    begin_read_array(name.c_str());
    for(int i = 0; i < nous::mat3::SizeAtCompileTime; ++i)
    {
        read(value.data()[i]);
    }
    end_read_array();
}

void SketchSerializer::advance_read(nous::mat4& value, const std::string& name)
{
    begin_read_array(name.c_str());
    for(int i = 0; i < nous::mat4::SizeAtCompileTime; ++i)
    {
        read(value.data()[i]);
    }
    end_read_array();
}

};// namespace sketch