#pragma once

#include <memory>

namespace sketch
{

class SketchSerializer;

enum class ESketchObjectType : unsigned int
{
    None,
    GeoData,
    NurbsData
};

class SketchObject
{
public:
    virtual ~SketchObject() = default;
    SketchObject(ESketchObjectType type = ESketchObjectType::None) : m_type(type) {}

    inline void set_type(ESketchObjectType type) { m_type = type; }
    inline ESketchObjectType get_type() const { return m_type; }

    virtual void save(SketchSerializer& serializer) = 0;

    virtual void load(SketchSerializer& serializer) = 0;

private:
    ESketchObjectType m_type;
};

using SketchObjectPtr = std::shared_ptr<SketchObject>;

};// namespace sketch
