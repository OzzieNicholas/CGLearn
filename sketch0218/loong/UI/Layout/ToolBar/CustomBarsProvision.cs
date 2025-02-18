using System.Collections.Generic;
using System.Linq;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using System;
using System.Diagnostics;
using IFWrapper;

namespace UI
{
    public class CustomBarsNodeBase : ICloneable
    {
        public CustomBarsNodeBase(string name, string tip)
        {
            m_name = name;
            m_tip = tip;
            m_sn = CustomBarsProvision.generateOrderId();
        }

        private string m_name = null;
        private string m_tip = null;
        protected int m_sn = -1;
        private bool m_provisioned = true;
        private CustomBarsNodeBase m_parent = null;
        protected List<CustomBarsNodeBase> m_children = new List<CustomBarsNodeBase>();
        protected Dictionary<string, CustomBarsNodeBase> m_childrenDict = new Dictionary<string, CustomBarsNodeBase>();

        public string Name { get => m_name; set => m_name = value; }
        public string Tip { get => m_tip; set => m_tip = value; }
        public int SN { get => m_sn; }
        public int ChildrenCount { get => Children.Count; }
        public List<CustomBarsNodeBase> Children { get => m_children; set => m_children = value; }
        public Dictionary<string, CustomBarsNodeBase> ChildrenDict { get => m_childrenDict; set => m_childrenDict = value; }
        public CustomBarsNodeBase Parent { get => m_parent; set => m_parent = value; }
        public int resetSn()
        {
            m_sn = CustomBarsProvision.generateOrderId();
            return m_sn;
        }
        public bool Provisioned 
        { 
            get => m_provisioned;
            set => m_provisioned = value;
        }

        public CustomBarsNodeBase getChild(string name)
        {
            if (ChildrenDict.ContainsKey(name))
                return ChildrenDict[name];
            else
                return null;
        }
        public virtual void addChild(CustomBarsNodeBase node)
        {
            if (ChildrenDict.ContainsKey(node.Name)) return;
            Children.Add(node);
            ChildrenDict[node.Name] = node;
            node.Parent = this;
        }
        public virtual void insertChild(int index, CustomBarsNodeBase node)
        {
            if (ChildrenDict.ContainsKey(node.Name)) return;
            Children.Insert(index, node);
            ChildrenDict[node.Name] = node;
            node.Parent = this;
        }
        public virtual void moveChild(int index, CustomBarsNodeBase node)
        {
            if (!ChildrenDict.ContainsKey(node.Name) || !Children.Contains(node) || node.Parent != this) return;
            Children.Remove(node);
            Children.Insert(index, node);
        }
        public virtual void removeChild(CustomBarsNodeBase node)
        {
            if (!ChildrenDict.ContainsKey(node.Name)) return;
            Children.Remove(node);
            ChildrenDict.Remove(node.Name);
        }
        public void moveUpChild(CustomBarsNodeBase node)
        {
            int idx = Children.FindIndex(item => item == node);
            if (idx == 0) return;
            Children.Remove(node);
            Children.Insert(idx - 1, node);
        }
        public void moveDownChild(CustomBarsNodeBase node)
        {
            int idx = Children.FindIndex(item => item == node);
            if (idx == Children.Count - 1) return;
            Children.Remove(node);
            Children.Insert(idx + 1, node);
        }
        public void moveUp()
        {
            if (this.Parent == null) return;
            this.Parent.moveUpChild(this);
        }
        public void moveDown()
        {
            if (this.Parent == null) return;
            this.Parent.moveDownChild(this);
        }
        public void remove()
        {
            if (Parent != null) { 
                Parent.removeChild(this);
            }
            if (GetType() == typeof(ButtonNode)) {
                if (Tip != "|")
                {
                    List<BarNode> bars = CustomBarsProvision.instance.FuncUnitToBarsDict[Name];
                    bars.Remove(Parent as BarNode);
                }

                if (!CustomBarsProvision.instance.isValidSlideBar(Parent as BarNode))
                {
                    CustomBarsProvision.instance.SlideBarNodes.Remove(Parent.Name);
                }
            }
            if (GetType() == typeof(BarNode) && CustomBarsProvision.instance.SlideBarNodes.ContainsKey(this.Name)) {
                CustomBarsProvision.instance.SlideBarNodes.Remove(this.Name);
            }
        }

        public virtual string toString()
        {
            return "Node type: " + this.GetType().ToString() + " - " + "(name: " + Name + ", tip: " + Tip + ", provisioned: " + Provisioned + ", hashcode: " + m_sn + ")";
        }

        public virtual object Clone()
        {
            CustomBarsNodeBase cloned = this.MemberwiseClone() as CustomBarsNodeBase;
            cloned.resetSn();
            cloned.Children = new List<CustomBarsNodeBase>();
            cloned.ChildrenDict = new Dictionary<string, CustomBarsNodeBase>();
            foreach (var ch in Children)
            {
                CustomBarsNodeBase child = ch.Clone() as CustomBarsNodeBase;
                cloned.addChild(child);
            }

            return cloned as object;
        }

        public override int GetHashCode()
        {
            return m_sn;
        }
    }


    public class LayoutNode : CustomBarsNodeBase
    {
        private Dictionary<string, Dictionary<int, List<BarNode>>> m_locationMap = new Dictionary<string, Dictionary<int, List<BarNode>>>();

        public Dictionary<string, Dictionary<int, List<BarNode>>> LocationMap
        {
            get => m_locationMap;
        }

        private void unsettleBar(BarNode bar)
        {
            //Debug.WriteLine("unprovisionBar: " + bar.toString());
            if (m_locationMap.ContainsKey(bar.Location))
            {
                if (m_locationMap[bar.Location].ContainsKey(bar.Row))
                {
                    //Debug.WriteLine("unprovisionBar: m_locationMap[bar.Location].[bar.Row].ContainsKey " + m_locationMap[bar.Location][bar.Row].Contains(bar));
                    m_locationMap[bar.Location][bar.Row].Remove(bar);
                    //Debug.WriteLine("unprovisionBar: m_locationMap[bar.Location].ContainsKey after remove " + m_locationMap[bar.Location][bar.Row].Contains(bar));
                    if (m_locationMap[bar.Location][bar.Row].Count == 0)
                    {
                        m_locationMap[bar.Location].Remove(bar.Row);
                    }
                }
                if (m_locationMap[bar.Location].Count == 0)
                {
                    m_locationMap.Remove(bar.Location);
                }
            }
        }
        private void settleBar(BarNode bar)
        {
            //Debug.WriteLine("provisionBar - bar - " + bar.toString());
            if (!m_locationMap.ContainsKey(bar.Location))
                m_locationMap[bar.Location] = new Dictionary<int, List<BarNode>>();
            if (!m_locationMap[bar.Location].ContainsKey(bar.Row))
                m_locationMap[bar.Location][bar.Row] = new List<BarNode>();
            m_locationMap[bar.Location][bar.Row].Add(bar);
            //CustomBarsProvision.instance.printLayoutLocations(this.Name);
        }
        public void changeBarLocation(BarNode bar, string newLocation, int newRow)
        {
            unsettleBar(bar);
            bar.Location = newLocation;
            bar.Row = newRow;
            settleBar(bar);
        }

        public Dictionary<int, List<BarNode>> getBarsByLocation(string location)
        {
            if (m_locationMap.ContainsKey(location))
                return m_locationMap[location];
            else
                return null;
        }

        public void syncActualBarsLocations()
        {
            string slideBarLoc = CustomBarsProvision.instance.whereIsSlideBar();

            foreach (string loc in CustomBarsProvision.BAR_LOCATIONS)
            {
                List<List<BarNode>> bars = LayoutManager.instance.getBarListByLocation(loc);
                syncActualBarsLocation(loc, slideBarLoc, bars);
            }
        }

        private void syncActualBarsLocation(string loc, string slideBarsLoc, List<List<BarNode>> bars)
        {
            if (m_locationMap.ContainsKey(loc))
            {
                m_locationMap[loc].Clear();
                for (int row = 0; row < bars.Count; row++)
                {
                    m_locationMap[loc][row] = new List<BarNode>();
                    foreach (BarNode bar in bars[row])
                    {
                        syncActualBarLocationRow(bar, loc, row);
                    }
                    if (loc.Equals(slideBarsLoc))
                    {
                        foreach (BarNode bar in CustomBarsProvision.instance.SlideBarNodes.Values)
                        {
                            syncActualBarLocationRow(bar, loc, row);
                        }
                    }
                }
            }
        }

        private void syncActualBarLocationRow(BarNode bar, string loc, int row)
        {
            if (loc == CustomBarsProvision.BAR_LOCATION_FLOATY)
            {
                if (bar.Provisioned)
                {
                    bar.x = LayoutManager.instance.getBar(bar).floaty.Location.X;
                    bar.y = LayoutManager.instance.getBar(bar).floaty.Location.Y;
                }
            }
            m_locationMap[loc][row].Add(bar);
        }

        private void syncSlideBars()
        {

        }

        public DObject getLayoutSubnodesProvision()
        {
            DObject prov = new DObject();
            foreach (string loc in m_locationMap.Keys)
            {
                //Console.WriteLine("getLayoutSubnodesProvision - loc name " + loc);
                DObject locObj = new DObject();
                foreach (int row in m_locationMap[loc].Keys)
                {
                    //Console.WriteLine("getLayoutSubnodesProvision - row " + row);
                    List<string> rowList = new List<string>();

                    foreach (BarNode bar in m_locationMap[loc][row])
                    {
                        //Console.WriteLine("getLayoutSubnodesProvision - bar " + bar.Name);
                        rowList.Add(bar.Name);
                    }
                    locObj[row.ToString()] = rowList.ToArray();
                }
                prov[loc] = locObj;
            }
            return prov;
        }

        public LayoutNode(string name, string tip = null) : base(name, tip)
        {
        }
        public BarNode this[string name] { get => getChild(name) as BarNode; set=> addChild(value); }

        public override object Clone()
        {
            LayoutNode cloned = this.MemberwiseClone() as LayoutNode;
            cloned.m_locationMap = new Dictionary<string, Dictionary<int, List<BarNode>>>();
            cloned.Children = new List<CustomBarsNodeBase>();
            cloned.ChildrenDict = new Dictionary<string, CustomBarsNodeBase>();
            foreach (var ch in Children)
            {
                CustomBarsNodeBase child = ch.Clone() as CustomBarsNodeBase;
                cloned.addChild(child);
            }

            return cloned as object;
        }

        public override void addChild(CustomBarsNodeBase node)
        {
            if (ChildrenDict.ContainsKey(node.Name)) return;
            if (node.GetType() != typeof(BarNode)) return;
            base.addChild(node);
            BarNode bar = node as BarNode;
            settleBar(bar);
            CustomBarsProvision.instance.BarNodeDict[bar.GetHashCode()] = bar;
        }

        public void moveBarBefore(BarNode beforeBar, BarNode insertBar)
        {
            insertBar.changeLocation(beforeBar.Location, beforeBar.Row);

            syncActualBarsLocations();
            List<BarNode> toRow = m_locationMap[beforeBar.Location][beforeBar.Row];
            int idx = toRow.FindIndex(item => item.GetHashCode() == beforeBar.GetHashCode());
            //Console.WriteLine("*************** toRow count: " + toRow.Count + ", row number: " + beforeBar.Row + ", idx: " + idx);

            if (!toRow.Contains(beforeBar) || idx == -1) return;
            //Console.WriteLine("*************** found beforeBar ");
            toRow.Remove(beforeBar);
            toRow.Insert(idx, insertBar);
        }

        /*public override void insertChild(int index, CustomBarsNodeBase node)
        {
            this.addChild(node);
        }*/

        public override void removeChild(CustomBarsNodeBase node)
        {
            if (!ChildrenDict.ContainsKey(node.Name)) return;
            if (node.GetType() != typeof(BarNode)) return;
            base.removeChild(node);
            BarNode bar = node as BarNode;
            unsettleBar(bar);
            CustomBarsProvision.instance.BarNodeDict.Remove(bar.GetHashCode());
        }
        public void printLayout()
        {
            foreach (var key in m_locationMap.Keys)
            {
                Debug.WriteLine("printLayout - location: " + key + ", children count: " + this.Children.Count);
                Debug.WriteLine("            - rows count: " + m_locationMap[key].Count);
                foreach (var rkey in m_locationMap[key].Keys)
                {
                    Debug.WriteLine("              - row " + rkey + " count: " + m_locationMap[key][rkey].Count);
                    foreach (BarNode b in m_locationMap[key][rkey])
                        Debug.WriteLine("                - bar: " + b.Tip);
                }

            }
        }
    }


    public class BarNode : CustomBarsNodeBase
    {
        private string m_location = null;
        private int m_x = -1;
        private int m_y = -1;
        private int m_row = 0;
        private bool m_isBuildinBar = false;

        public string Location 
        { 
            get => m_location; 
            set => m_location = value;
        }
        public void changeLocation(string location, int row)
        {
            if (Parent != null) (Parent as LayoutNode).changeBarLocation(this, location, row);
            else
            {
                Location = location;
                Row = row;
            }
        }
        public int Row 
        { 
            get => m_row;
            set => m_row = value;
        }
        public void moveBarBefore(BarNode beforeBar)
        {
            (Parent as LayoutNode).moveBarBefore(beforeBar, this);
        }
        public bool IsBuiltinBar { get => m_isBuildinBar; }
        public ButtonNode this[string name] { get => getChild(name) as ButtonNode; set => addChild(value); }
        public BarNode(string name, string tip, string locatoin = null, int row = 0, bool builtin = false, int x = -1, int y = -1) : base(name, tip)
        {
            m_location = locatoin;
            m_row = row;
            m_isBuildinBar = builtin;
            m_x = x;
            m_y = y;
            this.Tip = tip;
        }
        public int x
        {
            get => m_x;
            set => m_x = value;
        }
        public int y
        {
            get => m_y;
            set => m_y = value;
        }
        public ButtonNode addChildByFUName(string funcName)
        {
            if (ChildrenDict.ContainsKey(funcName)) 
                return ChildrenDict[funcName] as ButtonNode;
            ButtonNode btnnode = new ButtonNode(FuncUnit.units[funcName]);
            btnnode.Provisioned = true;
            this.addChild(btnnode);
            return btnnode;
        }   
        public override void addChild(CustomBarsNodeBase node)
        {
            base.addChild(node);
            CustomBarsProvision.instance.addNodeToFuncUnitToBarsDict(node);
        }
        public override void removeChild(CustomBarsNodeBase node)
        {
            base.removeChild(node);
            CustomBarsProvision.instance.removeNodeFromFuncUnitToBarsDict(node);
        }
        public List<string> getBarSubnodesProvision()
        {
            List<string> fulist = new List<string>();
            foreach (ButtonNode btn in Children)
            {
                //Debug.WriteLine("    getBarSubnodesProvision: btn (" + btn.toString() + ")");
                fulist.Add(btn.Name);
            }
            return fulist;
        }
        public override string toString()
        {
            return "Node type: " + this.GetType().ToString() + " - " + "(name: " + Name + ", tip: " + Tip + ", provisioned: "
                + Provisioned + ", hashcode: " + m_sn + ", location: " + Location + ", row: " + Row + ", builtin: " + IsBuiltinBar
                + ", isSlide: " + CustomBarsProvision.instance.isValidSlideBar(this) + ")";
        }
        public void printBar()
        {
            Debug.WriteLine("printBar - bar name: " + Name + ", children count: " + this.Children.Count);
            Debug.WriteLine("            - bar string: " + toString());
            foreach (var c in Children) {
                Debug.WriteLine("              - button name: " + c.toString());
            }
        }
    }


    public class ButtonNode : CustomBarsNodeBase
    {
        private string m_icon;
        private string m_group;
        public string Icon { get => m_icon; set => m_icon = value; }
        public string Group { get => m_group; set => m_group = value; }
        public string GroupTip { 
            get { 
                if (CustomBarsProvision.FuncUnitGroupMapping.ContainsKey(m_group))
                    return CustomBarsProvision.FuncUnitGroupMapping[m_group];
                return m_group;
            } 
        }
        public ButtonNode(string name, string tip = null, string icon = null, string group = null, bool isProvisioned = true) : base(name, tip)
        {
            m_icon = icon;
            m_group = group;
            Provisioned = isProvisioned;
        }
        public ButtonNode(FuncUnit fu) : base(fu.name, fu.tip)
        {
            m_icon = fu.icon;
            m_group = fu.group;
            Provisioned = false;
        }
    }


    public class CustomBarsProvision
    {
        private static readonly Dictionary<string, string> m_layoutNameMapping = new Dictionary<string, string>{ {"main", "模型编辑器" },
            { "drawing", "图纸编辑器" }, { "drawing_template", "模板编辑器" }
        };
        private static Dictionary<string, string> m_barNameMapping = new Dictionary<string, string>{ {"file", "文件" },
            { "edit", "编辑" }, { "view", "视图" }, { "box", "工作区" }, { "comp", "组件" }, 
            { "create_sp", "钢材" }, { "create_cp", "混凝土" }, { "create_part_by_grid", "批量布置" },
            { "detail", "细部" }, { "cut", "裁切" }, { "helper", "辅助" }, { "measure", "测量" }, { "dr_file", "文件" },
            { "dr_edit", "编辑" }, { "dr_view", "视图" }, { "dr_shape", "形状" }, { "dr_mark", "标记" }, { "dr_dim", "尺寸" },
            { "dr_tmpl_file", "文件" }, { "dr_tmpl_zoom", "缩放" }, { "dr_tmpl_object", "对象" }, { "sym_editor_file", "文件" },
            { "sym_editor_edit", "编辑" }, { "sym_editor_misc", "杂项" }, { "sym_editor_element", "元素" }, { "view_active", "活动视图" },
            { "pick", "选择" }, { "snap", "捕捉" },{ "dr_pick", "图纸|选择" }, { "dr_snap", "图纸|捕捉" }
        };
        private static readonly Dictionary<string, string> m_funcUnitGroupMapping = new Dictionary<string, string> { { "ALL", "全部" },
            { "sys", "系统"}, { "file", "文件" }, { "edit", "编辑" }, { "view", "视图" }, { "sel", "对象组属性" },
            { "steel_part", "钢结构零件" }, { "concrete_part", "混凝土零件" }, { "comp", "组件" }, { "assembly", "构件" }, { "helper", "辅助" },
            { "detail", "细部" }, { "cut", "裁剪" }, { "measure", "测量标注" }, { "profile", "截面" }, { "mtrl", "材质" },
            { "report", "报告" }, { "number", "编码" }, { "tool", "工具" }, { "help", "帮助" }, { "dr_file", "图纸|文件" },
            { "dr_edit", "图纸|编辑" }, { "dr_view", "图纸|视图" }, { "dr_draw", "图纸|实例" },  { "", "其他" },
            { "dr_dim", "图纸|尺寸" }, { "dr_mark", "图纸|标记" }, { "dr_shape", "图纸|图形" }, { "dr_tmpl_file", "图纸模板|文件" },
            { "dr_tmpl_edit", "图纸模板|编辑" }, { "dr_tmpl_object", "图纸模板|插入" }, { "dr_tmpl_view", "图纸模板|视图" }
        };

        private static CustomBarsProvision m_instance;
        private static readonly object locker = new object();
        private static Dictionary<string, List<BarNode>> m_floatyBars4Layout = new Dictionary<string, List<BarNode>>();
        public const string BAR_LOCATION_TOP = "top_bar";
        public const string BAR_LOCATION_BOTTOM = "bottom_bar";
        public const string BAR_LOCATION_LEFT = "left_bar";
        public const string BAR_LOCATION_RIGHT = "right_bar";
        public const string BAR_LOCATION_FLOATY = "floaty_bar";
        public static readonly string[] BAR_LOCATIONS = { BAR_LOCATION_TOP, BAR_LOCATION_BOTTOM, BAR_LOCATION_LEFT, BAR_LOCATION_RIGHT, BAR_LOCATION_FLOATY };
        private static int ORDER = 0;

        private Dictionary<string, LayoutNode> m_all = new Dictionary<string, LayoutNode>();
        private Dictionary<string, LayoutNode> m_previous = new Dictionary<string, LayoutNode>();

        private Dictionary<string, List<ButtonNode>> m_buttonGroupsDict = new Dictionary<string, List<ButtonNode>>();
        private Dictionary<int, ButtonNode> m_buttonNodeDict = new Dictionary<int, ButtonNode>();
        private Dictionary<string, List<BarNode>> m_funcUnitToBarsDict = new Dictionary<string, List<BarNode>>();

        private Dictionary<int, BarNode> m_barNodeDict = new Dictionary<int, BarNode>();
        private Dictionary<string, BarNode> m_slideBarNodes = new Dictionary<string, BarNode>();

        private Dictionary<string, Dictionary<string, BarNode>> m_builtinBarNodes = new Dictionary<string, Dictionary<string, BarNode>>();

        private CustomBarsProvision() {
        }
        public static CustomBarsProvision instance {
            get {
                lock (locker) {
                    if (m_instance == null) {
                        m_instance = new CustomBarsProvision();
                        m_instance.initProvision();
                    }
                }
                return m_instance;
            }
        }

        public BarNode getBuiltinBar(string layout, string barName) {
            if (m_builtinBarNodes.ContainsKey(layout))
                if (m_builtinBarNodes[layout].ContainsKey(barName))
                    return m_builtinBarNodes[layout][barName];
            return null;
        }

        public void setBuiltinBar(string layout, BarNode bar) {
            if (!m_builtinBarNodes.ContainsKey(layout)) {
                m_builtinBarNodes[layout] = new Dictionary<string, BarNode>();
            }
            m_builtinBarNodes[layout][bar.Name] = bar;
        }

        public static Dictionary<string, List<BarNode>> floatyBarsWithinLayout {
            get => m_floatyBars4Layout;
        }

        public static Dictionary<string, string> FuncUnitGroupMapping {
            get => m_funcUnitGroupMapping;
        }
        public Dictionary<string, List<BarNode>> FuncUnitToBarsDict {
            get => m_funcUnitToBarsDict;
        }
        public void addNodeToFuncUnitToBarsDict(CustomBarsNodeBase node)
        {
            if (node == null || node.Parent.GetType() != typeof(BarNode))
            {
                //Console.Error.WriteLine("CUST TOOLBAR: get wrong node when add node to FU-Bar Dictionary!");
                Debug.WriteLine("CUST TOOLBAR: get wrong node when add node to FU-Bar Dictionary!");
                return;
            }
            if (!FuncUnitToBarsDict.ContainsKey(node.Name))
                FuncUnitToBarsDict[node.Name] = new List<BarNode>();
            FuncUnitToBarsDict[node.Name].Add(node.Parent as BarNode);
        }
        public void removeNodeFromFuncUnitToBarsDict(CustomBarsNodeBase node)
        {
            if (node == null || node.Parent.GetType() != typeof(BarNode))
            {
                //Console.Error.WriteLine("CUST TOOLBAR: get wrong node when delete node from FU-Bar Dictionary!");
                Debug.WriteLine("CUST TOOLBAR: get wrong node when delete node from FU-Bar Dictionary!");
                return;
            }
            if (FuncUnitToBarsDict.ContainsKey(node.Name))
                FuncUnitToBarsDict[node.Name].Remove(node.Parent as BarNode);
        }
        public Dictionary<int, BarNode> BarNodeDict {
            get {
                return m_barNodeDict;
            }
        }
        public Dictionary<int, ButtonNode> ButtonNodeDict {
            get {
                return m_buttonNodeDict;
            }
        }

        public Dictionary<string, List<ButtonNode>> ButtonGroupsDict {
            get => m_buttonGroupsDict;
        }

        public Dictionary<string, BarNode> SlideBarNodes {
            get => m_slideBarNodes;
        }

        public BarNode GetNextSlideBar(string barName) {
            if (SlideBarNodes.Count == 0)
                return null;
            List<string> keys = SlideBarNodes.Keys.ToList();
            int pos = keys.IndexOf(barName);
            //Debug.WriteLine("######## GetNextSlideBar - cur: " + barName + ", pos: " + pos.ToString() + ", first: " + SlideBarNodes[keys[0]] + " ########");
            if (pos == keys.Count - 1)
                return SlideBarNodes[keys[0]];
            else
                return SlideBarNodes[keys[pos + 1]];
        }

        public BarNode GetPreviousSlideBar(string barName) {
            if (SlideBarNodes.Count == 0)
                return null;
            List<string> keys = SlideBarNodes.Keys.ToList();
            int pos = keys.IndexOf(barName);
            //Debug.WriteLine("######## GetPreviousSlideBar - cur: " + barName + ", pos: " + pos.ToString() + ", last: " + keys.Last() + " ########");
            if (pos == 0)
                return SlideBarNodes[keys.Last()];
            if (pos != -1)
                return SlideBarNodes[keys[pos - 1]];
            return SlideBarNodes[keys[0]];
        }

        public void setSlideBar(BarNode bar) {
            if (!SlideBarNodes.ContainsKey(bar.Name)) {
                //Debug.WriteLine("######## bar - " + bar + " ########");
                SlideBarNodes[bar.Name] = bar;
            }
        }

        public string whereIsSlideBar() {
            foreach (BarNode bar in SlideBarNodes.Values) {
                if (bar.Provisioned)
                    return bar.Location;
            }
            return CustomBarsProvision.BAR_LOCATION_FLOATY;
        }

        public bool isValidSlideBar(BarNode bar) {
            foreach (ButtonNode btn in bar.Children) {
                if (btn.Name == "tool_next_bar" || btn.Name == "tool_previous_bar")
                    return true;
            }
            return false;
        }

        private void printSlideBars() {
            List<string> keys = SlideBarNodes.Keys.ToList();
            foreach (var key in keys) {
                Debug.WriteLine("######## printSlideBars - " + key + " ########");
            }
        }

        public LayoutNode[] AllNodes {
            get {
                return m_all.Values.ToArray();
            }
        }

        public void addLayout(string name, LayoutNode layout) {
            m_all[name] = layout;
        }
        public void removeLayout(string name) {
            this.m_all.Remove(name);
        }
        public bool isLayoutProvisioned(string layoutName) {
            if (this.m_all[layoutName] == null)
                return false;
            return this.m_all[layoutName].Provisioned;
        }
        public void backupPrevious() {
            foreach (var key in m_all.Keys) {
                m_previous[key] = m_all[key].Clone() as LayoutNode;
            }
        }
        public void restorePrevious() {
            if (m_previous.Count != 0) {
                m_all = m_previous;
                m_previous = new Dictionary<string, LayoutNode>();
            }
        }
        public void confirm(bool needSync = false) {
            m_previous = new Dictionary<string, LayoutNode>();
            foreach (var layout in m_all.Values) {

                //Console.WriteLine("confirm: layout.Name (" + layout.Name + "), LayoutManager.instance.curLayout(" + LayoutManager.instance.curLayout + ")");
                if (layout.Name == LayoutManager.instance.curLayout)
                    layout.syncActualBarsLocations();

                //Console.WriteLine("confirm: layout (" + layout.toString() + ")");
                Native.provisionLayout(layout.Name, layout.Tip, layout.getLayoutSubnodesProvision());

                foreach (BarNode bar in layout.Children) {
                    //Console.WriteLine("confirm - bar x = " + bar.x + ", y = " + bar.y);
                    /*Debug.WriteLine("confirm: bar (" + bar.toString() + ")");
                    Console.WriteLine("confirm: bar (" + bar.toString() + ")");
                    foreach (ButtonNode btn in bar.Children)
                    {
                        Debug.WriteLine("confirm: btn (" + btn.toString() + ")");
                        Console.WriteLine("confirm: btn (" + btn.toString() + ")");
                    }*/
                    Native.provisionBar(bar.Name, bar.IsBuiltinBar, bar.Tip, bar.Provisioned, bar.Location, bar.x, bar.y, bar.getBarSubnodesProvision().ToArray());
                }
            }
            //printAll();
            if (needSync) {
                Native.saveUIUserLayouts();
                Native.saveUIUserBars();
            }
        }
        public BarNode createBarNode(LayoutNode layout, string tip, string location = CustomBarsProvision.BAR_LOCATION_TOP) {
            string name = generateNewBarName();
            BarNode bar = new BarNode(name, tip, location);
            bar.Provisioned = true;
            return layout[name] = bar;
        }
        private string generateNewBarName() {
            string name = "user_bar_" + ORDER.ToString();
            //int suffix = 1;
            //bool go = true;
            //for (; go; suffix++)
            //{
            //    bool conflict = false;
            //    foreach (LayoutNode bar in m_all.Values)
            //    {
            //        name = name + suffix.ToString();
            //        if (bar[name] != null)
            //        {
            //            conflict = true;
            //            break;
            //        }
            //    }
            //    go &= conflict;
            //}
            return name;
        }
        public static int generateOrderId() {
            return ORDER++;
        }
        public LayoutNode getLayout(string name) {
            if (m_all.ContainsKey(name))
                return m_all[name];
            return null;
        }
        public LayoutNode getLayout(CustomBarsNodeBase node) {
            if (node == null)
                return null;
            if (node.GetType() == typeof(LayoutNode))
                return node as LayoutNode;
            if (node.GetType() == typeof(BarNode))
                return node.Parent as LayoutNode;
            if (node.GetType() == typeof(ButtonNode)) {
                if (node.Parent == null)
                    return null;
                return node.Parent.Parent as LayoutNode;
            }
            return null;
        }
        public BarNode getBar(int bar) {
            if (BarNodeDict.ContainsKey(bar))
                return BarNodeDict[bar];
            return null;
        }
        public BarNode getBar(CustomBarsNodeBase node) {
            if (node.GetType() == typeof(BarNode))
                return node as BarNode;
            if (node.GetType() == typeof(ButtonNode))
                return node.Parent as BarNode;
            return null;
        }
        public ButtonNode getButton(int barHashCode, string btnName) {
            if (BarNodeDict.ContainsKey(barHashCode))
                return BarNodeDict[barHashCode][btnName];
            return null;
        }
        public ButtonNode getButton(CustomBarsNodeBase node) {
            if (node.GetType() == typeof(ButtonNode))
                return node as ButtonNode;
            return null;
        }

        private ButtonNode generateSeparator() {
            string separatorName = "separator_" + generateOrderId().ToString();
            return new ButtonNode(separatorName, "|");
        }

        public ButtonNode insertSeparatorOnBar(int barHashCode, ButtonNode current) {
            if (BarNodeDict.ContainsKey(barHashCode)) {
                int idx = BarNodeDict[barHashCode].Children.FindIndex(item => item == current);
                ButtonNode separator = generateSeparator();
                BarNodeDict[barHashCode].insertChild(idx + 1, separator);
                //Debug.WriteLine(" +++---  insertSeparatorOnBar: separator ( name: " + separator.Parent.Name + ", sn: " + separator.Parent.SN + " )");
                return separator;
            }
            return null;
        }

        public ButtonNode appendSeparatorOnBar(int barHashCode) {
            if (BarNodeDict.ContainsKey(barHashCode)) {
                ButtonNode separator = generateSeparator();
                BarNodeDict[barHashCode].addChild(separator);
                //Debug.WriteLine(" ---+++  insertSeparatorOnBar: separator ( name: " + separator.Parent.Name + ", sn: " + separator.Parent.SN + " )");
                return separator;
            }
            return null;
        }

        private void initAllButtons() {
            foreach (FuncUnit fu in FuncUnit.units.Values) {
                List<ButtonNode> group = null;
                if (ButtonGroupsDict.ContainsKey(fu.group))
                    group = ButtonGroupsDict[fu.group];
                if (group == null) {
                    group = new List<ButtonNode>();
                    ButtonGroupsDict[fu.group] = group;
                }
                ButtonNode bnode = new ButtonNode(fu);
                group.Add(bnode);
                ButtonNodeDict[bnode.GetHashCode()] = bnode;
            }
        }

        private void initProvision() {
            initAllButtons();
            foreach (var item in m_layoutNameMapping) {
                string layoutName = item.Key;
                var layout = Native.queryUILayout(layoutName);
                LayoutNode layoutNode = new LayoutNode(layoutName, item.Value);
                this.m_all[layoutName] = layoutNode;
                //Debug.WriteLine("CustomBarsProvision - init: layout (" + item.Key + ", " + item.Value + ")");
                foreach (var loc in layout) {
                    string locationName = loc.Key as string;
                    DObject locations = loc.Value as DObject;
                    //Debug.WriteLine("CustomBarsProvision - init: locations (" + locationName + ", " + loc.Value + ")");
                    if (!locationName.Contains("bar"))
                        continue;

                    foreach (var barRow in locations) {
                        string[] bars = barRow.Value as string[];
                        int row = Int32.Parse(barRow.Key);

                        foreach (var bar in bars) {
                            var barProps = Native.queryUIBar(bar);

                            int x = -1, y = -1;
                            if (barProps != null && barProps.ContainsKey("x"))
                                x = (int)barProps["x"];
                            if (barProps != null && barProps.ContainsKey("y"))
                                y = (int)barProps["y"];

                            /* Console.WriteLine("initProvision - bar name = " + bar);
                             if (barProps != null && barProps.ContainsKey("y"))
                                 Console.WriteLine("initProvision - bar x = " + x + ", y = " + y);*/

                            if (barProps == null || (barProps.ContainsKey("builtin") && (bool)barProps["builtin"]))
                            {
                                BarNode builtinBar = new BarNode(bar, m_barNameMapping[bar], locationName, row, true, x, y);
                                setBuiltinBar(layoutName, builtinBar);
                                layoutNode.addChild(builtinBar);
                                continue;
                            }

                            string barTip = bar;
                            if (m_barNameMapping.ContainsKey(bar))
                                barTip = m_barNameMapping[bar];
                            else if (barProps.ContainsKey("tip"))
                                barTip = barProps["tip"] as string;

                            bool barProvioned = true;
                            if (barProps.ContainsKey("provisioned"))
                                barProvioned = (bool)barProps["provisioned"];

                            BarNode barNode = new BarNode(bar, barTip, locationName, row, false, x, y);
                            layoutNode.addChild(barNode);
                            //Debug.WriteLine("CustomBarsProvision - init: bar (" + bar + ", " + m_barNameMapping[bar] + ", " + locationName + ")");

                            string[] funcUnits = barProps["func_units"] as string[];
                            foreach (var fuName in funcUnits) {
                                if (fuName.StartsWith("separator_"))
                                    appendSeparatorOnBar(barNode.GetHashCode());
                                if (FuncUnit.get(fuName) == null)
                                    continue;

                                if (fuName == "tool_next_bar" || fuName == "tool_previous_bar")
                                    setSlideBar(layoutNode[bar]);

                                ButtonNode btn = new ButtonNode(FuncUnit.get(fuName));
                                layoutNode[bar].addChild(btn);
                                layoutNode[bar][fuName].Provisioned = true;
                                layoutNode[bar].Provisioned = barProvioned;
                            }
                        }
                    }
                }
            }
            //printSlideBars();
            //printAll();
        }

        public void printAll() {
            Debug.WriteLine("######## CustomBarsProvision - printAll START ########");
            foreach (LayoutNode l in m_all.Values.ToArray()) {
                Debug.WriteLine("\n\n");
                Debug.WriteLine(l.toString());
                foreach (BarNode bar in l.Children) {
                    Debug.WriteLine("==> " + bar.toString());
                    foreach (ButtonNode btn in bar.Children) {
                        Debug.WriteLine("====> " + btn.toString());
                    }
                }
                Debug.WriteLine("\n------- CustomBarsProvision - Layout Location");
                foreach (string loc in l.LocationMap.Keys)  // Dictionary<string, Dictionary<int, List<BarNode>>>
                {
                    Debug.WriteLine("==> location " + loc);
                    foreach (int r in l.LocationMap[loc].Keys) {
                        Debug.WriteLine("====> Row " + r.ToString());
                        foreach (BarNode b in l.LocationMap[loc][r]) {
                            Debug.WriteLine("======> bar " + b.toString());
                        }
                    }
                }
            }

            Debug.WriteLine("\n\n--------------- CustomBarsProvision - BarNodeDict");
            foreach (int key in BarNodeDict.Keys) {
                Debug.WriteLine("==> " + BarNodeDict[key].toString());
            }
            Debug.WriteLine("######## CustomBarsProvision - printAll END ########");
        }

        public void printLayoutLocations(string layoutName) {

            Debug.WriteLine("\n------- CustomBarsProvision - Layout Location - " + layoutName);
            foreach (string loc in m_all[layoutName].LocationMap.Keys)  // Dictionary<string, Dictionary<int, List<BarNode>>>
            {
                Debug.WriteLine("==> location " + loc);
                foreach (int r in m_all[layoutName].LocationMap[loc].Keys) {
                    Debug.WriteLine("====> Row " + r.ToString());
                    foreach (BarNode b in m_all[layoutName].LocationMap[loc][r]) {
                        Debug.WriteLine("======> bar " + b.toString());
                    }
                }
            }
        }

    }

}
