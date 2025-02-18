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
    public partial class Form_ViewProps : UI.ParaForm
    {
        public static Form_ViewProps m_instance = new Form_ViewProps();
        private DObject m_dispSetting = null;
        private DObject m_dispFilter = null;
        private string m_runtimeCfg = null;

        public static Form_ViewProps instance => m_instance;

        public Form_ViewProps() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
            this.initStartPosition(MainForm.instance.Handle, Native.screenWorkAreaRelativeToPrimary(MainForm.instance.Handle), 0.618f, 0.2f);
        }

        public void _initControls() {
            bar_cfg.init(this, "CFG_VIEW_PROPS");

            rb_type.labels = new string[] { "平面", "3D" };
            rb_type.keys = new object[] { Define.ViewType.VIEW_2D, Define.ViewType.VIEW_3D };
            rb_type.refresh();
            rb_type.Value = Define.ViewType.VIEW_2D;
            rb_type.onUserModified += btn_modify_Click;

            rb_proj.labels = new string[] { "透视", "正交" };
            rb_proj.keys = new object[] { Define.ViewProjMode.VIEWPROJ_PERSPECTIVE, Define.ViewProjMode.VIEWPROJ_ORTHO };
            rb_proj.refresh();
            rb_proj.Value = Define.ViewProjMode.VIEWPROJ_ORTHO;
            rb_proj.onUserModified += btn_modify_Click;

            lb_name_warnings.Font = new Font(lb_name_warnings.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
            lb_name_warnings.Text = "";

            tb_name.TextChanged += Tb_name_TextChanged;

            this.updateTabOrder();
        }

        private void Tb_name_TextChanged(object sender, EventArgs e) {
            lb_name_warnings.Text = "";
        }

        public DObject dispSetting {
            get => m_dispSetting;
            set => m_dispSetting = value;
        }

        public DObject dispFilter {
            get => m_dispFilter;
            set => m_dispFilter = value;
        }

        public void show(object props) {
            bar_cfg.reset();
            this.setChecks(true);
            this.updateDispUnitEnob(Form_Options.instance.props);
            m_runtimeCfg = null;
            lb_name_warnings.Text = "";

            if (props == null) {
                var result = Native.getCommonCfg("CFG_VIEW_PROPS", "$runtime");
                this.updateUIByParas(result);
            }
            else if (props is DObject) {
                DObject viewProps = props as DObject;
                this.updateUIByParas(viewProps);

                if (viewProps.ContainsKey("runtime_cfg"))
                    m_runtimeCfg = viewProps.GetString("runtime_cfg");
            }
            else if (props is string) {//cfg name
                var qry = Native.getCommonCfg("CFG_VIEW_PROPS", props as string);
                if (qry == null)
                    return;

                this.updateUIByParas(qry);
                bar_cfg.selectedCfg = props as string;
                m_runtimeCfg = props as string;
            }

            this.Visible = true;
            this.focus();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.hide();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_VIEW_PROPS", m_runtimeCfg?? "$runtime", this.getParasByUI(), true, false);
            this.hide();
        }

        private void btn_apply_Click(object sender, EventArgs e) {
            Native.setCommonCfg("CFG_VIEW_PROPS", m_runtimeCfg?? "$runtime", this.getParasByUI(), true, false);
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            long[] viewIds = IFWrapper.Native.focusedViewIds();
            if (viewIds.Length == 0)
                return;

            LoongCAD.Util.DObject paras = this.getParasByUI();
            if (viewIds.Length == 1) {
                if (paras.ContainsKey("name")) {
                    if (!IFWrapper.Native.isViewNameValid(paras.GetString("name"), viewIds[0])) {
                        lb_name_warnings.Text = "已有同名视图，请更换名称";
                        return;
                    }

                    string curViewName = paras["name"] as string;
                    if (IFWrapper.Native.isTempView(viewIds[0]) && !curViewName.StartsWith("*")) {
                        paras["temp"] = false;
                        paras["save_temp"] = true;
                    }
                }

                Native.modifyView(viewIds[0], paras);
            }
            else {
                paras.Remove("name");
                Native.modifyViews(viewIds, paras);
            }

            Native.redrawFocusedViews();

            if (paras.ContainsKey("disp_setting")) {
                var dispSets = paras.GetDObject("disp_setting");
                if (dispSets != null
                    && dispSets.ContainsKey("pt_size")
                    && dispSets.ContainsKey("pt_size_mode")
                    && dispSets.GetInt("pt_size_mode") == 0) {
                    Native.modifyPointSizeInAllViews(dispSets.GetDouble("pt_size"));
                }
            }

            lb_name_warnings.Text = "";
        }

        private void btn_pick_Click(object sender, EventArgs e) {
            LoongCAD.Util.DObject qry = Native.focusedViewProps();
            if (qry == null)
                return;

            this.updateUIByParas(qry);
            this.setChecks(true);
            lb_name_warnings.Text = "";
        }

        private void Btn_chb_Click(object sender, EventArgs e) {
            this.switchChecks(this, null);
        }

        public override void updateUIByParas(LoongCAD.Util.DObject paras, Control root = null) {
            base.updateUIByParas(paras, root);

            if (paras.ContainsKey("disp_setting"))
                m_dispSetting = paras.GetDObject("disp_setting").Clone();
            else
                m_dispSetting = null;

            if (paras.ContainsKey("disp_filter"))
                m_dispFilter = paras.GetDObject("disp_filter").Clone();
            else
                m_dispFilter = null;
        }

        public override LoongCAD.Util.DObject getParasByUI(Control root = null, List<Control> exclude = null, bool check = true) {
            DObject props = base.getParasByUI(root, exclude, check);

            if (!check || chb_workBox_depth_down.Checked) {
                props["depth_down"] = tb_workBox_depth_down.Value;
            }

            if (!check || chb_workBox_depth_up.Checked) {
                props["depth_up"] = tb_workBox_depth_up.Value;
            }

            return props;
        }

    }
}
