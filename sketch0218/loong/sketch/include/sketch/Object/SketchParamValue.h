#pragma once
#include "./SketchObject.h"
#include "nous_base/nous_base.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace sketch
{
class SketchParamMap;
using SketchParamMapPtr = std::shared_ptr<SketchParamMap>;
using SketchParamInnerValue =
    std::variant<std::monostate, bool, char, int, unsigned int, long long, unsigned long long, float, double, nous::vec2, nous::vec3, nous::vec4,
                 nous::mat3, nous::mat4, std::string, std::vector<bool>, std::vector<char>, std::vector<int>, std::vector<unsigned int>,
                 std::vector<long long>, std::vector<unsigned long long>, std::vector<float>, std::vector<double>, std::vector<std::string>,
                 SketchObjectPtr, std::vector<SketchObjectPtr>, std::unordered_set<SketchObjectPtr>, std::vector<nous::vec2>,
                 std::unordered_map<std::string, SketchObjectPtr>, std::vector<nous::vec3>, std::vector<nous::vec4>, SketchParamMapPtr, std::wstring,
                 std::vector<std::wstring>, const char*>;

static constexpr nous::StringView SK_UNDEFINED_VALUE_NAME = "undefined";
static constexpr nous::StringView SK_BOOL_VALUE_NAME = "bool";
static constexpr nous::StringView SK_CHAR_VALUE_NAME = "char";
static constexpr nous::StringView SK_INT_VALUE_NAME = "int";
static constexpr nous::StringView SK_UINT_VALUE_NAME = "uint";
static constexpr nous::StringView SK_LONG_VALUE_NAME = "long";
static constexpr nous::StringView SK_ULONG_VALUE_NAME = "ulong";
static constexpr nous::StringView SK_FLOAT_VALUE_NAME = "float";
static constexpr nous::StringView SK_DOUBLE_VALUE_NAME = "double";
static constexpr nous::StringView SK_OBJECT_VALUE_NAME = "object";
static constexpr nous::StringView SK_STRING_VALUE_NAME = "string";
static constexpr nous::StringView SK_WILD_STRING_VALUE_NAME = "wstring";
static constexpr nous::StringView SK_VECTOR_CONATINER_NAME = "vector";
static constexpr nous::StringView SK_UNORDERED_SET_PREFIX_NAME = "unordered_set";
static constexpr nous::StringView SK_UNORDERED_MAP_PREFIX_NAME = "unordered_map";
static constexpr nous::StringView SK_CCP_VALUE_NAME = "const_char_ptr";
static constexpr nous::StringView SK_EIGEN_VEC_VALUE_NAME = "vec";
static constexpr nous::StringView SK_EIGEN_MAT_VALUE_NAME = "mat";
static constexpr nous::StringView SK_SHARED_PTR_PREFIX_NAME = "shared_ptr";
static constexpr nous::StringView SK_L_ANGLE_BRACKET = "<";
static constexpr nous::StringView SK_R_ANGLE_BRACKET = ">";
static constexpr nous::StringView SK_COMMA = ",";
static constexpr nous::StringView SK_PARAM_MAP_NAME = "param_map";

template<typename T> struct sketch_value_type_name
{
    static constexpr nous::StringView value = SK_UNDEFINED_VALUE_NAME;
};

template<> struct sketch_value_type_name<bool>
{
    static constexpr nous::StringView value = SK_BOOL_VALUE_NAME;
};

template<> struct sketch_value_type_name<char>
{
    static constexpr nous::StringView value = SK_CHAR_VALUE_NAME;
};

template<> struct sketch_value_type_name<int>
{
    static constexpr nous::StringView value = SK_INT_VALUE_NAME;
};

template<> struct sketch_value_type_name<unsigned int>
{
    static constexpr nous::StringView value = SK_UINT_VALUE_NAME;
};

template<> struct sketch_value_type_name<long long>
{
    static constexpr nous::StringView value = SK_LONG_VALUE_NAME;
};

template<> struct sketch_value_type_name<unsigned long long>
{
    static constexpr nous::StringView value = SK_ULONG_VALUE_NAME;
};

template<> struct sketch_value_type_name<float>
{
    static constexpr nous::StringView value = SK_FLOAT_VALUE_NAME;
};

template<> struct sketch_value_type_name<double>
{
    static constexpr nous::StringView value = SK_DOUBLE_VALUE_NAME;
};

template<> struct sketch_value_type_name<SketchObject>
{
    static constexpr nous::StringView value = SK_OBJECT_VALUE_NAME;
};

template<> struct sketch_value_type_name<std::string>
{
    static constexpr nous::StringView value = SK_STRING_VALUE_NAME;
};

template<> struct sketch_value_type_name<std::wstring>
{
    static constexpr nous::StringView value = SK_WILD_STRING_VALUE_NAME;
};

template<> struct sketch_value_type_name<const char*>
{
    static constexpr nous::StringView value = SK_CCP_VALUE_NAME;
};

template<> struct sketch_value_type_name<SketchParamMapPtr>
{
    static constexpr nous::StringView value = SK_PARAM_MAP_NAME;
};

template<typename T>
static constexpr auto sketch_value_type_name_with_bractet_simple =
    nous::ctti::join_sv<SK_L_ANGLE_BRACKET, sketch_value_type_name<T>::value, SK_R_ANGLE_BRACKET>;
template<typename A, typename B>
static constexpr auto sketch_value_type_name_with_bractet_pair =
    nous::ctti::join_sv<SK_L_ANGLE_BRACKET, sketch_value_type_name<A>::value, SK_COMMA, sketch_value_type_name<B>::value, SK_R_ANGLE_BRACKET>;

template<typename T> struct sketch_value_type_name<std::vector<T>>
{
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_VECTOR_CONATINER_NAME, sketch_value_type_name_with_bractet_simple<T>>;
};

template<typename T> struct sketch_value_type_name<std::unordered_set<T>>
{
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_UNORDERED_SET_PREFIX_NAME, sketch_value_type_name_with_bractet_simple<T>>;
};

template<typename K, typename V> struct sketch_value_type_name<std::unordered_map<K, V>>
{
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_UNORDERED_MAP_PREFIX_NAME, sketch_value_type_name_with_bractet_pair<K, V>>;
};

template<typename T, int N> struct sketch_value_type_name<Eigen::Vector<T, N>>
{
    static constexpr nous::StringView N_SV = nous::ctti::to_string<N>();
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_EIGEN_VEC_VALUE_NAME, N_SV, sketch_value_type_name_with_bractet_simple<T>>;
};

template<typename T, int N> struct sketch_value_type_name<Eigen::Matrix<T, N, N>>
{
    static constexpr nous::StringView N_SV = nous::ctti::to_string<N>();
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_EIGEN_MAT_VALUE_NAME, N_SV, sketch_value_type_name_with_bractet_simple<T>>;
};

template<typename T> struct sketch_value_type_name<std::shared_ptr<T>>
{
    static constexpr nous::StringView value = nous::ctti::join_sv<SK_SHARED_PTR_PREFIX_NAME, sketch_value_type_name_with_bractet_simple<T>>;
};

template<typename T> std::string_view get_sketch_value_type_name()
{
    nous::StringView type_name = sketch_value_type_name<T>::value;
    return type_name.data();
}

enum class ESketchParamValueType : uint8_t
{
    Invalid = 0,
    Boolean,
    Char,
    Integer,
    UnsignedInteger,
    Long,
    UnsignedLong,
    Float,
    Double,
    Vector2,
    Vector3,
    Vector4,
    Matrix3x3,
    Matrix4x4,
    String,
    BooleanArray,
    CharArray,
    IntegerArray,
    UnsignedIntegerArray,
    LongArray,
    UnsignedLongArray,
    FloatArray,
    DoubleArray,
    StringArray,
    SketchObjectPtr,
    SketchObjectPtrArray,
    SketchObjectPtrSet,
    SketchObjectPtrMap,
    Vector2Array,
    Vector3Array,
    Vector4Array,
    SketchParamMap,
    WildString,
    WildStringArray,
    ConstCharPtr
};

class SketchParamValue
{
public:
    SketchParamValue() = default;

    template<typename DataType> bool set_value(const DataType& data)
    {
        m_param = data;
        return true;
    }

    template<typename DataType> const DataType& get_value() const
    {
        const DataType* value = std::get_if<DataType>(&m_param);
        if(!value)
        {
            if(m_param.index() != 0)
            {
                throw -1;
            }
        }
        return *value;
    }

    bool set_param(const SketchParamInnerValue& value)
    {
        m_param = value;
        return true;
    }

    ESketchParamValueType get_type() const;

private:
    SketchParamInnerValue m_param;
};

};// namespace sketch