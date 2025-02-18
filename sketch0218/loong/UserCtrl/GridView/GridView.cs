using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Data;
using System.Drawing;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class GridView : DataGridView, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";

        public GridView() {
            this.DefaultCellStyle.SelectionBackColor = Color.FromArgb(200, 200, 200);
            this.DefaultCellStyle.SelectionForeColor = this.DefaultCellStyle.ForeColor;
        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        public DataRow dataRow(DataGridViewRow row) {
            return (row.DataBoundItem as DataRowView).Row;
        }

        public DataRow dataRow(int viewRowIdx) {
            var row = this.Rows[viewRowIdx];
            return (row.DataBoundItem as DataRowView).Row;
        }
    }
}
