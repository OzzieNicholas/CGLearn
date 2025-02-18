using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class Bar_Pick : CustomToolBar
    {
        protected static Bar_Pick m_instance = new Bar_Pick();

        protected Dictionary<ToolStripButton, uint> m_ctrl2Flag = new Dictionary<ToolStripButton, uint>();
        protected Dictionary<uint, ToolStripButton> m_flag2Ctrl = new Dictionary<uint, ToolStripButton>();

        protected Dictionary<ToolStripButton, Define.PickMode> m_ctrl2Mode = new Dictionary<ToolStripButton, Define.PickMode>();
        protected Dictionary<Define.PickMode, ToolStripButton> m_mode2Ctrl = new Dictionary<Define.PickMode, ToolStripButton>();

        protected ToolStripComboBox m_cboObjFilter = new ToolStripComboBox();
        protected ToolFuncButton m_btnObjFilter = null;
        protected ToolFuncButton m_btnSelGroup = null;
        protected ToolFuncButton m_btnSelUndo = null;
        protected ToolFuncButton m_btnSelRedo = null;

        public static Bar_Pick instance => m_instance;

        public Bar_Pick() {
            ToolStripButton btn = null;

            this.SuspendLayout();

            btn = new BarToggleButton("tb_icon_pick_all");
            btn.ToolTipText = "全选";
            m_ctrl2Flag[btn] = Define.PICK_BAR_ALL;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_connect");
            btn.ToolTipText = "节点";
            m_ctrl2Flag[btn] = Define.PICK_CONNECTION;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_part");
            btn.ToolTipText = "零件";
            m_ctrl2Flag[btn] = Define.PICK_PART;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_surf");
            btn.ToolTipText = "表面处理";
            m_ctrl2Flag[btn] = Define.PICK_SURF_TREATMENT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_point");
            btn.ToolTipText = "点";
            m_ctrl2Flag[btn] = Define.PICK_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_grid");
            btn.ToolTipText = "轴网";
            m_ctrl2Flag[btn] = Define.PICK_GRID;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_grid_line");
            btn.ToolTipText = "单根轴线";
            m_ctrl2Flag[btn] = Define.PICK_GRID_LINE;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_weld");
            btn.ToolTipText = "焊接";
            m_ctrl2Flag[btn] = Define.PICK_WELD;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_cut");
            btn.ToolTipText = "切割";
            m_ctrl2Flag[btn] = Define.PICK_CUT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_view");
            btn.ToolTipText = "视图";
            m_ctrl2Flag[btn] = Define.PICK_VIEW;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_bolts");
            btn.ToolTipText = "螺栓组";
            m_ctrl2Flag[btn] = Define.PICK_BOLTS;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_single_bolt");
            btn.ToolTipText = "单个螺栓";
            m_ctrl2Flag[btn] = Define.PICK_SINGLE_BOLT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_rfbar");
            btn.ToolTipText = "钢筋";
            m_ctrl2Flag[btn] = Define.PICK_RFBAR;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_plane");
            btn.ToolTipText = "平面";
            m_ctrl2Flag[btn] = Define.PICK_PLANE;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_dist");
            btn.ToolTipText = "距离";
            m_ctrl2Flag[btn] = Define.PICK_DISTANCE;
            this.Items.Add(btn);

            this.Items.Add(new ToolStripSeparator());

            btn = new BarToggleButton("tb_icon_pick_mode_comp");
            btn.ToolTipText = "选择组件";
            m_ctrl2Mode[btn] = Define.PickMode.PICK_MODE_COMPONENT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_mode_comp_obj");
            btn.ToolTipText = "选择组件内对象";
            m_ctrl2Mode[btn] = Define.PickMode.PICK_MODE_COMPONENT_OBJ;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_mode_ass");
            btn.ToolTipText = "选择构件";
            m_ctrl2Mode[btn] = Define.PickMode.PICK_MODE_ASSEMBLE;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_pick_mode_ass_obj");
            btn.ToolTipText = "选择构件内对象";
            m_ctrl2Mode[btn] = Define.PickMode.PICK_MODE_ASSEMBLE_OBJ;
            this.Items.Add(btn);

            foreach (var it in m_ctrl2Flag) {
                it.Key.CheckOnClick = true;
                it.Key.DoubleClickEnabled = true;
                it.Key.DisplayStyle = ToolStripItemDisplayStyle.Image;
                it.Key.ImageScaling = ToolStripItemImageScaling.None;
                it.Key.Click += FlagBtn_Click;
                it.Key.DoubleClick += FlagBtn_DoubleClick;

                m_flag2Ctrl[it.Value] = it.Key;
            }

            foreach (var it in m_ctrl2Mode) {
                it.Key.DisplayStyle = ToolStripItemDisplayStyle.Image;
                it.Key.ImageScaling = ToolStripItemImageScaling.None;
                it.Key.Click += ModeBtn_Click;

                m_mode2Ctrl[it.Value] = it.Key;
            }

            m_cboObjFilter.ComboBox.SelectionChangeCommitted += ObjFilter_SelectionChangeCommitted;
            m_cboObjFilter.DropDownStyle = ComboBoxStyle.DropDownList;
            m_cboObjFilter.ToolTipText = "物体选择条件过滤";
            this.Items.Add(m_cboObjFilter);

            m_btnObjFilter = new ToolFuncButton();
            m_btnObjFilter.funcUnit = FuncUnit.get("sel_pick_obj_filter_props");
            this.Items.Add(m_btnObjFilter);

            this.Items.Add(new ToolStripSeparator());

            m_btnSelGroup = new ToolFuncButton();
            m_btnSelGroup.funcUnit = FuncUnit.get("sel_obj_group_mgr");
            this.Items.Add(m_btnSelGroup);

            m_btnSelUndo = new ToolFuncButton();
            m_btnSelUndo.funcUnit = FuncUnit.get("sel_cur_objs_undo");
            this.Items.Add(m_btnSelUndo);

            m_btnSelRedo = new ToolFuncButton();
            m_btnSelRedo.funcUnit = FuncUnit.get("sel_cur_objs_redo");
            this.Items.Add(m_btnSelRedo);

            this.ResumeLayout();

            this.BarName = "拣选";

            this.refreshObjFilterCfgs();
        }

        public void refresh(Define.PickMode pickMode, uint pickFlag, string pickObjFilterCfg) {
            this.refreshObjFilterCfgs();

            this.pickMode = pickMode;
            this.pickFlag = pickFlag;
            this.objFilter = pickObjFilterCfg;
        }

        public void refreshObjFilterCfgs() {
            string[] cfgs = Native.getCommonCfgsByType("CFG_OBJ_FILTER_PROPS", false);
            string oldItem = m_cboObjFilter.SelectedItem as string;
            m_cboObjFilter.Items.Clear();
            m_cboObjFilter.Items.AddRange(cfgs);
            if (oldItem != null && m_cboObjFilter.Items.Contains(oldItem))
                m_cboObjFilter.SelectedItem = oldItem;
        }

        public string objFilter {
            get => m_cboObjFilter.SelectedItem as string;
            set {
                if (!m_cboObjFilter.Items.Contains(value))
                    return;

                m_cboObjFilter.SelectedItem = value;
            }
        }


        public uint pickFlag {
            get {
                uint flag = 0;
                foreach (var it in m_ctrl2Flag) {
                    flag |= it.Key.Checked ? it.Value : 0;
                }
                return flag;
            }

            set {
                foreach (var it in m_ctrl2Flag) {
                    it.Key.Checked = ((value & it.Value) == it.Value);
                }
            }
        }

        public Define.PickMode pickMode {
            get {
                foreach (var it in m_ctrl2Mode) {
                    if (it.Key.Checked)
                        return it.Value;
                }

                return Define.PickMode.PICK_MODE_NONE;
            }

            set {
                foreach (var it in m_ctrl2Mode) 
                    it.Key.Checked = (it.Value == value);

                Native.setPickMode(Define.LayoutCellType.CELL_VIEW, value, false);
            }
        }


        public override void refresh() {
            //this.pickMode = Native.pickMode(Define.LayoutCellType.CELL_VIEW);
            //this.pickFlag = Native.pickFlag(Define.LayoutCellType.CELL_VIEW);

            this.refreshObjFilterCfgs();
        }

        private void FlagBtn_Click(object sender, EventArgs e) {
            if (!(sender is BarToggleButton))
                return;

            BarToggleButton btn = sender as BarToggleButton;
            if (!m_ctrl2Flag.ContainsKey(btn))
                return;

            uint flag = m_ctrl2Flag[btn];
            if (flag == Define.PICK_BAR_ALL) {
                if (btn.Checked) {
                    Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_BAR_ALL, true, false);
                }
                else {
                    Native.setPickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_PART, false);
                    
                    flag = Define.PICK_PART;
                }

                foreach (var it in m_ctrl2Flag) {
                    it.Key.Checked = ((flag & it.Value) == it.Value);
                }
            }
            else if (flag == Define.PICK_GRID) {
                Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_GRID, btn.Checked, false);
                
                if (btn.Checked)
                    Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_GRID_LINE, false, false);

                m_flag2Ctrl[Define.PICK_GRID_LINE].Checked = false;
            }
            else if (flag == Define.PICK_GRID_LINE) {
                Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_GRID_LINE, btn.Checked, false);

                if (btn.Checked) {
                    Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_GRID, false, false);
                }

                m_flag2Ctrl[Define.PICK_GRID].Checked = false;
            }
            else if (flag == Define.PICK_BOLTS) {
                Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_BOLTS, btn.Checked, false);

                if (btn.Checked) {
                    Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_SINGLE_BOLT, false, false);
                }

                m_flag2Ctrl[Define.PICK_SINGLE_BOLT].Checked = false;
            }
            else if (flag == Define.PICK_SINGLE_BOLT) {
                Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_SINGLE_BOLT, btn.Checked, false);
                
                if (btn.Checked) {
                    Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, Define.PICK_BOLTS, false, false);
                }

                m_flag2Ctrl[Define.PICK_BOLTS].Checked = false;
            }
            else {
                Native.updatePickFlag(Define.LayoutCellType.CELL_VIEW, flag, btn.Checked, false);
            }
        }

        private void FlagBtn_DoubleClick(object sender, EventArgs e) {
            if (!(sender is BarToggleButton))
                return;

            BarToggleButton btn = sender as BarToggleButton;

            if (!m_ctrl2Flag.ContainsKey(btn))
                return;

            uint flag = m_ctrl2Flag[btn];
            foreach (var it in m_ctrl2Flag) {
                it.Key.Checked = ((flag & it.Value) == it.Value);
            }

            Native.setPickFlag(Define.LayoutCellType.CELL_VIEW, flag, false);
        }

        private void ModeBtn_Click(object sender, EventArgs e) {
            if (!(sender is BarToggleButton))
                return;

            BarToggleButton btn = sender as BarToggleButton;

            if (!m_ctrl2Mode.ContainsKey(btn))
                return;

            if (btn.Checked) {
                btn.Checked = false;
                Native.setPickMode(Define.LayoutCellType.CELL_VIEW, Define.PickMode.PICK_MODE_NONE, false);
                return;
            }

            Define.PickMode pickMode = m_ctrl2Mode[btn];
            foreach (var it in m_ctrl2Mode) {
                if (it.Key == btn)
                    continue;

                it.Key.Checked = false;
            }

            Native.setPickMode(Define.LayoutCellType.CELL_VIEW, pickMode, false);

            btn.Checked = true;
        }

        private void ObjFilter_SelectionChangeCommitted(object sender, EventArgs e) {
            if (m_cboObjFilter.SelectedItem == null)
                return;

            Native.setPickObjFilterCfg((string)m_cboObjFilter.SelectedItem);
        }
    }
}
