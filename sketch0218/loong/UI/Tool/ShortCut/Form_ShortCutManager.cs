using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using IFWrapper;
using UserCtrl;

namespace UI
{
    public partial class Form_ShortCutManager : UI.ParaForm
    {
        public static Form_ShortCutManager m_instance = new Form_ShortCutManager();

        private ShortCutGrid m_grid = new ShortCutGrid();

        public static Form_ShortCutManager instance => m_instance;

        public Form_ShortCutManager() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            tb_filter.onLookUp += onFilter;
            
            pan_grid.Controls.Add(m_grid);

            cbo_group.ClearItems();
            cbo_group.AddItem("", Lang.Local("$group$all", "func_unit"));
            cbo_group.AddItem("file", Lang.Local("file", "func_unit"));
            cbo_group.AddItem("edit", Lang.Local("edit", "func_unit"));
            cbo_group.AddItem("view", Lang.Local("view", "func_unit"));
            cbo_group.AddItem("steel_part", Lang.Local("steel_part", "func_unit"));
            cbo_group.AddItem("concrete_part", Lang.Local("concrete_part", "func_unit"));
            cbo_group.AddItem("comp", Lang.Local("comp", "func_unit"));
            cbo_group.AddItem("comp_instance", Lang.Local("comp_instance", "func_unit"));
            cbo_group.AddItem("assembly", Lang.Local("assembly", "func_unit"));
            cbo_group.AddItem("cast_unit", Lang.Local("cast_unit", "func_unit"));
            cbo_group.AddItem("helper", Lang.Local("helper", "func_unit"));
            cbo_group.AddItem("detail", Lang.Local("detail", "func_unit"));
            cbo_group.AddItem("cut", Lang.Local("cut", "func_unit"));
            cbo_group.AddItem("measure", Lang.Local("measure", "func_unit"));
            cbo_group.AddItem("profile", Lang.Local("profile", "func_unit"));
            cbo_group.AddItem("mtrl", Lang.Local("mtrl", "func_unit"));
            cbo_group.AddItem("report", Lang.Local("report", "func_unit"));
            cbo_group.AddItem("number", Lang.Local("number", "func_unit"));
            cbo_group.AddItem("tool", Lang.Local("tool", "func_unit"));
            cbo_group.AddItem("help", Lang.Local("help", "func_unit"));
            cbo_group.AddItem("dr_file", Lang.Local("dr_file", "func_unit"));
            cbo_group.AddItem("dr_edit", Lang.Local("dr_edit", "func_unit"));
            cbo_group.AddItem("dr_view", Lang.Local("dr_view", "func_unit"));
            cbo_group.AddItem("dr_draw", Lang.Local("dr_draw", "func_unit"));
            cbo_group.AddItem("dr_dim", Lang.Local("dr_dim", "func_unit"));
            cbo_group.AddItem("dr_mark", Lang.Local("dr_mark", "func_unit"));
            cbo_group.AddItem("dr_shape", Lang.Local("dr_shape", "func_unit"));
            cbo_group.AddItem("dr_tmpl_file", Lang.Local("dr_tmpl_file", "func_unit"));
            cbo_group.AddItem("dr_tmpl_edit", Lang.Local("dr_tmpl_edit", "func_unit"));
            cbo_group.AddItem("dr_tmpl_object", Lang.Local("dr_tmpl_object", "func_unit"));
            cbo_group.AddItem("dr_tmpl_view", Lang.Local("dr_tmpl_view", "func_unit"));
            cbo_group.UpdateItems();
            cbo_group.SelectedKey = "";

            this.updateTabOrder();
        }

        public void show() {
            if (this.Visible) {
                this.focus();
                return;
            }

            m_grid.reset();
            m_grid.refresh();
            this.Visible = true;
            this.focus();
        }

        private void btn_restore_Click(object sender, EventArgs e) {
            if (MessageBox.Show("所有修改过的快捷键将被还原为初始设置，确定要恢复默认配置吗?", "提示", MessageBoxButtons.OKCancel) != DialogResult.OK)
                return;

            Native.restoreShortCuts();
            FuncUnit.updateShortCuts();
            LayoutManager.instance.refreshItems();

            m_grid.reset();
            m_grid.refresh();
            m_grid.group = cbo_group.SelectedStrKey;
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            m_grid.saveUserCfg();
            this.hide();
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            m_grid.saveUserCfg();
        }

        private void btn_export_Click(object sender, EventArgs e) {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "快捷键配置(*.lgst)|*.lgst";
            DialogResult ret = dlg.ShowDialog();
            if (ret == DialogResult.OK) {
                m_grid.export(dlg.FileName);
            }
        }

        private void btn_import_Click(object sender, EventArgs e) {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "快捷键配置(*.lgst)|*.lgst";
            DialogResult ret = dlg.ShowDialog();
            if (ret == DialogResult.OK) {
                m_grid.import(dlg.FileName);
                m_grid.refresh();
            }
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void onFilter(InputLookUp sender, string text) {
            m_grid.filter = text;
            m_grid.refresh();
        }

        private void cbo_group_SelectionChangeCommitted(object sender, EventArgs e) {
            if (cbo_group.SelectedIndex == -1)
                return;

            m_grid.group = cbo_group.SelectedStrKey;
            m_grid.refresh();
        }
    }
}
