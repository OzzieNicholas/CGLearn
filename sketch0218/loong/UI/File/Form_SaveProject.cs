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
using UserCtrl;
using IFWrapper;

namespace UI
{
    public partial class Form_SaveProject : UI.ParaForm
    {
        private ProjectInfoGrid m_grid = null;

        public Form_SaveProject() {
            InitializeComponent();

            this.disposeOnClose = true;

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            m_grid = new ProjectInfoGrid();
            pan_proj_list.Controls.Add(m_grid);

            m_grid.init();
            m_grid.CellDoubleClick += grid_CellDoubleClick;

            cbo_base_dir.onDirChanged = onBaseDirChanged;

            this.updateTabOrder();
        }

        private void onBaseDirChanged(string dir) {
            m_grid.basePath = dir;
        }

        private void grid_CellDoubleClick(object sender, DataGridViewCellEventArgs e) {
            if (m_grid.selectedProjName != null) {
                tb_proj_name.Text = m_grid.selectedProjName;
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        public DialogResult show(string projName) {
            cbo_base_dir.reset();

            projName = string.IsNullOrEmpty(projName) ? "NewModel" : projName.Trim();

            int n = 1;
            string newName = projName;
            while (m_grid.hasProject(newName))
                newName = projName + n++;
            tb_proj_name.Text = newName;

            return this.ShowDialog(MainForm.instance);
        }

        private void Btn_base_dir_Click(object sender, EventArgs e) {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = cbo_base_dir.dir;

            var ret = dlg.ShowDialog();
            if (ret == DialogResult.OK) 
                cbo_base_dir.dir = dlg.SelectedPath;
        }

        public string selectedProjPath {
            get {
                var s = tb_proj_name.Text.Trim();
                if (String.IsNullOrEmpty(s))
                    return "";

                return cbo_base_dir.dir + "\\" + s;
            }
        }

        private void Btn_save_Click(object sender, EventArgs e) {
            if (Native.isPathHasDocument(this.selectedProjPath)) {
                MessageBox.Show("同名项目已存在，请更换项目名称", "提示", MessageBoxButtons.OK);
                return;
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }
    }
}
