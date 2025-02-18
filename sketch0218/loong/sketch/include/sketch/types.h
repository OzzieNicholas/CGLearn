#pragma once

#include <EASTL/type_traits.h>
#include <nous_base/color.h>
#include <nous_base/math.h>
#include <nous_base/types.h>

namespace sketch
{
// vector
using vec2 = nous::vec2;
using vec2f = nous::vec2f;
using vec2d = nous::vec2d;
using vec2i = nous::vec2i;

using vec3 = nous::vec3;
using vec3f = nous::vec3f;
using vec3d = nous::vec3d;
using vec3i = nous::vec3i;

using vec4 = nous::vec4;
using vec4f = nous::vec4f;
using vec4d = nous::vec4d;
using vec4i = nous::vec4i;

// matrix
using mat2 = nous::mat2;
using mat2f = nous::mat2f;
using mat2d = nous::mat2d;

using mat3 = nous::mat3;
using mat3f = nous::mat3f;
using mat3d = nous::mat3d;

using mat4 = nous::mat4;
using mat4f = nous::mat4f;
using mat4d = nous::mat4d;

// quaternion
using quat = nous::quat;
using quatf = nous::quatf;
using quatd = nous::quatd;

// transform
using transform2 = nous::transform2;
using transform2f = nous::transform2f;
using transform2d = nous::transform2d;

using transform3 = nous::transform3;
using transform3f = nous::transform3f;
using transform3d = nous::transform3d;

using transform4 = nous::transform4;
using transform4f = nous::transform4f;
using transform4d = nous::transform4d;

// others
using color = nous::color;

}// namespace sketch
