using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Localization;
using System.Drawing.Printing;

namespace UI
{
    public partial class Form_Test : Form
    {

        private List<long> m_helperIds = new List<long>();

        public Form_Test() {
            InitializeComponent();

            this.Owner = MainForm.instance;
        }

    }
}
