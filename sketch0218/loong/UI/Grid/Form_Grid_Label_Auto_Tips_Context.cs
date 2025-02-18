using System;
using System.Collections.Generic;
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
using IFWrapper;

namespace UI
{
    public partial class Form_Grid_Label_Auto_Tips_Context : UserCtrl.ContextForm
    {
        private static Form_Grid_Label_Auto_Tips_Context m_instance = new Form_Grid_Label_Auto_Tips_Context();
        public static Form_Grid_Label_Auto_Tips_Context instance => m_instance;

        private long m_hideFrame = -1;

        public Form_Grid_Label_Auto_Tips_Context() {
            InitializeComponent();

            this.Owner = MainForm.instance;
            this._initControls();
        }

        private void _initControls() {
            grid_tips.RowCount = 8;
            grid_tips[0, 0].Value = "空";
            grid_tips[1, 0].Value = "+0 +3600 +7200";
            grid_tips[0, 1].Value = "A";
            grid_tips[1, 1].Value = "A B C";
            grid_tips[0, 2].Value = "1";
            grid_tips[1, 2].Value = "1 2 3";
            grid_tips[0, 3].Value = "A1";
            grid_tips[1, 3].Value = "A1 A2 A3";
            grid_tips[0, 4].Value = "A#";
            grid_tips[1, 4].Value = "A+0 A+3600 A+7200";
            grid_tips[0, 5].Value = "A 1 x";
            grid_tips[1, 5].Value = "A1x A2x A3x";
            grid_tips[0, 6].Value = "#[m]";
            grid_tips[1, 6].Value = "+0 +3.6 +7.2";
            grid_tips[0, 7].Value = "有效单位";
            grid_tips[1, 7].Value = "[mm] [cm] [m] [inch] [ft] [ft-in]";
            this.updateTabOrder();
        }

        public void show(Form owner, Rectangle rcBtn) {
            if (this.Visible) 
                return;

            var pos = evalShowPos(Native.screenWorkAreaRelativeToPrimary(this.Handle), rcBtn);
            this.Owner = owner;
            this.Location = pos;
            this.Visible = true;
            this.focus();
        }

        public long hideFrame => m_hideFrame;

        protected override void OnDeactivate(EventArgs e) {
            base.OnDeactivate(e);

            m_hideFrame = Native.STCoreFrame();
            this.hide();
        }

    }
}
