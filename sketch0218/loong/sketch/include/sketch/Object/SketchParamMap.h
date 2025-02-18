#pragma once

#include "./SketchParamValue.h"
#include "nous_base/nous_base.h"
#include "sketch/Serializer/SketchSerializer.h"
#include <Engine/Util/DObject.h>
#include <any>
#include <string>

namespace sketch
{
class SketchParamMap
{
public:
    static std::shared_ptr<SketchParamMap> alloc(SketchParamMap& others);

    void copy_from(const SketchParamMap& others);

    SketchParamMap() = default;

    SketchParamMap(const SketchParamMap& others);

    SketchParamMap(std::initializer_list<std::pair<std::string, SketchParamInnerValue>> list);

    bool has_field(const std::string_view& field_name) const;

    template<typename DataType> bool set_value(const std::string& name, const DataType& val)
    {
        SketchParamValue data_value;
        data_value.set_value(val);
        m_inner_map[name] = data_value;
        return true;
    }

    bool set_value(const std::string& name, const SketchParamMap& val)
    {
        SketchParamValue data_value;
        data_value.set_value(std::make_shared<SketchParamMap>(val));
        m_inner_map[name] = data_value;
        return true;
    }

    template<typename DataType> bool get_value(const std::string& name, DataType& val) const
    {
        auto& map_iter = m_inner_map.find(name);
        if(map_iter == m_inner_map.end())
        {
            return false;
        }

        const SketchParamValue data_value = map_iter->second;
        val = data_value.get_value<DataType>();
        return true;
    }

    template<typename DataType> DataType get_value(const std::string& name) const
    {
        auto& map_iter = m_inner_map.find(name);
        if(map_iter == m_inner_map.end())
        {
            return false;
        }

        const SketchParamValue data_value = map_iter->second;
        return data_value.get_value<DataType>();
    }

    bool remove_value(const std::string& name);

    ESketchParamValueType get_value_type(const std::string& name) const;

    bool save(SketchSerializer& serializer) const;

    bool load(SketchSerializer& serializer);

    std::vector<std::string> get_all_field_names() const;

    SketchParamValue get_value_wrapper(const std::string& name) const;

    void set_value_wrapper(const std::string& name, SketchParamValue& value);

    // static SketchParamMap* alloc(const Engine::DObject* dobject);

    static SketchParamMap alloc(const Engine::DObject& dobject);

    Engine::DObject to_dobject() const;

    void clear();

private:
    template<typename T> void serialize_array(SketchSerializer& serializer, const std::string& param_name, const std::vector<T>& param_arr) const
    {
        serializer.begin_write_object(param_name.c_str());
        serializer.write(nous::String(get_sketch_value_type_name<std::vector<T>>().data()), "type");
        serializer.begin_write_array("data");
        for(int i = 0; i < param_arr.size(); ++i)
        {
            serializer.write(param_arr[i]);
        }
        serializer.end_write_array();
        serializer.end_write_object();
    }

    template<typename T> void deserialize_array(SketchSerializer& serializer, std::vector<T>& param_arr)
    {
        size_t value_count = 0;
        serializer.begin_read_array("data", value_count);
        for(int i = 0; i < value_count; ++i)
        {
            T value;
            serializer.read(value);
            param_arr.emplace_back(value);
        }
        serializer.end_read_array();
    }

    template<typename T> void serialize_value(SketchSerializer& serializer, const std::string& param_name, const T& param_value) const
    {
        serializer.begin_write_object(param_name.c_str());
        serializer.write(nous::String(get_sketch_value_type_name<T>().data()), "type");
        serializer.write(param_value, "data");
        serializer.end_write_object();
    }

    template<> void serialize_value(SketchSerializer& serializer, const std::string& param_name, const std::string& param_value) const
    {
        serializer.begin_write_object(param_name.c_str());
        serializer.write(nous::String(get_sketch_value_type_name<std::string>().data()), "type");
        nous::String nous_str(param_value.c_str());
        serializer.write(nous_str, "data");
        serializer.end_write_object();
    }

    template<> void serialize_array(SketchSerializer& serializer, const std::string& param_name, const std::vector<std::string>& param_arr) const
    {
        serializer.begin_write_object(param_name.c_str());
        serializer.write(nous::String(get_sketch_value_type_name<std::vector<std::string>>().data()), "type");
        serializer.begin_write_array("data");
        for(int i = 0; i < param_arr.size(); ++i)
        {
            nous::String nous_str(param_arr[i].c_str());
            serializer.write(nous_str);
        }
        serializer.end_write_array();
        serializer.end_write_object();
    }

    template<> void deserialize_array(SketchSerializer& serializer, std::vector<std::string>& param_arr)
    {
        size_t value_count = 0;
        serializer.begin_read_array("data", value_count);
        for(int i = 0; i < value_count; ++i)
        {
            nous::String value;
            serializer.read(value);
            param_arr.emplace_back(value.c_str());
        }
        serializer.end_read_array();
    }

private:
    std::unordered_map<std::string, SketchParamValue> m_inner_map;
};

}// namespace sketch