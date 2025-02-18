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
    public partial class SubMenu_Snap_Point : SubMenu
    {
        protected static SubMenu_Snap_Point m_instance = new SubMenu_Snap_Point();
        private Dictionary<ToolStripMenuItem, uint> m_snapMap = null;

        public static SubMenu_Snap_Point instance => m_instance;

        public SubMenu_Snap_Point() {
            m_snapMap = new Dictionary<ToolStripMenuItem, uint>() {
                { this.checkedItem("点或轴线交点", false, item_snap_flag_Click), Define.SNAP_PT_HELPER_POINT },
                { this.checkedItem("端点", false, item_snap_flag_Click), Define.SNAP_PT_END_POINT },
                { this.checkedItem("圆心", false, item_snap_flag_Click), Define.SNAP_PT_CENTER_POINT },
                { this.checkedItem("中点", false, item_snap_flag_Click), Define.SNAP_PT_MID_POINT },
                { this.checkedItem("交点", false, item_snap_flag_Click), Define.SNAP_PT_CROSS_POINT },
                { this.checkedItem("垂足", false, item_snap_flag_Click), Define.SNAP_PT_FOOT_POINT },
                { this.checkedItem("延长线", false, item_snap_flag_Click), Define.SNAP_PT_LINE_EXT },
                { this.checkedItem("最近点", false, item_snap_flag_Click), Define.SNAP_PT_LINE_NEAREST },
                { this.checkedItem("直线", false, item_snap_flag_Click), Define.SNAP_PT_LINE},
                { this.checkedItem("自由", false, item_snap_flag_Click), Define.SNAP_PT_FREE }
            };

            this.addItems(new ToolStripItem[] {
                this.fu("中断", "edit_stop"),
                sep(),
            });

            this.addItems(m_snapMap.Keys.ToArray());

            this.addItems(new ToolStripItem[] {
                sep(),
                this.group("缩放", new ToolStripMenuItem[] {
                    this.fu("放大", "view_zoom_in"),
                    this.fu("缩小", "view_zoom_out"),
                    this.fu("恢复原始尺寸", "view_zoom_by_workbox"),
                }),
                sep(),
                this.fu("下一窗口", "view_switch_to_next"),
            });
        }

        protected override void onShown() {
            base.onShown();

            uint flags = Native.tmpUsrPtSnapFlag(Define.LayoutCellType.CELL_NONE);
            foreach (var it in m_snapMap) {
                var item = it.Key;
                var bit = it.Value;
                item.Checked = (flags & bit) != 0;
            }
        }

        private void item_snap_flag_Click(object sender, EventArgs e) {
            
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            if (item.Checked) {
                foreach (var snapItem in m_snapMap.Keys) {
                    if (snapItem != item && snapItem.Checked)
                        snapItem.Checked = false;
                }

                Native.setTmpUsrPtSnapFlag(Define.LayoutCellType.CELL_NONE, m_snapMap[item] | Define.SNAP_PT_GEO_POINT | Define.SNAP_PT_REF_POINT);
            }
            else {
                foreach (var snapItem in m_snapMap.Keys) {
                    if (snapItem.Checked)
                        return;
                }

                Native.setTmpUsrPtSnapFlag(Define.LayoutCellType.CELL_NONE, Define.SNAP_NONE);
            }
        }

    }
}
