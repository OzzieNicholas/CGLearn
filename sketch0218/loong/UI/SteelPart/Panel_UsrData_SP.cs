using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;

namespace UI
{
    [ToolboxItem(false)]
    public partial class Panel_UsrData_SP : UserControl
    {
        public Panel_UsrData_SP()
        {
            InitializeComponent();
            this._initControls();
        }

        private void _initControls()
        {
            this.Dock = DockStyle.Fill;

            cbo_usrdata_dr_main_view.ClearItems();
            cbo_usrdata_dr_main_view.AddItem(Define.ObjectSide.SIDE_NONE, "无");
            cbo_usrdata_dr_main_view.AddItem(Define.ObjectSide.SIDE_TOP, "顶面");
            cbo_usrdata_dr_main_view.AddItem(Define.ObjectSide.SIDE_BACK, "后面");
            cbo_usrdata_dr_main_view.AddItem(Define.ObjectSide.SIDE_BOTTOM, "底面");
            cbo_usrdata_dr_main_view.AddItem(Define.ObjectSide.SIDE_FRONT, "前面");
            cbo_usrdata_dr_main_view.UpdateItems();
        }

        public void reset() {
            tb_usrdata_comment.Value = "";
            tb_usrdata_short_commnet.Value = "";
            tb_usrdata_curve.Value = "";
            tb_usrdata_prepare_mark.Value = "";
            tb_usrdata_prepare_mark_assembly.Value = "";
            tb_usrdata_maker_name.Value = "";
            tb_usrdata_user_prop1.Value = "";
            tb_usrdata_user_prop2.Value = "";
            tb_usrdata_user_prop3.Value = "";
            tb_usrdata_user_prop4.Value = "";
            tb_usrdata_user_state.Value = "";
            cbo_usrdata_dr_main_view.SelectedKey = Define.ObjectSide.SIDE_NONE;
            tb_usrdata_product_no.Value = "";
            tb_usrdata_product_desc.Value = "";
            tb_usrdata_product_weight.Value = 0.0;
            tb_usrdata_product_company.Value = "";
            tb_guid.Value = Guid.Empty;
        }
    }
}
