using System;
using System.Collections.Generic;
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
using IFWrapper;

namespace UI
{
    public partial class Form_Snap_Point_By_Input : Form
    {
        public static Form_Snap_Point_By_Input m_instance = new Form_Snap_Point_By_Input();
        public static Form_Snap_Point_By_Input instance = m_instance;

        public Form_Snap_Point_By_Input() {
            InitializeComponent();
            this._initControls();
        }

        private void _initControls() {
            this.Owner = MainForm.instance;
            this.TopMost = true;

            tb_offset.KeyDown += Tb_offset_KeyDown;
        }

        public void show(string input) {
            if (this.Visible
                && this.Focused) {
                return;
            }

            if (!this.Visible) {
                this.Visible = true;
                this.Opacity = 0.8;
            }
            else
                this.Focus();
            
            tb_offset.Text = input;
            tb_offset.Select(tb_offset.Text.Length, 1);
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (e.CloseReason == CloseReason.UserClosing) {
                e.Cancel = true;
                this.Hide();
            }
        }

        private void Tb_offset_KeyDown(object sender, KeyEventArgs e) {
            if (this.Opacity < 1.0)
                this.Opacity = 1.0;

            if (e.KeyCode == Keys.Enter) {
                Native.snapPointByInput(Define.LayoutCellType.CELL_NONE, tb_offset.Value.Trim(), Define.SNAP_PT_FREE, false);
                //this.hide();
                this.Hide();
            }
            else if (e.KeyCode == Keys.Escape) {
                this.Hide();
            }
        }
    }
}
