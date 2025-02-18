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
using IFWrapper;

namespace LoongCAD.UI
{
    class ProfileTree_Profile : ProfileTree_Node
    {
        protected bool m_init = false;
        protected bool m_visible = true;

        public ProfileTree_Profile(ProfileTree tree, string type, string name) : base(tree) {
            m_props["type"] = type;
            m_props["name"] = name;
            this.Name = name;
            this.Text = name;
        }

        public bool visible {
            get => m_visible;
            set => m_visible = value;
        }

        public bool isInit {
            get => m_init;
            set => m_init = value;
        }

        public string type {
            get => m_props.ContainsKey("type") ? m_props.GetString("type") : "";
        }

        public string subType {
            get => m_props.ContainsKey("sub_type") ? m_props.GetString("sub_type") : "";
            set {
                if (!m_init)
                    this._initProps();

                if (!(this is ProfileTree_DynamicProfile)) {
                    m_props["sub_type"] = value;
                    this.modified = true;
                }
            }
        }

        public override string name {
            get => base.name;

            set {
                if (value == base.name)
                    return;

                //this method is only called while changing name
                //don't change rule position in this situation
                //if (m_parent != null)
                //    m_parent.onProfileNameChanged(this, this.name, value);

                m_props["name"] = (value != null) ? value : "";
                this.Text = value;
                this.modified = true;
            }
        }

        public virtual string profileName {
            get => this.name;
            set { }
        }

        public void filterByName() {
            string filter = m_tree.filter;
            m_visible = (filter == null) || (filter.Length == 0) || (filter == "*")
                        || Utils.RegExprMatch(this.name, m_tree.filterRegExpr);
        }

        public override DObject props {
            get {
                if (!m_init) 
                    this._initProps();

                return m_props;
            }
        }

        public override void modify(DObject value) {
            if (value == null)
                return;

            foreach (var it in value) {
                if (it.Key == "type" //type can not be changed
                    || it.Key == "name") //name is changed by property this.name
                    continue;

                if (it.Value is string) {
                    string val = it.Value as string;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetString(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is double) {
                    double val = (double)it.Value;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetDouble(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is int) {
                    int val = (int)it.Value;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetInt(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is uint) {
                    uint val = (uint)it.Value;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetUint(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is long) {
                    long val = (long)it.Value;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetLong(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is bool) {
                    bool val = (bool)it.Value;
                    if (!m_props.ContainsKey(it.Key) || val != m_props.GetBool(it.Key)) {
                        m_props[it.Key] = val;
                        m_modified = true;
                    }
                }
                else if (it.Value is DObject) {
                    DObject val = it.Value as DObject;
                    if (!m_props.ContainsKey(it.Key) || !val.Equals(m_props[it.Key])) {
                        m_props[it.Key] = val.Clone();
                        m_modified = true;
                    }
                }
                else if (it.Value is Vec2[]) {
                    var val = it.Value as Vec2[];
                    if (!m_props.ContainsKey(it.Key)
                        || !val.SequenceEqual(m_props[it.Key] as Vec2[])) {
                        m_props[it.Key] = val.Clone();
                        m_modified = true;
                    }
                }
            }
        }

        public List<string> layout(string subType) {
            return m_tree.layout(this.type, subType);
        }

        public string shortName(string subType, string fullName) {
            var layout = m_tree.layout(this.type, subType);
            if (layout == null)
                return "";

            int idx = layout.IndexOf(fullName);
            if (idx < 0)
                return "";

            return layout[idx + 1];
        }

        public string fullName(string subType, string shortName) {
            var layout = m_tree.layout(this.type, subType);
            if (layout == null)
                return "";

            int idx = layout.IndexOf(shortName);
            if (idx < 1)
                return "";

            return layout[idx - 1];
        }

        protected virtual void _initProps() {
            //var qry = Native.queryProfileProps(this.name);
            //if (qry == null)
            //    return;

            //bool isModified = m_modified;

            //var props = Native.profileInitProps(qry.GetString("type"));
            //props.CopyFrom(qry);
            //this.modify(props);

            //m_modified = isModified;
            m_init = true;
        }

    }

    
}
