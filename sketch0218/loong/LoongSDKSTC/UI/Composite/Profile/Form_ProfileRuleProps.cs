using System;
using System.Collections.Generic;
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


namespace LoongCAD.UI
{
    partial class Form_ProfileRuleProps : ParamFormBase
    {
        private DataGridView m_targetTypesGrid = null;
        private string[] m_profileTypes = { "All", "I", "L", "Z", "U", "Plate", "Circular", "CircularHollow", "RectHollow", "C", "T", "WeldedBox", "WQ", "CC", "SPD", "Sphere" };

        public Form_ProfileRuleProps() {
            InitializeComponent();

            this.DisposeOnClose = true;

            this._initControls();
        }

        private void _initControls() {
            m_targetTypesGrid = new DataGridView();
            m_targetTypesGrid.Dock = DockStyle.Fill;
            m_targetTypesGrid.RowHeadersVisible = false;
            m_targetTypesGrid.ColumnHeadersVisible = false;
            m_targetTypesGrid.AllowUserToResizeColumns = true;
            m_targetTypesGrid.AllowUserToResizeRows = false;
            m_targetTypesGrid.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            m_targetTypesGrid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            m_targetTypesGrid.MultiSelect = false;
            m_targetTypesGrid.DefaultCellStyle.SelectionBackColor = Color.FromArgb(200, 200, 200);
            m_targetTypesGrid.DefaultCellStyle.SelectionForeColor = m_targetTypesGrid.DefaultCellStyle.ForeColor;

            DataGridViewCheckBoxColumn col0 = new DataGridViewCheckBoxColumn();
            col0.ReadOnly = false;
            col0.FalseValue = false;
            col0.TrueValue = true;
            col0.FillWeight = 30;
            m_targetTypesGrid.Columns.Add(col0);

            DataGridViewTextBoxColumn col1 = new DataGridViewTextBoxColumn();
            col1.ReadOnly = true;
            col1.FillWeight = 70;
            m_targetTypesGrid.Columns.Add(col1);

            m_targetTypesGrid.RowCount = m_profileTypes.Length;
            pan_target_types.Controls.Add(m_targetTypesGrid);

            for (int row = 0; row < m_profileTypes.Length; ++row) {
                var cellCheck = m_targetTypesGrid[0, row];
                var cellVal = m_targetTypesGrid[1, row];
                cellCheck.Value = false;
                cellVal.Value = m_profileTypes[row];
            }
        }

        public DialogResult show(DObject props) {
            this.SetParameters(props);
            return this.ShowDialog();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        public DObject Props {
            get => this.GetParameters();
        }

        public override void SetParameters(DObject paras, Control root = null)
        {
            base.SetParameters(paras, root);

            if (paras.ContainsKey("profile_types")) {
                List<string> targetTypes = new List<string>(paras["profile_types"] as string[]);
                if (targetTypes.Count == 0) {
                    m_targetTypesGrid[0, 0].Value = true;
                    for (int row = 1; row < m_profileTypes.Length; ++row) {
                        m_targetTypesGrid[0, row].Value = false;
                    }
                } else {
                    for (int row = 0; row < m_profileTypes.Length; ++row) {
                        string type = m_profileTypes[row];
                        m_targetTypesGrid[0, row].Value = (targetTypes.IndexOf(type) >= 0);
                    }
                }
            } else {
                m_targetTypesGrid[0, 0].Value = true;
                for (int row = 1; row < m_profileTypes.Length; ++row) {
                    m_targetTypesGrid[0, row].Value = false;
                }
            }
        }

        public override DObject GetParameters(Control root = null, List<Control> exclude = null, bool check = true)
        {
            DObject retParams = base.GetParameters(root, exclude, check);

            //target types
            List<string> targetTypes = new List<string>();
            if ((bool)m_targetTypesGrid[0, 0].Value == false) {
                for (int row = 1; row < m_profileTypes.Length; ++row) {
                    var cell = m_targetTypesGrid[0, row];
                    if ((bool)cell.Value)
                        targetTypes.Add(m_profileTypes[row]);
                }
            }

            retParams["profile_types"] = targetTypes.ToArray();
            return retParams;
        }

    }
}
