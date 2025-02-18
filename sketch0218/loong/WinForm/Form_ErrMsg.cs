using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WinForm
{
    public partial class Form_ErrMsg : Form
    {
        public Form_ErrMsg() {
            InitializeComponent();
        }

        public string msg {
            get => tb_msg.Text;
            set => tb_msg.Text = value;
        }

        private void btn_ok_Click(object sender, EventArgs e) {
            this.Close();
        }
    }
}
