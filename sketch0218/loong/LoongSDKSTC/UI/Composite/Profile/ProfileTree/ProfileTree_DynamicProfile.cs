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
    class ProfileTree_DynamicProfile : ProfileTree_Profile
    {
        private List<string> m_fmts = null;
        
        public ProfileTree_DynamicProfile(ProfileTree tree, string type, string name) : base(tree, type, name) {
            this.ForeColor = Color.OrangeRed;
        }

        public override DObject props {
            get {
                if (!m_init) 
                    this._initProps();

                return m_props;
            }
        }

        public List<string> fmts {
            get {
                if (!m_init)
                    this._initProps();

                return m_fmts;
            }
        }

        public int fmtIdx {
            get {
                if (!m_init)
                    this._initProps();

                if (m_props.ContainsKey("fmt_idx"))
                    return m_props.GetInt("fmt_idx");

                if (m_fmts.Count > 0)
                    return 0;

                return -1;
            }

            set {
                if (!m_init)
                    this._initProps();

                var tmpProps = new DObject();
                tmpProps["fmt_idx"] = value;

                if (value >= 0 && value < m_fmts.Count)
                    tmpProps["sub_type"] = this.fmtSubType(m_fmts[value]);

                this.modify(tmpProps);
            }
        }

        public override string profileName {
            get {
                if (!m_init)
                    this._initProps();

                if (m_fmts == null)
                    return this.name;

                int fmtIdx = this.fmtIdx;
                if (fmtIdx < 0 || fmtIdx >= m_fmts.Count)
                    return this.name;

                string curFmt = m_fmts[fmtIdx];
                string subType = this.fmtSubType(curFmt);
                var layout = this.fmtLayout(curFmt);
                string prfname = this.name + this.fmtExpr(curFmt);
                foreach (var slice in layout) {
                    if (slice.IndexOf('|') < 0) {
                        string pname = slice;
                        string pshort = this.shortName(subType, pname);
                        if (String.IsNullOrEmpty(pshort))
                            continue;

                        if (!m_props.TryGetValue(pname, out object val))
                            continue;

                        string dst = '{' + pshort + '}';
                        prfname = prfname.Replace(dst, "" + String.Format("{0:0.######}", val));
                    }
                    else {
                        string[] pnames = slice.Split('|');
                        if (pnames.Length == 0)
                            continue;

                        string pshorts = "";
                        foreach (var pname in pnames) {
                            string pshort = this.shortName(subType, pname);
                            if (String.IsNullOrEmpty(pshort))
                                continue;

                            if (pshorts.Length > 0)
                                pshorts += '|';
                            pshorts += pshort;
                        }

                        if (!m_props.TryGetValue(pnames[0], out object val))
                            continue;

                        string dst = '{' + pshorts + '}';
                        prfname = prfname.Replace(dst, "" + String.Format("{0:0.######}", val));
                    }
                }

                if (prfname.IndexOf('{') < 0)
                    return prfname;
                
                return this.name;
            }

            //set {
            //    if (value == this.name)
            //        return;

            //    if (!m_init)
            //        this._initProps();

            //    var qry = Native.queryProfileProps(value);
            //    var subType = qry.GetString("sub_type");

            //    var layout = m_tree.layout(this.type, subType);
            //    var nameProps = new DObject();
            //    for (int i = 0; i < layout.Count; i += 2) {
            //        var pname = layout[i];
            //        if (qry.ContainsKey(pname)) {
            //            nameProps[pname] = qry[pname];
            //        }
            //    }

            //    if (qry.ContainsKey("fmt_idx")) 
            //        nameProps["fmt_idx"] = qry.GetInt("fmt_idx");

            //    nameProps["sub_type"] = subType;                      

            //    this.modify(nameProps);
            //}
        }

        public string fmtTip(string fmt) {
            var tip = fmt.Substring(fmt.LastIndexOf('$') + 1);
            tip = tip.Replace("{", "");
            tip = tip.Replace("}", "");
            return tip;
        }

        public string fmtSubType(string fmt) {
            return fmt.Substring(0, fmt.IndexOf('$'));
        }

        public string fmtExpr(string fmt) {
            return fmt.Substring(fmt.LastIndexOf('$') + 1);
        }

        public List<string> fmtLayout(string fmt) {
            var subType = fmt.Substring(0, fmt.IndexOf('$'));
            var expr = fmt.Substring(fmt.LastIndexOf('$') + 1);
            var layout = this.layout(subType);

            List<string> res = new List<string>();

            int off = expr.IndexOf('{') + 1;
            int end = expr.IndexOf('}', off);
            while (off > 0) {
                var pshort = expr.Substring(off, end - off);
                if (pshort.IndexOf('|') < 0) {
                    var p = layout.IndexOf(pshort);
                    if (p <= 0)
                        return new List<string> { };

                    var pname = layout[p - 1];
                    res.Add(pname);
                }
                else {
                    string[] pshorts = pshort.Split('|');
                    string pname = "";
                    foreach (var it in pshorts) {
                        var p = layout.IndexOf(it);
                        if (p <= 0)
                            return new List<string> { };

                        if (pname.Length > 0)
                            pname += '|';

                        pname += layout[p - 1];
                    }
                    res.Add(pname);
                }
                
                off = expr.IndexOf('{', end + 1) + 1;
                end = expr.IndexOf('}', off);
            }

            return res;
        }

        protected override void _initProps() {
            //var qry = Native.queryProfileProps(this.name);
            //if (qry == null)
            //    return;

            //qry.Remove("name");

            //if (qry.ContainsKey("name_fmt"))
            //    m_fmts = qry.GetStringArr("name_fmt").ToList();

            //int fmtIdx = qry.ContainsKey("fmt_idx") ? qry.GetInt("fmt_idx") : 0;
            //if (fmtIdx >= 0 && fmtIdx < m_fmts.Count)
            //    qry["sub_type"] = this.fmtSubType(m_fmts[qry.GetInt("fmt_idx")]);

            //bool isModified = m_modified;

            //this.modify(qry);

            //m_modified = isModified;

            m_init = true;
        }
    }

    
}
