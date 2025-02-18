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
    public partial class Form_Move_ByMirror : UI.ParaForm
    {
        public static Form_Move_ByMirror m_instance = new Form_Move_ByMirror();

        public static Form_Move_ByMirror instance => m_instance;

        public Form_Move_ByMirror() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.382f, 0.2f);
            this._reset();
        }

        private void _initControls() {
            this.updateTabOrder();
        }

        private void _reset() {
            this.updateDispUnitEnob(Form_Options.instance.props);

            tb_x0.Value = 0.0;
            tb_y0.Value = 0.0;
            tb_angl.Value = 0.0;
        }

        public void show(DObject paras) {
            this.updateDispUnitEnob(Form_Options.instance.props);

            if (tb_angl.unitDisp == "rad")
                lb_angl.Text = "弧度";
            else
                lb_angl.Text = "角度";

            if (paras != null)
                this.updateUIByParas(paras);

            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_move_Click(object sender, EventArgs e) {
            var res = Native.moveCurObjsByMirrorInWorkSpace(tb_x0.Value, tb_y0.Value, tb_angl.Value);
            if (res == Define.MoveResult.MOVE_RESULT_OBJ_LOCKED)
                MessageBox.Show("有锁定对象，无法移动", "提示", MessageBoxButtons.OK);
        }

        private void btn_reset_Click(object sender, EventArgs e) {
            this._reset();
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            //IFWrapper.Native.activateController("Move_By_Mirror");
        }
    }
}
