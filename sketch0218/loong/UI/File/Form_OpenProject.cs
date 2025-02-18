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
using UserCtrl;
using IFWrapper;

namespace UI
{
    public partial class Form_OpenProject : UI.ParaForm
    {
        public static Form_OpenProject m_instance = new Form_OpenProject();
        private ProjectInfoGrid m_grid = null;
        private string m_projPath = "";
        private int m_lockState = 0;
        private int m_loadStep = 0;
        private bool m_tryBackup = false;
        private ProgressMsgBox m_progBox = null;

        public static Form_OpenProject instance => m_instance;

        public Form_OpenProject() {
            InitializeComponent();

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

            Callback.regUICmdListener(this.onCommand, "on_doc_loaded");
        }

        private void onBaseDirChanged(string dir) {
            m_grid.basePath = dir;
        }

        public void show() {
            if (this.Visible) {
                this.focus();
                return;
            }

            cbo_base_dir.reset();
            this.Visible = true;
            this.focus();
        }

        public string baseDir {
            get => cbo_base_dir.dir;
            set => cbo_base_dir.dir = value;
        }

        public void openProject(string projPath) {
            m_projPath = projPath;

            //query if projPath is existed
            if (!Native.hasPath(m_projPath)) {
                MessageBox.Show("模型不存在", "提示", MessageBoxButtons.OK);
                return;
            }

            //Save current document
            if (Native.isDocumentModifiedAfterLastSave()) {
                DialogResult dlgRes = MessageBox.Show("是否保存当前模型？", "保存", MessageBoxButtons.YesNoCancel);
                if (dlgRes == DialogResult.Cancel)
                    return;
                else if (dlgRes == DialogResult.Yes)
                    Native.saveDocument(true, "");
            }

            m_progBox = ProgressMsgBox.show("open_document", Lang.Local("progress_load_scene_message"));
            m_loadStep = 0;
            m_lockState = (Native.documentPath() != m_projPath) ? Native.testDocumentLockFile(m_projPath) : 0;
            m_tryBackup = false;
            if (m_lockState == 2
                && Native.isDocumentHasNewerBackup(m_projPath)) {//may crach on last run
                if (MessageBox.Show("模型在上一次编辑中未正常关闭，要尝试从临时存档恢复最近版本吗？", "提示", MessageBoxButtons.YesNo) == DialogResult.Yes) {
                    m_loadStep = 1;
                    Native.openDocument(m_projPath, "load", false, "open_document");
                    return;
                }
            }
            else if (m_lockState == 1) {//locking by other run
                MessageBox.Show("模型已被其它应用打开，请不要进行编辑或保存操作", "提示", MessageBoxButtons.OK);
            }

            m_loadStep = 2;
            Native.openDocument(m_projPath, "", m_lockState == 1, "open_document");
        }

        public DObject onCommand(string cmd, DObject paras) {
            switch (cmd) {
                case "on_doc_loaded": {
                    var path = paras.GetString("proj_path");
                    if (!String.IsNullOrEmpty(path)) {
                        m_progBox.Close();
                        LayoutManager.instance.refreshItems();
                        return null;
                    }

                    switch (m_loadStep) {
                        case 1: {
                            MessageBox.Show("无法从临时存档恢复，开始加载主存档", "提示", MessageBoxButtons.OK);
                            m_tryBackup = true;

                            m_loadStep = 2;
                            Native.openDocument(m_projPath, "", (m_lockState == 1), "open_document");
                        }
                        break;
                        case 2: {
                            if (m_tryBackup) {
                                MessageBox.Show("加载失败", "提示", MessageBoxButtons.OK);
                                m_progBox.Close();
                                LayoutManager.instance.refreshItems();
                                return null;
                            }
                            else {
                                m_loadStep = 3;
                                MessageBox.Show("加载失败，尝试从临时存档恢复最近版本", "提示", MessageBoxButtons.OK);
                                Native.openDocument(m_projPath, "load", (m_lockState == 1), "open_document");
                            }
                        }
                        break;
                        case 3: {
                            MessageBox.Show("加载失败", "提示", MessageBoxButtons.OK);
                            m_progBox.Close();
                            LayoutManager.instance.refreshItems();
                        }
                        break;
                    }
                }
                break;
            }

            return null;
        }

        private void Btn_base_dir_Click(object sender, EventArgs e) {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.SelectedPath = cbo_base_dir.dir;

            var ret = dlg.ShowDialog();
            if (ret == DialogResult.OK) {
                this.baseDir = dlg.SelectedPath;
            }
        }

        private void Btn_open_Click(object sender, EventArgs e) {
            if (m_grid.selectedProjName != null) {
                this.openProject(cbo_base_dir.dir + "\\" + m_grid.selectedProjName);
            }

            this.hide();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void grid_CellDoubleClick(object sender, DataGridViewCellEventArgs e) {
            if (e.ColumnIndex < 0 || e.RowIndex < 0)
                return;
            
            Btn_open_Click(sender, e);
        }
    }
}
