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
    public partial class Form_ViewDisplaySetting_Context : UserCtrl.ContextForm
    {
        private static Form_ViewDisplaySetting_Context m_instance = new Form_ViewDisplaySetting_Context();
        public static Form_ViewDisplaySetting_Context instance => m_instance;

        private UserCtrl.Check[] m_visCtrls = null;
        private UserCtrl.Check[] m_compVisCtrls = null;
        private long m_hideFrame = -1;

        public Form_ViewDisplaySetting_Context() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            m_visCtrls = new UserCtrl.Check[] {
                chb_pt_vis,
                chb_part_vis,
                chb_bolt_vis,
                chb_hole_vis,
                chb_weld_vis,
                chb_construct_pl_vis,
                chb_rfbar_vis,
                chb_surf_treat_vis,
                chb_cut_vis,
                chb_fitting_vis,
                chb_comp_symbol_vis,
                chb_grid_vis,
                chb_construct_line_vis,
                chb_ref_obj_vis
            };

            m_compVisCtrls = new UserCtrl.Check[] {
                chb_part_vis_comp,
                chb_bolt_vis_comp,
                chb_hole_vis_comp,
                chb_weld_vis_comp,
                chb_construct_pl_vis_comp,
                chb_rfbar_vis_comp,
                chb_cut_vis_comp,
                chb_fitting_vis_comp,
                chb_comp_symbol_vis_comp
            };

            foreach (var chb in m_visCtrls)
                chb.MouseClick += onCheckBoxUserChanged;
            
            foreach (var chb in m_compVisCtrls)
                chb.MouseClick += onCheckBoxUserChanged;

            chb_vis_all.MouseClick += chb_vis_all_Click;
            chb_vis_comp_all.MouseClick += chb_vis_comp_all_Click;

            cbo_part_quality.ClearItems();
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_FAST, "快速");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_EXACT, "精确");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_REF_LINE, "参考线");
            cbo_part_quality.AddItem(Define.ViewRenderQuality.RENDERQUALITY_DETAIL, "高精细(较慢)");
            cbo_part_quality.UpdateItems();
            cbo_part_quality.SelectionChangeCommitted += onComboChanged;

            cbo_part_mode.ClearItems();
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_WIREFRAME, "线框");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_SHADED_WIREFRAME, "半透明");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_HIDDEN_LINES, "隐藏线");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_EDGEFACE, "着色");
            cbo_part_mode.AddItem(Define.ViewRenderMode.RENDERMODE_SEL_ONLY, "仅选中");
            cbo_part_mode.UpdateItems();
            cbo_part_mode.SelectionChangeCommitted += onComboChanged;

            cbo_part_mode_comp.ClearItems();
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_WIREFRAME, "线框");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_SHADED_WIREFRAME, "半透明");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_HIDDEN_LINES, "隐藏线");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_EDGEFACE, "着色");
            cbo_part_mode_comp.AddItem(Define.ViewRenderMode.RENDERMODE_SEL_ONLY, "仅选中");
            cbo_part_mode_comp.UpdateItems();
            cbo_part_mode_comp.SelectionChangeCommitted += onComboChanged;

            rb_bolt_quality.labels = new string[] { "快速", "精确" };
            rb_bolt_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_bolt_quality.refresh();
            rb_bolt_quality.onUserModified += onRadioButtonChanged;

            rb_hole_quality.labels = new string[] { "快速", "精确" };
            rb_hole_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_hole_quality.refresh();
            rb_hole_quality.onUserModified += onRadioButtonChanged;

            rb_weld_quality.labels = new string[] { "快速", "精确" };
            rb_weld_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_weld_quality.refresh();
            rb_weld_quality.onUserModified += onRadioButtonChanged;

            rb_rfbar_quality.labels = new string[] { "快速", "精确" };
            rb_rfbar_quality.keys = new object[] { Define.ViewRenderQuality.RENDERQUALITY_FAST, Define.ViewRenderQuality.RENDERQUALITY_EXACT };
            rb_rfbar_quality.refresh();
            rb_rfbar_quality.onUserModified += onRadioButtonChanged;

            chb_part_center_line_vis.MouseClick += onCheckBoxUserChanged;
            chb_part_center_line_vis_comp.MouseClick += onCheckBoxUserChanged;
            chb_part_ref_line_vis.MouseClick += onCheckBoxUserChanged;
            chb_part_ref_line_vis_comp.MouseClick += onCheckBoxUserChanged;
            chb_part_label_vis.MouseClick += onCheckBoxUserChanged;
            chb_part_label_vis_comp.MouseClick += onCheckBoxUserChanged;
            chb_part_label_vis_ass_main.MouseClick += onCheckBoxUserChanged;
            chb_connection_text_vis.MouseClick += onCheckBoxUserChanged;

            li_part_label_props.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);
            li_part_label_active_props.Font = new Font(this.Font.FontFamily, DPI.Scale(13F), this.Font.Style, System.Drawing.GraphicsUnit.Pixel);

            li_part_label_props.Items.Clear();
            li_part_label_active_props.Items.Clear();
            li_part_label_props.Items.AddRange(Form_ViewDisplaySetting.instance.partLabels());

            btn_part_lable_add.MouseClick += btn_part_lable_add_Click;
            btn_part_label_remove.MouseClick += btn_part_label_remove_Click;

            tb_pt_size.Validated += onPointSizeChanged;

            rb_pt_size_mode.labels = new string[] { "所有视图", "当前视图" };
            rb_pt_size_mode.keys = new object[] { 0, 1 };
            rb_pt_size_mode.refresh();

            this.updateTabOrder();
        }

        public void show(Form owner, Rectangle rcBtn) {
            var qry = Native.focusedViewDisplaySettingProps();
            this.updateUIByParas(qry);
            this._updateVisAll();

            if (this.Visible) 
                return;

            var pos = evalShowPos(Native.screenWorkAreaRelativeToPrimary(this.Handle), rcBtn);
            this.Owner = owner;
            this.Location = pos;
            this.Visible = true;
            this.focus();
        }

        public long hideFrame => m_hideFrame;

        public override void updateUIByParas(DObject paras, Control root = null) {
            base.updateUIByParas(paras, root);

            li_part_label_active_props.Items.Clear();
            if (paras.ContainsKey("part_label_props")) {
                string[] src = paras["part_label_props"] as string[];
                for (int i = 0; i < src.Length; ++i) {
                    if (Form_ViewDisplaySetting.instance.partProp2LabelMap.ContainsKey(src[i]))
                        li_part_label_active_props.Items.Add(Form_ViewDisplaySetting.instance.partProp2LabelMap[src[i]]);
                }
            }
        }

        private void _updateVisAll() {
            //for vis all
            bool allCheck = true;
            foreach (var ctrl in m_visCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            chb_vis_all.Checked = allCheck;

            //for comp vis all
            allCheck = true;
            foreach (var ctrl in m_compVisCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            chb_vis_comp_all.Checked = allCheck;
        }

        private void chb_vis_all_Click(object sender, EventArgs e) {
            bool allCheck = true;
            foreach (var ctrl in m_visCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            var paras = new DObject();
            foreach (var ctrl in m_visCtrls) {
                ctrl.Checked = !allCheck;

                if (!String.IsNullOrEmpty(ctrl.PropName))
                    paras[ctrl.PropName] = ctrl.Checked;
            }

            Native.modifyDisplaySettingInFocusedViews(paras);
            Native.redrawFocusedViews();
        }

        private void chb_vis_comp_all_Click(object sender, EventArgs e) {
            bool allCheck = true;
            foreach (var ctrl in m_compVisCtrls) {
                if (ctrl.Checked == false) {
                    allCheck = false;
                    break;
                }
            }

            var paras = new DObject();
            foreach (var ctrl in m_compVisCtrls) {
                ctrl.Checked = !allCheck;

                if (!String.IsNullOrEmpty(ctrl.PropName))
                    paras[ctrl.PropName] = ctrl.Checked;
            }

            Native.modifyDisplaySettingInFocusedViews(paras);
            Native.redrawFocusedViews();
        }

        private void btn_part_lable_add_Click(object sender, EventArgs e) {
            if (li_part_label_props.SelectedItem == null)
                return;

            foreach (var item in li_part_label_props.SelectedItems) {
                if (li_part_label_active_props.Items.Contains(item))
                    continue;

                li_part_label_active_props.Items.Add(item);
            }

            string[] partLabel = new string[li_part_label_active_props.Items.Count];
            int n = 0;
            foreach (string s in li_part_label_active_props.Items) {
                partLabel[n++] = Form_ViewDisplaySetting.instance.partLabel2Prop(s);
            }

            Native.modifyDisplaySettingInFocusedViews(new DObject {
                {"part_label_props", partLabel }
            });
            Native.redrawFocusedViews();
        }

        private void btn_part_label_remove_Click(object sender, EventArgs e) {
            if (li_part_label_active_props.SelectedItems.Count == 0)
                return;

            object[] selItems = new object[li_part_label_active_props.SelectedItems.Count];
            li_part_label_active_props.SelectedItems.CopyTo(selItems, 0);

            foreach (var item in selItems) {
                li_part_label_active_props.Items.Remove(item);
            }

            string[] partLabel = new string[li_part_label_active_props.Items.Count];
            int n = 0;
            foreach (string s in li_part_label_active_props.Items) {
                partLabel[n++] = Form_ViewDisplaySetting.instance.partLabel2Prop(s);
            }

            Native.modifyDisplaySettingInFocusedViews(new DObject {
                {"part_label_props", partLabel }
            });
            Native.redrawFocusedViews();
        }

        private void onComboChanged(object sender, EventArgs e) {
            var cbo = sender as UserCtrl.ComboBoxKey;
            if (cbo == null
                || String.IsNullOrEmpty(cbo.PropName)
                || cbo.SelectedIndex < 0)
                return;

            Native.modifyDisplaySettingInFocusedViews(new DObject {
                {cbo.PropName, cbo.SelectedKey }
            });
            Native.redrawFocusedViews();
        }

        private void onCheckBoxUserChanged(object sender, MouseEventArgs e) {
            var chb = sender as UserCtrl.Check;
            if (chb == null
                || String.IsNullOrEmpty(chb.PropName))
                return;

            Native.modifyDisplaySettingInFocusedViews(new DObject {
                {chb.PropName, chb.Checked }
            });
            Native.redrawFocusedViews();
        }

        private void onRadioButtonChanged(object sender, EventArgs e) {
            var rb = sender as UserCtrl.RadioButtons;
            if (rb == null
                || String.IsNullOrEmpty(rb.PropName)
                || rb.Value == null)
                return;

            Native.modifyDisplaySettingInFocusedViews(new DObject {
                {rb.PropName, rb.Value }
            });
            Native.redrawFocusedViews();
        }

        private void onPointSizeChanged(object sender, EventArgs e) {
            var ptSizeMode = rb_pt_size_mode.Value;
            if (ptSizeMode != null && ptSizeMode.Equals(0)) {
                Native.modifyPointSizeInAllViews(tb_pt_size.Value);
                Native.redrawActiveViews();
            }
            else {
                Native.modifyDisplaySettingInFocusedViews(new DObject {
                    {"pt_size", tb_pt_size.Value }
                });

                Native.redrawFocusedViews();
            }
        }

        protected override void OnDeactivate(EventArgs e) {
            base.OnDeactivate(e);

            m_hideFrame = Native.STCoreFrame();
            this.hide();
        }

    }
}
