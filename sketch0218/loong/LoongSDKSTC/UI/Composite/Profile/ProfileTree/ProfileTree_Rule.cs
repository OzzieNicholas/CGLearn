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

namespace LoongCAD.UI
{
    class ProfileTree_Rule : ProfileTree_Node
    {
        protected List<ProfileTree_Rule> m_rules = new List<ProfileTree_Rule>();
        protected SortedDictionary<string, ProfileTree_Profile> m_profiles = new SortedDictionary<string, ProfileTree_Profile>();
        private string[] m_types = new string[] { };
        private string[] m_filterRegExpr = new string[] { };

        public ProfileTree_Rule(ProfileTree tree) : base(tree) {

        }

        public override void dispose() {
            foreach (var child in m_rules) {
                child.parent = null;
                m_tree.removeRuleNode(child);
            }
            m_rules.Clear();

            m_profiles.Clear();

            base.dispose();
        }

        public string[] profileTypes {
            get {
                return m_types;
            }
        }

        public string nameFilter {
            get {
                return m_props.ContainsKey("name_filter") ? m_props["name_filter"] as string : "*";
            }
        }

        public List<ProfileTree_Rule> rules {
            get {
                return m_rules;
            }
        }

        public SortedDictionary<string, ProfileTree_Profile> profiles {
            get {
                return m_profiles;
            }
        }

        public override void modify(DObject value) {
            if (value == null)
                return;

            foreach (var it in value) {
                switch (it.Key) {
                    case "name": {
                        string val = it.Value as string;
                        if (!m_props.ContainsKey("name") || val != m_props.GetString("name")) {
                            m_props["name"] = val;
                            this.Name = it.Value as string;
                            this.Text = it.Value as string;
                            m_modified = true;
                        }
                    }
                    break;
                    case "type": {
                        string val = it.Value as string;
                        if (!m_props.ContainsKey("type") || val != m_props.GetString("type")) {
                            m_props["type"] = val;
                            m_modified = true;
                        }
                    }
                    break;
                    case "profile_types": {
                        IEnumerable<string> val = it.Value as IEnumerable<string>;
                        if (!m_props.ContainsKey("profile_types") || !val.SequenceEqual(m_props["profile_types"] as string[])) {
                            m_types = val.ToArray();
                            m_props["profile_types"] = val.ToArray();
                            m_modified = true;
                        }
                    }
                    break;
                    case "name_filter": {
                        string val = it.Value as string;
                        if (!m_props.ContainsKey("name_filter") || val != m_props.GetString("name_filter")) {
                            m_props["name_filter"] = val;

                            List<string> li = new List<string>();
                            string[] filters = val.Split(' ');
                            foreach (var f in filters) {
                                string tf = f.Trim();
                                if (tf.Length > 0)
                                    li.Add(Utils.ToRegExpr(tf));
                            }

                            m_filterRegExpr = li.ToArray();
                            m_modified = true;
                        }
                    }
                    break;
                    case "temp": {
                        bool val = (bool)it.Value;
                        if (!m_props.ContainsKey("temp") || val != (bool)m_props["temp"]) {
                            m_props["temp"] = val;
                            m_modified = true;
                        }
                    }
                    break;
                }
            }
        }

        public void insertRule(ProfileTree_Rule pos, ProfileTree_Rule child) {
            if (child.parent != null && child.parent != this)
                child.parent.removeChild(child);

            int idx = m_rules.IndexOf(pos as ProfileTree_Rule);

            if (idx + 1 < m_rules.Count)
                m_rules.Insert(idx + 1, child as ProfileTree_Rule);
            else
                m_rules.Add(child as ProfileTree_Rule);

            child.parent = this;
        }

        public virtual void moveUp() {
            int idx = m_parent.m_rules.IndexOf(this);
            if (idx <= 0)
                return;

            m_parent.m_rules.Remove(this);
            m_parent.m_rules.Insert(idx - 1, this);
        }

        public virtual void moveDown() {
            int idx = m_parent.m_rules.IndexOf(this);
            if (idx == m_parent.m_rules.Count - 1)
                return;

            m_parent.m_rules.Remove(this);

            if (idx + 1 < m_parent.m_rules.Count)
                m_parent.m_rules.Insert(idx + 1, this);
            else
                m_parent.m_rules.Add(this);
        }

        public void addChild(ProfileTree_Node child) {
            if (child.parent == this)
                return;

            if (child.parent != null)
                child.parent.removeChild(child);

            if (child is ProfileTree_Rule)
                m_rules.Add(child as ProfileTree_Rule);
            else if (child is ProfileTree_Profile)
                m_profiles[_profileSortKey(child.name)] = child as ProfileTree_Profile;

            child.parent = this;
        }

        public void addChildren(List<ProfileTree_Node> children) {
            foreach (var child in children) {
                if (child.parent == this)
                    continue;

                if (child.parent != null)
                    child.parent.removeChild(child);

                if (child is ProfileTree_Rule)
                    m_rules.Add(child as ProfileTree_Rule);
                else if (child is ProfileTree_Profile)
                    m_profiles[_profileSortKey(child.name)] = child as ProfileTree_Profile;

                child.parent = this;
            }
        }

        public void removeChild(ProfileTree_Node child) {
            child.parent = null;

            if (child is ProfileTree_Rule)
                m_rules.Remove(child as ProfileTree_Rule);
            else if (child is ProfileTree_Profile)
                m_profiles.Remove(_profileSortKey(child.name));
        }

        public void clearChildren() {
            foreach (var rule in m_rules) {
                rule.parent = null;
            }
            m_rules.Clear();

            foreach (var it in m_profiles) {
                it.Value.parent = null;
            }
            m_profiles.Clear();
        }

        public void onProfileNameChanged(ProfileTree_Profile child, string oldName, string newName) {
            m_profiles.Remove(_profileSortKey(oldName));
            m_profiles[_profileSortKey(newName)] = child;
        }


        public DObject fullProps {
            get {
                var childProps = new DObject[m_rules.Count];
                for (int i = 0; i < m_rules.Count; ++i) {
                    childProps[i] = m_rules[i].fullProps;
                }

                DObject curProps = new DObject();

                if (m_tree.root == this) {
                    curProps["root"] = childProps;
                }
                else {
                    curProps.CopyFrom(this.props);

                    if (childProps.Length > 0)
                        curProps["children"] = childProps;
                }

                return curProps;
            }
        }

        public List<ProfileTree_Profile> dumpProfiles() {
            List<ProfileTree_Profile> prfs = new List<ProfileTree_Profile>();

            Stack<ProfileTree_Rule> rules = new Stack<ProfileTree_Rule>();
            rules.Push(this);
            while (rules.Count > 0) {
                var rule = rules.Pop();
                prfs.AddRange(rule.m_profiles.Values);

                foreach (var child in rule.m_rules)
                    rules.Push(child);
            }

            return prfs;
        }

        //根据rule将prf分配到合适的逻辑子树
        public virtual bool relayout(ProfileTree_Profile prf) {
            if (prf == null)
                return false;

            if (!this.filterProfile(prf))
                return false;

            foreach (var rule in m_rules) {
                if (rule.relayout(prf))
                    return true;
            }

            this.addChild(prf);
            return true;
        }

        //根据rule将profiles分配到合适的逻辑子树
        public virtual List<ProfileTree_Profile> relayout(List<ProfileTree_Profile> profiles = null) {
            if (profiles == null)
                profiles = this.dumpProfiles();

            List<ProfileTree_Profile> pickedPrfs = null;
            List<ProfileTree_Profile> unpickedPrfs = null;
            foreach (var prf in profiles) {
                if (this.filterProfile(prf)) {
                    if (pickedPrfs == null)
                        pickedPrfs = new List<ProfileTree_Profile>();

                    pickedPrfs.Add(prf);
                }
                else {
                    if (unpickedPrfs == null)
                        unpickedPrfs = new List<ProfileTree_Profile>();

                    unpickedPrfs.Add(prf);
                }
            }

            if (pickedPrfs == null)
                return unpickedPrfs;

            foreach (var rule in m_rules) {
                if (pickedPrfs == null || pickedPrfs.Count == 0)
                    break;

                pickedPrfs = rule.relayout(pickedPrfs);
            }

            if (pickedPrfs != null) {
                foreach (var prf in pickedPrfs)
                    this.addChild(prf);
            }

            return unpickedPrfs;
        }

        //根据tree的name filter遍历设置所有叶子节点profile的可见性
        public virtual void filterByName() {
            foreach (var rule in m_rules)
                rule.filterByName();

            foreach (var it in m_profiles)
                it.Value.filterByName();
        }

        public bool filterProfile(ProfileTree_Profile node) {
            if (m_types.Length > 0) {
                bool typeIn = false;
                foreach (var type in m_types) {
                    if (type == "All" || type == node.type) {
                        typeIn = true;
                        break;
                    }
                }

                if (!typeIn)
                    return false;
            }

            if (m_filterRegExpr.Length == 0)
                return true;

            foreach (var expr in m_filterRegExpr) {
                if (Utils.RegExprMatch(node.name, expr))
                    return true;
            }

            return false;
        }

        //根据叶子节点profile的可见性，重建UI Tree
        public virtual bool refreshUI() {
            m_tree.SuspendLayout();

            bool vis = this._refreshUI();

            m_tree.ResumeLayout();

            return vis;
        }

        protected bool _refreshUI() {
            bool vis = false;
            bool nofilter = (m_tree.filter == "*");

            if (m_tree.root == this) {//root
                m_tree.Nodes.Clear();

                foreach (var rule in m_rules) {
                    if (!rule._refreshUI())
                        continue;

                    vis = true;

                    rule.Remove();
                    m_tree.Nodes.Add(rule);
                }

                foreach (var it in m_profiles) {
                    if (!it.Value.visible)
                        continue;

                    vis = true;

                    it.Value.Remove();
                    m_tree.Nodes.Add(it.Value);
                }
            }
            else {
                this.Nodes.Clear();

                foreach (var rule in m_rules) {
                    if (!rule._refreshUI())
                        continue;

                    vis = true;

                    rule.Remove();
                    this.Nodes.Add(rule);
                }

                foreach (var it in m_profiles) {
                    if (!it.Value.visible)
                        continue;

                    vis = true;

                    it.Value.Remove();
                    this.Nodes.Add(it.Value);
                }
            }

            return vis || nofilter;
        }

        protected string _profileSortKey(string name) {
            string key = "";
            string number = "";
            foreach (char c in name) {
                if (c < '0' || c > '9') {
                    if (number.Length > 0) {
                        key += number.PadLeft(6, '0');
                        number = "";
                    }

                    key += c;
                }
                else {
                    number += c;
                }
            }

            if (number.Length > 0) {
                key += number.PadLeft(6, '0');
            }

            return key;
        }
    }

    
}
