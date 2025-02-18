using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using System.Windows.Forms;
using System.ComponentModel;

namespace UI
{
    [ToolboxItem(false)]
    public class CusToolBarGridCell : DataGridViewTextBoxCell
    {
        private string m_funcUnitName = null;
        public CusToolBarGridCell(string name) : base()
        {
            m_funcUnitName = name;
            FuncUnit fu = FuncUnit.units[m_funcUnitName];
            if (fu != null || fu.tip != null)
                this.Value = fu.tip;
            else
                this.Value = "";
        }

        public override Type ValueType => typeof(string);
        public string FuncUnitName => m_funcUnitName;
        public override object DefaultNewRowValue => 0.0;

        public string getProvisionedBars()
        {
            string ret = "";
            if (!CustomBarsProvision.instance.FuncUnitToBarsDict.ContainsKey(FuncUnitName)) 
                return ret;
            List<BarNode> list = CustomBarsProvision.instance.FuncUnitToBarsDict[FuncUnitName];
            if (list.Count > 0)
            {
                if (list[0].Tip == null) return ret;
                ret = list[0].Tip;

                Dictionary<string, bool> barNameDict = new Dictionary<string, bool>();
                barNameDict[list[0].Tip] = true;

                for (int i = 1; i < list.Count; i++)
                {
                    if (barNameDict.ContainsKey(list[i].Tip))
                        continue;

                    barNameDict[list[i].Tip] = true;
                    ret += ("，" + list[i].Tip);
                }
            }
            return ret;
        }

        public override object ParseFormattedValue(object formattedValue, DataGridViewCellStyle cellStyle, TypeConverter formattedValueTypeConverter, TypeConverter valueTypeConverter)
        {
            if (formattedValue == null)
                return "";

            if (formattedValue.GetType() != typeof(string))
                return "";

            return formattedValue;
        }
    }

    [ToolboxItem(false)]
    public class CusToolBarGrid : DataGridView
    {
        private string m_filter = "*";
        private string m_filterRegExpr = Utils.ToRegExpr("*");
        private string m_group = "ALL";
        private int m_panWidth = 0;
        private Form_CusToolBar m_form = null;
        private Dictionary<string, List<ButtonNode>> m_buttonGroupsDict = null;
        private Dictionary<string, CusToolBarGridCell> m_btnCellMapping = null;
        public CusToolBarGrid(Form_CusToolBar form, int panWidth) {
            m_form = form;
            m_panWidth = panWidth - 19;
            this.ColumnCount = 3;
            this.RowCount = 1;
            this.Dock = DockStyle.Fill;
            this.ColumnHeadersVisible = false;
            this.RowHeadersVisible = false;
            this.ColumnHeadersVisible = true;
            this.Columns[0].HeaderText = "功能名称";
            this.Columns[1].HeaderText = "分组";
            this.Columns[2].HeaderText = "已添加至";
            this.Columns[0].FillWeight = 44;
            this.Columns[1].FillWeight = 28;
            this.Columns[2].FillWeight = 28;

            DataGridViewCellStyle cellStyle = new DataGridViewCellStyle();
            cellStyle.Padding = new Padding(3, 0, 7, 0);
            this.DefaultCellStyle = cellStyle;

            this.AllowUserToResizeRows = false;
            this.AllowUserToResizeColumns = true;
            this.AllowUserToAddRows = false;
            this.AllowUserToDeleteRows = false;
            this.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            this.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            this.BorderStyle = BorderStyle.FixedSingle;
            this.CellBorderStyle = DataGridViewCellBorderStyle.SingleVertical;
            this.ReadOnly = true;
            this.TabStop = false;

            m_btnCellMapping = new Dictionary<string, CusToolBarGridCell>();
            m_buttonGroupsDict = CustomBarsProvision.instance.ButtonGroupsDict;
            refreshItems();
        }

        public void refreshItems() {
            this.SuspendLayout();
            this.RowCount = 0;
            int pos = 0;
            bool pass = false;

            if (m_group == "ALL") {
                this.RowCount = FuncUnit.units.Keys.Count;
                pass = true;
            }

            foreach (string key in m_buttonGroupsDict.Keys) {
                if (pass || key == m_group) {
                    List<ButtonNode> nodes = new List<ButtonNode>();
                    foreach (ButtonNode node in m_buttonGroupsDict[key]) {
                        if (Utils.RegExprMatch(node.Tip, m_filterRegExpr)) {
                            nodes.Add(node);
                        }
                    }
                    if (!pass)
                        this.RowCount = nodes.Count;
                    foreach (ButtonNode node in nodes) {
                        CusToolBarGridCell cell = new CusToolBarGridCell(node.Name);
                        this[0, pos] = cell;
                        this[0, pos].ReadOnly = true;
                        this[1, pos].Value = node.GroupTip;
                        this[1, pos].ReadOnly = true;
                        this[2, pos].Value = cell.getProvisionedBars();
                        this[2, pos].ReadOnly = true;
                        m_btnCellMapping[node.Name] = cell;
                        //System.Console.WriteLine("no at (" + this[0, pos].ToString() + ")");
                        pos++;
                    }
                    if (!pass)
                        break;
                }
            }
            this.ResumeLayout();
        }

        public string group {
            get => m_group;
            set {
                m_group = value;
                this.refreshItems();
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

                m_filter = str;
                m_filterRegExpr = Utils.ToRegExpr(m_filter);
                this.refreshItems();
            }
        }

        private int getSelectedRowId() {
            foreach (DataGridViewRow r in Rows) {
                if (r.Selected == true)
                    return r.Index;
            }
            return -1;
        }

        private void moveToNextRow() {
            int prevId = getSelectedRowId() + 1;
            if (prevId < Rows.Count) {
                Rows[prevId].Selected = true;
                Rows[prevId - 1].Selected = false;
            }
        }

        public void addButtonNodeInSelected(ButtonNode node) {
            if (SelectedCells[0] == null) {
                moveToNextRow();
                return;
            }
            CusToolBarGridCell cell0 = SelectedCells[0] as CusToolBarGridCell;
            DataGridViewCell cell2 = SelectedCells[2];
            cell2.Value = cell0.getProvisionedBars();
            moveToNextRow();
        }
        public void removeButtonNode(ButtonNode node) {
            if (!m_btnCellMapping.ContainsKey(node.Name))
                return;
            CusToolBarGridCell cell0 = m_btnCellMapping[node.Name];
            if (cell0.RowIndex < 0)
                return;
            DataGridViewCell cell2 = Rows[cell0.RowIndex].Cells[2];
            cell2.Value = cell0.getProvisionedBars();
        }

        public string getSelectedButtonNode() {
            if (SelectedCells[0] == null || SelectedCells[0].GetType() != typeof(CusToolBarGridCell))
                return null;
            return (SelectedCells[0] as CusToolBarGridCell).FuncUnitName;
        }
    }
}
