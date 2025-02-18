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
    public partial class Form_Copy_ByRotate : UI.ParaForm
    {
        public static Form_Copy_ByRotate m_instance = new Form_Copy_ByRotate();

        public static Form_Copy_ByRotate instance => m_instance;

        public Form_Copy_ByRotate() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.382f, 0.2f);
            this._reset();
        }

        private void _initControls() {
            cbo_axis.ClearItems();
            cbo_axis.AddItem(Define.SpaceAxis.AXIS_X, "X");
            cbo_axis.AddItem(Define.SpaceAxis.AXIS_Y, "Y");
            cbo_axis.AddItem(Define.SpaceAxis.AXIS_Z, "Z");
            cbo_axis.UpdateItems();
            cbo_axis.SelectedKey = Define.SpaceAxis.AXIS_Z;

            this.updateTabOrder();
        }

        private void _reset() {
            this.updateDispUnitEnob(Form_Options.instance.props);

            tb_x0.Value = 0.0;
            tb_y0.Value = 0.0;
            tb_z0.Value = 0.0;

            tb_count.Value = 1;
            tb_zoff.Value = 0.0;
            tb_angl.Value = new double[] { 180.0 };
            cbo_axis.SelectedKey = Define.SpaceAxis.AXIS_Z;
        }

        public void show() {
            this.updateDispUnitEnob(Form_Options.instance.props);

            if (tb_angl.unitDisp == "rad")
                lb_angl.Text = "旋转弧度";
            else
                lb_angl.Text = "旋转角度";

            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_copy_Click(object sender, EventArgs e) {
            Native.copyCurObjsByRotateInWorkSpace(tb_x0.Value, tb_y0.Value, tb_z0.Value, (Define.SpaceAxis)cbo_axis.SelectedKey, tb_angl.Value, tb_zoff.Value, tb_count.Value);
        }

        private void btn_reset_Click(object sender, EventArgs e) {
            this._reset();
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            //IFWrapper.Native.activateController("Copy_By_Rotate");
        }
    }
}
