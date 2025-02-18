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
    class ProfileTree_Rule_Menu : SubMenuBase
    {
        private ProfileTree m_tree = null;

        public ProfileTree_Rule_Menu(ProfileTree tree)
        {
            m_tree = tree;

            this.AddItems(new ToolStripItem[] {
                this.NewItem("新建截面", NewProfile_Click),
                this.NewSep(),
                this.NewItem("新建标准", NewRule_Click),
                this.NewItem("新建子标准", NewChildRule_Click),
                this.NewItem("编辑标准", EditRule_Click),
                this.NewItem("删除标准", DelRule_Click),
                this.NewSep(),
                this.NewItem("向上移动", MoveUp_Click),
                this.NewItem("向下移动", MoveDown_Click),
            });
        }

        private void MoveDown_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule node = m_tree.selectedNode as ProfileTree_Rule;
            if (node == null)
                return;

            node.moveDown();

            node.parent.relayout();
            node.parent.refreshUI();
            m_tree.selectedNode = node;
        }

        private void MoveUp_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule node = m_tree.selectedNode as ProfileTree_Rule;
            if (node == null)
                return;

            node.moveUp();

            node.parent.relayout();
            node.parent.refreshUI();
            m_tree.selectedNode = node;
        }

        private void DelRule_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule node = m_tree.selectedNode as ProfileTree_Rule;
            if (node == null)
                return;

            var par = node.parent;
            var prfs = node.dumpProfiles();
            m_tree.removeRuleNode(node);
            par.relayout(prfs);
            par.refreshUI();
        }

        private void EditRule_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule node = m_tree.selectedNode as ProfileTree_Rule;
            if (node == null)
                return;

            Form_ProfileRuleProps dlg = new Form_ProfileRuleProps();
            if (dlg.show(node.props) == DialogResult.OK)
            {
                node.modify(dlg.Props);

                node.parent.relayout();
                node.parent.refreshUI();
            }
        }

        private void NewChildRule_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule parent = m_tree.selectedNode as ProfileTree_Rule;
            if (parent == null)
                return;

            ProfileTree_Rule node = m_tree.createRuleNode(new object[]{
                                                "type", "rule",
                                                "name", "新标准",
                                                "target_types", new string[]{ },
                                                "filter", "*"
                                            });

            parent.addChild(node);
            
            parent.relayout();
            parent.refreshUI();
            m_tree.selectedNode = node;
        }

        private void NewRule_Click(object sender, EventArgs e)
        {
            ProfileTree_Rule bro = m_tree.selectedNode as ProfileTree_Rule;
            if (bro == null)
                return;

            ProfileTree_Rule node = m_tree.createRuleNode(new object[]{ 
                "type", "rule",
                "name", "新标准",
                "target_types", new string[]{ },
                "filter", "*"
            });

            bro.parent.insertRule(bro, node);
            bro.parent.relayout();
            bro.parent.refreshUI();
            m_tree.selectedNode = node;
        }

        private void NewProfile_Click(object sender, EventArgs e)
        {
        //    ProfileTree_Rule parNode = m_tree.selectedNode as ProfileTree_Rule;
        //    if (parNode == null)
        //        return;

        //    parNode = parNode.typeRule;
        //    string[] types = parNode.profileTypes;
        //    string type = "I";
        //    if (types != null) {
        //        for (int i = 0; i < types.Length; ++i) {
        //            if (types[i] == "All")
        //                continue;

        //            type = types[i];
        //            break;
        //        }
        //    }
            
        //    var nodeProps = Native.profileInitProps(type);
        //    if (nodeProps == null)
        //        return;

        //    nodeProps["name"] = m_tree.validProfileName(type + "_新截面");

        //    ProfileTree_Profile node = m_tree.createProfileNode(nodeProps);
        //    if (node == null)
        //        return;

        //    node.modified = true;

        //    if (parNode.relayout(node)) {
        //        parNode.refreshUI();
        //    }
        //    else {
        //        m_tree.root.relayout(node);
        //        m_tree.root.refreshUI();
        //    }
            
        //    m_tree.selectedNode = node;
        }
    }

    
}
