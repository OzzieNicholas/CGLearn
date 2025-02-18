using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using LoongCAD.Localization;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public class ShortCutGrid : UserCtrl.GridView
    {
        private bool m_editable = true;
        private string m_group = "";
        private string m_filter = "*";
        private string m_filterRegExpr = Utils.ToRegExpr("*");
        private List<Tuple<string, uint>> m_builtInScs = new List<Tuple<string, uint>>();
        private Dictionary<string, uint> m_fu2sc = new Dictionary<string, uint>();
        private Dictionary<string, ShortCutCell> m_valCells = new Dictionary<string, ShortCutCell>();
        

        public ShortCutGrid() {
            this.ColumnCount = 3;
            this.RowCount = 1;
            this.Dock = DockStyle.Fill;
            this.ColumnHeadersVisible = false;
            this.RowHeadersVisible = false;
            this.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            this.AllowUserToResizeRows = false;
            this.AllowUserToResizeColumns = true;
            this.BorderStyle = BorderStyle.FixedSingle;
            this.TabStop = false;
            this.EditMode = DataGridViewEditMode.EditProgrammatically;

            this.Columns[0].FillWeight = 15;
            this.Columns[1].FillWeight = 30;
            this.Columns[2].FillWeight = 30;
        }

        public void reset() {
            m_builtInScs.Clear();
            m_fu2sc.Clear();

            if (Native.queryBuiltInShortCuts(out string[] bfus, out uint[] bscs)) {
                for (int i = 0; i < bfus.Length; ++i)
                    m_builtInScs.Add(new Tuple<string, uint>(bfus[i], bscs[i]));
            }

            if (Native.queryShortCuts(out string[] fus, out uint[] scs)) {
                for (int i = 0; i < fus.Length; ++i) 
                    m_fu2sc[fus[i]] = scs[i];
            }
        }

        public bool editable {
            get => m_editable;
            set {
                m_editable = value;

                for (int row = 0; row < this.RowCount; ++row) 
                    this[1, row].ReadOnly = !m_editable;
            }
        }

        public string group {
            get => m_group;
            set {
                if (m_group == value)
                    return;

                m_group = value;
            }
        }

        public string filter {
            get => m_filter;
            set {
                string str = (value != null) ? value.Trim() : null;
                if (str == null || str.Length == 0)
                    str = "*";

                if (m_filter == str)
                    return;

                m_filter = str.Trim();

                string softFilter = m_filter;
                if (!softFilter.StartsWith("*"))
                    softFilter = "*" + softFilter;
                if (!softFilter.EndsWith("*"))
                    softFilter = softFilter + "*";

                m_filterRegExpr = Utils.ToRegExpr(softFilter);
            }
        }

        public void saveUserCfg() {
            if (m_fu2sc.Count == 0)
                return;
                
            Native.setShortCuts(m_fu2sc.Keys.ToArray(), m_fu2sc.Values.ToArray());
            FuncUnit.updateShortCuts();
            LayoutManager.instance.refreshItems();
        }

        public void export(string file) {
            Native.exportShortCuts(file, m_fu2sc.Keys.ToArray(), m_fu2sc.Values.ToArray());
        }

        public void import(string file) {
            Native.importShortCuts(file, out string[] fus, out uint[] scs);

            for (int i = 0; i < fus.Length; ++i) {
                m_fu2sc[fus[i]] = scs[i];
            }
        }

        public DataGridViewCell groupCell(int row) {
            DataGridViewCell cell = this[0, row];
            cell.Style.SelectionBackColor = this.DefaultCellStyle.BackColor;
            cell.Style.SelectionForeColor = this.DefaultCellStyle.ForeColor;
            return cell;
        }

        public DataGridViewCell tipCell(int row) {
            DataGridViewCell cell = this[1, row];
            cell.Style.SelectionBackColor = this.DefaultCellStyle.BackColor;
            cell.Style.SelectionForeColor = this.DefaultCellStyle.ForeColor;
            return cell;
        }

        public ShortCutCell valCell(int row) {
            if (!(this[2, row] is ShortCutCell))
                this[2, row] = new ShortCutCell();

            return this[2, row] as ShortCutCell;
        }

        public void clearRows() {
            this.RowCount = 1;
            m_valCells.Clear();

            this.tipCell(0).Value = "";
            this.tipCell(0).ReadOnly = true;

            this.valCell(0).Value = "";
            this.valCell(0).ReadOnly = true;
        }

        public void refresh() {
            this.SuspendLayout();

            this.clearRows();

            var funcs = FuncUnit.units;
            List<FuncUnit> funcList = new List<FuncUnit>();
            if (m_filter == null || m_filter.Length == 0 || m_filter == "*") {
                foreach (var it in funcs) {
                    FuncUnit unit = it.Value;
                    if (unit.userUnit) {
                        if (m_group.Length == 0 || unit.group == m_group) //filter by group
                            funcList.Add(unit);
                    }
                }
            }
            else {
                foreach (var it in funcs) {
                    FuncUnit unit = it.Value;
                    if (m_group.Length == 0 || unit.group == m_group) {//filter by group
                        if (unit.userUnit && Utils.RegExprMatch(unit.tip, m_filterRegExpr)) //filter by filter text
                            funcList.Add(unit);
                    }
                }
            }

            if (funcList.Count > 0) {
                this.RowCount = funcList.Count;
                for (int row = 0; row < funcList.Count; ++row) {
                    FuncUnit fu = funcList[row];
                    var cellGroup = this.groupCell(row);
                    var cellName = this.tipCell(row);
                    ShortCutCell cellVal = this.valCell(row);

                    cellGroup.Value = Lang.Local(fu.group, "func_unit");
                    cellGroup.ReadOnly = true;

                    cellName.Value = fu.tip;
                    cellName.ReadOnly = true;

                    cellVal.fu = fu;
                    cellVal.shortCut = m_fu2sc.ContainsKey(fu.name) ? m_fu2sc[fu.name] : 0u;
                    cellVal.ReadOnly = false;

                    m_valCells[fu.name] = cellVal;
                }
            }

            this.ResumeLayout();
        }

        public bool updateCell(ShortCutCell cell, uint sc) {
            if (cell == null || cell.shortCut == sc)
                return false;

            var fu = cell.fu;
            if (sc != 0) {
                int nkeys = ((sc & 0xffffff) <= 0xff) ? 8 : ((sc & 0xffffff) <= 0xffff) ? 16 : 24;
                foreach (var it in m_builtInScs) {
                    if (it.Item1 == fu.name
                        || (it.Item2 & 0xffffff) == 0)
                        continue;

                    uint dsc = it.Item2;

                    //检查控制键是否相同
                    if ((dsc & 0xf000000) != (sc & 0xf000000))
                        continue;

                    //检查主键组合是否彼此包含
                    int ndkeys = ((dsc & 0xffffff) <= 0xff) ? 8 : ((dsc & 0xffffff) <= 0xffff) ? 16 : 24;

                    if (nkeys == ndkeys) {
                        if ((sc & 0xffffff) != (dsc & 0xffffff))
                            continue;
                    }
                    else if (nkeys < ndkeys) {
                        if ((sc & 0xffffff) != ((dsc & 0xffffff) >> (ndkeys - nkeys)))
                            continue;
                    }
                    else {
                        if (((sc & 0xffffff) >> (nkeys - ndkeys)) != (dsc & 0xffffff))
                            continue;
                    }

                    //检查cellType
                    var dcellType = (Define.LayoutCellType)(dsc >> 28);
                    if (fu.cellType != dcellType 
                        && fu.cellType != Define.LayoutCellType.CELL_NONE
                        && dcellType != Define.LayoutCellType.CELL_NONE)
                        continue;

                    MessageBox.Show("[" + FuncUnit.shortCut2Str(sc) + "] 已被系统保留,无法用做快捷键", "提示", MessageBoxButtons.OK);
                    return false;
                }

                List<FuncUnit> conflicts = new List<FuncUnit>();
                foreach (var it in m_fu2sc) {
                    if (it.Key == fu.name 
                        || (it.Value & 0xffffff) == 0)
                        continue;

                    uint dsc = it.Value;

                    //检查控制键是否相同
                    if ((dsc & 0xf000000) != (sc & 0xf000000))
                        continue;

                    //检查主键组合是否彼此包含
                    int ndkeys = ((dsc & 0xffffff) <= 0xff) ? 8 : ((dsc & 0xffffff) <= 0xffff) ? 16 : 24;

                    if (nkeys == ndkeys) {
                        if ((sc & 0xffffff) != (dsc & 0xffffff))
                            continue;
                    }
                    else if (nkeys < ndkeys) {
                        if ((sc & 0xffffff) != ((dsc & 0xffffff) >> (ndkeys - nkeys)))
                            continue;
                    }
                    else {
                        if (((sc & 0xffffff) >> (nkeys - ndkeys)) != (dsc & 0xffffff))
                            continue;
                    }

                    var dfu = FuncUnit.get(it.Key);
                    if (dfu == null)
                        continue;

                    //检查cellType
                    if (fu.cellType != dfu.cellType
                        && fu.cellType != Define.LayoutCellType.CELL_NONE
                        && dfu.cellType != Define.LayoutCellType.CELL_NONE)
                        continue;

                    if (!dfu.userUnit) {
                        MessageBox.Show("[" + FuncUnit.shortCut2Str(sc) + "] 已被系统保留,无法用做快捷键", "提示", MessageBoxButtons.OK);
                        return false;
                    }

                    conflicts.Add(dfu);
                }

                if (conflicts.Count > 0) {
                    Form_ShortCutConflict dlg = new Form_ShortCutConflict();
                    if (dlg.show(sc, conflicts) == DialogResult.Cancel)
                        return false;

                    foreach (var dfu in conflicts) {
                        m_fu2sc[dfu.name] = 0;
                        if (m_valCells.ContainsKey(dfu.name))
                            m_valCells[dfu.name].shortCut = 0;
                    }
                }
            }

            m_fu2sc[fu.name] = sc;
            cell.shortCut = sc;
            return true;
        }
    }
}
