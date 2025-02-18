using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(false)]
    public partial class MainFormLayoutContainer : UserControl
    {
        private ToolBarPanelTop pan_top = new ToolBarPanelTop();
        private ToolBarPanelBottom pan_bottom = new ToolBarPanelBottom();
        private ToolBarPanelLeft pan_left = new ToolBarPanelLeft();
        private ToolBarPanelRight pan_right = new ToolBarPanelRight();
        private ToolBarPanelCenter pan_center = new ToolBarPanelCenter();
        private ToolBarPanel[] m_barPanels = null;
        private IOHandler m_io = null;

        public MainFormLayoutContainer() {
            InitializeComponent();

            this.BackColor = Color.Gray;

            this.SuspendLayout();

            pan_left.SetBounds(0, pan_top.Height, pan_left.Width, this.Height - pan_top.Height - pan_bottom.Height);
            pan_left.Dock = DockStyle.Left;

            pan_right.SetBounds(this.Width - pan_right.Width, pan_top.Height, pan_right.Width, this.Height - pan_top.Height - pan_bottom.Height);
            pan_right.Dock = DockStyle.Right;

            pan_top.SetBounds(0, 0, this.Width, pan_top.Height);
            pan_top.Dock = DockStyle.Top;

            pan_bottom.SetBounds(0, this.Height - pan_bottom.Height, this.Width, pan_bottom.Height);
            pan_bottom.Dock = DockStyle.Bottom;

            pan_center.SetBounds(pan_left.Width, pan_top.Height, this.Width - pan_left.Width - pan_right.Width, this.Height - pan_top.Height - pan_bottom.Height);

            this.Controls.AddRange(new Control[] { pan_center, pan_left, pan_right, pan_top, pan_bottom });

            pan_left.SizeChanged += center_Resize;
            pan_right.SizeChanged += center_Resize;
            pan_top.SizeChanged += center_Resize;
            pan_bottom.SizeChanged += center_Resize;

            this.ResumeLayout();

            m_barPanels = new ToolBarPanel[] { pan_top, pan_bottom, pan_left, pan_right };

            m_io = new IOHandler(null);
            m_io.mouseDown += onMouseDown;
            m_io.mouseMove += onMouseMove;
            m_io.mouseUp += onMouseUp;
            m_io.start();
        }

        public ToolBarPanel[] barPanels => m_barPanels;
        public ToolBarPanel leftPanel => pan_left;
        public ToolBarPanel rightPanel => pan_right;
        public ToolBarPanel topPanel => pan_top;
        public ToolBarPanel bottomPanel => pan_bottom;
        public ToolBarPanelCenter centerPanel => pan_center;

        protected override void OnSizeChanged(EventArgs e) {
            base.OnSizeChanged(e);
            center_Resize(this, e);
        }

        private void center_Resize(object sender, EventArgs e) {
            if (pan_center == null)
                return;

            var bounds = new Rectangle(pan_left.Width, pan_top.Height, this.Width - pan_left.Width - pan_right.Width, this.Height - pan_top.Height - pan_bottom.Height);
            if (pan_center.Bounds != bounds)
                pan_center.Bounds = bounds;
        }

        public bool onMouseDown(Control sender, MouseEventArgs e) {
            if (sender == MainForm.instance.viewPanel) //不影响主视图刷新
                return false;

            if (e.Button != MouseButtons.Left)
                return false;

            if (Utils.IsEqualOrContained(this, sender)) {
                var scrPos = e.Location;
                foreach (var pan in m_barPanels) {
                    foreach (CustomToolBar bar in pan.Controls) {
                        if (!Utils.IsEqualOrContained(bar, sender))
                            continue;

                        if (bar.isInGripArea(bar.PointToClient(e.Location))) {
                            HoverForm.instance.beginDrag(bar, pan, e.Location);
                            return true;
                        }
                    }
                }
            }
            else {
                foreach (var bar in LayoutManager.instance.floatyBars) {
                    if (!Utils.IsEqualOrContained(bar, sender))
                        continue;

                    if (bar.isInGripArea(bar.PointToClient(e.Location))) {
                        HoverForm.instance.beginDrag(bar, null, e.Location);
                        return true;
                    }
                }
            }

            return false;
        }

        public bool onMouseMove(Control sender, MouseEventArgs e) {
            if (!HoverForm.instance.Visible)
                return false;

            var bar = HoverForm.instance.toolBar;
            if (bar == null) //something wrong
                return false;

            if (bar.panel != null) { //remove from origin panel
                var pan = bar.panel;
                pan.removeToolBar(bar);
                pan.relayout();
            }

            var probeRect = HoverForm.instance.probeRect;
            int line = -1, idx = -1;
            foreach (var pan in m_barPanels) {
                var panRect = this.RectangleToScreen(pan.Bounds);
                if (panRect.IntersectsWith(probeRect)
                    && pan.indexOf(probeRect, out line, out idx)) {
                    HoverForm.instance.drag(pan, line, idx, e.Location);
                    return true;
                }
            }

            HoverForm.instance.drag(null, -1, -1, e.Location);
            return true;
        }

        public bool onMouseUp(Control sender, MouseEventArgs e) {
            if (!HoverForm.instance.Visible)
                return false;

            var bar = HoverForm.instance.toolBar;
            if (bar == null) //something wrong
                return false;

            HoverForm.instance. endDrag();
            return true;
        }
    }
}
