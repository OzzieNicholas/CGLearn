#include "sketch/Object/SketchParamValue.h"

namespace sketch
{

ESketchParamValueType SketchParamValue::get_type() const { return static_cast<ESketchParamValueType>(m_param.index()); }

};// namespace sketch