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
    public partial class Form_SetWorkPlaneByBaseAxis : UI.ParaForm
    {
        public static Form_SetWorkPlaneByBaseAxis m_instance = new Form_SetWorkPlaneByBaseAxis();

        public static Form_SetWorkPlaneByBaseAxis instance => m_instance;

        public Form_SetWorkPlaneByBaseAxis()
        {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls()
        {
            cbo_work_plane.ClearItems();
            cbo_work_plane.AddItem(Define.SpacePlane.PLANE_XY, "X-Y");
            cbo_work_plane.AddItem(Define.SpacePlane.PLANE_YZ, "Y-Z");
            cbo_work_plane.AddItem(Define.SpacePlane.PLANE_ZX, "X-Z");
            cbo_work_plane.UpdateItems();
            cbo_work_plane.SelectedKey = Define.SpacePlane.PLANE_XY;

            tb_z.Value = 0.0;

            this.updateTabOrder();
        }

        public void show()
        {
            this.updateDispUnitEnob(Form_Options.instance.props);

            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            this.hide();
        }

        private void btn_modify_Click(object sender, EventArgs e)
        {
            Mat43 workAxis = new Mat43(1.0);
            if ((Define.SpacePlane)cbo_work_plane.SelectedKey == Define.SpacePlane.PLANE_XY)
            {
                workAxis.X = new Vec3(1.0, 0.0, 0.0);
                workAxis.Y = new Vec3(0.0, 1.0, 0.0);
                workAxis.Z = new Vec3(0.0, 0.0, 1.0);
            }
            else if ((Define.SpacePlane)cbo_work_plane.SelectedKey == Define.SpacePlane.PLANE_YZ)
            {
                workAxis.X = new Vec3(0.0, 1.0, 0.0);
                workAxis.Y = new Vec3(0.0, 0.0, 1.0);
                workAxis.Z = new Vec3(1.0, 0.0, 0.0);
            }
            else if ((Define.SpacePlane)cbo_work_plane.SelectedKey == Define.SpacePlane.PLANE_ZX)
            {
                workAxis.X = new Vec3(-1.0, 0.0, 0.0);
                workAxis.Y = new Vec3(0.0, 0.0, 1.0);
                workAxis.Z = new Vec3(0.0, 1.0, 0.0);
            }

            Vec3 axisZ = workAxis.Z;
            workAxis.Pos = axisZ * tb_z.Value;
            Native.setFocusedWorkAxis(workAxis);
        }
        
    }
}
