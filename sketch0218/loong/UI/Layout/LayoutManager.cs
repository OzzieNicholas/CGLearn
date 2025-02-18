using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public class LayoutManager
    {
        private static LayoutManager m_instance = new LayoutManager();

        protected Dictionary<string, CustomMenu> m_menus = new Dictionary<string, CustomMenu>();
        protected Dictionary<string, CustomToolBar> m_bars = new Dictionary<string, CustomToolBar>();
        protected Dictionary<string, CustomToolBar> m_builtInBars = new Dictionary<string, CustomToolBar>();
        protected Dictionary<string, DObject> m_miniBars = new Dictionary<string, DObject>();

        private HashSet<CustomToolBar> m_floatyBars = new HashSet<CustomToolBar>();
        private string m_curLayout = null;

        public static LayoutManager instance => m_instance;
        public HashSet<CustomToolBar> floatyBars => m_floatyBars;

        public void init() {
            this._initBuiltInMenus();
            this._initBuiltInBars();
            this.curLayout = "main";
        }

        private void _initBuiltInMenus() {
            m_menus["main"] = new MainMenu();

            //TODO:showery
            //有待完成符号编辑器的功能
            //m_menus["symbol_editor"] = new SymbolMenu();
        }

        private void _initBuiltInBars() {
            m_builtInBars["view_active"] = new Bar_View_Active();
            m_builtInBars["view_active"].BarName = "view_active";
            m_builtInBars["pick"] = Bar_Pick.instance;
            m_builtInBars["pick"].BarName = "pick";
            m_builtInBars["snap"] = Bar_Snap.instance;
            m_builtInBars["snap"].BarName = "snap";
        }

        public Dictionary<string, CustomToolBar> builtInBars => m_builtInBars;

        public string curLayout {
            get => m_curLayout;
            set {
                if (value == null || m_curLayout == value)
                    return;

                m_curLayout = value;

                m_builtInBars["view_active"].provsion = CustomBarsProvision.instance.getBuiltinBar(value, "view_active");
                m_builtInBars["pick"].provsion = CustomBarsProvision.instance.getBuiltinBar(value, "pick");
                m_builtInBars["snap"].provsion = CustomBarsProvision.instance.getBuiltinBar(value, "snap");

                foreach (var bar in m_floatyBars.ToArray()) {
                    bar.hideFloaty();
                }
                this.refreshLayout();
            }
        }

        public CustomToolBar getExistedBar(string name) {
            if (m_builtInBars.ContainsKey(name))
                return m_builtInBars[name];

            if (m_bars.ContainsKey(name))
                return m_bars[name];

            return null;
        }

        public CustomToolBar getBuiltInBar(string name)
        {
            if (m_builtInBars.ContainsKey(name))
                return m_builtInBars[name];
            return new CustomToolBar();
        }

        public CustomToolBar getBar(BarNode barNode) {
            string name = barNode.Name;
            if (m_builtInBars.ContainsKey(name))
                return m_builtInBars[name];

            if (m_bars.ContainsKey(name))
                return m_bars[name];

            var bar = new CustomToolBar();
            bar.provsion = barNode;
            m_bars[name] = bar;
            return bar;
        }

        public DObject getMiniBar(string name) {
            if (m_miniBars.ContainsKey(name)) {
                return m_miniBars[name];
            }
            var barConfig = Native.queryUIMiniBar(name);
            if (barConfig == null) {
                return null;
            }
            m_miniBars[name] = barConfig;
            return barConfig;
        }

        public DObject getMiniBars() {
            return Native.queryUIMiniBars();
        }

        public void updateMiniBars(DObject rootConfig) {
            Callback.execute("update_ui_mini_bars", new object[] {
                "rootConfig", rootConfig
            });
            foreach (string barKey in m_miniBars.Keys) {
                m_miniBars[barKey] = Native.queryUIMiniBar(barKey);
            }
        }

        //刷新菜单/工具栏
        //刷新内容包括快捷键信息，Checked值等
        public void refreshItems() {
            foreach (var it in m_menus) {
                it.Value.refresh();
            }

            foreach (var it in m_bars) {
                it.Value.updateShortCuts();
            }
        }

        public List<List<BarNode>> getBarListByLocation(string loc) {
            List<List<BarNode>> ret = new List<List<BarNode>>();
            var barRoot = MainForm.instance.barRoot;

            List<List<CustomToolBar>> rows = null;

            if (loc == CustomBarsProvision.BAR_LOCATION_FLOATY) {
                ret.Add(getFloatyBarsList());
            }
            else {
                if (loc == CustomBarsProvision.BAR_LOCATION_TOP) {
                    rows = barRoot.topPanel.lines;
                }
                if (loc == CustomBarsProvision.BAR_LOCATION_BOTTOM) {
                    rows = barRoot.bottomPanel.lines;
                }
                if (loc == CustomBarsProvision.BAR_LOCATION_LEFT) {
                    rows = barRoot.leftPanel.lines;
                }
                if (loc == CustomBarsProvision.BAR_LOCATION_RIGHT) {
                    rows = barRoot.rightPanel.lines;
                }

                foreach (List<CustomToolBar> row in rows) {
                    List<CustomToolBar> orderedCtrlsList;
                    if (loc == CustomBarsProvision.BAR_LOCATION_TOP || loc == CustomBarsProvision.BAR_LOCATION_BOTTOM)
                        orderedCtrlsList = row.OrderBy(item => item.Location.X).ToList();
                    else
                        orderedCtrlsList = row.OrderBy(item => item.Location.Y).ToList();
                    List<BarNode> rowList = new List<BarNode>();
                    foreach (CustomToolBar ctrl in orderedCtrlsList) {
                        rowList.Add(ctrl.provsion);
                    }
                    ret.Add(rowList);
                }
            }

            return ret;
        }

        public List<BarNode> getFloatyBarsList() {
            return m_floatyBars.Select(x=>x.provsion).ToList();
        }

        public void refreshLayout() {
            if (m_curLayout == null)
                return;

            var layout = Native.queryUILayout(m_curLayout);
            if (layout == null)
                return;

            MainForm.instance.mainMenu = m_menus[(string)layout["menu"]];

            var barRoot = MainForm.instance.barRoot;

            barRoot.SuspendLayout();

            if (!layout.ContainsKey("name"))
                return;

            LayoutNode layoutNode = CustomBarsProvision.instance.getLayout(layout["name"].ToString());
            refreshLayoutTopBar(layoutNode);
            refreshLayoutBottomBar(layoutNode);
            refreshLayoutLeftBar(layoutNode);
            refreshLayoutRightBar(layoutNode);
            refreshLayoutFloatyBar(layoutNode);

            //layoutNode.printLayout();
            barRoot.ResumeLayout();
        }

        public void refreshLayoutTopBar(LayoutNode layoutNode) {
            var barRoot = MainForm.instance.barRoot;
            barRoot.topPanel.clearBars();

            Dictionary<int, List<BarNode>> topBars = layoutNode.getBarsByLocation(CustomBarsProvision.BAR_LOCATION_TOP);
            if (topBars != null && topBars.Count > 0) {
                Point pos = new Point(0, 0);

                foreach (int row in topBars.Keys) {
                    List<BarNode> bars = topBars[row];

                    int h = 0;
                    foreach (BarNode bar in bars) {
                        //System.Console.WriteLine("Layout refresh: top bar (" + bar.toString() + ")");
                        //foreach (ButtonNode btn in bar.Children)
                        //    System.Console.WriteLine("          - add btn for bar " + bar.Name + ": (" + btn.toString() + ")");

                        CustomToolBar ctrl = this.getBar(bar);
                        if (ctrl == null)
                            continue;

                        ctrl.Location = pos;
                        ctrl.Location = new Point(0, 0);
                        ctrl.refresh();
                        barRoot.topPanel.addToolBar(ctrl, row, bar.Provisioned);
                        //Debug.WriteLine("++++ bar: (" + pos.ToString() + ")");
                        if (!bar.Provisioned)
                        {
                            //Debug.WriteLine("unprovisioned bar: (" + bar.toString() + ")");
                            continue;
                        }
                        pos.X += ctrl.Width + 3;

                        h = Math.Max(h, ctrl.Height);
                        //Debug.WriteLine("---- bar: (" + pos.ToString() + ")");
                    }

                    pos.X = 0;
                    pos.Y += h;
                }
            }

            barRoot.topPanel.relayout();
        }

        public void refreshLayoutBottomBar(LayoutNode layoutNode) {
            var barRoot = MainForm.instance.barRoot;
            barRoot.bottomPanel.clearBars();

            Dictionary<int, List<BarNode>> bottomBars = layoutNode.getBarsByLocation(CustomBarsProvision.BAR_LOCATION_BOTTOM);
            if (bottomBars != null && bottomBars.Count > 0) {
                Point pos = new Point(0, 0);

                foreach (int row in bottomBars.Keys) {
                    List<BarNode> bars = bottomBars[row];

                    int h = 0;
                    foreach (BarNode bar in bars) {

                        CustomToolBar ctrl = this.getBar(bar);
                        if (ctrl == null)
                            continue;

                        ctrl.Location = pos;
                        ctrl.refresh();
                        barRoot.bottomPanel.addToolBar(ctrl, row, bar.Provisioned);
                        if (!bar.Provisioned)
                            continue;
                        pos.X += ctrl.Width + 3;

                        h = Math.Max(h, ctrl.Height);
                    }

                    pos.X = 0;
                    pos.Y += h;
                }
            }
            barRoot.bottomPanel.relayout();
        }

        public void refreshLayoutLeftBar(LayoutNode layoutNode) {
            var barRoot = MainForm.instance.barRoot;
            barRoot.leftPanel.clearBars();

            Dictionary<int, List<BarNode>> leftBars = layoutNode.getBarsByLocation(CustomBarsProvision.BAR_LOCATION_LEFT);
            if (leftBars != null && leftBars.Count > 0) {
                Point pos = new Point(0, 0);

                foreach (int row in leftBars.Keys) {
                    List<BarNode> bars = leftBars[row];

                    int w = 0;
                    foreach (BarNode bar in bars) {
                        /*if (!bar.Provisioned)
                            continue;*/
                        CustomToolBar ctrl = this.getBar(bar);
                        if (ctrl == null)
                            continue;

                        ctrl.Location = pos;
                        ctrl.refresh();
                        barRoot.leftPanel.addToolBar(ctrl, row, bar.Provisioned);
                        if (!bar.Provisioned)
                            continue;
                        pos.Y += ctrl.Height + 3;

                        w = Math.Max(w, ctrl.Width);
                    }

                    pos.X = w;
                    pos.Y = 0;
                }
            }

            barRoot.leftPanel.relayout();
        }
        public void refreshLayoutRightBar(LayoutNode layoutNode) {
            var barRoot = MainForm.instance.barRoot;
            barRoot.rightPanel.clearBars();

            Dictionary<int, List<BarNode>> rightBars = layoutNode.getBarsByLocation(CustomBarsProvision.BAR_LOCATION_RIGHT);
            if (rightBars != null && rightBars.Count > 0) {
                Point pos = new Point(0, 0);

                foreach (int row in rightBars.Keys) {
                    List<BarNode> bars = rightBars[row];

                    int w = 0;
                    foreach (BarNode bar in bars) {
                        /*if (!bar.Provisioned)
                            continue;*/
                        CustomToolBar ctrl = this.getBar(bar);
                        if (ctrl == null)
                            continue;

                        ctrl.Location = pos;
                        ctrl.refresh();
                        barRoot.rightPanel.addToolBar(ctrl, row, bar.Provisioned);
                        if (!bar.Provisioned)
                            continue;
                        pos.Y += ctrl.Height + 3;

                        w = Math.Max(w, ctrl.Width);
                    }

                    pos.X = w;
                    pos.Y = 0;
                }
            }

            barRoot.rightPanel.relayout();
        }

        public void refreshLayoutFloatyBar(LayoutNode layoutNode) {
            Dictionary<int, List<BarNode>> floatyBars = layoutNode.getBarsByLocation(CustomBarsProvision.BAR_LOCATION_FLOATY);
            if (floatyBars != null && floatyBars.Count > 0) {
                foreach (var bar in floatyBars[0]) {
                    var ctrl = this.getBar(bar);
                    if (ctrl == null)
                        continue;
                    if (!bar.Provisioned && ctrl.isFloatyVisiable())
                        ctrl.floaty.closeEye();
                    else
                        ctrl.showFloaty(bar.x, bar.y);
                }
            }
        }

        public void refreshLayoutBarByLocation(string location, LayoutNode layoutNode) {
            switch (location) {
                case CustomBarsProvision.BAR_LOCATION_TOP:
                    this.refreshLayoutTopBar(layoutNode);
                break;
                case CustomBarsProvision.BAR_LOCATION_BOTTOM:
                    this.refreshLayoutBottomBar(layoutNode);
                break;
                case CustomBarsProvision.BAR_LOCATION_LEFT:
                    this.refreshLayoutLeftBar(layoutNode);
                break;
                case CustomBarsProvision.BAR_LOCATION_RIGHT:
                    this.refreshLayoutRightBar(layoutNode);
                break;
            }
        }
    }
}
