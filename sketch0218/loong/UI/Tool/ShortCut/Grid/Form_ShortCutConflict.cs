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
    public partial class Form_ShortCutConflict : UI.ParaForm
    {
        public Form_ShortCutConflict() {
            InitializeComponent();

            this.disposeOnClose = true;
        }

        public DialogResult show(uint flag, List<FuncUnit> conflicts) {
            li_conflicts.Items.Clear();
            li_conflicts.Items.AddRange(conflicts.Select(fu => fu.tip + "[" + fu.shortCutTip + "]").ToArray());

            lb_msg.Text = "[" + FuncUnit.shortCut2Str(flag) + "] 与下列功能冲突";
            return this.ShowDialog();
        }

        private void Btn_overlap_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.OK;
            this.close();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.Cancel;
            this.close();
        }
    }
}
