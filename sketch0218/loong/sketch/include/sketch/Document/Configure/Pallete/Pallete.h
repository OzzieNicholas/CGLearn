#pragma once

#include "..\..\..\Core.h"
#include <string>

#include "Engine\Math\Vec3f.h"
#include "Engine\Util\DObject.h"

namespace sketch
{
	class Pallete
	{
	public:
		Pallete(const std::wstring& name);
		~Pallete(void);

		inline const std::wstring& name() const {
			return m_name;
		}

		void modify(const Engine::DObject& props);

		inline const Engine::DObject& props() {
			this->_updateProps();
			return m_props;
		}

		inline const Engine::Vec3f& viewBackTopLeft() const {
			return m_viewBackTopLeft;
		}

		inline const Engine::Vec3f& viewBackTopRight() const {
			return m_viewBackTopRight;
		}

		inline const Engine::Vec3f& viewBackBottomLeft() const {
			return m_viewBackBottomLeft;
		}

		inline const Engine::Vec3f& viewBackBottomRight() const {
			return m_viewBackBottomRight;
		}

		inline const Engine::Vec3f& viewBackWFTopLeft() const {
			return m_viewBackWFTopLeft;
		}

		inline const Engine::Vec3f& viewBackWFTopRight() const {
			return m_viewBackWFTopRight;
		}

		inline const Engine::Vec3f& viewBackWFBottomLeft() const {
			return m_viewBackWFBottomLeft;
		}

		inline const Engine::Vec3f& viewBackWFBottomRight() const {
			return m_viewBackWFBottomRight;
		}

		inline const Engine::Vec3f& cellBorder() const {
			return m_cellBorder;
		}

		inline const Engine::Vec3f& cellPickBorder() const {
			return m_cellPickBorder;
		}

		inline const Engine::Vec3f& workLine() const {
			return m_workLine;
		}

		inline const Engine::Vec3f& snapTag() const {
			return m_snapTag;
		}

		inline const Engine::Vec3f& snapTagRef() const {
			return m_snapTagRef;
		}

		inline const Engine::Vec3f& snapPlane() const {
			return m_snapPlane;
		}

		inline const Engine::Vec3f& objHover() const {
			return m_objHover;
		}

		inline const Engine::Vec3f& objPick() const {
			return m_objPick;
		}

		inline const Engine::Vec3f& objHL() const {
			return m_objHL;
		}

		inline const Engine::Vec3f& objLabel() const {
			return m_objLabel;
		}

		inline const Engine::Vec3f& ctrlPoint() const {
			return m_ctrlPoint;
		}

		inline const Engine::Vec3f& ctrlPointFirst() const {
			return m_ctrlPointFirst;
		}

		inline const Engine::Vec3f& gridOnView() const {
			return m_gridOnView;
		}

		inline const Engine::Vec3f& gridOnWork() const {
			return m_gridOnWork;
		}

		inline const Engine::Vec3f& gridPlane() const {
			return m_gridPlane;
		}

		inline const Engine::Vec3f& point() const {
			return m_point;
		}

		inline const Engine::Vec3f& pointOnPlane() const {
			return m_pointOnPlane;
		}

		inline const Engine::Vec3f& boltInWorkshop() const {
			return m_boltInWorkshop;
		}

		inline const Engine::Vec3f& boltInSite() const {
			return m_boltInSite;
		}

		inline const Engine::Vec3f& holeInWorkshop() const {
			return m_holeInWorkshop;
		}

		inline const Engine::Vec3f& holeInSite() const {
			return m_holeInSite;
		}

		inline const Engine::Vec3f& weldInWorkshop() const {
			return m_weldInWorkshop;
		}

		inline const Engine::Vec3f& weldInSite() const {
			return m_weldInSite;
		}

		inline const Engine::Vec3f& measure() const {
			return m_measure;
		}

		inline const Engine::Vec3f& drBack() const {
			return m_drBack;
		}

		inline const Engine::Vec3f& drWorkLine() const {
			return m_drWorkLine;
		}

		inline const Engine::Vec3f& drSnapTag() const {
			return m_drSnapTag;
		}

		inline const Engine::Vec3f& drSnapTagRef() const {
			return m_drSnapTagRef;
		}

		inline const Engine::Vec3f drHiddenEle() const {
			return m_drHiddenEle;
		}

	protected:
		void _updateProps();
		
	protected:
		std::wstring m_name;

		Engine::DObject m_props;

		Engine::Vec3f m_viewBackTopLeft;
		Engine::Vec3f m_viewBackTopRight;
		Engine::Vec3f m_viewBackBottomLeft;
		Engine::Vec3f m_viewBackBottomRight;
		Engine::Vec3f m_viewBackWFTopLeft;
		Engine::Vec3f m_viewBackWFTopRight;
		Engine::Vec3f m_viewBackWFBottomLeft;
		Engine::Vec3f m_viewBackWFBottomRight;
		Engine::Vec3f m_cellBorder;
		Engine::Vec3f m_cellPickBorder;
		Engine::Vec3f m_workLine;
		Engine::Vec3f m_snapTag;
		Engine::Vec3f m_snapTagRef;
		Engine::Vec3f m_snapPlane;
		Engine::Vec3f m_objHover;
		Engine::Vec3f m_objPick;
		Engine::Vec3f m_objHL;
		Engine::Vec3f m_objLabel;
		Engine::Vec3f m_ctrlPoint;
		Engine::Vec3f m_ctrlPointFirst;
		Engine::Vec3f m_gridOnView;
		Engine::Vec3f m_gridOnWork;
		Engine::Vec3f m_gridPlane;
		Engine::Vec3f m_point;
		Engine::Vec3f m_pointOnPlane;
		Engine::Vec3f m_boltInWorkshop;
		Engine::Vec3f m_boltInSite;
		Engine::Vec3f m_holeInWorkshop;
		Engine::Vec3f m_holeInSite;
		Engine::Vec3f m_weldInWorkshop;
		Engine::Vec3f m_weldInSite;
		Engine::Vec3f m_measure;

		Engine::Vec3f m_drBack;
		Engine::Vec3f m_drWorkLine;
		Engine::Vec3f m_drSnapTag;
		Engine::Vec3f m_drSnapTagRef;
		Engine::Vec3f m_drHiddenEle;
	};

}


