#include "sketch/Object/SketchParamMap.h"
#include "Engine/Util/StrUtil.h"
#include "sketch/NousEngine/NousConvert.h"

namespace sketch
{
std::shared_ptr<SketchParamMap> SketchParamMap::alloc(SketchParamMap& others)
{
    std::shared_ptr<SketchParamMap> result = std::make_shared<SketchParamMap>();

    for(const std::string& field_name: others.get_all_field_names())
    {
        result->set_value_wrapper(field_name, others.get_value_wrapper(field_name));
    }

    return result;
}

void SketchParamMap::copy_from(const SketchParamMap& others)
{
    for(const std::string& field_name: others.get_all_field_names())
    {
        set_value_wrapper(field_name, others.get_value_wrapper(field_name));
    }
}

bool SketchParamMap::has_field(const std::string_view& field_name) const { return m_inner_map.find(field_name.data()) != m_inner_map.end(); }

SketchParamMap::SketchParamMap(const SketchParamMap& others) { this->copy_from(others); }

SketchParamMap::SketchParamMap(std::initializer_list<std::pair<std::string, SketchParamInnerValue>> list)
{
    for(const auto& one_pair: list)
    {
        SketchParamValue param_value;
        param_value.set_param(one_pair.second);
        if(m_inner_map.find(one_pair.first) == m_inner_map.end())
        {
            m_inner_map.insert({ one_pair.first, param_value });
        }
        else
        {
            m_inner_map[one_pair.first] = param_value;
        }
    }
}

ESketchParamValueType SketchParamMap::get_value_type(const std::string& name) const
{
    const auto map_iter = m_inner_map.find(name);
    if(map_iter == m_inner_map.end())
    {
        return ESketchParamValueType::Invalid;
    }

    return map_iter->second.get_type();
}

bool SketchParamMap::remove_value(const std::string& name)
{
    if(m_inner_map.find(name) != m_inner_map.end())
    {
        m_inner_map.erase(name);
        return true;
    }

    return false;
}

bool SketchParamMap::save(SketchSerializer& serializer) const
{
    for(auto& param_pair: m_inner_map)
    {
        const std::string& param_name = param_pair.first;
        const SketchParamValue& param_value = param_pair.second;

        ESketchParamValueType param_type = param_value.get_type();

        switch(param_type)
        {
        case ESketchParamValueType::Boolean: {
            const bool& bool_v = param_value.get_value<bool>();
            serialize_value(serializer, param_name, bool_v);
        }
        break;
        case ESketchParamValueType::Char: {
            const char& char_v = param_value.get_value<char>();
            serialize_value(serializer, param_name, char_v);
        }
        break;
        case ESketchParamValueType::Integer: {
            const int& int_v = param_value.get_value<int>();
            serialize_value(serializer, param_name, int_v);
        }
        break;
        case ESketchParamValueType::UnsignedInteger: {
            const unsigned int& uint_v = param_value.get_value<unsigned int>();
            serialize_value(serializer, param_name, uint_v);
        }
        break;
        case ESketchParamValueType::Long: {
            const long long& long_v = param_value.get_value<long long>();
            serialize_value(serializer, param_name, long_v);
        }
        break;
        case ESketchParamValueType::UnsignedLong: {
            const unsigned long long& ulong_v = param_value.get_value<unsigned long long>();
            serialize_value(serializer, param_name, ulong_v);
        }
        break;
        case ESketchParamValueType::Float: {
            const float& float_v = param_value.get_value<float>();
            serialize_value(serializer, param_name, float_v);
        }
        break;
        case ESketchParamValueType::Double: {
            const double& double_v = param_value.get_value<double>();
            serialize_value(serializer, param_name, double_v);
        }
        break;
        case ESketchParamValueType::Vector2: {
            const nous::vec2& vec2_v = param_value.get_value<nous::vec2>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<nous::vec2>().data()), "type");
            serializer.advance_write(vec2_v, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Vector3: {
            const nous::vec3& vec3_v = param_value.get_value<nous::vec3>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<nous::vec3>().data()), "type");
            serializer.advance_write(vec3_v, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Vector4: {
            const nous::vec4& vec4_v = param_value.get_value<nous::vec4>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<nous::vec4>().data()), "type");
            serializer.advance_write(vec4_v, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Matrix3x3: {
            const nous::mat3& mat3_v = param_value.get_value<nous::mat3>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<nous::mat3>().data()), "type");
            serializer.advance_write(mat3_v, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Matrix4x4: {
            const nous::mat4& mat4_v = param_value.get_value<nous::mat4>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<nous::mat4>().data()), "type");
            serializer.advance_write(mat4_v, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::String: {
            const std::string& string_v = param_value.get_value<std::string>();
            serialize_value(serializer, param_name, string_v);
        }
        break;
        case ESketchParamValueType::BooleanArray: {
            const std::vector<bool>& bool_arr = param_value.get_value<std::vector<bool>>();
            serialize_array(serializer, param_name, bool_arr);
        }
        break;
        case ESketchParamValueType::CharArray: {
            const std::vector<char>& char_arr = param_value.get_value<std::vector<char>>();
            serialize_array(serializer, param_name, char_arr);
        }
        break;
        case ESketchParamValueType::IntegerArray: {
            const std::vector<int>& int_arr = param_value.get_value<std::vector<int>>();
            serialize_array(serializer, param_name, int_arr);
        }
        break;
        case ESketchParamValueType::UnsignedIntegerArray: {
            const std::vector<unsigned int>& uint_arr = param_value.get_value<std::vector<unsigned int>>();
            serialize_array(serializer, param_name, uint_arr);
        }
        break;
        case ESketchParamValueType::LongArray: {
            const std::vector<long long>& long_arr = param_value.get_value<std::vector<long long>>();
            serialize_array(serializer, param_name, long_arr);
        }
        break;
        case ESketchParamValueType::UnsignedLongArray: {
            const std::vector<unsigned long long>& ulong_arr = param_value.get_value<std::vector<unsigned long long>>();
            serialize_array(serializer, param_name, ulong_arr);
        }
        break;
        case ESketchParamValueType::FloatArray: {
            const std::vector<float>& float_arr = param_value.get_value<std::vector<float>>();
            serialize_array(serializer, param_name, float_arr);
        }
        break;
        case ESketchParamValueType::DoubleArray: {
            const std::vector<double>& double_arr = param_value.get_value<std::vector<double>>();
            serialize_array(serializer, param_name, double_arr);
        }
        break;
        case ESketchParamValueType::StringArray: {
            const std::vector<std::string>& string_arr = param_value.get_value<std::vector<std::string>>();
            serialize_array(serializer, param_name, string_arr);
        }
        break;
        case ESketchParamValueType::SketchObjectPtr: {
            const SketchObjectPtr skobj = param_value.get_value<SketchObjectPtr>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<SketchObjectPtr>().data()), "type");
            serializer.write_sketch_object_ref(skobj, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::SketchObjectPtrArray: {
            const std::vector<SketchObjectPtr> skobj_arr = param_value.get_value<std::vector<SketchObjectPtr>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<SketchObjectPtr>>().data()), "type");
            serializer.begin_write_array("data");
            for(int i = 0; i < skobj_arr.size(); ++i)
            {
                serializer.write_sketch_object_ref(skobj_arr[i]);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::SketchObjectPtrSet: {
            const std::unordered_set<SketchObjectPtr> skobj_set = param_value.get_value<std::unordered_set<SketchObjectPtr>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<SketchObjectPtr>>().data()), "type");
            serializer.begin_write_array("data");
            for(const auto& skobj: skobj_set)
            {
                serializer.write_sketch_object_ref(skobj);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::SketchObjectPtrMap: {
            const std::unordered_map<std::string, SketchObjectPtr> skobj_map =
                param_value.get_value<std::unordered_map<std::string, SketchObjectPtr>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::map<std::string, SketchObjectPtr>>().data()), "type");
            serializer.begin_write_object("data");
            for(const auto& skobj_pair: skobj_map)
            {
                serializer.write_sketch_object_ref(skobj_pair.second, skobj_pair.first);
            }
            serializer.end_write_object();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Vector2Array: {
            const std::vector<nous::vec2>& vec2_arr = param_value.get_value<std::vector<nous::vec2>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<nous::vec2>>().data()), "type");
            serializer.begin_write_array("data");
            for(int i = 0; i < vec2_arr.size(); ++i)
            {
                const nous::vec2 vec2_v = vec2_arr[i];
                serializer.advance_write(vec2_v);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Vector3Array: {
            const std::vector<nous::vec3>& vec3_arr = param_value.get_value<std::vector<nous::vec3>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<nous::vec3>>().data()), "type");
            serializer.begin_write_array("data");
            for(int i = 0; i < vec3_arr.size(); ++i)
            {
                const nous::vec3 vec3_v = vec3_arr[i];
                serializer.advance_write(vec3_v);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::Vector4Array: {
            const std::vector<nous::vec4>& vec4_arr = param_value.get_value<std::vector<nous::vec4>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<nous::vec4>>().data()), "type");
            serializer.begin_write_array("data");
            for(int i = 0; i < vec4_arr.size(); ++i)
            {
                const nous::vec4 vec4_v = vec4_arr[i];
                serializer.advance_write(vec4_v);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::SketchParamMap: {
            const auto param_map = param_value.get_value<SketchParamMapPtr>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<SketchParamMap>().data()), "type");
            serializer.begin_write_object("data");
            param_map->save(serializer);
            serializer.end_write_object();
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::WildString: {
            const std::wstring wild_string = param_value.get_value<std::wstring>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::wstring>().data()), "type");
            nous::String nous_str(Engine::StrUtil::w2s(wild_string).c_str());
            serializer.write(nous_str, "data");
            serializer.end_write_object();
        }
        break;
        case ESketchParamValueType::WildStringArray: {
            const std::vector<std::wstring>& wild_string_arr = param_value.get_value<std::vector<std::wstring>>();
            serializer.begin_write_object(param_name.c_str());
            serializer.write(nous::String(get_sketch_value_type_name<std::vector<std::wstring>>().data()), "type");
            serializer.begin_write_array("data");
            for(int i = 0; i < wild_string_arr.size(); ++i)
            {
                nous::String nous_str(Engine::StrUtil::w2s(wild_string_arr[i]).c_str());
                serializer.write(nous_str);
            }
            serializer.end_write_array();
            serializer.end_write_object();
        }
        default: break;
        }
    }

    return true;
}

bool SketchParamMap::load(SketchSerializer& serializer)
{
    nous::Array<nous::String> all_field_names;
    if(const bool is_success = serializer.try_get_current_object_all_field_names(all_field_names))
    {
        for(const auto& field_name: all_field_names)
        {
            serializer.begin_read_object(field_name);
            nous::String param_value_type_nous_str;
            serializer.read(param_value_type_nous_str, "type");
            std::string param_value_type(param_value_type_nous_str.data());

            SketchParamValue param_value;
            if(param_value_type == get_sketch_value_type_name<bool>())
            {
                bool bool_v;
                serializer.read(bool_v, "data");
                param_value.set_value(bool_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<char>())
            {
                char char_v;
                serializer.read(char_v, "data");
                param_value.set_value(char_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<int>())
            {
                int int_v;
                serializer.read(int_v, "data");
                param_value.set_value(int_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<unsigned int>())
            {
                unsigned int uint_v;
                serializer.read(uint_v, "data");
                param_value.set_value(uint_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<long long>())
            {
                long long long_v;
                serializer.read(long_v, "data");
                param_value.set_value(long_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<unsigned long long>())
            {
                unsigned long long ulong_v;
                serializer.read(ulong_v, "data");
                param_value.set_value(ulong_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<float>())
            {
                float float_v;
                serializer.read(float_v, "data");
                param_value.set_value(float_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<double>())
            {
                double double_v;
                serializer.read(double_v, "data");
                param_value.set_value(double_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<nous::vec2>())
            {
                nous::vec2 vec2_v;
                serializer.advance_read(vec2_v, "data");
                param_value.set_value(vec2_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<nous::vec3>())
            {
                nous::vec3 vec3_v;
                serializer.advance_read(vec3_v, "data");
                param_value.set_value(vec3_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<nous::vec4>())
            {
                nous::vec4 vec4_v;
                serializer.advance_read(vec4_v, "data");
                param_value.set_value(vec4_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<nous::mat3>())
            {
                nous::mat3 mat3_v;
                serializer.advance_read(mat3_v, "data");
                param_value.set_value(mat3_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<nous::mat4>())
            {
                nous::mat4 mat4_v;
                serializer.advance_read(mat4_v, "data");
                param_value.set_value(mat4_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::string>())
            {
                nous::String string_v;
                serializer.read(string_v, "value");
                const std::string std_str_v(string_v.data());
                param_value.set_value(std_str_v);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<bool>>())
            {
                std::vector<bool> bool_arr;
                deserialize_array(serializer, bool_arr);
                param_value.set_value(bool_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<char>>())
            {
                std::vector<char> char_arr;
                deserialize_array(serializer, char_arr);
                param_value.set_value(char_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<int>>())
            {
                std::vector<int> int_arr;
                deserialize_array(serializer, int_arr);
                param_value.set_value(int_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<unsigned int>>())
            {
                std::vector<unsigned int> uint_arr;
                deserialize_array(serializer, uint_arr);
                param_value.set_value(uint_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<long long>>())
            {
                std::vector<long long> long_arr;
                deserialize_array(serializer, long_arr);
                param_value.set_value(long_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<unsigned long long>>())
            {
                std::vector<unsigned long long> ulong_arr;
                deserialize_array(serializer, ulong_arr);
                param_value.set_value(ulong_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<float>>())
            {
                std::vector<float> float_arr;
                deserialize_array(serializer, float_arr);
                param_value.set_value(float_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<double>>())
            {
                std::vector<double> double_arr;
                deserialize_array(serializer, double_arr);
                param_value.set_value(double_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<std::string>>())
            {
                std::vector<std::string> string_arr;
                deserialize_array(serializer, string_arr);
                param_value.set_value(string_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<SketchObjectPtr>())
            {
                SketchObjectPtr skobj = serializer.read_sketch_object_ref("data");
                if(skobj)
                {
                    param_value.set_value(skobj);
                    m_inner_map.insert({ field_name.c_str(), param_value });
                }
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<SketchObjectPtr>>())
            {
                size_t value_count = 0;
                std::vector<SketchObjectPtr> skobj_arr;
                serializer.begin_read_array("data", value_count);
                for(int i = 0; i < value_count; ++i)
                {
                    SketchObjectPtr skobj = serializer.read_sketch_object_ref();
                    if(skobj)
                    {
                        skobj_arr.emplace_back(skobj);
                    }
                }
                serializer.end_read_array();
                param_value.set_value(skobj_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::unordered_set<SketchObjectPtr>>())
            {
                size_t value_count = 0;
                std::unordered_set<SketchObjectPtr> skobj_set;
                serializer.begin_read_array("data", value_count);
                for(int i = 0; i < value_count; ++i)
                {
                    SketchObjectPtr skobj = serializer.read_sketch_object_ref();
                    if(skobj)
                    {
                        skobj_set.insert(skobj);
                    }
                }
                serializer.end_read_array();
                param_value.set_value(skobj_set);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::unordered_map<std::string, SketchObjectPtr>>())
            {
                std::unordered_map<std::string, SketchObjectPtr> skobj_map;
                serializer.begin_read_object("data");
                nous::Array<nous::String> all_field_name_arr;
                serializer.try_get_current_object_all_field_names(all_field_name_arr);
                for(const auto& one_dict_name: all_field_name_arr)
                {
                    SketchObjectPtr skobj = serializer.read_sketch_object_ref(one_dict_name.data());
                    if(skobj)
                    {
                        skobj_map.insert({ one_dict_name.c_str(), skobj });
                    }
                }
                serializer.end_read_object();
                param_value.set_value(skobj_map);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<nous::vec2>>())
            {
                std::vector<nous::vec2> vec2_arr;
                size_t value_count;
                serializer.begin_read_array("data", value_count);
                for(int j = 0; j < value_count; ++j)
                {
                    nous::vec2 vec2_v;
                    serializer.advance_read(vec2_v);
                    vec2_arr.emplace_back(vec2_v);
                }
                serializer.end_read_array();
                param_value.set_value(vec2_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<nous::vec3>>())
            {
                std::vector<nous::vec3> vec3_arr;
                size_t value_count;
                serializer.begin_read_array("data", value_count);
                for(int j = 0; j < value_count; ++j)
                {
                    nous::vec3 vec3_v;
                    serializer.advance_read(vec3_v);
                    vec3_arr.emplace_back(vec3_v);
                }
                serializer.end_read_array();
                param_value.set_value(vec3_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<nous::vec4>>())
            {
                std::vector<nous::vec4> vec4_arr;
                size_t value_count;
                serializer.begin_read_array("data", value_count);
                for(int j = 0; j < value_count; ++j)
                {
                    nous::vec4 vec4_v;
                    serializer.advance_read(vec4_v);
                    vec4_arr.emplace_back(vec4_v);
                }
                serializer.end_read_array();
                param_value.set_value(vec4_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<SketchParamMap>())
            {
                auto param_map = std::make_shared<SketchParamMap>();
                serializer.begin_read_object("data");
                param_map->load(serializer);
                serializer.end_read_object();
                param_value.set_value(param_map);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::wstring>())
            {
                nous::String nous_str;
                serializer.read(nous_str, "data");
                param_value.set_value(Engine::StrUtil::s2w(std::string(nous_str.data())));
                m_inner_map.insert({ field_name.c_str(), param_value });
            }
            else if(param_value_type == get_sketch_value_type_name<std::vector<std::wstring>>())
            {
                std::vector<std::wstring> wild_string_arr;
                size_t array_size = 0;
                serializer.begin_read_array("data", array_size);
                for(int i = 0; i < array_size; ++i)
                {
                    nous::String nous_str;
                    serializer.read(nous_str);
                    wild_string_arr.emplace_back(Engine::StrUtil::s2w(std::string(nous_str.data())));
                }
                serializer.end_read_array();
                param_value.set_value(wild_string_arr);
                m_inner_map.insert({ field_name.c_str(), param_value });
            }

            serializer.end_read_object();
        }
    }

    return true;
}

std::vector<std::string> SketchParamMap::get_all_field_names() const
{
    std::vector<std::string> result;
    for(const auto& pair: m_inner_map)
    {
        result.emplace_back(pair.first);
    }
    return result;
}

SketchParamValue SketchParamMap::get_value_wrapper(const std::string& name) const { return m_inner_map.at(name); }

void SketchParamMap::set_value_wrapper(const std::string& name, SketchParamValue& value) { m_inner_map.insert({ name, value }); }

void SketchParamMap::clear() { m_inner_map.clear(); }

SketchParamMap SketchParamMap::alloc(const Engine::DObject& dobject)
{
    SketchParamMap param_map;

    for(const auto& [fst, snd]: dobject)
    {
        std::string param_name = fst;
        Engine::DValue param_value = snd;
        Engine::DValue::DataType data_type = param_value.type();

        if(data_type == Engine::DValue::DataType::TYPE_BOOL)
        {
            param_map.set_value(param_name, param_value._bool());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_CHAR)
        {
            param_map.set_value(param_name, param_value._char());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_INT)
        {
            param_map.set_value(param_name, param_value._int());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_UINT)
        {
            param_map.set_value(param_name, param_value._uint());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_LONG)
        {
            param_map.set_value(param_name, param_value._long());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_ULONG)
        {
            param_map.set_value(param_name, param_value._ulong());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_FLOAT)
        {
            param_map.set_value(param_name, param_value._float());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_DOUBLE)
        {
            param_map.set_value(param_name, param_value._double());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_SHORT)
        {
            param_map.set_value(param_name, static_cast<int>(param_value._short()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_USHORT)
        {
            param_map.set_value(param_name, static_cast<unsigned int>(param_value._ushort()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_VEC2)
        {
            param_map.set_value(param_name, NousConvert::convert(param_value._vec2()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_VEC3)
        {
            param_map.set_value(param_name, NousConvert::convert(param_value._vec3()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_VEC4)
        {
            param_map.set_value(param_name, NousConvert::convert(param_value._vec4()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_MAT43)
        {
            param_map.set_value(param_name, NousConvert::convert(param_value._mat3()));
        }
        else if(data_type == Engine::DValue::DataType::TYPE_WSTR)
        {
            param_map.set_value(param_name, param_value._wstr());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_STR_ARRAY)
        {
            param_map.set_value(param_name, param_value._wstrVec());
        }
        else if(data_type == Engine::DValue::DataType::TYPE_DOBJECT)
        {
            SketchParamMap child_map = SketchParamMap::alloc(*param_value._dobj());
            auto map_ptr = std::make_shared<SketchParamMap>(child_map);
            param_map.set_value(param_name, map_ptr);
        }
    }

    return param_map;
}

Engine::DObject SketchParamMap::to_dobject() const
{
    Engine::DObject dobject;
    for(const auto& [fst, snd]: m_inner_map)
    {
        const std::string param_name = fst;
        const SketchParamValue param_value = snd;

        ESketchParamValueType param_type = param_value.get_type();
        if(param_type == ESketchParamValueType::Boolean)
        {
            dobject[param_name] = param_value.get_value<bool>();
        }
        else if(param_type == ESketchParamValueType::Char)
        {
            dobject[param_name] = param_value.get_value<char>();
        }
        else if(param_type == ESketchParamValueType::Integer)
        {
            dobject[param_name] = param_value.get_value<int>();
        }
        else if(param_type == ESketchParamValueType::UnsignedInteger)
        {
            dobject[param_name] = param_value.get_value<unsigned int>();
        }
        else if(param_type == ESketchParamValueType::Long)
        {
            dobject[param_name] = param_value.get_value<long long>();
        }
        else if(param_type == ESketchParamValueType::UnsignedLong)
        {
            dobject[param_name] = param_value.get_value<unsigned long long>();
        }
        else if(param_type == ESketchParamValueType::String)
        {
            dobject[param_name] = Engine::StrUtil::s2w(param_value.get_value<std::string>());
        }
        else if(param_type == ESketchParamValueType::ConstCharPtr)
        {
            dobject[param_name] = Engine::StrUtil::s2w(param_value.get_value<const char*>());
        }
        else if(param_type == ESketchParamValueType::WildString)
        {
            dobject[param_name] = param_value.get_value<std::wstring>();
        }
        else if(param_type == ESketchParamValueType::WildStringArray)
        {
            dobject[param_name] = param_value.get_value<std::vector<std::wstring>>();
        }
        else if(param_type == ESketchParamValueType::Vector2)
        {
            dobject[param_name] = NousConvert::convert(param_value.get_value<nous::vec2>());
        }
        else if(param_type == ESketchParamValueType::Vector3)
        {
            dobject[param_name] = NousConvert::convert(param_value.get_value<nous::vec3>());
        }
        else if(param_type == ESketchParamValueType::Vector4)
        {
            dobject[param_name] = NousConvert::convert(param_value.get_value<nous::vec4>());
        }
        else if(param_type == ESketchParamValueType::Matrix4x4)
        {
            dobject[param_name] = NousConvert::convert(param_value.get_value<nous::mat4>());
        }
        else if(param_type == ESketchParamValueType::Float)
        {
            dobject[param_name] = param_value.get_value<float>();
        }
        else if(param_type == ESketchParamValueType::Double)
        {
            dobject[param_name] = param_value.get_value<double>();
        }
    }

    return dobject;
}

};// namespace sketch