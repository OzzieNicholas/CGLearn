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
    public partial class SubMenu_View : SubMenu
    {
        protected static SubMenu_View m_instance = new SubMenu_View();

        public static SubMenu_View instance => m_instance;

        public SubMenu_View() {
            this.addItems(new ToolStripItem[] {
                this.fu("中断", "edit_stop"),
                this.fu("重绘", "view_redraw"),

                this.sep(),

                this.fu("属性", "edit_props"),

                this.sep(),

                this.fu("扩展工作区到整个模型", "view_set_focused_box_by_model"),
               
                this.group("缩放", new ToolStripMenuItem[] {
                    this.fu("放大", "view_zoom_in"),
                    this.fu("缩小", "view_zoom_out"),
                    this.fu("恢复原始尺寸", "view_zoom_by_workbox"),
                }),

                this.cfu("显示裁剪框", "view_workbox_vis"),
                this.cfu("置顶", "view_topmost"),
                this.fu("下一窗口", "view_switch_to_next"),
                this.fu("重置视图位置", "view_reset_pos"),
                this.sep(),

                this.fu("创建整体布置图", "dr_create_draw_arrangement"),
                this.fu("创建裁剪面", "view_create_clip"),
            });
        }

    }
}
