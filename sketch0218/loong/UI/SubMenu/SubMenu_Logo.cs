using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public partial class SubMenu_Logo : SubMenu
    {
        protected static SubMenu_Logo m_instance = new SubMenu_Logo();

        public static SubMenu_Logo instance => m_instance;

        public SubMenu_Logo() {
            this.addItems(new ToolStripItem[] {
                fu("新建...", "file_new"),
                fu("打开...", "file_open"),
                fu("保存", "file_save"),
                fu("另存为...", "file_save_as"),
                sep(),
                group("proj_history", "最近打开", new ToolStripItem[] {
                }),
                sep(),
                fu("打开模型目录", "file_open_doc_directory"),
                sep(),
                fu("对象浏览器", "file_object_browser"),
                fu("导入参考模型", "file_import_ref_model"),
                fu("参考模型列表", "file_ref_model_list"),
                sep(),
                group("导入", new ToolStripItem[] {
                    fu("DWG", "file_import_dwg"),
                }),
                group("导出", new ToolStripItem[] {
                    fu("创建数控文件", "file_export_nc"),
#if false           /// FIXME: 暂时屏蔽创建管NC文件的功能
                    fu("创建管NC文件", "file_export_nc_tube"),
#endif
                    sep(),
                    fu("IFC", "file_export_ifc"),
                }),
                sep(),
                fu("工程属性...", "file_proj_info"),
                sep(),
                fu("关闭程序", "file_exit"),
            });
        }

        protected override void onShown() {
            base.onShown();

            //最近打开文件
            var item = this.getItem("proj_history") as ToolStripMenuItem;
            item.DropDownItems.Clear();

            var projs = Native.globalHistory("project_history", "proj_arr") as string[];
            if (projs != null) {
                for (int i = 0; i < projs.Length; ++i) {
                    item.DropDownItems.Add(this.item(projs[i], onProjHistoryClick));
                }
            }
        }

        protected void onProjHistoryClick(object sender, EventArgs e) {
            var item = sender as ToolStripMenuItem;
            if (item == null)
                return;

            Form_OpenProject.instance.openProject(item.Text);
        }

        protected override void OnClosing(ToolStripDropDownClosingEventArgs e) {
            base.OnClosing(e);

            if (e.CloseReason == ToolStripDropDownCloseReason.AppClicked) {
                var btn = MainForm.instance.topPanel.logoButton;

                var rect = MainForm.instance.topPanel.RectangleToScreen(btn.ClientRectangle);
                if (rect.Contains(Control.MousePosition))
                    e.Cancel = true;
            }
        }
    }
}
