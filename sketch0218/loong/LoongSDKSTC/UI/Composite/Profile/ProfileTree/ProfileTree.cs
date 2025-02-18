using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    class ProfileTree : TreeView
    {
        private bool m_editable = true;
        private string m_filter = "*";
        private string m_filterRegExpr = Utils.ToRegExpr("*");

        private ProfilePropsGrid m_propsGrid = null;
        private ContextMenuStrip m_ruleMenu = null;
        private ContextMenuStrip m_profileMenu = null;
        private Action<ProfileTree_Node, ProfileTree_Node> m_onSelectChange = null;

        private ProfileTree_Rule m_root = null;
        private Dictionary<string, ProfileTree_Profile> m_profileNodes = new Dictionary<string, ProfileTree_Profile>();
        private SortedSet<string> m_delProfiles = new SortedSet<string>();

        private ProfileTree_Node m_selectedNode = null;
        private ProfileTree_Rule m_uiDirtyRule = null;

        private Dictionary<string, Dictionary<string, List<string>>> m_layouts = new Dictionary<string, Dictionary<string, List<string>>>();

        public Action<TreeNodeMouseClickEventArgs> nodeMouseDoubleClick = null;

        public ProfileTree(bool editable) {
            m_editable = editable;

            this.Dock = DockStyle.Fill;
            this.Scrollable = true;
            this.ItemHeight = this.FontHeight + 6;
            this.LabelEdit = false;
            this.HideSelection = false;
            
            m_root = new ProfileTree_Rule(this);

            if (editable) {
                m_ruleMenu = new ProfileTree_Rule_Menu(this);
                m_profileMenu = new ProfileTree_Profile_Menu(this);
            }
        }

        public ProfileTree_Rule root => m_root;
        public bool editable => m_editable;

        public ProfilePropsGrid propsGrid {
            get => m_propsGrid;
            set => m_propsGrid = value;
        }

        public Action<ProfileTree_Node, ProfileTree_Node> onSelectChange {
            get => m_onSelectChange;
            set => m_onSelectChange = value;
        }

        public string filter {
            get => m_filter;
            set {
                string str = (value != null) ? value.Trim() : null;
                if (String.IsNullOrEmpty(str))
                    str = "*";

                if (m_filter == str)
                    return;

                m_filter = str;

                if (!str.StartsWith("*"))
                    str = "*" + str;
                if (!str.EndsWith("*"))
                    str = str + "*";
                m_filterRegExpr = Utils.ToRegExpr(str);

                m_root.filterByName();
                m_root.refreshUI();

                if (m_filter != "*")
                    this.ExpandAll();
            }
        }

        public string filterRegExpr => m_filterRegExpr;

        public void reset() {
            this.Nodes.Clear();
            this.m_root.clearChildren();
            this.m_profileNodes.Clear();
            this.m_delProfiles.Clear();
            m_selectedNode = null;

            //var infos = Native.queryProfilesBaseInfo();
            
            //List<ProfileTree_Profile> profileNodes = new List<ProfileTree_Profile>();
            //for (int i = 0; i < infos.Length; i += 4) {
            //    string name = infos[i];
            //    string type = infos[i + 1];
            //    string subType = infos[i + 2];
            //    bool dynamic = (infos[i + 3] == "1");

            //    if (name.IndexOf('$') == 0) //temp profile
            //        continue;

            //    ProfileTree_Profile node = this.createProfileNode(new object[] {
            //        "type", type,
            //        "sub_type", subType,
            //        "name", name,
            //        "dynamic", dynamic,
            //    });

            //    profileNodes.Add(node);
            //}

            //DObject retRules = Native.queryProfileRules();
            //if (retRules == null
            //    || !retRules.ContainsKey("root"))
            //    return;

            //DObject[] roots = retRules.GetDObjectArr("root");
            //if (roots != null) {
            //    foreach (var root in roots) {
            //        ProfileTree_Rule node = this.createRuleNode(root);
            //        m_root.addChild(node);
            //    }
            //}

            //m_root.relayout(profileNodes);
            m_root.filterByName();
            m_root.refreshUI();
        }

        public ProfileTree_Rule createRuleNode(DObject props) {
            ProfileTree_Rule node = new ProfileTree_Rule(this);
            node.modify(props);

            if (m_editable)
                node.ContextMenuStrip = m_ruleMenu;

            if (props.ContainsKey("children")) {
                var childrenProps = props.GetDObjectArr("children");
                foreach (var childProps in childrenProps) {
                    ProfileTree_Rule child = this.createRuleNode(childProps);
                    node.addChild(child);
                }
            }

            node.modified = false;
            return node;
        }

        public void removeRuleNode(ProfileTree_Rule node) {
            node.dispose();
        }

        public ProfileTree_Profile createProfileNode(DObject props) {
            string type = props.GetString("type");
            string subType = props.GetString("sub_type");
            string name = props.GetString("name");
            bool dynamic = props.ContainsKey("dynamic") ? props.GetBool("dynamic") : false;

            ProfileTree_Profile node = dynamic ? new ProfileTree_DynamicProfile(this, type, name) : new ProfileTree_Profile(this, type, name);
            node.modify(props);

            if (m_editable)
                node.ContextMenuStrip = m_profileMenu;

            node.modified = false;

            if (m_profileNodes.ContainsKey(node.name))
                m_profileNodes[node.name].dispose();

            m_profileNodes[node.name] = node;
            return node;
        }

        public void removeProfileNode(ProfileTree_Profile node) {
            //remove from profile list
            m_profileNodes.Remove(node.name);
            m_delProfiles.Add(node.name);

            node.dispose();
        }

        public ProfileTree_Profile getProfileNode(string name) {
            ProfileTree_Profile node = null;
            m_profileNodes.TryGetValue(name, out node);
            return node;
        }

        public SortedSet<string> delProfiles => m_delProfiles;

        public bool renameProfile(ProfileTree_Profile node, string newName) {
            if (node.name == newName)
                return true;

            if (String.IsNullOrEmpty(newName))
                return false;

            if (node is ProfileTree_DynamicProfile) 
                return false;
            
            if (m_profileNodes.ContainsKey(newName)) 
                return false;

            string oldName = node.name;
            node.name = newName;

            m_profileNodes.Remove(oldName);
            m_delProfiles.Add(oldName);
            m_profileNodes[newName] = node as ProfileTree_Profile;

            m_root.relayout(node as ProfileTree_Profile);
            this.selectedNode = node;

            this.Invalidate();

            return true;
        }

        protected override void OnPaint(PaintEventArgs e) {
            base.OnPaint(e);

            if (m_uiDirtyRule != null) {
                var sel = this.selectedNode;
                m_uiDirtyRule.refreshUI();
                if (sel != null)
                    this.selectedNode = sel;
                m_uiDirtyRule = null;
            }
        }

        public string validProfileName(string name) {
            if (!m_profileNodes.ContainsKey(name))
                return name;

            int n = 1;
            while (m_profileNodes.ContainsKey(name + n)) {
                n++;
            }

            return name + n;
        }

        public ProfileTree_Node selectedNode {
            get => m_selectedNode;
            set {
                ProfileTree_Node oldNode = m_selectedNode;
                m_selectedNode = value;
                this.SelectedNode = value;
                this.Select();

                if (oldNode != m_selectedNode)
                    this._onSelectChange(oldNode, m_selectedNode);
            }
        }

        public string selectedProfile {
            get {
                if (this.selectedNode == null
                    || !(m_selectedNode is ProfileTree_Profile))
                    return "";

                if (m_propsGrid.node == this.selectedNode)
                    m_propsGrid.saveToNode();
                
                return (m_selectedNode as ProfileTree_Profile).profileName;
            }

            set {
                if (value == null) {
                    this.selectedNode = null;
                    return;
                }

                if (m_profileNodes.ContainsKey(value)) {
                    this.selectedNode = m_profileNodes[value];
                }
                else {
                    //var qry = Native.queryProfileProps(value);
                    //if (qry == null || !qry.ContainsKey("fmt_base")) {
                    //    this.selectedNode = null;
                    //    return;
                    //}

                    //string fmtBase = qry.GetString("fmt_base");
                    //if (!m_profileNodes.ContainsKey(fmtBase)) {
                    //    this.selectedNode = null;
                    //    return;
                    //}

                    //ProfileTree_DynamicProfile node = m_profileNodes[fmtBase] as ProfileTree_DynamicProfile;
                    //if (node == null) {
                    //    this.selectedNode = null;
                    //    return;
                    //}

                    //node.profileName = value;
                    //this.selectedNode = node;
                }
            }
        }

        protected void _onSelectChange(ProfileTree_Node oldNode, ProfileTree_Node newNode) {
            if (m_onSelectChange != null)
                m_onSelectChange(oldNode, newNode);
        }

        public List<ProfileTree_Profile> modifiedProfiles {
            get {
                m_propsGrid.saveToNode();

                List<ProfileTree_Profile> modified = new List<ProfileTree_Profile>();
                foreach (var it in m_profileNodes) {
                    ProfileTree_Profile node = it.Value;
                    if (node.modified && !(node is ProfileTree_DynamicProfile)) {
                        modified.Add(node);
                    }
                }

                return modified;
            }
        }

        public void resetModifiedProfiles() {
            foreach (var it in m_profileNodes) {
                it.Value.modified = false;
            }

            m_delProfiles.Clear();
        }

        protected override void OnAfterSelect(TreeViewEventArgs e) {
            base.OnAfterSelect(e);

            if (this.selectedNode != e.Node)
                this.selectedNode = e.Node as ProfileTree_Node;
        }

        protected override void OnNodeMouseClick(TreeNodeMouseClickEventArgs e) {
            base.OnNodeMouseClick(e);

            if (e.Node == null)
                return;

            this.selectedNode = e.Node as ProfileTree_Node;
        }

        protected override void OnNodeMouseDoubleClick(TreeNodeMouseClickEventArgs e) {
            base.OnNodeMouseDoubleClick(e);

            if (nodeMouseDoubleClick != null)
                nodeMouseDoubleClick(e);
        }

        protected override void OnKeyPress(KeyPressEventArgs e) {
            base.OnKeyPress(e);

            var key = (Keys)e.KeyChar;
            if (key == Keys.Enter
                || key == Keys.Escape
                || key == Keys.Tab) //prevent beep sound
                e.Handled = true;
        }

        public List<string> layout(string type, string subType) {
            Dictionary<string, List<string>> dct = null;
            if (!m_layouts.TryGetValue(type, out dct)) {
                dct = new Dictionary<string, List<string>>();
                m_layouts[type] = dct;
            }

            List<string> layout = null;
            //if (!dct.TryGetValue(subType, out layout)) {
            //    var qry = Native.queryProfilePropsLayout(type, subType);
            //    if (qry != null) 
            //        layout = qry.ToList();

            //    dct[subType] = layout;
            //}

            return layout;
        }

        //protected override void OnDrawNode(DrawTreeNodeEventArgs e) {
        //    if (e.Node == null)
        //        return;

        //    Color foreColor;
        //    if (e.Node == this.SelectedNode) {
        //        foreColor = SystemColors.HighlightText;
        //        ControlPaint.DrawFocusRectangle(e.Graphics, e.Bounds, foreColor, SystemColors.Highlight);
        //    }
        //    else {
        //        foreColor = this.ForeColor;
        //    }

        //    TextRenderer.DrawText(
        //        e.Graphics,
        //        e.Node.Text,
        //        e.Node.NodeFont ?? e.Node.TreeView.Font,
        //        e.Bounds,
        //        foreColor,
        //        TextFormatFlags.VerticalCenter | TextFormatFlags.Left | TextFormatFlags.GlyphOverhangPadding);
        //}
    }

    
}
