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
    public partial class Form_CreateViewByGrid : UI.ParaForm
    {
        public static Form_CreateViewByGrid m_instance = new Form_CreateViewByGrid();

        public static Form_CreateViewByGrid instance => m_instance;

        public Form_CreateViewByGrid() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            bar_cfg.init(this, "CFG_CREATE_VIEW_BY_GRID_PROPS");

            cbo_number_xy.ClearItems();
            cbo_number_xy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_ALL, "全部");
            cbo_number_xy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_FIRST, "第一");
            cbo_number_xy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_LAST, "最后");
            cbo_number_xy.UpdateItems();

            cbo_number_xz.ClearItems();
            cbo_number_xz.AddItem((int)Define.ViewNoRange.VIEWNORANGE_ALL, "全部");
            cbo_number_xz.AddItem((int)Define.ViewNoRange.VIEWNORANGE_FIRST, "第一");
            cbo_number_xz.AddItem((int)Define.ViewNoRange.VIEWNORANGE_LAST, "最后");
            cbo_number_xz.UpdateItems();

            cbo_number_zy.ClearItems();
            cbo_number_zy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_ALL, "全部");
            cbo_number_zy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_FIRST, "第一");
            cbo_number_zy.AddItem((int)Define.ViewNoRange.VIEWNORANGE_LAST, "最后");
            cbo_number_zy.UpdateItems();

            this.updateTabOrder();
        }

        public void show(DObject props) {
            bar_cfg.reset();
            this.setChecks(true);
            this.refreshViewCfgs();

            if (props == null) {
                var result = Native.getCommonCfg("CFG_CREATE_VIEW_BY_GRID_PROPS", "$runtime");
                this.updateUIByParas(result);
                this.Visible = true;
            }
            else {
                this.updateUIByParas(props);
                this.Visible = true;
            }
        }

        public void refreshViewCfgs() {
            cbo_props_xy.Items.Clear();
            cbo_props_xz.Items.Clear();
            cbo_props_zy.Items.Clear();

            string[] cfgs = Native.getCommonCfgsByType("CFG_VIEW_PROPS", false);

            {//xy
                string oldItem = cbo_props_xy.SelectedItem as string;
                cbo_props_xy.Items.Clear();
                cbo_props_xy.Items.AddRange(cfgs);
                if (oldItem != null && cbo_props_xy.Items.Contains(oldItem))
                    cbo_props_xy.SelectedItem = oldItem;
            }

            {//xz
                string oldItem = cbo_props_xz.SelectedItem as string;
                cbo_props_xz.Items.Clear();
                cbo_props_xz.Items.AddRange(cfgs);
                if (oldItem != null && cbo_props_xz.Items.Contains(oldItem))
                    cbo_props_xz.SelectedItem = oldItem;
            }

            {//zy
                string oldItem = cbo_props_zy.SelectedItem as string;
                cbo_props_zy.Items.Clear();
                cbo_props_zy.Items.AddRange(cfgs);
                if (oldItem != null && cbo_props_zy.Items.Contains(oldItem))
                    cbo_props_zy.SelectedItem = oldItem;
            }
        }

        private void btn_create_Click(object sender, EventArgs e) {
            //Native.createModelViewsByCurGrid((Define.ViewNoRange)cbo_number_zy.SelectedKey, tb_prefix_zy.Value, cbo_props_zy.SelectedItem as string,
            //                                (Define.ViewNoRange)cbo_number_xz.SelectedKey, tb_prefix_xz.Value, cbo_props_xz.SelectedItem as string,
            //                                (Define.ViewNoRange)cbo_number_xy.SelectedKey, tb_prefix_xy.Value, cbo_props_xy.SelectedItem as string);

            //this.hide();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_props_xy_Click(object sender, EventArgs e) {
            Form_ViewProps.instance.show(cbo_props_xy.SelectedItem as string);
        }

        private void btn_props_zy_Click(object sender, EventArgs e) {
            Form_ViewProps.instance.show(cbo_props_xy.SelectedItem as string);
        }

        private void btn_props_xz_Click(object sender, EventArgs e) {
            Form_ViewProps.instance.show(cbo_props_xy.SelectedItem as string);
        }
    }
}
