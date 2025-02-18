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
    public partial class Form_MsgBox : Form
    {
        public Form_MsgBox()
        {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls()
        {
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
        }

        public void showModal(IWin32Window owner, string caption, string msg, bool delayInvoke)
        {
            Size msgSize = TextRenderer.MeasureText(msg, lb_message.Font);
            Rectangle scrClient = this.RectangleToScreen(this.ClientRectangle);
            int border = (this.Width - scrClient.Width) / 2;
            int titleHeight = scrClient.Top - this.Top;

            this.Width = Math.Max(200, msgSize.Width + 20) + border * 2;
            this.Height = Math.Max(100, msgSize.Height + 20) + titleHeight + border;

            scrClient = this.RectangleToScreen(this.ClientRectangle);

            this.Text = caption;
            this.lb_message.Text = msg;
            this.lb_message.Location = new Point((scrClient.Width - msgSize.Width) / 2, (scrClient.Height - msgSize.Height) / 2);

            if (delayInvoke) {
                this.BeginInvoke(new Action(() => {
                    if (this.IsDisposed)
                        return;

                    this.ShowDialog(owner);
                }));
            }
            else {
                this.ShowDialog(owner);
            }
        }

        public void closeModal()
        {
            if (!this.IsDisposed)
                this.Close();
        }
    }
}
