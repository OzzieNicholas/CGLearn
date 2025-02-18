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
using LoongCAD.Localization;
using IFWrapper;

namespace LoongCAD.UI
{
    [ToolboxItem(false)]
    class ProfilePropsGrid : CGrid
    {
        private ProfileTree_Profile m_node = null;
        private bool m_editable = true;

        public ProfilePropsGrid() {
            this.Dock = DockStyle.None;
            this.defColSizeType = SizeType.Percent;
            this.defRowSizeType = SizeType.AutoSize;
            //this.defColWidth = 60.0f;
            this.defRowHeight = 28.0f;
            this.colCount = 3;
            this.rowCount = 1;

            this.setColWidths(new float[] { 0.4f, 0.4f, 0.2f });

            var header0 = this.newCell(CGridCellType.LABEL, 0, 0);
            var header1 = this.newCell(CGridCellType.LABEL, 1, 0);
            var header2 = this.newCell(CGridCellType.LABEL, 2, 0);
            header0.value = "属性";
            header1.value = "值";
            header2.value = "单位";

            this.loadFromNode();
        }

        public bool editable {
            get => m_editable;
            set {
                m_editable = value;

                if (this.Visible) {
                    for (int row = 0; row < this.RowCount; ++row) {
                        CGridCell cellVal = this.cell(2, row);
                        if (cellVal == null)
                            continue;

                        cellVal.readOnly = !m_editable;
                    }
                }
            }
        }

        public ProfileTree_Profile node {
            get => m_node;
            set {
                if (m_node == value)
                    return;

                if (m_node != null) {
                    this.saveToNode();
                }
                    
                if (value == null) {
                    m_node = null;
                    this.Visible = false;
                    return;
                }

                m_node = value;

                this.loadFromNode();
            }
        }

        public string subType {
            get => m_node != null ? m_node.subType : "";
            set {
                if (m_node == null)
                    return;

                this.saveToNode();
                m_node.subType = value;
                this.loadFromNode();
            }
        }

        public int fmtIdx {
            get {
                if (!(m_node is ProfileTree_DynamicProfile))
                    return -1;

                return (m_node as ProfileTree_DynamicProfile).fmtIdx;
            }

            set {
                if (!(m_node is ProfileTree_DynamicProfile))
                    return;

                this.saveToNode();
                (m_node as ProfileTree_DynamicProfile).fmtIdx = value;
                this.loadFromNode();
            }
        }

        public bool saveToNode() {
            if (m_node == null)
                return false;

            //var dueArr = Native.queryProfileUnitData(m_node.name);
            
            //var props = new DObject();
            //for (int row = 1; row < this.RowCount; ++row) {
            //    CGridCell cellName = this.cell(0, row);
            //    CGridCell cellVal = this.cell(1, row);
            //    string pname = cellVal.attchedData as string;

            //    string unitData = "", unitDisp = "";
            //    if (dueArr != null) {
            //        var dueIdx = dueArr.IndexOf(pname);
            //        if (dueIdx >= 0) {
            //            unitData = dueArr[dueIdx + 1];
            //            unitDisp = Utils.ToStr(Native.options("unit_" + dueArr[dueIdx + 2]));
            //        }
            //    }

            //    var pval = cellVal.value;
            //    if (pval is string && pval as string == "lost")
            //        continue;

            //    if (pval is double && !String.IsNullOrEmpty(unitData) && !String.IsNullOrEmpty(unitDisp))
            //        pval = Unit.Cvt((double)pval, unitDisp, unitData);

            //    props[pname] = pval;
            //}
            //m_node.modify(props);

            return true;
        }

        public void loadFromNode() {
            if (m_node == null) {
                this.RowCount = 1;
                this.Visible = false;
                return;
            }

            var type = m_node.type;
            var subType = m_node.subType;
            var props = m_node.props;
            //var dueArr = Native.queryProfileUnitData(m_node.name);

            int row = 1;
            if (m_node is ProfileTree_DynamicProfile) {
                var fmtNode = m_node as ProfileTree_DynamicProfile;
                var fmts = fmtNode.fmts;
                var fmtIdx = fmtNode.fmtIdx;
                var layout = fmtNode.fmtLayout(fmts[fmtIdx]);
                if (layout.Count == 0) {
                    this.RowCount = 1;
                    this.Visible = false;
                    return;
                }

                this.rowCount = layout.Count + 1; //1 for header

                this.SuspendLayout();
                for (int i = 0; i < layout.Count; ++i) {
                    var slice = layout[i];

                    string[] pnames = null;
                    if (slice.IndexOf('|') < 0)
                        pnames = new string[] { slice };
                    else 
                        pnames = slice.Split('|');

                    foreach (var pname in pnames) {
                        var pshort = m_node.shortName(subType, pname);
                        if (!props.ContainsKey(pname)) {
                            this._initRow(false, row++, pname, pshort, "", "lost");
                            continue;
                        }

                        string unitData = "", unitDisp = "";
                        int enob = 2;
                        //if (dueArr != null) {
                        //    var dueIdx = dueArr.IndexOf(pname);
                        //    if (dueIdx >= 0) {
                        //        unitData = dueArr[dueIdx + 1];
                        //        unitDisp = Utils.ToStr(Native.options("unit_" + dueArr[dueIdx + 2]));
                        //        enob = Utils.ToInt(Native.options("enob_" + dueArr[dueIdx + 2]));
                        //    }
                        //}

                        var pval = props[pname];
                        if (pval is double && !String.IsNullOrEmpty(unitData) && !String.IsNullOrEmpty(unitDisp))
                            pval = Unit.Cvt((double)pval, unitData, unitDisp);

                        this._initRow(m_editable, row++,
                                        pname, pshort,
                                        unitDisp,
                                        pval,
                                        enob);
                    }
                }
                this.ResumeLayout();

            }
            else if (m_node is ProfileTree_Profile) {
                var layout = m_node.tree.layout(type, subType);
                if (layout.Count == 0) {
                    this.RowCount = 1;
                    this.Visible = false;
                    return;
                }

                this.rowCount = layout.Count / 2 + 1; //1 for header

                this.SuspendLayout();
                for (int i = 0; i < layout.Count; i += 2) {
                    var pname = layout[i];
                    var pshort = layout[i + 1];
                    if (!props.ContainsKey(pname)) {
                        this._initRow(false, row++, pname, pshort, "", "lost");
                        continue;
                    }

                    string unitData = "", unitDisp = "";
                    int enob = 2;
                    //if (dueArr != null) {
                    //    var dueIdx = dueArr.IndexOf(pname);
                    //    if (dueIdx >= 0) {
                    //        unitData = dueArr[dueIdx + 1];
                    //        unitDisp = Utils.ToStr(Native.options("unit_" + dueArr[dueIdx + 2]), "");
                    //        enob = Utils.ToInt(("enob_" + dueArr[dueIdx + 2]), 0);
                    //    }
                    //}

                    var pval = props[pname];
                    if (pval is double && !String.IsNullOrEmpty(unitData) && !String.IsNullOrEmpty(unitDisp))
                        pval = Unit.Cvt((double)pval, unitData, unitDisp);

                    this._initRow(m_editable, row++,
                                    pname, pshort,
                                    unitDisp,
                                    pval,
                                    enob);
                }
                this.ResumeLayout();
            }

            this.updateHeight();
            this.Visible = true;
        }

        protected void _initRow(bool editable, int row, string propName, string propShort, string unit, object val, int enob = 2) {
            CGridCell cellName = this.newCell(CGridCellType.LABEL, 0, row);
            CGridCell cellUnit = this.newCell(CGridCellType.LABEL, 2, row);
            CGridCell cellVal = null;

            if (val is double) {
                cellVal = this.newCell(CGridCellType.INPUT_DOUBLE, 1, row);
                (cellVal as CGridCellInputDouble).Enob = enob;
            }
            else if (val is string)
                cellVal = this.newCell(CGridCellType.INPUT_TEXT, 1, row);
            else if (val is int)
                cellVal = this.newCell(CGridCellType.INPUT_INT, 1, row);

            cellName.value = Lang.Local(propName, "profile") + " " + propShort;
            cellName.readOnly = true;

            cellVal.value = val;
            cellVal.attchedData = propName;
            cellVal.readOnly = !editable;

            cellUnit.value = (unit != null) ? unit : "";
            cellUnit.readOnly = true;
        }
    }

    
}
