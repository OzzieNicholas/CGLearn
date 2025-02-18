// Copyright 2007 Herre Kuijpers - <herre@xs4all.nl>
//
// This source file(s) may be redistributed, altered and customized
// by any means PROVIDING the authors name and all copyright
// notices remain intact.
// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED. USE IT AT YOUR OWN RISK. THE AUTHOR ACCEPTS NO
// LIABILITY FOR ANY DATA DAMAGE/LOSS THAT THIS PRODUCT MAY CAUSE.
//-----------------------------------------------------------------------
using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using System.Runtime.InteropServices;
using UserCtrl;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{

    public class Floaty : Form
    {
        private CustomToolBar m_toolBar;

        public Floaty(CustomToolBar toolBar) {
            m_toolBar = toolBar;

            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.AutoValidate = System.Windows.Forms.AutoValidate.Disable;
            this.CausesValidation = false;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.TopMost = true;
            this.ShowIcon = false;
            this.Name = "Floaty";
        }

        public CustomToolBar toolBar => m_toolBar;

        public void show(int x = -1, int y = -1)
        {
            if (!LayoutManager.instance.floatyBars.Contains(m_toolBar))
                LayoutManager.instance.floatyBars.Add(m_toolBar);
            if (!toolBar.provsion.Provisioned)
                return;
            this.Owner = MainForm.instance;
            m_toolBar.LayoutStyle = ToolStripLayoutStyle.HorizontalStackWithOverflow;
            m_toolBar.Location = Point.Empty;
            this.Controls.Add(m_toolBar);

            if (LayoutManager.instance.builtInBars.ContainsKey(m_toolBar.BarName)) {
                this.ControlBox = false;
            }

            BarNode barNode = toolBar.provsion;
            if (barNode == null)
            {
                Debug.WriteLine("Floaty: related toolbar doesn't have provisioned node.");
                return;
            }
            if (barNode.Location != CustomBarsProvision.BAR_LOCATION_FLOATY)
                barNode.changeLocation(CustomBarsProvision.BAR_LOCATION_FLOATY, 0);
            syncLocationToProvision();
            barNode.Provisioned = true;
            this.Text = barNode.Tip;

            var rcWin = this.Bounds;
            var rcClient = this.RectangleToScreen(this.ClientRectangle);
            int xoff = rcClient.X - rcWin.X;
            int yoff = rcClient.Y - rcWin.Y;
            if (x == -1 || y == -1) {
                x = Cursor.Position.X - xoff;
                y = Cursor.Position.Y - yoff;
            } else
            {
                if (!checkBounds(x, y))
                {
                    var formRect = MainForm.instance.ClientRectangle;
                    x = formRect.X + 260;
                    y = formRect.Y + 160;
                }
            }

            Size sz = m_toolBar.PreferredSize;
            this.SetBounds(x, y, sz.Width + xoff * 2, sz.Height + yoff + DPI.Scale(2));
            this.Visible = true;
            BringToFront();
        }

        public void syncLocationToProvision()
        {
            toolBar.provsion.x = Bounds.X;
            toolBar.provsion.y = Bounds.Y;
        }

        public void hide() {
            this.Owner = null;
            syncLocationToProvision();
            LayoutManager.instance.floatyBars.Remove(m_toolBar);
            this.Hide();
        }

        public void closeEye()
        {
            syncLocationToProvision();
            this.toolBar.provsion.Provisioned = false;
            this.Hide();
        }

        public bool checkBounds(int x, int y)
        {
            foreach (Screen scs in Screen.AllScreens) {
                if (scs.Bounds.Contains(x, y))
                    return true;
            }
            return false;
        }

        private void InitializeComponent() {
            this.SuspendLayout();
            // 
            // Floaty
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(284, 261);
            this.Name = "Floaty";
            this.ResumeLayout(false);

        }
        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                closeEye();
                e.Cancel = true;
            }
        }
    }

}
