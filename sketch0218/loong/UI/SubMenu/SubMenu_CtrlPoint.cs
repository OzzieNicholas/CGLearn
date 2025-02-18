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
    public partial class SubMenu_CtrlPoint : SubMenu
    {
        protected static SubMenu_CtrlPoint m_instance = new SubMenu_CtrlPoint();

        public static SubMenu_CtrlPoint instance => m_instance;

        public SubMenu_CtrlPoint()
        {
            this.addItems(new ToolStripItem[] {
                this.fu("中断", "edit_stop"),
                this.fu("重绘", "view_redraw"),

                this.sep(),

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

                this.fu("下一窗口", "view_switch_to_next"),
            });
        }


    }
}
