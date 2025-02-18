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
    public partial class Form_CreateView_Base: UI.ParaForm
    {
        public static Form_CreateView_Base m_instance = new Form_CreateView_Base();

        public static Form_CreateView_Base instance => m_instance;

        public Form_CreateView_Base()
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

            this.updateTabOrder();
        }

        public void show()
        {
            this.updateDispUnitEnob(Form_Options.instance.props);

            var result = Native.getCommonCfg("CFG_CREATE_VIEW_BASE", "$runtime");
            this.updateUIByParas(result);
            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            this.hide();
        }

        private void btn_create_Click(object sender, EventArgs e)
        {
            if (cbo_work_plane.SelectedKey == null)
                return;

            var paras = this.getParasByUI();

            //save config
            Native.setCommonCfg("CFG_CREATE_VIEW_BASE", "$runtime", paras, true, false);

            //create grid
            Native.createModelViewBase((Define.SpacePlane)cbo_work_plane.SelectedKey, tb_height.Value);
        }
        
    }
}
