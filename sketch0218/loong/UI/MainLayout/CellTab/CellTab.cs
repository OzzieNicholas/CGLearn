using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public partial class CellTab : UserControl
    {
        public static readonly int ORG_HEIGHT = 28;
        public static readonly int s_headerLeftMargin = 0;
        public static readonly int s_headerTopMagin = 4;
        public static readonly int s_space = 2;
        public int m_headerTabWidthMin = 118;
        public int m_headerTabWidthMax = 168;

        private List<CellTabHeader> m_tabHeaders = new List<CellTabHeader>();
        private CellTabHeader m_selectedTab = null;
        private CellTabHeader m_droppingTab = null;
        private int m_tabOffset = 0;

        private StringFormat m_textFmt = null;

        private Pen m_penCloseBtn = new Pen(Color.White, 1.5F);

        private SubMenuBase m_rightClickMenu = new SubMenuBase();

        public CellTab() {
            InitializeComponent();

            this.DoubleBuffered = true;
            this.BackColor = SystemColors.Control;

            m_headerTabWidthMin = DPI.Scale(118);
            m_headerTabWidthMax = DPI.Scale(168);

            m_textFmt = new StringFormat();
            m_textFmt.Alignment = StringAlignment.Center;
            m_textFmt.LineAlignment = StringAlignment.Center;

            m_rightClickMenu.addItems(new ToolStripItem[] {
                m_rightClickMenu.item("关闭", (object sender, EventArgs e)=>{ _closeCell(m_selectedTab); }),
                m_rightClickMenu.item("除此之外全部关闭", (object sender, EventArgs e)=>{ _closeOtherCells(m_selectedTab); }),
                m_rightClickMenu.item("全部关闭", (object sender, EventArgs e)=>{ _closeAllCells(); }),
                m_rightClickMenu.sep(),
                m_rightClickMenu.item("重置位置", (object sender, EventArgs e)=>{ FuncUnit.get("view_reset_pos").onClick(sender, e, Keys.None, null); }),
            });
        }

        public List<CellTabHeader> Tabs => m_tabHeaders;
        public int itemHeight => this.Height - s_headerTopMagin;
        public int itemWidthMax => m_headerTabWidthMax;
        public int itemWidthMin => m_headerTabWidthMin;

        public CellTabHeader selectedTab {
            get => m_selectedTab;
            set {
                if (m_selectedTab == value)
                    return;

                m_selectedTab = value;

                if (m_selectedTab != null)
                    IFWrapper.Native.setFocusedCell(m_selectedTab.id);
            }
        }

        public long selectedId {
            get => (m_selectedTab != null) ? m_selectedTab.id : -1;
            set {
                int idx = m_tabHeaders.FindIndex(item => item.id == value);
                m_selectedTab = (idx >= 0) ? m_tabHeaders[idx] : null;
                Invalidate();
            }
        }

        public void addCells(int[] types, long[] ids, string[] names) {
            for (int i = 0; i < ids.Length; ++i) {
                Define.LayoutCellType type = (Define.LayoutCellType)types[i];
                long id = ids[i];
                string name = names[i];
                if (m_tabHeaders.Find(item => item.id == id) != null) //already existed
                    continue;

                m_tabHeaders.Add(new CellTabHeader(this, type, id, name));
            }

            _checkScrollButtons();
            Invalidate();
        }

        public void addCell(Define.LayoutCellType type, long id, string name) {
            if (m_tabHeaders.Find(item => item.id == id) != null) //already existed
                return;

            var header = new CellTabHeader(this, type, id, name);
            m_tabHeaders.Add(header);

            _checkScrollButtons();
            Invalidate();
        }

        public void beginNativeLoad() {
            
        }

        public void addNativeCell(Define.LayoutCellType type, long id, string name) {
            m_tabHeaders.Add(new CellTabHeader(this, type, id, name));
        }

        public void removeNativeCell(long id) {
            CellTabHeader head = m_tabHeaders.Find(item => item.id == id);
            if (head == null)
                return;

            m_tabHeaders.Remove(head);

            if (m_selectedTab == head)
                m_selectedTab = null;
            if (m_droppingTab == head)
                m_droppingTab = null;
        }

        public void endNativeLoad() {
            _checkScrollButtons();
            Invalidate();
        }

        public void onCellNameUpdate(long[] ids, string[] names) {
            if (ids == null || names == null)
                return;

            for (int i = 0; i < ids.Length; ++i) {
                var header = m_tabHeaders.Find(item => item.id == ids[i]);
                if (header == null)
                    continue;

                header.name = names[i];
            }

            this.Invalidate();
        }

        private void _closeCell(CellTabHeader closeTab) {
            if (closeTab == null)
                return;

            CellTabHeader nextTab = null;
            if (m_selectedTab == closeTab) {
                var idx = m_tabHeaders.IndexOf(closeTab);
                if (idx < m_tabHeaders.Count - 1)
                    nextTab = m_tabHeaders[idx + 1];
                else if (idx > 0)
                    nextTab = m_tabHeaders[idx - 1];
            }

            Native.removeCellFromLayout(closeTab.id);

            if (nextTab != null)
                Native.setFocusedCell(nextTab.id);
        }

        private void _closeOtherCells(CellTabHeader remainingTab) {
            if (remainingTab == null)
                return;

            List<long> rmvIds = new List<long>();
            foreach (var header in m_tabHeaders) {
                if (header != remainingTab)
                    rmvIds.Add(header.id);
            }

            if (rmvIds.Count == 0)
                return;

            Native.removeCellsFromLayout(rmvIds.ToArray());

            if (m_selectedTab != remainingTab)
                Native.setFocusedCell(remainingTab.id);
        }

        private void _closeAllCells() {
            List<long> rmvIds = new List<long>();
            for (int i = 0; i < m_tabHeaders.Count; ++i) {
                rmvIds.Add(m_tabHeaders[i].id);
            }

            if (rmvIds.Count == 0)
                return;

            Native.removeCellsFromLayout(rmvIds.ToArray());
        }

        public void removeCell(long id) {
            CellTabHeader head = m_tabHeaders.Find(item => item.id == id);
            if (head == null)
                return;

            m_tabHeaders.Remove(head);

            if (m_selectedTab == head)
                m_selectedTab = null;
            if (m_droppingTab == head)
                m_droppingTab = null;

            _checkScrollButtons();
            Invalidate();
        }

        public void removeCells(long[] ids) {
            foreach (var id in ids) {
                CellTabHeader head = m_tabHeaders.Find(item => item.id == id);
                if (head == null)
                    continue;

                m_tabHeaders.Remove(head);

                if (m_selectedTab == head)
                    m_selectedTab = null;
                if (m_droppingTab == head)
                    m_droppingTab = null;
            }

            _checkScrollButtons();
            Invalidate();
        }

        public void clearCells() {
            m_tabHeaders.Clear();
            m_selectedTab = null;
            m_droppingTab = null;

            _checkScrollButtons();
            Invalidate();
        }

        public void switchNext() {
            if (m_tabHeaders.Count == 0)
                return;

            if (m_selectedTab == null) {
                this.selectedTab = m_tabHeaders[0];
                return;
            }

            int idx = (m_tabHeaders.IndexOf(m_selectedTab) + 1) % m_tabHeaders.Count;
            this.selectedTab = m_tabHeaders[idx];
        }

        private int _getExpectedWidth() {
            int x = s_headerLeftMargin;
            foreach (var head in m_tabHeaders) {
                x += head.width;
                x += s_space;
            }
            return x;
        }

        private void _onScrollLeftButtonClick(object sender, EventArgs e) {
            m_tabOffset = Math.Max(0, m_tabOffset - 1);
            this.Invalidate();
        }

        private void _onScrollRightButtonClick(object sender, EventArgs e) {
            m_tabOffset = Math.Min(m_tabHeaders.Count - 1, m_tabOffset + 1);
            this.Invalidate();
        }

        private void _checkScrollButtons() {
            if (this.Width >= this._getExpectedWidth()) {
                m_tabOffset = 0;
                btn_scrollLeft.Hide();
                btn_scrollRight.Hide();
            }
            else {
                btn_scrollLeft.Show();
                btn_scrollRight.Show();
            }
        }

        protected override void OnSizeChanged(EventArgs e) {
            base.OnSizeChanged(e);
            _checkScrollButtons();
        }

        protected override void OnPaint(PaintEventArgs e) {
            e.Graphics.SmoothingMode = SmoothingMode.HighQuality;
            e.Graphics.Clear(this.BackColor);

            var mouseLoc = this.PointToClient(Control.MousePosition);
            int startTab = (m_tabOffset == 0) ? m_tabOffset : m_tabOffset - 1;
            int eachBaseX = s_headerLeftMargin + ((m_tabOffset == 0) ? 0 : 10 - m_tabHeaders[startTab].width);
            for (int i = startTab; i < m_tabHeaders.Count; i++) {
                var t = m_tabHeaders[i];
                var tabRect = t.rect(eachBaseX);
                var closeRect = t.closeBtnRect(eachBaseX);

                eachBaseX += (s_space + t.width);

                if (m_selectedTab == t) {
                    e.Graphics.FillRectangle(Brushes.SteelBlue, tabRect);
                    e.Graphics.DrawRectangle(Pens.SteelBlue, tabRect);
                }
                else {
                    e.Graphics.FillRectangle(Brushes.DimGray, tabRect);
                    e.Graphics.DrawRectangle(Pens.DimGray, tabRect);
                }
                e.Graphics.DrawString(t.dispName, this.Font, Brushes.White, tabRect, m_textFmt);

                if (tabRect.Contains(mouseLoc)) {
                    if (closeRect.Contains(mouseLoc) && m_droppingTab == null) {
                        var closeBackRect = closeRect;
                        if (closeBackRect.Width + DPI.Scale(2) <= tabRect.Height)
                            closeBackRect.Inflate(DPI.Scale(2), DPI.Scale(2));

                        e.Graphics.FillRectangle(Brushes.DarkSlateGray, closeBackRect);
                    }

                    int glyphMargin = 2;
                    e.Graphics.DrawLine(m_penCloseBtn, new Point(closeRect.X + glyphMargin, closeRect.Y + glyphMargin), new Point(closeRect.Right - glyphMargin, closeRect.Bottom - glyphMargin));
                    e.Graphics.DrawLine(m_penCloseBtn, new Point(closeRect.Left + glyphMargin, closeRect.Bottom - glyphMargin), new Point(closeRect.Right - glyphMargin, closeRect.Top + glyphMargin));
                }
            }
        }

        protected override void OnMouseDown(MouseEventArgs e) {
            base.OnMouseDown(e);

            if (e.Button == MouseButtons.Left || e.Button == MouseButtons.Right) {
                int startTab = (m_tabOffset == 0) ? m_tabOffset : m_tabOffset - 1;
                int eachBaseX = s_headerLeftMargin + ((m_tabOffset == 0) ? 0 : 10 - m_tabHeaders[startTab].width);
                for (int i = startTab; i < m_tabHeaders.Count; i++) {
                    var t = m_tabHeaders[i];
                    Rectangle tabRect = t.rect(eachBaseX);
                    if (tabRect.Contains(e.Location)) {
                        m_droppingTab = t;
                        break;
                    }
                    eachBaseX += (s_space + t.width);
                }

                this.Invalidate();
            }
        }

        protected override void OnMouseMove(MouseEventArgs e) {
            base.OnMouseMove(e);

            if (m_droppingTab == null) {
                this.Invalidate(); //invoke redraw only
                return;
            }

            //calculate insert position for dropping header
            Point mousePos = new Point(Math.Max(e.X, s_headerLeftMargin + 1), this.Height / 2);
            int startTab = (m_tabOffset == 0) ? m_tabOffset : m_tabOffset - 1;
            int eachBaseX = s_headerLeftMargin + ((m_tabOffset == 0) ? 0 : 10 - m_tabHeaders[startTab].width);
            for (int i = startTab; i < m_tabHeaders.Count; i++) {
                var t = m_tabHeaders[i];

                Rectangle tabRect = t.rect(eachBaseX);
                Rectangle closeRect = t.closeBtnRect(eachBaseX);
                if (tabRect.Contains(mousePos)) {
                    if (t == m_droppingTab)
                        return;

                    m_tabHeaders.Remove(m_droppingTab);
                    m_tabHeaders.Insert(m_tabHeaders.IndexOf(t), m_droppingTab);
                    this.Invalidate();
                    return;
                }

                if (t != m_droppingTab) //dropping header itself should not be counted
                    eachBaseX += (s_space + t.width);
            }

            m_tabHeaders.Remove(m_droppingTab);
            m_tabHeaders.Add(m_droppingTab);
            this.Invalidate();
        }

        protected override void OnMouseClick(MouseEventArgs e) {
            base.OnMouseClick(e);

            int startTab = (m_tabOffset == 0) ? m_tabOffset : m_tabOffset - 1;
            int eachBaseX = s_headerLeftMargin + ((m_tabOffset == 0) ? 0 : 10 - m_tabHeaders[startTab].width);
            for (int i = startTab; i < m_tabHeaders.Count; i++) {
                var t = m_tabHeaders[i];
                if (e.Button == MouseButtons.Left) { //click close button
                    Rectangle tabRect = t.rect(eachBaseX);
                    var closeRect = t.closeBtnRect(eachBaseX);
                    if (closeRect.Contains(e.Location)) {
                        _closeCell(t);
                        break;
                    }
                    else if (tabRect.Contains(e.Location)) {
                        this.selectedTab = t;
                    }
                }
                else if (e.Button == MouseButtons.Right) { //submenu
                    var rect = t.rect(eachBaseX);
                    if (rect.Contains(e.Location)) {
                        m_rightClickMenu.Show(this, e.X, e.Y);
                        break;
                    }
                }

                eachBaseX += (s_space + t.width);
            }
        }

        protected override void OnMouseDoubleClick(MouseEventArgs e) {
            base.OnMouseDoubleClick(e);

            if (e.Button == MouseButtons.Left) { //switch focused cell full scr
                if (this.selectedTab == null)
                    return;

                int startTab = (m_tabOffset == 0) ? m_tabOffset : m_tabOffset - 1;
                int eachBaseX = s_headerLeftMargin + ((m_tabOffset == 0) ? 0 : 10 - m_tabHeaders[startTab].width);
                for (int i = startTab; i < m_tabHeaders.Count; i++) {
                    var t = m_tabHeaders[i];
                    if (t != this.selectedTab) {
                        eachBaseX += (s_space + t.width);
                        continue;
                    }
                        
                    Rectangle tabRect = t.rect(eachBaseX);
                    if (!tabRect.Contains(e.Location)) {
                        eachBaseX += (s_space + t.width);
                        continue;
                    }

                    IFWrapper.Native.switchCellFullScr(t.id);
                    break;
                }

                this.Invalidate();
            }
        }

        protected override void OnMouseUp(MouseEventArgs e) {
            base.OnMouseUp(e);

            m_droppingTab = null;
        }

        protected override void OnMouseLeave(EventArgs e) {
            base.OnMouseLeave(e);
            this.Invalidate();
        }
    }
}
