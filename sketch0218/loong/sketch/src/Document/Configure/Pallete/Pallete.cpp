#include "sketch/Document/Configure/Pallete/Pallete.h"
#include "sketch/STCore.h"

#include "Engine/Math/Vec3.h"
#include "Engine/Util/StrUtil.h"
#include "Engine/Util/Log.h"

using namespace Engine;

namespace sketch
{
	Pallete::Pallete(const std::wstring& name):
		m_name(name),
		m_viewBackTopLeft( 0.1f, 0.1f, 0.1f ),
		m_viewBackTopRight(0.1f, 0.1f, 0.1f),
		m_viewBackBottomLeft(0.1f, 0.1f, 0.1f),
		m_viewBackBottomRight(0.1f, 0.1f, 0.1f),
		m_viewBackWFTopLeft(0.1f, 0.1f, 0.1f),
		m_viewBackWFTopRight(0.1f, 0.1f, 0.1f),
		m_viewBackWFBottomLeft(0.1f, 0.1f, 0.1f),
		m_viewBackWFBottomRight(0.1f, 0.1f, 0.1f),
		m_cellBorder( 0.7f, 0.7f, 0.7f ),
		m_cellPickBorder( 1.0f, 1.0f, 0.0f ),
		m_workLine(Vec3f::YELLOW),
		m_snapTag(Vec3f::GREEN),
		m_snapTagRef( 1.0f, 0.5f, 0.0f ),
		m_snapPlane( 1.0f, 0.5f, 0.0f ),
		m_objHover(Vec3f::YELLOW),
		m_objPick( 0.5f, 1.0f, 0.0f ),
		m_objHL( 0.8f, 0.8f, 1.0f ),
		m_objLabel(0.8f, 0.8f, 1.0f),
		m_ctrlPoint( 1.0f, 0.0f, 1.0f ),
		m_ctrlPointFirst( 1.0f, 1.0f, 1.0f ),
		m_gridOnView( 0.745f, 0.745f, 0.745f ),
		m_gridOnWork( 0.745f, 0.47f, 0.745f ),
		m_gridPlane( 0.4f, 0.4f, 1.0f ),
		m_point( 1.0f, 0.2f, 0.0f ),
		m_pointOnPlane( 0.5f, 0.5f, 1.0f ),
		m_boltInWorkshop(1.0f, 0.7f, 0.0f),
		m_boltInSite(0.78f, 0.78f, 1.0f),
		m_holeInWorkshop(0.5f, 0.35f, 0.0f),
		m_holeInSite(0.4f, 0.4f, 0.5f),
		m_weldInWorkshop( 1.0f, 0.7f, 0.0f ),
		m_weldInSite( 0.0f, 0.7f, 1.0f ),
		m_measure( 1.0f, 0.55f, 0.0f ),
		m_drBack( 1.0f, 1.0f, 1.0f ),
		m_drWorkLine( 0.2f, 0.2f, 0.4f ),
		m_drSnapTag( 0.0f, 0.785f, 0.0f ),
		m_drSnapTagRef( 1.0f, 0.5f, 0.0f ),
		m_drHiddenEle( 0.9f, 0.9f, 0.9f )
	{
		
	}

	Pallete::~Pallete(void)
	{
		
	}

	void Pallete::modify(const Engine::DObject& props)
	{
		for (auto& it : props) {
			Vec3f clr = it.second._rgb();
			
			if (it.first == "view_back") {
				m_viewBackTopLeft = clr;
				m_viewBackTopRight = clr;
				m_viewBackBottomLeft = clr;
				m_viewBackBottomRight = clr;
			}
			else if (it.first == "view_back_top_left") {
				m_viewBackTopLeft = clr;
			}
			else if (it.first == "view_back_top_right") {
				m_viewBackTopRight = clr;
			}
			else if (it.first == "view_back_bottom_left") {
				m_viewBackBottomLeft = clr;
			}
			else if (it.first == "view_back_bottom_right") {
				m_viewBackBottomRight = clr;
			}
			else if (it.first == "view_back_wf_top_left") {
				m_viewBackWFTopLeft = clr;
			}
			else if (it.first == "view_back_wf_top_right") {
				m_viewBackWFTopRight = clr;
			}
			else if (it.first == "view_back_wf_bottom_left") {
				m_viewBackWFBottomLeft = clr;
			}
			else if (it.first == "view_back_wf_bottom_right") {
				m_viewBackWFBottomRight = clr;
			}
			else if (it.first == "cell_border") {
				m_cellBorder = clr;
			}
			else if (it.first == "cell_pick_border") {
				m_cellPickBorder = clr;
			}
			else if (it.first == "work_line") {
				m_workLine = clr;
			}
			else if (it.first == "snap_tag") {
				m_snapTag = clr;
			}
			else if (it.first == "snap_tag_ref") {
				m_snapTagRef = clr;
			}
			else if (it.first == "snap_plane") {
				m_snapPlane = clr;
			}
			else if (it.first == "measure") {
				m_measure = clr;
			}
			else if (it.first == "obj_hover") {
				m_objHover = clr;
			}
			else if (it.first == "obj_pick") {
				m_objPick = clr;
			}
			else if (it.first == "obj_hl") {
				m_objHL = clr;
			}
			else if (it.first == "obj_label") {
				m_objLabel = clr;
			}
			else if (it.first == "ctrl_point") {
				m_ctrlPoint = clr;
			}
			else if (it.first == "ctrl_point_first") {
				m_ctrlPointFirst = clr;
			}
			else if (it.first == "grid_on_view") {
				m_gridOnView = clr;
			}
			else if (it.first == "grid_on_work") {
				m_gridOnWork = clr;
			}
			else if (it.first == "grid_plane") {
				m_gridPlane = clr;
			}
			else if (it.first == "point") {
				m_point = clr;
			}
			else if (it.first == "bolt_in_workshop") {
				m_boltInWorkshop = clr;
			}
			else if (it.first == "bolt_in_site") {
				m_boltInSite = clr;
			}
			else if (it.first == "hole_in_workshop") {
				m_holeInWorkshop = clr;
			}
			else if (it.first == "hole_in_site") {
				m_holeInSite = clr;
			}
			else if (it.first == "weld_in_workshop") {
				m_weldInWorkshop = clr;
			}
			else if (it.first == "weld_in_site") {
				m_weldInSite = clr;
			}
			else if (it.first == "point_plane") {
				m_pointOnPlane = clr;
			}
			else if (it.first == "dr_back") {
				m_drBack = clr;
			}
			else if (it.first == "dr_work_line") {
				m_drWorkLine = clr;
			}
			else if (it.first == "dr_snap_tag") {
				m_drSnapTag = clr;
			}
			else if (it.first == "dr_snap_tag_ref") {
				m_drSnapTagRef = clr;
			}
			else if (it.first == "dr_hidden_ele") {
				m_drHiddenEle = clr;
			}
		}
	}

	void Pallete::_updateProps()
	{
		m_props["view_back_top_left"] = Vec3(m_viewBackTopLeft);
		m_props["view_back_top_right"] = Vec3(m_viewBackTopRight);
		m_props["view_back_bottom_left"] = Vec3(m_viewBackBottomLeft);
		m_props["view_back_bottom_right"] = Vec3(m_viewBackBottomRight);
		m_props["view_back_wf_top_left"] = Vec3(m_viewBackWFTopLeft);
		m_props["view_back_wf_top_right"] = Vec3(m_viewBackWFTopRight);
		m_props["view_back_wf_bottom_left"] = Vec3(m_viewBackWFBottomLeft);
		m_props["view_back_wf_bottom_right"] = Vec3(m_viewBackWFBottomRight);
		m_props["cell_border"] = Vec3(m_cellBorder);
		m_props["cell_pick_border"] = Vec3(m_cellPickBorder);
		m_props["work_line"] = Vec3(m_workLine);
		m_props["snap_tag"] = Vec3(m_snapTag);
		m_props["snap_tag_ref"] = Vec3(m_snapTagRef);
		m_props["snap_plane"] = Vec3(m_snapPlane);
		m_props["obj_hover"] = Vec3(m_objHover);
		m_props["obj_pick"] = Vec3(m_objPick);
		m_props["obj_hl"] = Vec3(m_objHL);
		m_props["obj_label"] = Vec3(m_objLabel);
		m_props["ctrl_point"] = Vec3(m_ctrlPoint);
		m_props["ctrl_point_first"] = Vec3(m_ctrlPointFirst);
		m_props["grid_on_view"] = Vec3(m_gridOnView);
		m_props["grid_on_work"] = Vec3(m_gridOnWork);
		m_props["grid_plane"] = Vec3(m_gridPlane);
		m_props["point"] = Vec3(m_point);
		m_props["point_on_plane"] = Vec3(m_pointOnPlane);
		m_props["bolt_in_workshop"] = Vec3(m_boltInWorkshop);
		m_props["bolt_in_site"] = Vec3(m_boltInSite);
		m_props["hole_in_workshop"] = Vec3(m_holeInWorkshop);
		m_props["hole_in_site"] = Vec3(m_holeInSite);
		m_props["weld_in_workshop"] = Vec3(m_weldInWorkshop);
		m_props["weld_in_site"] = Vec3(m_weldInSite);
		m_props["measure"] = Vec3(m_measure);
		m_props["dr_back"] = Vec3(m_drBack);
		m_props["dr_work_line"] = Vec3(m_drWorkLine);
		m_props["dr_snap_tag"] = Vec3(m_drSnapTag);
		m_props["dr_snap_tag_ref"] = Vec3(m_drSnapTagRef);
		m_props["dr_hidden_ele"] = Vec3(m_drHiddenEle);
	}
}