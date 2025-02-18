#pragma once

#include <string>

using namespace System;
using namespace System::Collections::Generic;

namespace Engine
{
	class DObject;
	class DValue;
	class Vec2;
	class Vec3;
	class Vec4;
	class Vec2i;
	class Vec3i;
	class Vec4i;
	class Mat43;
	class Mat4;
	class Line;
	class AxisPlane;
	class Guid;
	class OBB;
	class AABB;
}

#pragma make_public(Engine::DObject)
#pragma make_public(Engine::DValue)
#pragma make_public(Engine::Vec2)
#pragma make_public(Engine::Vec3)
#pragma make_public(Engine::Vec4)
#pragma make_public(Engine::Vec2i)
#pragma make_public(Engine::Vec3i)
#pragma make_public(Engine::Vec4i)
#pragma make_public(Engine::Mat43)
#pragma make_public(Engine::Mat4)
#pragma make_public(Engine::Line)
#pragma make_public(Engine::AxisPlane)
#pragma make_public(Engine::Guid)
#pragma make_public(Engine::OBB)
#pragma make_public(Engine::AABB)

namespace CLRUtils
{
	public ref class wrUtil
	{
	public:
		static void copyDObj(LoongCAD::Util::DObject ^pDst, const Engine::DObject* pSrc);
		static void copyDObj(Engine::DObject* pDst, LoongCAD::Util::DObject ^pSrc);

		static LoongCAD::Util::DObject^ wrap(const Engine::DObject* v);
		static System::Object^ wrap(const Engine::DValue& v);
		static LoongCAD::Geometry::Vec2 wrap(const Engine::Vec2& v);
		static LoongCAD::Geometry::Vec3 wrap(const Engine::Vec3& v);
		static LoongCAD::Geometry::Vec4 wrap(const Engine::Vec4& v);
		static LoongCAD::Geometry::Vec2i wrap(const Engine::Vec2i& v);
		static LoongCAD::Geometry::Vec3i wrap(const Engine::Vec3i& v);
		static LoongCAD::Geometry::Vec4i wrap(const Engine::Vec4i& v);
		static LoongCAD::Geometry::Mat43 wrap(const Engine::Mat43& v);
		static LoongCAD::Geometry::Mat4 wrap(const Engine::Mat4& v);
		static LoongCAD::Geometry::Line wrap(const Engine::Line& v);
		static LoongCAD::Geometry::Plane^ wrap(const Engine::AxisPlane& v);
		static System::Guid wrap(const Engine::Guid& v);
		static LoongCAD::Geometry::OBB wrap(const Engine::OBB& v);
		static LoongCAD::Geometry::AABB wrap(const Engine::AABB& v);

		static bool unwrap(Engine::DValue* pDst, System::Object^ pSrc);
		static Engine::Vec2 unwrap(const LoongCAD::Geometry::Vec2^ v);
		static Engine::Vec3 unwrap(const LoongCAD::Geometry::Vec3^ v);
		static Engine::Vec4 unwrap(const LoongCAD::Geometry::Vec4^ v);
		static Engine::Vec2i unwrap(const LoongCAD::Geometry::Vec2i^ v);
		static Engine::Vec3i unwrap(const LoongCAD::Geometry::Vec3i^ v);
		static Engine::Vec4i unwrap(const LoongCAD::Geometry::Vec4i^ v);
		static Engine::Mat43 unwrap(const LoongCAD::Geometry::Mat43^ v);
		static Engine::Mat4 unwrap(const LoongCAD::Geometry::Mat4^ v);
		static Engine::Line unwrap(const LoongCAD::Geometry::Line^ v);
		static Engine::AxisPlane unwrap(LoongCAD::Geometry::Plane^ v);
		static Engine::Guid unwrap(System::Guid^ v);
		static Engine::OBB unwrap(LoongCAD::Geometry::OBB^ v);
		static Engine::AABB unwrap(LoongCAD::Geometry::AABB^ v);
	};
}
