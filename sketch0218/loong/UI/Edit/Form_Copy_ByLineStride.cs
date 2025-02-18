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
    public partial class Form_Copy_ByLineStride: UI.ParaForm
    {
        public static Form_Copy_ByLineStride m_instance = new Form_Copy_ByLineStride();

        public static Form_Copy_ByLineStride instance => m_instance;

        public Form_Copy_ByLineStride()
        {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.382f, 0.2f);
        }

        private void _initControls()
        {
            tb_dX.Value = new double[] {0.0};
            tb_dY.Value = new double[] {0.0};
            tb_dZ.Value = new double[] {0.0};
            tb_count.Value = 1;

            this.updateTabOrder();
        }

        public void show()
        {
            this.updateDispUnitEnob(Form_Options.instance.props);

            this.Visible = true;
            this.focus();
        }

        private void btn_ok_Click(object sender, EventArgs e)
        {
            this.hide();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_reset_Click(object sender, EventArgs e)
        {
            tb_dX.Value = new double[] {0.0};
            tb_dY.Value = new double[] {0.0};
            tb_dZ.Value = new double[] {0.0};
            tb_count.Value = 1;
        }

        private void btn_chb_Click(object sender, EventArgs e)
        {
            this.switchChecks();
        }

        private void btn_copy_Click(object sender, EventArgs e)
        {
            double[] dx = chb_dX.Checked ? tb_dX.Value : new double[] {0.0};
            double[] dy = chb_dY.Checked ? tb_dY.Value : new double[] {0.0};
            double[] dz = chb_dZ.Checked ? tb_dZ.Value : new double[] {0.0};
            Native.copyCurObjsByLineStrideInWorkSpace(dx, dy, dz, tb_count.Value);
        }

        private void btn_pick_Click(object sender, EventArgs e)
        {
            //IFWrapper.Native.activateController("Copy_By_LineStride");
        }
    }
}
