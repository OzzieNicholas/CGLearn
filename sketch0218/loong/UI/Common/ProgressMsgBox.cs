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
using IFWrapper;

namespace UI
{
    public partial class ProgressMsgBox : Form
    {
        static int s_counter = 0;
        private int m_id = s_counter++;
        private static Dictionary<string, ProgressMsgBox> m_instances = new Dictionary<string, ProgressMsgBox>();

        public ProgressMsgBox() {
            InitializeComponent();

            this._initControls();
        }

        private void _initControls() {
            this.TopMost = true;

            pan_progress.Width = 0;
        }

        public static ProgressMsgBox instance(string name) {
            ProgressMsgBox inst = null;
            m_instances.TryGetValue(name, out inst);
            return inst;
        }

        protected override void OnResize(EventArgs e) {
            base.OnResize(e);

            pan_progress.Location = new Point(0, this.Height - pan_progress.Height);
        }

        protected override void OnFormClosed(FormClosedEventArgs e) {
            m_instances.Remove(this.Name);
            
            base.OnFormClosed(e);
        }

        public int progress {
            get => pan_progress.Width * 100 / this.Width;
            set {
                pan_progress.Width = Math.Max(0, Math.Min(this.Width, value * this.Width / 100));
                pan_progress.Refresh();
            }
        }

        public string message {
            get => lb_message.Text;
            set {
                if (lb_message.Text == value)
                    return;

                lb_message.Text = value;
                lb_message.Refresh();
            }
        }

        private void _setup(string msg) {
            Size msgSize = TextRenderer.MeasureText(msg, this.lb_message.Font);
            
            this.Size = new Size(Math.Max(DPI.Scale(450), msgSize.Width + 24), Math.Max(DPI.Scale(60), msgSize.Height + 20 + pan_progress.Height));
            lb_message.Text = msg;
        }

        public static ProgressMsgBox show(string name, string msg, Form owner = null) {
            if (owner == null)
                owner = MainForm.instance;

            ProgressMsgBox dlg = new ProgressMsgBox();
            m_instances.Add(name, dlg);

            dlg.Name = name;
            dlg._setup(msg);
            dlg.progress = 0;
            Native.setProgress(name, 0.0f);

            Point pos = new Point(owner.Width / 2 - dlg.Width / 2, owner.Height / 2 - dlg.Height / 2);
            dlg.Location = owner.PointToScreen(pos);
            dlg.Show(owner);
            dlg.Refresh();
            return dlg;
        }
    }
}
