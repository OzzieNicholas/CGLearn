using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.UI;
using IFWrapper;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    class ProfileTree_Profile_Menu : SubMenuBase
    {
        private ProfileTree m_tree = null;

        public ProfileTree_Profile_Menu(ProfileTree tree) {
            m_tree = tree;

            this.AddItems(new ToolStripItem[] {
                this.NewItem("新建截面", NewProfile_Click),
                this.NewItem("复制截面", CopyProfile_Click),
                this.NewItem("删除截面", DelProfile_Click),
            });
        }

        private void NewProfile_Click(object sender, EventArgs e) {
            //ProfileTree_Profile selNode = m_tree.selectedNode as ProfileTree_Profile;
            //if (selNode == null)
            //    return;

            //var nodeProps = Native.profileInitProps(selNode.type);
            //if (nodeProps == null)
            //    return;

            //nodeProps["name"] = m_tree.validProfileName(selNode.type + "_新截面");

            //ProfileTree_Profile node = m_tree.createProfileNode(nodeProps);
            //if (node == null)
            //    return;

            //node.modified = true;

            //if (selNode.typeRule != null
            //    && selNode.typeRule.relayout(node)) {
            //    selNode.typeRule.refreshUI();
            //}
            //else {
            //    m_tree.root.relayout(node);
            //    m_tree.root.refreshUI();
            //}
            
            //m_tree.selectedNode = node;
        }

        private void CopyProfile_Click(object sender, EventArgs e) {
            ProfileTree_Profile selNode = m_tree.selectedNode as ProfileTree_Profile;
            if (selNode == null)
                return;

            DObject copyProps = selNode.props.Clone();
            copyProps["name"] = m_tree.validProfileName(selNode.name + "副本");

            if (selNode is ProfileTree_DynamicProfile) {
                copyProps.Remove("dynamic");
                copyProps.Remove("name_fmt");
                copyProps.Remove("fmt_tip");
                copyProps.Remove("fmt_idx");
            }

            ProfileTree_Profile node = m_tree.createProfileNode(copyProps);
            if (node == null)
                return;

            node.modified = true;

            if (selNode.typeRule != null
                && selNode.typeRule.relayout(node)) {
                selNode.typeRule.refreshUI();
            }
            else {
                m_tree.root.relayout(node);
                m_tree.root.refreshUI();
            }

            m_tree.selectedNode = node;
        }

        private void DelProfile_Click(object sender, EventArgs e) {
            ProfileTree_Profile selNode = m_tree.selectedNode as ProfileTree_Profile;
            if (selNode == null)
                return;

            if (selNode is ProfileTree_DynamicProfile) {
                MessageBox.Show("系统截面无法删除", "提示", MessageBoxButtons.OK);
                return;
            }

            m_tree.removeProfileNode(selNode);
        }
    }
}
