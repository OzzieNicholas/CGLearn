using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public partial class SubMenu_SP : SubMenu
    {
        protected static SubMenu_SP m_instance = new SubMenu_SP();
        public static SubMenu_SP instance => m_instance;

        public SubMenu_SP() {
            this.addItems(new ToolStripItem[] {
                this.fu("中断", "edit_stop"),
                this.fu("重绘", "view_redraw"),

                this.sep(),

                this.fu("属性", "edit_props"),
                this.group("查询", new ToolStripItem[] {
                    this.fu("零件", "tool_inquire"),
                    this.fu("构件", "tool_inquire_ass"),
                }),

                this.sep("sep_beam"),
                this.sep("sep_plate"),
                this.fu("组合", "edit_combine_part", new object[] { "cur_objs", true }),
                this.fu("分割", "edit_split_part", new object[] { "cur_objs", true, "once", true }),
                this.fu("分割", "edit_split_plate", new object[] { "cur_objs", true, "once", false }),
                this.fu("转换为板", "edit_split_part_to_plates"),
                this.fu("编辑形状", "detail_edit_polygon_plate", new object[] { "cur_objs", true }),

                this.sep("sep_cutter"),
                this.fu("设置切割目标", "cut_edit_cuttee", new object[] { "cur_objs", true }),
                this.cfu("焊接准备", "cut_switch_prepare"),

                this.sep(),

                this.fu("复制", "edit_copy"),
                this.group("参数复制", new ToolStripItem[] {
                    this.fu("线性复制", "edit_copy_line"),
                    this.fu("旋转复制", "edit_copy_rotate"),
                    this.fu("镜像复制", "edit_copy_mirror"),
                    this.fu("到另一平面", "edit_copy_by_plane"),
                    this.fu("到另一物体", "edit_copy_by_obj"),
                }),

                this.fu("移动", "edit_move"),
                this.group("参数移动", new ToolStripItem[] {
                    this.fu("线性移动", "edit_move_line"),
                    this.fu("旋转移动", "edit_move_rotate"),
                    this.fu("镜像移动", "edit_move_mirror"),
                    this.fu("到另一平面", "edit_move_by_plane"),
                    this.fu("到另一物体", "edit_move_by_obj"),
                }),

                this.fu("删除", "edit_del"),

                this.sep(),

                this.fu("隐藏", "view_hide_objs", new DObject{ { "cur_objs", true} }),
                this.fu("仅显示选中对象", "view_focus_objs"),
                this.cfu("显示精确边界", "view_rdquality_high"),
                this.group("根据零件调整工作区", new ToolStripItem[] {
                    this.fu("所有视图", "view_set_active_box_by_part"),
                    this.fu("当前视图", "view_set_focused_box_by_part"),
                }),
                this.group("创建视图", new ToolStripItem[] {
                    this.fu("3D视图", "view_create_obj_3d", new object[] {
                        "name", "3d",
                        "temp", true,
                    }),
                    this.fu("零件默认视图", "view_create_obj_def"),
                    this.fu("零件无形变视图", "view_create_obj_ortho"),
                    
                    this.sep("sep_view_create_obj_from_side"),
                    
                    this.fu("前面", "view_create_obj_front", new object[] {
                            "name", "front",
                            "temp", true,
                    }),
                    this.fu("顶面", "view_create_obj_top", new object[] {
                        "name", "top",
                        "temp", true,
                    }),
                    this.fu("后面", "view_create_obj_back", new object[] {
                        "name", "back",
                        "temp", true,
                    }),
                    this.fu("底面", "view_create_obj_bottom", new object[] {
                        "name", "bottom",
                        "temp", true,
                    }),
                    this.fu("头部", "view_create_obj_head", new object[] {
                        "name", "head",
                        "temp", true,
                    }),
                    this.fu("尾部", "view_create_obj_tail", new object[] {
                        "name", "tail",
                        "temp", true,
                    }),
                }),
                this.group("gr_create_dr", "创建图纸", new ToolStripItem[] {
                    this.fu("零件图纸", "dr_create_draw_part"),
                    this.fu("构件图纸", "dr_create_draw_ass"),
                    this.group("多件图", new ToolStripItem[] {
                        this.fu("零件图纸", "dr_create_draw_multi_part"),
                        this.fu("构件图纸", "dr_create_draw_multi_ass"),
                        this.fu("带布置的零件图纸", "dr_create_draw_multi_part", new object[] {"with_layout", true }),
                        this.fu("带布置的构件图纸", "dr_create_draw_multi_ass", new object[] {"with_layout", true }),
                    }),
                }),

                this.sep("sep_ass"),

                this.group("gr_ass", "构件", new ToolStripItem[] {
                    this.fu("加入到构件", "assembly_add_sub", new object[] { "cur_objs", true }),
                    this.fu("设置为新主零件", "assembly_set_main", new object[] { "cur_objs", true }),
                    this.fu("从构件移除", "assembly_remove", new object[] { "cur_objs", true }),
                    this.fu("组合成构件", "assembly_new", new object[] { "cur_objs", true }),
                }),

                this.sep(),

                this.group("缩放", new ToolStripItem[] {
                    this.fu("缩放选中对象", "view_zoom_by_part"),
                    this.fu("放大", "view_zoom_in"),
                    this.fu("缩小", "view_zoom_out"),
                    this.fu("恢复原始尺寸", "view_zoom_by_workbox"),
                }),

                this.fu("下一窗口", "view_switch_to_next"),
            });
        }

        protected override void onShown() {
            base.onShown();

            //var ids = Native.curObjIdsByBaseType(Define.ObjectType.OBJ_STEEL_PART_BEGIN, Define.ObjectType.OBJ_STEEL_PART_END);
            //switch (ids.Length) {
            //    case 1: {
            //        if (Native.isPlatePart(ids[0])) {
            //            this.enableItem("sep_beam", false);
            //            this.enableItem("edit_split_part_to_plates", false);
            //            this.enableItem("edit_split_part", false);

            //            this.enableItem("sep_plate", true);
            //            this.enableItem("detail_edit_polygon_plate", true);
            //            this.enableItem("edit_split_plate", true);
            //        }
            //        else {
            //            this.enableItem("sep_beam", true);
            //            this.enableItem("edit_split_part_to_plates", true);
            //            this.enableItem("edit_split_part", Native.isBeamOrPoly(ids[0]));

            //            this.enableItem("sep_plate", false);
            //            this.enableItem("detail_edit_polygon_plate", false);
            //            this.enableItem("edit_split_plate", false);
            //        }

            //        if (Native.isCutterPart(ids[0])) {
            //            this.enableItem("sep_cutter", true);
            //            this.enableItem("cut_edit_cuttee", true);
            //            this.enableItem("cut_switch_prepare", true);
            //        }
            //        else {
            //            this.enableItem("sep_cutter", false);
            //            this.enableItem("cut_edit_cuttee", false);
            //            this.enableItem("cut_switch_prepare", false);
            //        }

            //        this.enableItem("edit_combine_part", false);
            //        this.enableItem("view_create_obj_def", true);
            //        this.enableItem("sep_view_create_obj_from_side", true);
            //        this.enableItem("view_create_obj_front", true);
            //        this.enableItem("view_create_obj_top", true);
            //        this.enableItem("view_create_obj_back", true);
            //        this.enableItem("view_create_obj_bottom", true);
            //        this.enableItem("view_create_obj_head", true);
            //        this.enableItem("view_create_obj_tail", true);

            //        if (Native.isCutterPart(ids[0])) {
            //            this.enableItem("gr_create_dr", false);
            //            this.enableItem("sep_ass", false);
            //            this.enableItem("gr_ass", false);
            //        }
            //        else {
            //            this.enableItem("gr_create_dr", true);
            //            this.enableItem("sep_ass", true);
            //            this.enableItem("gr_ass", true);
            //        }
            //    }
            //    break;
            //    default: {
            //        bool bAllCutter = true;
            //        bool bAllPlate = true;
            //        bool bAllBeam = true;
            //        for (int i = 0; i < ids.Length; ++i) {
            //            if (!Native.isCutterPart(ids[i])) 
            //                bAllCutter = false;
                        
            //            if (!Native.isPlatePart(ids[i])) 
            //                bAllPlate = false;

            //            if (!Native.isBeamOrPoly(ids[i]))
            //                bAllBeam = false;


            //            if (!bAllCutter && !bAllPlate && !bAllBeam)
            //                break;
            //        }

            //        this.enableItem("edit_combine_part", true);
            //        this.enableItem("sep_beam", !bAllPlate);
            //        this.enableItem("edit_split_part_to_plates", !bAllPlate);
            //        this.enableItem("edit_split_part", bAllBeam);
            //        this.enableItem("sep_plate", bAllPlate);
            //        this.enableItem("detail_edit_polygon_plate", false);
            //        this.enableItem("edit_split_plate", bAllPlate);
            //        this.enableItem("sep_cutter", bAllCutter);
            //        this.enableItem("cut_edit_cuttee", bAllCutter);
            //        this.enableItem("cut_switch_prepare", bAllCutter);
            //        this.enableItem("view_create_obj_def", false);
            //        this.enableItem("sep_view_create_obj_from_side", true);
            //        this.enableItem("view_create_obj_front", true);
            //        this.enableItem("view_create_obj_top", true);
            //        this.enableItem("view_create_obj_back", true);
            //        this.enableItem("view_create_obj_bottom", true);
            //        this.enableItem("view_create_obj_head", true);
            //        this.enableItem("view_create_obj_tail", true);

            //        if (bAllCutter) {
            //            this.enableItem("gr_create_dr", false);
            //            this.enableItem("sep_ass", false);
            //            this.enableItem("gr_ass", false);
            //        }
            //        else {
            //            this.enableItem("gr_create_dr", true);
            //            this.enableItem("sep_ass", true);
            //            this.enableItem("gr_ass", true);
            //        }
            //    }
            //    break;
            //}

            
        }

    }
}
