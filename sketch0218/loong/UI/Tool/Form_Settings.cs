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
    public partial class Form_Settings : UI.ParaForm
    {
        public static Form_Settings m_instance = new Form_Settings();
        protected DObject m_props = new DObject();

        public static Form_Settings instance => m_instance;

        public Form_Settings() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        public DObject props => m_props;

        private void _initControls() {
            bar_cfg.init(this, "CFG_SETTINGS");

            m_props = Native.settings();

            cbo_disp_ratio.ClearItems();
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_AUTO, "自动");
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_100, "100%");
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_125, "125%");
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_150, "150%");
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_175, "175%");
            cbo_disp_ratio.AddItem(Define.DisplayRatio.DISP_RATIO_200, "200%");
            cbo_disp_ratio.UpdateItems();

            this.updateUIByParas(m_props);
            this.updateTabOrder();
        }

        public void show() {
            if (this.Visible) {
                this.focus();
                return;
            }

            this.Visible = true;
            this.focus();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            m_props.CopyFrom(this.getParasByUI());
            Native.setSettings(m_props);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            m_props.CopyFrom(this.getParasByUI());
            Native.setSettings(m_props);
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        public object customPara(string group, string pname) { 
            object val = null;
            m_props.TryGetValue(group + '$' + pname, out val);
            return val;
        }
    }
}
