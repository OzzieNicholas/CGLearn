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
using IFWrapper;

namespace LoongCAD.UI
{
    public partial class ProfileExplorerDialog : ParamFormBase
    {
        private ProfileTree m_tree = null;
        private ProfilePropsGrid m_propsGrid = null;

        public ProfileExplorerDialog() {
            InitializeComponent();

            this.DisposeOnClose = true;
            this.CacheBoundsKey = "profile_explorer";

            this._initControls();
            this.InitStartPosition(Native.mainHandle(), Native.screenWorkAreaRelativeToPrimary(Native.mainHandle()), 0.618f, 0.2f);
        }

        private void _initControls() {
            tb_filter.onLookUp += onFilter;

            m_tree = new ProfileTree(false);
            m_propsGrid = new ProfilePropsGrid();

            m_tree.propsGrid = m_propsGrid;
            m_tree.onSelectChange = this.onSelectChange;
            m_tree.nodeMouseDoubleClick += this.OnNodeMouseDoubleClick;
            pan_tree.Controls.Add(m_tree);

            m_propsGrid.editable = false;
            m_propsGrid.Width = pan_props.Width;
            pan_props.Controls.Add(m_propsGrid);

            tb_profile_type.Text = "";
        }

        public string SelectedProfile {
            get {
                m_propsGrid.saveToNode();
                return m_tree.selectedProfile;
            }

            set {
                if (m_tree.selectedProfile == value)
                    return;

                m_tree.selectedProfile = value;
            }
        }

        public DialogResult ShowModal(string profile) {
            tb_filter.Text = "";
            this.m_tree.reset();
            this.m_tree.filter = "";
            this.m_propsGrid.node = null;
            this.SelectedProfile = profile;
            return this.ShowDialog(Form.FromHandle(Native.mainHandle()));
        }

        protected new void Show() {
            base.Show();
        }

        protected new DialogResult ShowDialog() {
            return base.ShowDialog();
        }

        protected new bool Visible => base.Visible;
            
        private void btn_ok_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        protected void OnNodeMouseDoubleClick(TreeNodeMouseClickEventArgs e) {
            //NOTE: 这里不能用e.Node，m_tree.selectedNode是在Click阶段选中的节点，e.Node会误选成展开后的子节点
            if (m_tree.selectedNode is ProfileTree_Profile) {
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        void onSelectChange(ProfileTree_Node oldNode, ProfileTree_Node newNode) {
            if (!(newNode is ProfileTree_Profile)) {
                tb_profile_type.Text = "";

                lb_profile_subType.Text = "子类型";
                cbo_profile_subType.ClearItems();
                cbo_profile_subType.UpdateItems();
                img_tips.modify("", "");
                m_propsGrid.node = null;
                return;
            }

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

                cbo_profile_subType.Enabled = true;
                m_propsGrid.editable = true;
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

                //    if (cbo_profile_subType.ContainsKey(prfNode.subType))
                //        cbo_profile_subType.SelectedKey = prfNode.subType;
                //    else if (cbo_profile_subType.ComboItems.Count > 0)
                //        cbo_profile_subType.SelectedIndex = 0;
                //}
                //else {
                //    cbo_profile_subType.ClearItems();
                //    cbo_profile_subType.UpdateItems();
                //}

                img_tips.modify(prfNode.type, cbo_profile_subType.SelectedStrKey);

                cbo_profile_subType.Enabled = false;
                m_propsGrid.editable = false;
            }

            m_propsGrid.node = newNode as ProfileTree_Profile;
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
                var prfNode = node as ProfileTree_DynamicProfile;
                img_tips.modify(prfNode.type, prfNode.subType);
            }
            else
                img_tips.modify("", "");
        }
    }
}
