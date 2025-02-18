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

namespace LoongCAD.UI
{
    class ProfileTree_Node : TreeNode
    {
        protected ProfileTree m_tree = null;
        protected ProfileTree_Rule m_parent = null;
        protected bool m_modified = false;
        protected DObject m_props = new DObject();

        public ProfileTree_Node(ProfileTree tree) {
            m_tree = tree;
        }

        public virtual void dispose() {
            if (m_parent != null) {
                m_parent.removeChild(this);
                m_parent = null;
            }

            this.Remove();
        }

        public ProfileTree tree {
            get => m_tree;
        }

        public bool modified {
            get => m_modified;
            set => m_modified = value;
        }

        public ProfileTree_Rule parent {
            get => m_parent;
            set => m_parent = value;
        }

        public bool isRoot() {
            return m_parent == null;
        }

        public virtual string name {
            get => m_props.ContainsKey("name") ? m_props.GetString("name") : "";

            set {
                m_props["name"] = (value != null) ? value : "";
                this.Text = value;
                this.modified = true;
            }
        }

        public virtual DObject props {
            get => m_props;
        }

        public virtual void modify(DObject value) {

        }

        //返回每个type的根rule(不是看不见的总root)
        public ProfileTree_Rule typeRule {
            get {
                ProfileTree_Node cur = this;
                while (cur != null) {
                    if (cur.parent == m_tree.root)
                        return cur as ProfileTree_Rule;

                    cur = cur.parent;
                }

                return null;
            }
        }

        
    }

    
}
