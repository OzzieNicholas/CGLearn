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

namespace UI
{
    [ToolboxItem(false)]
    public partial class SubMenu_MultiObjs : SubMenu
    {
        protected static SubMenu_MultiObjs m_instance = new SubMenu_MultiObjs();

        public static SubMenu_MultiObjs instance => m_instance;

        public SubMenu_MultiObjs()
        {
            this.addItems(new ToolStripItem[] {
                this.fu("中断", "edit_stop"),
                this.fu("重绘", "view_redraw"),

                this.sep(),

                this.fu("属性", "edit_props"),

                this.sep(),

                this.fu("复制", "edit_copy"),
                this.group("参数复制", new ToolStripMenuItem[] {
                    this.fu("线性复制", "edit_copy_line"),
                    this.fu("旋转复制", "edit_copy_rotate"),
                    this.fu("镜像复制", "edit_copy_mirror"),
                    this.fu("到另一平面", "edit_copy_by_plane"),
                    this.fu("到另一物体", "edit_copy_by_obj"),
                }),

                this.fu("移动", "edit_move"),
                this.group("参数移动", new ToolStripMenuItem[] {
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
                this.group("根据零件调整工作区", new ToolStripMenuItem[] {
                    this.fu("所有视图", "view_set_active_box_by_part"),
                    this.fu("当前视图", "view_set_focused_box_by_part"),
                }),
                this.group("创建视图", new ToolStripMenuItem[] {
                    this.fu("3D视图", "view_create_obj_3d", new object[] {
                        "name", "3d",
                        "temp", true,
                    }),
                }),

                this.group("创建图纸", new ToolStripMenuItem[] {
                    this.fu("零件图纸", "dr_create_draw_part"),
                    this.fu("创建构件图", "dr_create_draw_ass"),
                    this.group("多件图", new ToolStripMenuItem[] {
                        this.fu("零件图纸", "dr_create_draw_multi_part"),
                        this.fu("构件图纸", "dr_create_draw_multi_ass"),
                    }),
                }),

                this.sep(),

                this.group("缩放", new ToolStripMenuItem[] {
                    this.fu("缩放选中对象", "view_zoom_by_part"),
                    this.fu("放大", "view_zoom_in"),
                    this.fu("缩小", "view_zoom_out"),
                    this.fu("恢复原始尺寸", "view_zoom_by_workbox"),
                }),

                this.fu("下一窗口", "view_switch_to_next"),

                this.sep(),

                this.group("构件", new ToolStripMenuItem[] {
                    this.fu("加入到构件", "assembly_add_sub", new object[] { "cur_objs", true }),
                    this.fu("从构件移除", "assembly_remove", new object[] { "cur_objs", true }),
                    this.fu("组合成构件", "assembly_new", new object[] { "cur_objs", true }),
                }),
            });
        }

        
    }
}
