using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using System.Threading;


namespace UI
{
    public partial class Form_AskOverlappedObjs : Form
    {
        DObject m_props = new DObject();
        string m_result = "";

        public Form_AskOverlappedObjs() {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls() {
            
        }

        public string result => m_result;

        private void btn_keep_Click(object sender, EventArgs e) {
            m_result = "keep";
            this.Close();
        }

        private void btn_replace_Click(object sender, EventArgs e) {
            m_result = "replace";
            this.Close();
        }

        private void btn_cancel_Click(object sender, EventArgs e) {
            m_result = "cancel";
            this.Close();
        }
    }
}
