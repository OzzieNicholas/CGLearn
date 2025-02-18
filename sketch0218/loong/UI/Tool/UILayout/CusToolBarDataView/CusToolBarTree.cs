using System;
using System.Collections.Generic;
using System.Linq;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;

namespace UI
{
    [ToolboxItem(false)]
    public class CusToolBarTreeNode : TreeNode
    {
        private CustomBarsNodeBase m_cusNode = null;
        public CusToolBarTreeNode() {  }

        public CusToolBarTreeNode(CustomBarsNodeBase node) 
        { 
            m_cusNode = node;
            if (node.Tip == "|") this.Text = "------";
            else this.Text = node.Tip;
        }

        public bool foundChild(CusToolBarTreeNode node)
        {
            return this.Nodes.Contains(node);
        }

        public CustomBarsNodeBase CustomNode
        {
            get { return m_cusNode; }
            set { m_cusNode = value; }
        }
    }

    [ToolboxItem(false)]
    public class CusToolBarTree : TreeView
    {
        private Form_CusToolBar m_form = null;
        private CustomBarsProvision m_prov = null;
        private Dictionary<int, CusToolBarTreeNode> m_flatNodeMapping = new Dictionary<int, CusToolBarTreeNode>();

        public CusToolBarTree(Form_CusToolBar form) {
            this.Dock = DockStyle.Fill;
            this.Scrollable = true;
            this.ItemHeight = this.FontHeight + 6;
            this.LabelEdit = false;
            this.TabStop = false;
            this.m_prov = CustomBarsProvision.instance;
            this.CheckBoxes = true;
            this.HideSelection = false;
            //
            //this.DrawMode = TreeViewDrawMode.OwnerDrawText;
            //this.DrawNode += new DrawTreeNodeEventHandler(TreeView_DrawNode);
            this.m_form = form;
            this.FullRowSelect = true;

            this.StateImageList = new ImageList();
            this.StateImageList.Images.Add(Res.dpiImage("uilayout_treenode_invisible"));
            this.StateImageList.Images.Add(Res.dpiImage("uilayout_treenode_visible"));
        }

        private void TreeView_DrawNode(object sender, DrawTreeNodeEventArgs e) {
            e.DrawDefault = true; //使用默认颜色，只需要在TreeView失去焦点时选中节点仍然突显  
            Color foreColor;
            Color backColor;
            if ((e.State & TreeNodeStates.Selected) > 0) {
                foreColor = Color.White;//鼠标点击节点时文字颜色
                backColor = Color.DodgerBlue;//鼠标点击节点时背景颜色
            }
            else {
                foreColor = this.ForeColor;
                backColor = this.BackColor;
            }

            using (var br = new SolidBrush(backColor)) {
                e.Graphics.FillRectangle(br, new Rectangle(e.Bounds.Location, new Size(this.Width + 6, e.Bounds.Height)));
            }

            using (var br = new SolidBrush(foreColor)) {
                e.Graphics.DrawString(e.Node.Text, this.Font, br, e.Bounds.X, e.Bounds.Y + 4);
            }
            
            return;
        }

        public void reset() {
            m_flatNodeMapping.Clear();
            foreach (TreeNode node1 in this.Nodes) {
                if (node1 != null) node1.Nodes.Clear();
            }
            this.Nodes.Clear();
            this.refilterTree();
        }

        public BarNode getSelectedBar() {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            if (sel_tnode == null) {
                return null;
            }

            CustomBarsNodeBase sel_cnode = sel_tnode.CustomNode;
            if (sel_cnode.GetType() == typeof(LayoutNode)) {
                return null;
            }

            return CustomBarsProvision.instance.getBar(sel_cnode);
        }

        public ButtonNode AddTreeButtonNode(string funcName) {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            if (sel_tnode == null) {
                MessageBox.Show("请选择放置功能项的工具栏。", "未选择放置位置");
                return null;
            }

            CustomBarsNodeBase sel_cnode = sel_tnode.CustomNode;
            if (sel_cnode.GetType() == typeof(LayoutNode)) {
                MessageBox.Show("当前选择位于编辑器节点。请选择放置功能项的工具栏", "无法放置功能项");
                return null;
            }

            LayoutNode layout = CustomBarsProvision.instance.getLayout(sel_cnode);
            BarNode bar = CustomBarsProvision.instance.getBar(sel_cnode);
            CustomBarsNodeBase btn = null;
            if (bar.ChildrenDict.ContainsKey(funcName))
                btn = bar.ChildrenDict[funcName];

            CusToolBarTreeNode tlayout = getFlatTreeNode(layout);
            CusToolBarTreeNode tbar = getFlatTreeNode(bar);
            CusToolBarTreeNode tbtn = getFlatTreeNode(btn);

            if (btn == null) {
                if (tbtn != null) {
                    Debug.WriteLine("[ERROR] AddTreeButtonNode - provision sync error! Btn existed in provision, but disappeared in tree view! (" + funcName + ")");
                    this.SelectedNode = tbtn;
                    return btn as ButtonNode;
                }
                btn = bar.addChildByFUName(funcName);
                tbtn = generateFlatTreeNode(btn);
                tbar.Nodes.Add(tbtn);
                if (btn.Name == "tool_next_bar" || btn.Name == "tool_previous_bar")
                    CustomBarsProvision.instance.setSlideBar(bar);
                HideCheckBox(getFlatTreeNode(btn));
            }

            tlayout.Expand();
            tbar.Expand();
            this.SelectedNode = tbtn;
            //CustomBarsProvision.instance.printAll();
            return btn as ButtonNode;
        }

        public void AddTreeBarNode(string name) {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            if (sel_tnode == null) {
                MessageBox.Show("请选择放置新工具栏的编辑器。", "未选择放置位置");
                return;
            }

            CustomBarsNodeBase sel_cnode = sel_tnode.CustomNode;

            LayoutNode layout = CustomBarsProvision.instance.getLayout(sel_cnode);
            if (layout == null) {
                MessageBox.Show("未找到编辑器，程序内部错误，请联系软件供应商。", "内部错误");
                return;
            }
            BarNode node = CustomBarsProvision.instance.createBarNode(layout, name);
            node.Provisioned = true;

            CusToolBarTreeNode tlayout = getFlatTreeNode(layout);
            CusToolBarTreeNode tbar = generateFlatTreeNode(node);

            //if (sel_cnode.GetType() == typeof(BarNode))
            //{
            //    tlayout.Nodes.Insert(sel_tnode.Index + 1, tbar);
            //}
            //else
            //{
            tlayout.Nodes.Add(tbar);
            //}

            tbar.Checked = true;

            tlayout.Expand();
            tbar.Expand();
            this.SelectedNode = tbar;
        }

        public void AddSeparator() {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            if (sel_tnode == null) {
                MessageBox.Show("请选择放置功能项的工具栏。", "未选择放置位置");
                return;
            }
            CustomBarsNodeBase sel_cnode = sel_tnode.CustomNode;
            if (sel_cnode.GetType() == typeof(LayoutNode)) {
                MessageBox.Show("当前选择位于编辑器节点。请选择放置功能项的工具栏或功能按钮", "无法添加分隔线");
                return;
            }

            LayoutNode layout = CustomBarsProvision.instance.getLayout(sel_cnode);
            BarNode bar = CustomBarsProvision.instance.getBar(sel_cnode);
            ButtonNode btn = CustomBarsProvision.instance.getButton(sel_cnode);

            CusToolBarTreeNode tlayout = getFlatTreeNode(layout);
            CusToolBarTreeNode tbar = getFlatTreeNode(bar);

            ButtonNode separator = null;
            CusToolBarTreeNode tseparator;
            if (sel_cnode.GetType() != typeof(ButtonNode)) {
                separator = CustomBarsProvision.instance.appendSeparatorOnBar(bar.GetHashCode());
                tseparator = generateFlatTreeNode(separator);
                tbar.Nodes.Add(tseparator);
            }
            else {
                separator = CustomBarsProvision.instance.insertSeparatorOnBar(bar.GetHashCode(), btn);
                tseparator = generateFlatTreeNode(separator);
                int idx = this.SelectedNode.Index;
                tbar.Nodes.Insert(idx + 1, tseparator);
            }
            HideCheckBox(getFlatTreeNode(separator));

            tlayout.Expand();
            tbar.Expand();
            this.SelectedNode = tseparator;
        }

        public CustomBarsNodeBase RemoveTreeBarNode()
        {
            CustomBarsNodeBase node = (this.SelectedNode as CusToolBarTreeNode).CustomNode;
            if (node.GetType() != typeof(BarNode)) 
                return null;
            foreach (CustomBarsNodeBase n in node.Children)
            {
                removeFlatTreeNode(n);
            }
            removeFlatTreeNode(node);
            node.remove();
            this.SelectedNode.Remove();
            return node;
        }

        public CustomBarsNodeBase RemoveTreeBtnNode()
        {
            CustomBarsNodeBase node = (this.SelectedNode as CusToolBarTreeNode).CustomNode;
            if (node.GetType() != typeof(ButtonNode)) 
                return null;
            removeFlatTreeNode(node);
            node.remove();
            this.SelectedNode.Remove();
            return node;
        }

        public void MoveUpSelected() {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            CusToolBarTreeNode prev_tnode;
            if (sel_tnode == null || sel_tnode.CustomNode.GetType() == typeof(LayoutNode) || sel_tnode.CustomNode.GetType() == typeof(BarNode)) {
                MessageBox.Show("请在主界面拖动工具栏位置以调整顺序。", "未选功能按钮");
                return;
            }
            CustomBarsNodeBase cnode = sel_tnode.CustomNode;
            cnode.moveUp();

            int tag = 0;
            string text = "";

            if (sel_tnode == null) return;
            else {
                prev_tnode = sel_tnode.PrevNode as CusToolBarTreeNode;
                if (prev_tnode == null) return;
                else {
                    text = prev_tnode.Text;
                    tag = Convert.ToInt32(prev_tnode.Tag);
                    prev_tnode.Tag = sel_tnode.Tag;
                    prev_tnode.Text = sel_tnode.Text;
                    sel_tnode.CustomNode = prev_tnode.CustomNode;
                    prev_tnode.CustomNode = cnode;
                    sel_tnode.Tag = tag;
                    sel_tnode.Text = text;
                }
            }
            this.SelectedNode = prev_tnode;
        }

        public void MoveDownSelected() {
            CusToolBarTreeNode sel_tnode = this.SelectedNode as CusToolBarTreeNode;
            CusToolBarTreeNode next_tnode;
            if (sel_tnode == null || sel_tnode.CustomNode.GetType() == typeof(LayoutNode) || sel_tnode.CustomNode.GetType() == typeof(BarNode)) {
                MessageBox.Show("请在主界面拖动工具栏位置以调整顺序。", "未选功能按钮");
                return;
            }
            CustomBarsNodeBase cnode = sel_tnode.CustomNode;
            cnode.moveDown();

            int tag = 0;
            string text = "";

            if (sel_tnode == null) return;
            else {
                next_tnode = sel_tnode.NextNode as CusToolBarTreeNode;
                if (next_tnode == null) return;
                else {
                    text = next_tnode.Text;
                    tag = Convert.ToInt32(next_tnode.Tag);
                    next_tnode.Tag = sel_tnode.Tag;
                    next_tnode.Text = sel_tnode.Text;
                    sel_tnode.CustomNode = next_tnode.CustomNode;
                    next_tnode.CustomNode = cnode;
                    sel_tnode.Tag = tag;
                    sel_tnode.Text = text;
                }
            }
            this.SelectedNode = next_tnode;
        }

        private bool _isVisableInTree(CusToolBarTreeNode node) {
            var cn = node.CustomNode;
            if (cn.GetType() == typeof(BarNode)) {
                return !((cn as BarNode).IsBuiltinBar);
            }
            return true;
        }

        public void filterNode(CusToolBarTreeNode node, CusToolBarTreeNode parent, bool isUnderRoot = false) {
            TreeNodeCollection children = (parent == null ? this.Nodes : parent.Nodes);
            if (children.Contains(node)) children.Remove(node);
            if (_isVisableInTree(node)) {
                children.Add(node);
                if (node.CustomNode.GetType() != typeof(BarNode))
                {
                    HideCheckBox(node);
                }
                else
                    node.Checked = node.CustomNode.Provisioned;
            }
            else removeFlatTreeNode(node.CustomNode);
        }

        public CusToolBarTreeNode getFlatTreeNode(CustomBarsNodeBase node) {
            if (node == null) return null;
            if (!m_flatNodeMapping.ContainsKey(node.GetHashCode()))
                return null;
            return m_flatNodeMapping[node.GetHashCode()];
        }
        public CusToolBarTreeNode generateFlatTreeNode(CustomBarsNodeBase node) {
            if (node == null) return null;
            if (getFlatTreeNode(node) == null)
                m_flatNodeMapping[node.GetHashCode()] = new CusToolBarTreeNode(node);
            return m_flatNodeMapping[node.GetHashCode()];
        }
        public void removeFlatTreeNode(CustomBarsNodeBase node) {
            if (getFlatTreeNode(node) == null)
                return;
            m_flatNodeMapping[node.GetHashCode()] = null;
        }

        public void printFlat() {
            foreach (int hashcode in m_flatNodeMapping.Keys) {
                Debug.WriteLine("printFlat - Tip: " + m_flatNodeMapping[hashcode].CustomNode.Tip);
                Debug.WriteLine("printFlat - Name: " + m_flatNodeMapping[hashcode].CustomNode.Name);
            }

        }

        public void refilterTree() {
            //printFlat();
            foreach (LayoutNode layout in m_prov.AllNodes) {
                CusToolBarTreeNode layoutNode = generateFlatTreeNode(layout);
                //Debug.WriteLine("refilterTree - layout: " + layout.toString());
                //Debug.WriteLine("refilterTree - layoutNode: " + layoutNode.Text);
                filterNode(layoutNode, null, true);
                if (LayoutManager.instance.curLayout == layout.Name)
                    this.SelectedNode = layoutNode;
                foreach (BarNode bar in layout.Children) {
                    CusToolBarTreeNode barNode = generateFlatTreeNode(bar);
                    filterNode(barNode, layoutNode);
                    foreach (ButtonNode btn in bar.Children) {
                        CusToolBarTreeNode btnNode = generateFlatTreeNode(btn);
                        //Debug.WriteLine("refilterTree - btn: " + btn.toString());
                        //Debug.WriteLine("refilterTree - btnNode: " + btnNode.Text);
                        filterNode(btnNode, barNode);
                    }
                }
                HideCheckBox(layoutNode);
                layoutNode.Expand();
            }

            m_flatNodeMapping.Values.First().EnsureVisible();
        }

        protected override void OnNodeMouseClick(TreeNodeMouseClickEventArgs e) {
            base.OnNodeMouseClick(e);

            if (e.Node == null)
                return;

            CusToolBarTreeNode node = e.Node as CusToolBarTreeNode;
            if (node.CustomNode.GetType() == typeof(BarNode)) {
                node.CustomNode.Provisioned = node.Checked;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct TVITEM
        {
            public int mask;
            public IntPtr hItem;
            public int state;
            public int stateMask;
            public int pszText;
            public int cchTextMax;
            public int iImage;
            public int iSelectedImage;
            public int cChildren;
            public int lParam;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct POINTAPI
        {
            public int x;
            public int y;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct TVHITTESTINFO
        {
            public POINTAPI pt;
            public int flags;
            public IntPtr hItem;
        }
        //Messages
        private const int TV_FIRST = 0x1100;
        private const int TVM_SETIMAGELIST = TV_FIRST + 9;
        private const int TVM_GETITEM = TV_FIRST + 12;
        private const int TVM_SETITEM = TV_FIRST + 13;
        private const int TVM_HITTEST = TV_FIRST + 17;

        // TVM_SETIMAGELIST image list kind
        private const int TVSIL_STATE = 2;

        //TVITEM.mask flags
        private const int TVIF_STATE = 0x8;
        private const int TVIF_HANDLE = 0x10;
        //TVITEM.state flags
        private const int TVIS_STATEIMAGEMASK = 0xF000;
        //TVHITTESTINFO.flags flags
        private const int TVHT_ONITEMSTATEICON = 0x40;

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SendMessage(IntPtr hwnd, int msg, int wparam, IntPtr lparam);
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SendMessage(IntPtr hwnd, int msg, int wparam, ref TVITEM lparam);
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SendMessage(IntPtr hwnd, int msg, int wparam, ref TVHITTESTINFO lparam);
        public void HideCheckBox(TreeNode node) {
            if (node == null) return;
            TVITEM tvi = new TVITEM();
            tvi.hItem = node.Handle;
            tvi.mask = TVIF_STATE;
            tvi.stateMask = TVIS_STATEIMAGEMASK;
            tvi.state = 0;
            SendMessage(node.TreeView.Handle, TVM_SETITEM, (int)IntPtr.Zero, ref tvi);
        }

    }


}
