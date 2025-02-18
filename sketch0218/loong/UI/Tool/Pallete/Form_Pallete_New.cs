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
using System.Windows.Forms.Design;

namespace UI
{
    public partial class Form_Pallete_New : UI.ParaForm
    {
        public static Form_Pallete_New m_instance = new Form_Pallete_New();
        public static Form_Pallete_New instance => m_instance;

        public Form_Pallete_New() {
            InitializeComponent();

            this.disposeOnClose = false;
            this.Owner = Form_PalleteManager.instance;

            this._initControls();
        }

        private void _initControls() {

        }

        public void show(string[] srcs, string src) {
            if (this.Visible) {
                this.focus();
                return;
            }

            tb_name.Text = "";
            cbo_src.Items.Clear();
            cbo_src.Items.AddRange(srcs);
            cbo_src.SelectedItem = src;
            this.ShowDialog(Form_PalleteManager.instance);
        }

        private void Btn_new_Click(object sender, EventArgs e) {
            string name = tb_name.Text.Trim();
            if (name.Length == 0) {
                MessageBox.Show("名称不能为空", "提示", MessageBoxButtons.OK);
                return;
            }

            if (!Form_PalleteManager.instance.isPalNameValid(name)) {
                MessageBox.Show("同名配色方案已存在", "提示", MessageBoxButtons.OK);
                return;
            }

            Form_PalleteManager.instance.newPallete(name, cbo_src.SelectedItem as string);

            this.hide();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }
    }
}
