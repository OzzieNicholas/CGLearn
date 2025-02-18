using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    public partial class LookUpInput : UserControl, ITabOrderCtrl
    {
        protected string m_tabGroup = "";
        protected string m_waterMark = "";
        public Action<LookUpInput, string> onLookUp = null;

        public LookUpInput() {
            InitializeComponent();

            tb.TextChanged += Tb_TextChanged;
            tb.KeyDown += Tb_KeyDown;
            tb.KeyPress += Tb_KeyPress;
            img.MouseClick += Img_MouseClick;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        [DefaultValue(typeof(Font), "宋体, 13pt")]
        public new Font Font {
            get => tb.Font;
            set => tb.Font = value;
        }

        [DefaultValue(24)]
        public int ButtonWidth {
            get => img.Width;
            set {
                img.Width = value;
                this._updateLayout();
            }
        }

        [DefaultValue("")]
        public string WaterMark {
            get => m_waterMark;
            set {
                m_waterMark = value;
                if (String.IsNullOrEmpty(m_waterMark))
                    Win32.SendMessage(tb.Handle, Win32.EM_SETCUEBANNER, 0, "");
                else
                    Win32.SendMessage(tb.Handle, Win32.EM_SETCUEBANNER, 1, m_waterMark);
            }
        }

        [DefaultValue("")]
        public new string Text {
            get => tb.Text;
            set {
                tb.Text = value;
                this._updateImg();
            }
        }

        protected override void OnResize(EventArgs e) {
            base.OnResize(e);

            this._updateLayout();            
        }

        protected void _updateLayout() {
            img.Location = new Point(this.Width - img.Width - 2, Math.Max((this.Height - img.Height) / 2, 0));

            tb.Location = new Point(5, Math.Max((this.Height - tb.Height) / 2, 0));
            tb.Width = Math.Max(img.Location.X - 1 - 5, 2);
        }

        protected void _updateImg() {
            if (String.IsNullOrEmpty(tb.Text))
                img.BackgroundImage = Properties.Resources.input_lookup;
            else
                img.BackgroundImage = Properties.Resources.input_lookup_clear;
        }

        private void Tb_TextChanged(object sender, EventArgs e) {
            this._updateImg();
        }

        private void Tb_KeyDown(object sender, KeyEventArgs e) {
            if (e.KeyCode == Keys.Enter) {
                if (onLookUp != null)
                    onLookUp(this, tb.Text.Trim());

                e.SuppressKeyPress = true; //disable beep sound
            }
        }

        private void Tb_KeyPress(object sender, KeyPressEventArgs e) {
            var key = (Keys)e.KeyChar;
            if (key == Keys.Enter
                || key == Keys.Escape
                || key == Keys.Tab) //prevent beep sound
                e.Handled = true;
        }

        private void Img_MouseClick(object sender, MouseEventArgs e) {
            if (String.IsNullOrEmpty(tb.Text))
                return;

            tb.Text = "";

            if (onLookUp != null)
                onLookUp(this, "");
        }
    }
}
