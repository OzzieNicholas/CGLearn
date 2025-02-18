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

namespace UI
{
    public partial class Form_PalleteManager : UI.ParaForm
    {
        private static Form_PalleteManager m_instance = new Form_PalleteManager();
        private DObject m_modifiedPals = new DObject();
        private List<string> m_removedPals = new List<string>();
        private bool m_modified = false;

        public static Form_PalleteManager instance => m_instance;

        public Form_PalleteManager() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            li_colors.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            li_colors.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None;
            li_colors.AllowUserToResizeColumns = false;
            li_colors.AllowUserToResizeRows = false;
            li_colors.ColumnCount = 3;
            li_colors.RowCount = 37;
            li_colors.ShowCellToolTips = true;
            li_colors.EditMode = DataGridViewEditMode.EditOnEnter;
            li_colors.SelectionMode = DataGridViewSelectionMode.CellSelect;

            li_colors.Columns[0].FillWeight = 30;
            li_colors.Columns[1].FillWeight = 40;
            li_colors.Columns[2].FillWeight = 30;

            int row = 0;
            this._initRow(row++, "view_back_top_left", "左上视图背景");
            this._initRow(row++, "view_back_top_right", "右上视图背景");
            this._initRow(row++, "view_back_bottom_left", "左下视图背景");
            this._initRow(row++, "view_back_bottom_right", "右下视图背景");
            this._initRow(row++, "view_back_wf_top_left", "左上视图背景[线框]");
            this._initRow(row++, "view_back_wf_top_right", "右上视图背景[线框]");
            this._initRow(row++, "view_back_wf_bottom_left", "左下视图背景[线框]");
            this._initRow(row++, "view_back_wf_bottom_right", "右下视图背景[线框]");
            this._initRow(row++, "cell_border", "视图边框");
            this._initRow(row++, "cell_pick_border", "选中视图边框");
            this._initRow(row++, "work_line", "选择");
            this._initRow(row++, "obj_pick", "物体选中");
            this._initRow(row++, "obj_hover", "鼠标滑过物体");
            this._initRow(row++, "obj_hl", "默认高亮");
            this._initRow(row++, "obj_label", "物体标签");
            this._initRow(row++, "ctrl_point", "控制点");
            this._initRow(row++, "ctrl_point_first", "首个控制点");
            this._initRow(row++, "grid_on_view", "视图平面轴线");
            this._initRow(row++, "grid_on_work", "工作平面轴线");
            this._initRow(row++, "grid_plane", "轴面");
            this._initRow(row++, "point_on_plane", "面上点");
            this._initRow(row++, "point", "点");
            this._initRow(row++, "bolt_in_workshop", "工厂螺栓");
            this._initRow(row++, "bolt_in_site", "工地螺栓");
            this._initRow(row++, "hole_in_workshop", "工厂孔洞");
            this._initRow(row++, "hole_in_site", "工地孔洞");
            this._initRow(row++, "weld_in_workshop", "工厂焊接");
            this._initRow(row++, "weld_in_site", "工地焊接");
            this._initRow(row++, "measure", "测量");
            this._initRow(row++, "snap_tag", "捕捉点");
            this._initRow(row++, "snap_tag_ref", "捕捉参考点");
            this._initRow(row++, "snap_plane", "捕捉面");
            this._initRow(row++, "dr_back", "图纸:背景");
            this._initRow(row++, "dr_work_line", "图纸:选择");
            this._initRow(row++, "dr_snap_tag", "图纸:捕捉点");
            this._initRow(row++, "dr_snap_tag_ref", "图纸:捕捉参考点");
            this._initRow(row++, "dr_hidden_ele", "图纸:隐藏对象");
        }

        public void show() {
            if (this.Visible) {
                this.Focus();
                return;
            }

            this.refreshPalletes();
            if (cbo_pallete.SelectedItem != null)
                this.loadColors(cbo_pallete.SelectedItem as string, false);

            m_modifiedPals.Clear();
            m_removedPals.Clear();
            m_modified = false;
            this.Visible = true;
        }

        private void _initRow(int row, string prop, string text) {
            li_colors[0, row].Value = text;
            li_colors[0, row].Style.BackColor = Color.White;
            li_colors[0, row].Style.SelectionBackColor = Color.White;
            li_colors[0, row].Style.SelectionForeColor = Color.Black;
            li_colors[0, row].Style.Alignment = DataGridViewContentAlignment.MiddleLeft;
            li_colors[0, row].Tag = prop;
            li_colors[0, row].ReadOnly = true;

            Color defColor = Color.White;
            li_colors[1, row].Value = "";
            li_colors[1, row].Style.BackColor = defColor;
            li_colors[1, row].Style.SelectionBackColor = defColor;
            li_colors[1, row].Style.SelectionForeColor = Color.Black;
            li_colors[1, row].ToolTipText = "双击编辑";
            li_colors[1, row].ReadOnly = true;

            li_colors[2, row].Value = "" + defColor.R + "," + defColor.G + "," + defColor.B;
            li_colors[2, row].ValueType = typeof(string);
            li_colors[2, row].Style.BackColor = Color.White;
            li_colors[2, row].Style.SelectionBackColor = Color.White;
            li_colors[2, row].Style.SelectionForeColor = Color.Black;
            li_colors[2, row].ReadOnly = false;
        }

        private void _setColor(int row, Color clr, object src) {
            li_colors[1, row].Style.BackColor = clr;
            li_colors[1, row].Style.SelectionBackColor = clr;

            if (src != li_colors[2, row])
                li_colors[2, row].Value = "" + clr.R + "," + clr.G + "," + clr.B;
        }

        public void refreshPalletes() {
            cbo_pallete.Items.Clear();

            var qry = Callback.execute("query_palletes_info", null);
            if (qry == null)
                return;

            var palNames = qry.GetStringArr("pal_names");
            cbo_pallete.Items.AddRange(palNames);

            var activeName = qry.GetString("active_pal");

            if (cbo_pallete.Items.Contains(activeName))
                cbo_pallete.SelectedItem = activeName;
            else if (cbo_pallete.Items.Contains("dark"))
                cbo_pallete.SelectedItem = "dark";
            else if (cbo_pallete.Items.Count > 0)
                cbo_pallete.SelectedIndex = 0;
        }

        public void checkModified() {
            if (!m_modified || cbo_pallete.SelectedItem == null)
                return;

            m_modifiedPals[cbo_pallete.SelectedItem as string] = this.palProps;
        }

        public void resetModified() {
            m_modifiedPals.Clear();
            m_removedPals.Clear();
            m_modified = false;
        }

        private DObject palProps {
            get {
                DObject paras = new DObject();

                for (int r = 0; r < li_colors.RowCount; ++r) {
                    string prop = li_colors[0, r].Tag as string;
                    Color color = li_colors[1, r].Style.BackColor;
                    paras[prop] = new Vec3((double)color.R / 255.0, (double)color.G / 255.0, (double)color.B / 255.0);
                }

                return paras;
            }
        }

        public void clearColors() {
            for (int r = 0; r < li_colors.RowCount; ++r) {
                this._setColor(r, Color.White, null);
            }
        }

        public void loadColors(string name, bool reset) {
            if (name == null)
                return;

            var pal = Callback.execute("query_pallete", new object[] {
                "pal_name", name,
                "sys", reset,
            });
            if (pal == null)
                return;

            foreach (var it in pal) {
                string prop = it.Key;
                Vec3 vec = (Vec3)it.Value;
                Color clr = Color.FromArgb(Math.Min((int)(vec.X * 255.0), 255), Math.Min((int)(vec.Y * 255.0), 255), Math.Min((int)(vec.Z * 255.0), 255));

                for (int r = 0; r < li_colors.RowCount; ++r) {
                    if (li_colors[0, r].Tag as string != prop)
                        continue;

                    this._setColor(r, clr, null);
                    break;
                }
            }

            li_colors.ClearSelection();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.save();
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            this.save();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.checkModified();
            if (m_modifiedPals.Count > 0 || m_removedPals.Count > 0) {
                var ret = MessageBox.Show("是否保存编辑结果？", "提示", MessageBoxButtons.YesNoCancel);
                if (ret == DialogResult.Yes)
                    this.save();
                else if (ret == DialogResult.Cancel)
                    return;
            }

            this.hide();
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (e.CloseReason == CloseReason.UserClosing) {
                this.checkModified();
                if (m_modifiedPals.Count > 0 || m_removedPals.Count > 0) {
                    var ret = MessageBox.Show("是否保存编辑结果？", "提示", MessageBoxButtons.YesNoCancel);
                    if (ret == DialogResult.Yes) {
                        this.save();
                        this.hide();
                    }
                    else if (ret == DialogResult.No) {
                        this.hide();
                    }
                }
                else
                    this.hide();

                e.Cancel = true;
            }
        }

        public void save() {
            this.checkModified();

            if (m_modified || m_removedPals.Count > 0) {//nothing need to be saved
                Callback.pushCommand("modify_palletes", new object[] {
                    "active_pal", cbo_pallete.SelectedItem as string,
                    "modified_pals", m_modifiedPals.Clone(),
                    "removed_pals", m_removedPals.ToArray(),
                }, null);
            }
            else {
                Callback.pushCommand("modify_palletes", new object[] {
                    "active_pal", cbo_pallete.SelectedItem as string,
                }, null);
            }

            m_modifiedPals.Clear();
            m_removedPals.Clear();
            m_modified = false;
        }

        private void cbo_pallete_SelectionChangeCommitted(object sender, EventArgs e) {
            if (cbo_pallete.SelectedItem == null)
                return;

            this.checkModified();
            this.loadColors(cbo_pallete.SelectedItem as string, false);
        }

        public bool isPalNameValid(string name) {
            return !cbo_pallete.Items.Contains(name);
        }

        public void newPallete(string name, string src) {
            if (cbo_pallete.Items.Contains(name))
                return;

            if (m_removedPals.Contains(name))
                m_removedPals.Remove(name);

            cbo_pallete.Items.Add(name);
            cbo_pallete.SelectedItem = name;
            this.loadColors(src, false);
            m_modified = true;
        }

        private void btn_remove_Click(object sender, EventArgs e) {
            if (cbo_pallete.SelectedItem == null)
                return;

            string name = cbo_pallete.SelectedItem as string;
            if (name == "dark" 
                || name == "classic" 
                || name == "mild"
                || name == "modern"
                || name == "silver") {
                MessageBox.Show("系统配色方案无法删除", "提示", MessageBoxButtons.OK);
                return;
            }

            if (MessageBox.Show("要删除当前配色方案吗?", "提示", MessageBoxButtons.YesNo) != DialogResult.Yes)
                return;

            m_removedPals.Add(name);
            m_modifiedPals.Remove(name);

            cbo_pallete.Items.Remove(name);
            if (cbo_pallete.Items.Count > 0) {
                cbo_pallete.SelectedIndex = 0;
                this.loadColors(cbo_pallete.SelectedItem as string, false);
            }
        }

        private void btn_add_Click(object sender, EventArgs e) {
            string[] srcs = new string[cbo_pallete.Items.Count];
            cbo_pallete.Items.CopyTo(srcs, 0);
            Form_Pallete_New.instance.show(srcs, cbo_pallete.SelectedItem != null ? cbo_pallete.SelectedItem as string : "dark");
        }

        private void li_colors_CellDoubleClick(object sender, DataGridViewCellEventArgs e) {
            if (e.ColumnIndex != 1)
                return;

            ColorDialog dlg = new ColorDialog();
            dlg.AllowFullOpen = true;
            dlg.FullOpen = true;
            dlg.Color = li_colors[1, e.RowIndex].Style.BackColor;

            DialogResult ret = dlg.ShowDialog(this);
            if (ret == DialogResult.OK && li_colors[1, e.RowIndex].Style.BackColor != dlg.Color) {
                this._setColor(e.RowIndex, dlg.Color, li_colors[1, e.RowIndex]);
                m_modified = true;
            }
        }

        private void _updateResetButton() {
            string activeName = cbo_pallete.SelectedItem as string;
            if (activeName == null)
                btn_reset.Enabled = false;
            else
                btn_reset.Enabled = (activeName == "dark" || activeName == "light" || activeName == "light_blue");
        }

        private void li_colors_CellEndEdit(object sender, DataGridViewCellEventArgs e) {
            if (e.ColumnIndex != 2)
                return;

            var cell = li_colors[e.ColumnIndex, e.RowIndex];
            string str = cell.Value as string;
            if (String.IsNullOrEmpty(str)) {
                this._setColor(e.RowIndex, li_colors[1, e.RowIndex].Style.BackColor, null);
                return;
            }

            string[] eles = str.Trim().Split(',');
            if (eles.Length != 3)
                eles = str.Trim().Split(' ');

            if (eles.Length != 3) {//not valid color, reset
                this._setColor(e.RowIndex, li_colors[1, e.RowIndex].Style.BackColor, null);
                return;
            }

            try {
                int r = int.Parse(eles[0]);
                int g = int.Parse(eles[1]);
                int b = int.Parse(eles[2]);

                Color clr = Color.FromArgb(r, g, b);
                if (clr != li_colors[1, e.RowIndex].Style.BackColor) {
                    this._setColor(e.RowIndex, Color.FromArgb(r, g, b), null);
                    m_modified = true;
                }
            }
            catch {
                this._setColor(e.RowIndex, li_colors[1, e.RowIndex].Style.BackColor, null);
            }
        }

        private void btn_reset_Click(object sender, EventArgs e) {
            string activeName = cbo_pallete.SelectedItem as string;
            if (activeName != "dark" && activeName != "light" || activeName == "light_blue") {
                MessageBox.Show("非内置配色方案，无法恢复默认值", "提示", MessageBoxButtons.OK);
                return;
            }

            if (MessageBox.Show("要将当前配色方案恢复为系统默认值吗?", "提示", MessageBoxButtons.YesNoCancel) == DialogResult.Yes) {
                this.loadColors(activeName, true);
                m_modified = true;
                this.save();
            }
        }
    }
}

