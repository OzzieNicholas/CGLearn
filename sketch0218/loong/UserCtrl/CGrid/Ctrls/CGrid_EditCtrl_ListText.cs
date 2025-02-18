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
    public partial class CGrid_EditCtrl_ListText : Form
    {
        private string[] m_value = null;

        public CGrid_EditCtrl_ListText()
        {
            InitializeComponent();

        }

        public DialogResult show(string[] value)
        {
            lb_main.Items.Clear();

            if (value != null)
            {
                lb_main.Items.AddRange(value);
            }

            return this.ShowDialog();
        }

        public string[] value
        {
            get
            {
                return m_value;
            }
        }

        private void btn_ok_Click(object sender, EventArgs e)
        {
            List<string> items = new List<string>();
            foreach (var s in lb_main.Items)
                items.Add(s as string);

            m_value = items.ToArray();

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            lb_main.Items.Clear();
            this.DialogResult = DialogResult.Cancel;
            this.Close();
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

        private void tb_add_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                string str = tb_add.Text.Trim();
                if (str.Length > 0)
                {
                    lb_main.Items.Add(str);
                    lb_main.SelectedIndex = lb_main.Items.Count - 1;
                    tb_add.Text = "";
                }
            }
        }
    }
}
