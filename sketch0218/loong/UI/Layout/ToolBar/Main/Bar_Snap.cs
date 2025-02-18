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

namespace UI
{
    [ToolboxItem(false)]
    public class Bar_Snap : CustomToolBar
    {
        protected static Bar_Snap m_instance = new Bar_Snap();

        protected Dictionary<ToolStripButton, uint> m_ctrl2Flag = new Dictionary<ToolStripButton, uint>();
        protected Dictionary<uint, ToolStripButton> m_flag2Ctrl = new Dictionary<uint, ToolStripButton>();
        protected ToolStripComboBox m_cboMode = new ToolStripComboBox();
        protected ToolStripComboBox m_cboRefPl = new ToolStripComboBox();
        protected ToolStripComboBox m_cboPlFlag = new ToolStripComboBox();

        Dictionary<string, Define.SnapMode> m_modeMap = new Dictionary<string, Define.SnapMode>(){
                { "自动", Define.SnapMode.SNAP_MODE_AUTO},
                { "平面", Define.SnapMode.SNAP_MODE_PLANE},
                { "3D", Define.SnapMode.SNAP_MODE_3D},
            };

        Dictionary<string, Define.SnapRefPlane> m_refPlMap = new Dictionary<string, Define.SnapRefPlane>(){
                { "视图平面", Define.SnapRefPlane.SNAP_REF_PLANE_VIEW},
                { "工作平面", Define.SnapRefPlane.SNAP_REF_PLANE_WORK},
            };

        Dictionary<string, uint> m_plFlagMap = new Dictionary<string, uint>(){
                { "边界面", Define.SNAP_PL_BOUND},
                { "中心面", Define.SNAP_PL_CENTER},
                { "几何面", Define.SNAP_PL_GEO},
                { "切割面", Define.SNAP_PL_CUT},
                { "轴线平面", Define.SNAP_PL_AXIS},
            };

        public static Bar_Snap instance => m_instance;

        public Bar_Snap() {
            ToolStripButton btn = null;

            btn = new BarToggleButton("tb_icon_snap_pt_point_grid_cross");
            btn.ToolTipText = "捕捉点与轴网交点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_HELPER_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_end_point");
            btn.ToolTipText = "捕捉端点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_END_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_center_point");
            btn.ToolTipText = "捕捉圆心";
            m_ctrl2Flag[btn] = Define.SNAP_PT_CENTER_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_mid_point");
            btn.ToolTipText = "捕捉中点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_MID_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_cross");
            btn.ToolTipText = "捕捉交点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_CROSS_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_foot_point");
            btn.ToolTipText = "捕捉垂足";
            m_ctrl2Flag[btn] = Define.SNAP_PT_FOOT_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_line_ext");
            btn.ToolTipText = "捕捉延长线上点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_LINE_EXT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_line_nearest");
            btn.ToolTipText = "捕捉线上点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_LINE_NEAREST;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_free");
            btn.ToolTipText = "捕捉任意点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_FREE;
            this.Items.Add(btn);

            this.Items.Add(new ToolStripSeparator());

            btn = new BarToggleButton("tb_icon_snap_line");
            btn.ToolTipText = "捕捉直线";
            m_ctrl2Flag[btn] = Define.SNAP_PT_LINE;
            this.Items.Add(btn);

            this.Items.Add(new ToolStripSeparator());

            btn = new BarToggleButton("tb_icon_snap_pt_ctrl");
            btn.ToolTipText = "捕捉参考线/点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_REF_POINT;
            this.Items.Add(btn);

            btn = new BarToggleButton("tb_icon_snap_pt_geo");
            btn.ToolTipText = "捕捉几何点";
            m_ctrl2Flag[btn] = Define.SNAP_PT_GEO_POINT;
            this.Items.Add(btn);

            foreach (var it in m_ctrl2Flag) {
                it.Key.CheckOnClick = true;
                it.Key.DoubleClickEnabled = true;
                it.Key.DisplayStyle = ToolStripItemDisplayStyle.Image;
                it.Key.ImageScaling = ToolStripItemImageScaling.None;
                it.Key.Click += PtFlagBtn_Click;
                it.Key.DoubleClick += PtFlagBtn_DoubleClick;

                m_flag2Ctrl[it.Value] = it.Key;
            }

            this.Items.Add(new ToolStripSeparator());

            m_cboMode.ComboBox.SelectionChangeCommitted += Mode_SelectionChangeCommitted;
            m_cboMode.DropDownStyle = ComboBoxStyle.DropDownList;
            m_cboMode.Items.AddRange(m_modeMap.Keys.ToArray());
            this.Items.Add(m_cboMode);

            m_cboRefPl.ComboBox.SelectionChangeCommitted += RefPl_SelectionChangeCommitted;
            m_cboRefPl.DropDownStyle = ComboBoxStyle.DropDownList;
            m_cboRefPl.Items.AddRange(m_refPlMap.Keys.ToArray());
            this.Items.Add(m_cboRefPl);

            m_cboPlFlag.ComboBox.SelectionChangeCommitted += PlFlag_SelectionChangeCommitted;
            m_cboPlFlag.DropDownStyle = ComboBoxStyle.DropDownList;
            m_cboPlFlag.Items.AddRange(m_plFlagMap.Keys.ToArray());
            this.Items.Add(m_cboPlFlag);

            this.BarName = "捕捉";
        }

        public void refresh(Define.SnapMode snapMode, Define.SnapRefPlane snapRefPlane, uint snapFlagPt, uint snapFlagPl) {
            this.snapMode = snapMode;
            this.snapRefPlane = snapRefPlane;
            this.ptSnapFlag = snapFlagPt;
            this.plSnapFlag = snapFlagPl;
        }

        public uint ptSnapFlag {
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

        public Define.SnapMode snapMode {
            get {
                if (m_cboMode.SelectedItem == null)
                    return Define.SnapMode.SNAP_MODE_NONE;

                return m_modeMap[m_cboMode.SelectedItem as string];
            }

            set {
                foreach (var it in m_modeMap) {
                    if (it.Value == value) {
                        m_cboMode.SelectedItem = it.Key;
                        return;
                    }
                }
            }
        }

        public Define.SnapRefPlane snapRefPlane {
            get {
                if (m_cboRefPl.SelectedItem == null)
                    return Define.SnapRefPlane.SNAP_REF_PLANE_NONE;

                return m_refPlMap[m_cboRefPl.SelectedItem as string];
            }

            set {
                foreach (var it in m_refPlMap) {
                    if (it.Value == value) {
                        m_cboRefPl.SelectedItem = it.Key;
                        return;
                    }
                }
            }
        }

        public uint plSnapFlag {
            get {
                if (m_cboPlFlag.SelectedItem == null)
                    return Define.SNAP_NONE;

                return m_plFlagMap[m_cboPlFlag.SelectedItem as string];
            }

            set {
                foreach (var it in m_plFlagMap) {
                    if (it.Value == value) {
                        m_cboPlFlag.SelectedItem = it.Key;
                        return;
                    }
                }
            }
        }

        public override void refresh() {
            //this.ptSnapFlag = IFWrapper.Native.ptSnapFlag(Define.LayoutCellType.CELL_VIEW);
            //this.plSnapFlag = IFWrapper.Native.plSnapFlag(Define.LayoutCellType.CELL_VIEW);
            //this.snapMode = IFWrapper.Native.snapMode(Define.LayoutCellType.CELL_VIEW);
            //this.snapRefPlane = IFWrapper.Native.snapRefPlane(Define.LayoutCellType.CELL_VIEW);
        }

        private void PtFlagBtn_Click(object sender, EventArgs e) {
            if (!(sender is BarToggleButton))
                return;

            BarToggleButton btn = sender as BarToggleButton;
            if (!m_ctrl2Flag.ContainsKey(btn))
                return;

            uint flag = m_ctrl2Flag[btn];
            if (flag == Define.SNAP_PT_LINE_NEAREST) {
                IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_VIEW, Define.SNAP_PT_LINE_NEAREST, btn.Checked, false);

                var btnRev = m_flag2Ctrl[Define.SNAP_PT_LINE_EXT];
                if (btn.Checked && btnRev.Checked) {
                    btnRev.Checked = false;
                    IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_VIEW, Define.SNAP_PT_LINE_EXT, false, false);
                }
            }
            else if (flag == Define.SNAP_PT_LINE_EXT) {
                IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_VIEW, Define.SNAP_PT_LINE_EXT, btn.Checked, false);

                var btnRev = m_flag2Ctrl[Define.SNAP_PT_LINE_NEAREST];
                if (btn.Checked && btnRev.Checked) {
                    btnRev.Checked = false;
                    IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_VIEW, Define.SNAP_PT_LINE_NEAREST, false, false);
                }
            }
            else {
                IFWrapper.Native.updatePtSnapFlag(Define.LayoutCellType.CELL_VIEW, flag, btn.Checked, false);
            }
        }

        private void PtFlagBtn_DoubleClick(object sender, EventArgs e) {
            if (!(sender is BarToggleButton))
                return;

            BarToggleButton btn = sender as BarToggleButton;
            if (!m_ctrl2Flag.ContainsKey(btn))
                return;

            uint flag = m_ctrl2Flag[btn];
            if (flag == Define.SNAP_PT_REF_POINT || flag == Define.SNAP_PT_GEO_POINT)
                return;

            foreach (var it in m_ctrl2Flag) {
                if (it.Value == Define.SNAP_PT_REF_POINT
                    || it.Value == Define.SNAP_PT_GEO_POINT)
                    continue;

                it.Key.Checked = (it.Key == btn);
            }

            IFWrapper.Native.setPtSnapFlag(Define.LayoutCellType.CELL_VIEW, this.flags, false);
        }

        public uint flags {
            get {
                uint ret = 0;
                foreach (var it in m_flag2Ctrl) {
                    if (it.Value.Checked)
                        ret |= it.Key;
                }
                return ret;
            }
        }

        private void Mode_SelectionChangeCommitted(object sender, EventArgs e) {
            if (m_cboMode.SelectedItem == null)
                return;

            IFWrapper.Native.setSnapMode(Define.LayoutCellType.CELL_VIEW, m_modeMap[(string)m_cboMode.SelectedItem], false);
        }

        private void RefPl_SelectionChangeCommitted(object sender, EventArgs e) {
            if (m_cboRefPl.SelectedItem == null)
                return;

            IFWrapper.Native.setSnapRefPlane(Define.LayoutCellType.CELL_VIEW, m_refPlMap[(string)m_cboRefPl.SelectedItem], false);
        }

        private void PlFlag_SelectionChangeCommitted(object sender, EventArgs e) {
            uint snapFlag = 0;
            if (m_cboPlFlag.SelectedItem == null)
                snapFlag = Define.SNAP_NONE;
            else
                snapFlag = m_plFlagMap[(string)m_cboPlFlag.SelectedItem];

            IFWrapper.Native.setPlSnapFlag(Define.LayoutCellType.CELL_VIEW, snapFlag, false);
        }
    }
}
