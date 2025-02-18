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
    public partial class Form_Move_ByLineStride : UI.ParaForm
    {
        public static Form_Move_ByLineStride m_instance = new Form_Move_ByLineStride();

        public static Form_Move_ByLineStride instance => m_instance;

        public Form_Move_ByLineStride() {
            InitializeComponent();

            this.Owner = MainForm.instance;

            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.382f, 0.2f);

            this._reset();
        }

        private void _initControls() {
            tb_dx.Value = 0.0;
            tb_dy.Value = 0.0;
            tb_dz.Value = 0.0;

            this.updateTabOrder();
        }

        private void _reset() {
            tb_dx.Value = 0.0;
            tb_dy.Value = 0.0;
            tb_dz.Value = 0.0;
        }

        public void show() {
            this.updateDispUnitEnob(Form_Options.instance.props);

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
            double dx = chb_dX.Checked ? tb_dx.Value : 0;
            double dy = chb_dY.Checked ? tb_dy.Value : 0;
            double dz = chb_dZ.Checked ? tb_dz.Value : 0;

            if (Native.curObjIds().Length == 0 && Native.focusedViewIds().Length > 0) { //移动视图平面
                if (MessageBox.Show("没有选中物体，是否移动视图平面?", "提示", MessageBoxButtons.OKCancel) == DialogResult.OK) {
                    Native.moveFocusedViewsPlaneByLineStrideInWorkSpace(dx, dy, dz);
                }
            }
            else { //移动物体
                var res = Native.moveCurObjsByLineStrideInWorkSpace(dx, dy, dz);
                if (res == Define.MoveResult.MOVE_RESULT_OBJ_LOCKED)
                    MessageBox.Show("有锁定对象，无法移动", "提示", MessageBoxButtons.OK);
            }
        }

        private void btn_reset_Click(object sender, EventArgs e) {
            this._reset();
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            IFWrapper.Native.activateController("Move_By_LineStride");
        }

        private void btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks();
        }
    }
}
