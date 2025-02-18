using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserCtrl
{
    [ToolboxItem(false)]
    public partial class DGrid_EditCtrl_TextList : Form
    {
        private static DGrid_EditCtrl_TextList m_instance = null;
        private DGridCellTextList m_cell = null;

        public DGrid_EditCtrl_TextList()
        {
            InitializeComponent();
        }

        public static DGrid_EditCtrl_TextList instance
        {
            get
            {
                if (m_instance == null)
                    m_instance = new DGrid_EditCtrl_TextList();

                return m_instance;
            }
        }

        public void show(DGridCellTextList cell)
        {
            m_cell = cell;

            if (m_cell != null)
            {
                this.Data = (string[])m_cell.data;
            }

            this.Visible = true;
        }

        private void Form_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                lb_main.Items.Clear();
                e.Cancel = true;
                this.Visible = false;
            }
        }

        public string[] Data
        {
            get
            {
                if (lb_main.Items.Count == 0)
                    return null;

                string[] arr = new string[lb_main.Items.Count];
                for (int i = 0; i < lb_main.Items.Count; ++i)
                {
                    arr[i] = (string)lb_main.Items[i];
                }

                return arr;
            }

            set
            {
                lb_main.Items.Clear();

                if (value != null)
                {
                    for (int i = 0; i < value.Length; ++i)
                    {
                        lb_main.Items.Add(value[i]);
                    }
                }

                if (value == null || value.Length == 0)
                    lb_main.SelectedIndex = -1;
                else
                    lb_main.SelectedIndex = 0;
            }
        }

        private void btn_ok_Click(object sender, EventArgs e)
        {
            if (m_cell != null)
            {
                m_cell.data = this.Data;
            }

            lb_main.Items.Clear();
            m_cell.grid.onCellValidated(m_cell.ColumnIndex, m_cell.RowIndex);
            this.Visible = false;
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            lb_main.Items.Clear();
            this.Visible = false;
        }

        private void btn_del_Click(object sender, EventArgs e)
        {
            if (lb_main.SelectedItem != null)
            {
                int idx = lb_main.SelectedIndex;
                lb_main.Items.Remove(lb_main.SelectedItem);

                if (idx < lb_main.Items.Count)
                    lb_main.SelectedIndex = idx;
                else
                    lb_main.SelectedIndex = lb_main.Items.Count - 1;
            }
                
        }

        private void btn_up_Click(object sender, EventArgs e)
        {
            if (lb_main.SelectedItem != null && lb_main.SelectedIndex > 0)
            {
                string item = (string)lb_main.SelectedItem;
                int idx = lb_main.SelectedIndex;
                lb_main.Items.Remove(lb_main.SelectedItem);
                lb_main.Items.Insert(idx - 1, item);
                lb_main.SelectedIndex = idx - 1;
            }
        }

        private void btn_down_Click(object sender, EventArgs e)
        {
            if (lb_main.SelectedItem != null && lb_main.SelectedIndex < lb_main.Items.Count - 1)
            {
                string item = (string)lb_main.SelectedItem;
                int idx = lb_main.SelectedIndex;
                lb_main.Items.Remove(lb_main.SelectedItem);
                lb_main.Items.Insert(idx + 1, item);
                lb_main.SelectedIndex = idx + 1;
            }
        }

        private void btn_add_Click(object sender, EventArgs e)
        {
            string str = tb_add.Text.Trim();
            if (str.Length > 0)
            {
                lb_main.Items.Add(str);
                lb_main.SelectedIndex = lb_main.Items.Count - 1;
            }
        }
    }
}
