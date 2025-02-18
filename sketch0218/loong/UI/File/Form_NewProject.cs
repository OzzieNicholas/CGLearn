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
using System.Windows.Forms.Design;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using UserCtrl;
using IFWrapper;


namespace UI
{

    public partial class Form_NewProject : UI.ParaForm
    {
        public static Form_NewProject m_instance = new Form_NewProject();

        public static Form_NewProject instance => m_instance;

        public Form_NewProject() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            this.updateTabOrder();

            Native.queryValidDocumentEnv(out string[] envs);
            cbo_env.ClearItems();
            foreach (var env in envs)
                cbo_env.AddItem(env, Lang.Local(env, "env"));
            cbo_env.UpdateItems();
            cbo_env.SelectedKey = "cn";
        }

        public void show() {
            if (this.Visible) {
                this.focus();
                return;
            }

            cbo_base_dir.reset();

            if (cbo_env.SelectedItem == null)
                cbo_env.SelectedIndex = 0;

            this.Visible = true;
            this.focus();
        }

        public string baseDir {
            get => cbo_base_dir.dir;
            set => cbo_base_dir.dir = value;
        }

        private void Btn_base_dir_Click(object sender, EventArgs e) {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = cbo_base_dir.dir;

            var ret = dlg.ShowDialog();
            if (ret == DialogResult.OK) {
                this.baseDir = dlg.SelectedPath;
            }
        }

        public void force_create_proj()
        {
            //string projName = tb_proj_name.Text.Trim();
            string projName = "D5-Test";
            if (projName.Length == 0)
            {
                MessageBox.Show(MainForm.instance, "项目名称不能为空", "提示", MessageBoxButtons.OK);
                return;
            }

            string projPath = cbo_base_dir.dir + "\\" + projName.Trim();
            //if (Native.isPathHasDocument(projPath))
            //{
            //    MessageBox.Show(MainForm.instance, "同名项目已存在，请更换项目名称", "提示", MessageBoxButtons.OK);
            //    return;
            //}

            //Save current document
            if (Native.isDocumentModifiedAfterLastSave())
            {
                var dlgRes = MessageBox.Show(MainForm.instance, "是否保存当前模型？", "保存", MessageBoxButtons.YesNoCancel);
                if (dlgRes == DialogResult.Cancel)
                    return;

                if (dlgRes == DialogResult.Yes)
                    Native.saveDocument(true, "");
            }

            Native.newDocument(projPath, cbo_env.SelectedKey as string);

            LayoutManager.instance.refreshItems();

            this.hide();
        }

        private void Btn_new_Click(object sender, EventArgs e) {
            string projName = tb_proj_name.Text.Trim();
            if (projName.Length == 0) {
                MessageBox.Show(MainForm.instance, "项目名称不能为空", "提示", MessageBoxButtons.OK);
                return;
            }

            if (cbo_env.SelectedItem == null) {
                MessageBox.Show(MainForm.instance, "请为项目选择一个环境", "提示", MessageBoxButtons.OK);
                return;
            }

            string projPath = cbo_base_dir.dir + "\\" + projName.Trim();
            if (Native.isPathHasDocument(projPath)) {
                MessageBox.Show(MainForm.instance, "同名项目已存在，请更换项目名称", "提示", MessageBoxButtons.OK);
                return;
            }

            //Save current document
            if (Native.isDocumentModifiedAfterLastSave()) {
                var dlgRes = MessageBox.Show(MainForm.instance, "是否保存当前模型？", "保存", MessageBoxButtons.YesNoCancel);
                if (dlgRes == DialogResult.Cancel)
                    return;

                if (dlgRes == DialogResult.Yes)
                    Native.saveDocument(true, "");
            }

            Native.newDocument(projPath, cbo_env.SelectedKey as string);

            LayoutManager.instance.refreshItems();

            this.hide();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }
    }
}
