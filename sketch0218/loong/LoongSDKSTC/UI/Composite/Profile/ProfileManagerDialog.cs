using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using LoongCAD.Common;
using IFWrapper;

namespace LoongCAD.UI
{
    public partial class ProfileManagerDialog : ParamFormBase
    {
        public static ProfileManagerDialog m_instance = new ProfileManagerDialog();
        private ProfileTree m_tree = null;
        private ProfilePropsGrid m_propsGrid = null;

        public static ProfileManagerDialog instance => m_instance;

        public ProfileManagerDialog() {
            InitializeComponent();

            this.DisposeOnClose = false;
            this.Owner = Form.FromHandle(Native.mainHandle()) as Form;
            this._initControls();
            this.InitStartPosition(Native.mainHandle(), Native.screenWorkAreaRelativeToPrimary(Native.mainHandle()), 0.618f, 0.2f);
        }

        private void _initControls() {
            this.DoubleBuffered = true;

            tb_filter.onLookUp += onFilter;

            tb_name.KeyDown += Tb_name_KeyDown;
            tb_name.LostFocus += Tb_name_LostFocus;

            m_tree = new ProfileTree(true);
            m_propsGrid = new ProfilePropsGrid();

            m_tree.propsGrid = m_propsGrid;
            m_tree.onSelectChange = onSelectChange;

            m_propsGrid.editable = true;
            m_propsGrid.Width = pan_props.Width;

            pan_tree.Controls.Add(m_tree);
            pan_props.Controls.Add(m_propsGrid);

            tb_profile_type.Text = "";

            this.UpdateTabOrder();

            Callback.regUICmdListener(onCommand, "on_doc_new");
            Callback.regUICmdListener(onCommand, "on_doc_loaded");
        }

        public DObject onCommand(string cmd, DObject paras) {
            switch (cmd) {
                case "on_doc_new":
                case "on_doc_loaded": {
                    this._reset();
                    this.Hide();
                }
                break;
            }

            return null;
        }

        public void ShowWindow() {
            if (this.Visible) {
                this.Focus();
                return;
            }

            var selPrf = m_tree.selectedProfile;
            var selRule = (m_tree.selectedNode is ProfileTree_Rule)? (m_tree.selectedNode as ProfileTree_Rule).name : "";
            var selRuleExpand = (m_tree.selectedNode is ProfileTree_Rule) ? (m_tree.selectedNode as ProfileTree_Rule).IsExpanded : false;

            this.m_tree.reset();
            this.m_tree.filter = tb_filter.Text;

            if (!String.IsNullOrEmpty(selPrf))
                this.m_tree.selectedProfile = selPrf;
            else if (!String.IsNullOrEmpty(selRule)) {
                Stack<ProfileTree_Rule> st = new Stack<ProfileTree_Rule>();
                st.Push(m_tree.root);
                bool bFound = false;
                while (st.Count > 0) {
                    var cur = st.Pop();
                    if (cur.name == selRule) {
                        m_tree.selectedNode = cur;
                        if (selRuleExpand)
                            cur.Expand();

                        bFound = true;
                        break;
                    }

                    foreach (var node in cur.rules) 
                        st.Push(node);
                }

                if (!bFound)
                    m_tree.selectedNode = null;
            }
            else
                m_tree.selectedNode = null;

            this.Visible = true;
            this.Focus();
        }

        protected void _reset() {
            this.m_tree.reset();
            this.lb_name.Visible = false;
            this.tb_name.Visible = false;
            this.tb_name.Text = "";
            this.m_propsGrid.node = null;
        }

        protected new void Show() {
            base.Show();
        }

        protected new DialogResult ShowDialog() {
            return base.ShowDialog();
        }

        protected override void OnFormClosing(FormClosingEventArgs e) {
            if (this.Visible
                && this._askAndSave() == DialogResult.Cancel 
                && e.CloseReason == CloseReason.UserClosing)
                e.Cancel = true;
            else
                base.OnFormClosing(e);
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            if (this._askAndSave() == DialogResult.Cancel)
                return;

            this.Hide();
        }

        private void btn_modify_Click(object sender, EventArgs e) {
            this._askAndSave();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.Hide();
        }

        void onSelectChange(ProfileTree_Node oldNode, ProfileTree_Node newNode) {
            if (!(newNode is ProfileTree_Profile)) {
                tb_profile_type.Text = "";

                lb_profile_subType.Text = "子类型";
                cbo_profile_subType.ClearItems();
                cbo_profile_subType.UpdateItems();

                img_tips.modify("", "");

                lb_name.Visible = false;
                tb_name.Visible = false;
                tb_name.Text = "";

                m_propsGrid.node = null;
                return;
            }

            var nodeProps = newNode.props;
            var subType = nodeProps.GetString("sub_type");

            if (newNode is ProfileTree_DynamicProfile) {
                var fmtNode = newNode as ProfileTree_DynamicProfile;

                tb_profile_type.Text = fmtNode.type;
                lb_profile_subType.Text = "格式";

                cbo_profile_subType.ClearItems();
                for (int i = 0; i < fmtNode.fmts.Count; ++i) {
                    var fmt = fmtNode.fmts[i];
                    var fmtTip = fmtNode.fmtTip(fmt);
                    cbo_profile_subType.AddItem(i, fmtTip);
                }
                cbo_profile_subType.UpdateItems();

                if (fmtNode.fmtIdx >= 0) 
                    cbo_profile_subType.SelectedKey = fmtNode.fmtIdx;

                img_tips.modify(fmtNode.type, fmtNode.subType);

                tb_name.Text = fmtNode.name + "  [动态配置]";
                lb_name.Visible = true;
                tb_name.Visible = true;
                tb_name.Enabled = false;

                m_propsGrid.editable = false;
            }
            else {
                var prfNode = newNode as ProfileTree_Profile;

                tb_profile_type.Text = prfNode.type;
                lb_profile_subType.Text = "子类型";
                
                //var subTypes = Native.queryProfileSubtypes(prfNode.type);
                //if (subTypes != null) {
                //    List<ComboItem> items = new List<ComboItem>();
                //    foreach (var st in subTypes) {
                //        items.Add(new ComboItem(st, Lang.Local(st, "profile_subtype")));
                //    }
                //    cbo_profile_subType.LoadItems(items.ToArray());

                //    if (cbo_profile_subType.ContainsKey(subType)) 
                //        cbo_profile_subType.SelectedKey = subType;
                //    else if (cbo_profile_subType.ComboItems.Count > 0) 
                //        cbo_profile_subType.SelectedIndex = 0;
                //}
                //else {
                //    cbo_profile_subType.ClearItems();
                //    cbo_profile_subType.UpdateItems();
                //}

                img_tips.modify(prfNode.type, cbo_profile_subType.SelectedStrKey);

                tb_name.Text = prfNode.name;
                lb_name.Visible = true;
                tb_name.Visible = true;
                tb_name.Enabled = true;

                m_propsGrid.editable = true;
            }

            m_propsGrid.node = newNode as ProfileTree_Profile;
            this.UpdateTabOrder();
        }

        private void onFilter(LookUpInput sender, string text) {
            m_tree.filter = text;
        }

        private void cbo_profile_subType_SelectionChangeCommitted(object sender, EventArgs e) {
            if (cbo_profile_subType.SelectedItem == null)
                return;

            var node = m_tree.selectedNode;
            if (node is ProfileTree_DynamicProfile) {
                var fmtNode = node as ProfileTree_DynamicProfile;
                m_propsGrid.fmtIdx = cbo_profile_subType.SelectedIntKey;
                img_tips.modify(fmtNode.type, fmtNode.subType);
            }
            else if (node is ProfileTree_Profile) {
                var prfNode = node as ProfileTree_Profile;
                m_propsGrid.subType = cbo_profile_subType.SelectedStrKey;
                img_tips.modify(prfNode.type, prfNode.subType);
            }
            else
                img_tips.modify("", "");
        }

        private void Tb_name_LostFocus(object sender, EventArgs e) {
            if (m_tree.selectedNode == null) {
                tb_name.Text = "";
                return;
            }

            var node = m_tree.selectedNode;
            var newName = tb_name.Text.Trim();

            if (node is ProfileTree_Rule) {
                tb_name.Text = "";
                return;
            }

            if (node is ProfileTree_DynamicProfile) {
                tb_name.Text = node.name + "  [动态配置]";
                return;
            }

            if (!(node is ProfileTree_Profile))
                return;

            if (node.name == newName)
                return;
            
            m_propsGrid.saveToNode();

            if (!m_tree.renameProfile(node as ProfileTree_Profile, newName)) {
                MessageBox.Show("新截面名称冲突或非法", "提示", MessageBoxButtons.OK);
                tb_name.Text = node.name;
            }

            m_propsGrid.loadFromNode();
        }

        private void Tb_name_KeyDown(object sender, KeyEventArgs e) {
            if (!e.KeyCode.HasFlag(Keys.Enter)
                || m_tree.selectedNode == null)
                return;

            var node = m_tree.selectedNode;
            var newName = tb_name.Text.Trim();

            if (String.IsNullOrEmpty(newName) || newName == node.name) {
                tb_name.Text = node.name;
                return;
            }

            if (node is ProfileTree_Rule) {
                tb_name.Text = "";
                return;
            }
                
            if (node is ProfileTree_DynamicProfile) {
                tb_name.Text = node.name + "  [动态配置]";
                return;
            }

            if (!(node is ProfileTree_Profile))
                return;

            if (node.name == newName)
                return;

            m_propsGrid.saveToNode();

            if (!m_tree.renameProfile(node as ProfileTree_Profile, newName)) {
                tb_name.Text = node.name;
                MessageBox.Show("截面名冲突", "提示", MessageBoxButtons.OK);
            }

            m_propsGrid.loadFromNode();
        }

        private DialogResult _askAndSave() {
            if (Native.isDocumentReadOnly()) {
                MessageBox.Show("无法修改只读模型", "提示", MessageBoxButtons.OK);
                return DialogResult.Cancel;
            }
            
            m_propsGrid.saveToNode();

            //DObject orgRules = Native.queryProfileRules();
            //DObject rules = m_tree.root.fullProps;
            //List<ProfileTree_Profile> profiles = m_tree.modifiedProfiles;
            //SortedSet<string> delProfiles = m_tree.delProfiles;
            //bool updateRule = !orgRules.Equals(rules);

            //if (updateRule || profiles.Count > 0 || delProfiles.Count > 0) {
            //    DialogResult ret = MessageBox.Show("要保存对截面的修改吗?", "截面型材管理器", MessageBoxButtons.YesNoCancel);

            //    if (ret == DialogResult.Yes) {
            //        //delete profiles
            //        if (delProfiles.Count > 0) {
            //            Native.delUserProfiles(delProfiles.ToArray());
            //        }

            //        //save modified profiles
            //        Native.saveUserProfilesAndRules(updateRule ? rules : null, profiles.Select(a => a.props).ToArray(), true);
            //        m_tree.resetModifiedProfiles();
            //    }
            //    else if (ret == DialogResult.No) {
            //        this.m_tree.reset();
            //    }

            //    return ret;
            //}

            return DialogResult.OK;
        }


    }
}
