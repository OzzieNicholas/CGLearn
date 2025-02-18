using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using System.Data;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class ProjectInfoGrid : UserCtrl.GridView
    {
        private string m_basePath = null;
        private DataTable m_data = new DataTable();
        private string m_keyFilter = "";
        private long m_keyFilterTm = 0;

        public ProjectInfoGrid() {
            //this.DoubleBuffered = true; //打开时resize column，提示线会消失，所以先注掉
            this.Dock = DockStyle.Fill;
            this.BackgroundColor = Color.White;
            this.ColumnHeadersVisible = true;
            this.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
            this.RowHeadersVisible = false;
            this.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            this.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            this.AllowUserToResizeColumns = true;
            this.AllowUserToResizeRows = false;
            this.AllowUserToAddRows = false;
            this.AllowUserToDeleteRows = false;
            this.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.ReadOnly = true;
            this.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            this.MultiSelect = false;
            this.ShowCellToolTips = true;
            this.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;
            this.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            this.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
        }

        public void init() {
            m_data.Columns.Add("名称", typeof(string));
            m_data.Columns.Add("修改时间", typeof(string));
            m_data.Columns.Add("设计师", typeof(string));
            m_data.Columns.Add("描述", typeof(string));

            this.DataSource = m_data;

            this.Columns[0].FillWeight = 50;
            this.Columns[1].FillWeight = 35;
            this.Columns[2].FillWeight = 25;
            this.Columns[3].FillWeight = 25;

            this.Columns[0].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
            this.Columns[1].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
            this.Columns[2].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
            this.Columns[3].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
        }

        public string selectedProjName {
            get {
                if (this.SelectedRows.Count == 0)
                    return null;

                return dataRow(this.SelectedRows[0])[0].ToString();
            }

            set {
                int selRow = -1;
                for (var r = 0; r < this.RowCount; ++r) {
                    var dRow = dataRow(this.Rows[r]);
                    if (dRow.ToString() == value) {
                        selRow = r;
                        break;
                    }
                }

                for (var r = 0; r < this.RowCount; ++r) {
                    this.Rows[r].Selected = (r == selRow);
                }
            }
        }

        public string basePath {
            get => m_basePath;
            set {
                m_basePath = value;

                Native.getDocumentBaseInfosFromBasePath(m_basePath, out string[] paths, 
                                                                    out long[] modifyTms, 
                                                                    out string[] designers, 
                                                                    out string[] cmts);
                this._updateData(paths, modifyTms, designers, cmts);
            }
        }

        private void _updateData(string[] paths, long[] modifyTms, string[] designers, string[] cmts) {
            
            int oldSortedCol = (this.SortedColumn != null)? this.SortedColumn.Index : 0;
            var oldSortOrder = this.SortOrder;

            m_data.BeginLoadData();
            m_data.Rows.Clear();

            for (int i = 0; i < paths.Length; ++i) {
                string projName = paths[i].Substring(paths[i].LastIndexOf('\\') + 1);
                m_data.Rows.Add(projName, Utils.Epoch2Str(modifyTms[i], "yyyy-MM-dd HH:mm"), designers[i], cmts[i]);
            }

            m_data.EndLoadData();

            switch (oldSortOrder) {
                case SortOrder.None:
                case SortOrder.Ascending:
                    this.Sort(this.Columns[oldSortedCol], ListSortDirection.Ascending);
                break;
                case SortOrder.Descending:
                    this.Sort(this.Columns[oldSortedCol], ListSortDirection.Descending);
                break;
            }
        }

        public bool hasProject(string projName) {
            for (int r = 0; r < m_data.Rows.Count; ++r) {
                if (m_data.Rows[r][0].ToString() == projName)
                    return true;
            }

            return false;
        }

        protected override void OnCellToolTipTextNeeded(DataGridViewCellToolTipTextNeededEventArgs e) {
            //base.OnCellToolTipTextNeeded(e);

            if (e.RowIndex < 0
                || e.ColumnIndex != 0)
                return;

            e.ToolTipText = this.dataRow(e.RowIndex)[0].ToString();
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            if (!char.IsLetterOrDigit(e.KeyChar)
                && !char.IsSymbol(e.KeyChar))
                return;

            var tm = (long)Win32.GetTickCount64();
            var dur = tm - m_keyFilterTm;
            m_keyFilterTm = tm;

            int idx = 0;
            if (dur > 300) { //与上一次按键时间间隔超过阈值，不作为连续按键
                m_keyFilter = e.KeyChar.ToString().ToLower();

                if (this.SelectedRows.Count > 0)
                    idx = (this.SelectedRows[0].Index + 1) % this.RowCount;
            }
            else { //连续按键
                m_keyFilter += e.KeyChar.ToString().ToLower();

                if (this.SelectedRows.Count > 0)
                    idx = this.SelectedRows[0].Index;
            }

            //先搜索以filter开头的模型
            int r = idx;
            do {
                if ((this[0, r].Value as string).ToLower().IndexOf(m_keyFilter) == 0) {
                    this.CurrentCell = this.Rows[r].Cells[0];
                    return;
                }

                r = (r + 1) % this.RowCount;
            } while (r != idx);

            //以filter开头的模型未找到，尝试搜索包含filter的模型
            r = idx;
            do {
                if ((this[0, r].Value as string).ToLower().IndexOf(m_keyFilter) >= 0) {
                    this.CurrentCell = this.Rows[r].Cells[0];
                    return;
                }

                r = (r + 1) % this.RowCount;
            } while (r != idx);
        }
    }
    
}
