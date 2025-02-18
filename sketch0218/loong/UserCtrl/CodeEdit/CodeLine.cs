using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using LoongCAD.Util;

namespace UserCtrl
{
    public class CodeLine : RichTextBox
    {
        

        private CodeInput m_code = null;

        public CodeLine()
        {
            this.AutoSize = false;
            this.Multiline = true;
            this.ScrollBars = RichTextBoxScrollBars.None;
            this.AcceptsTab = false;
            this.Font = new Font(new FontFamily("Courier New"), this.Font.Size);
            this.BorderStyle = BorderStyle.None;
            this.ReadOnly = true;
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
        }

        public void setCodeInput(CodeInput code)
        {
            m_code = code;
            m_code.onLineChanged = _onCodeLineChanged;
            m_code.VScroll += code_VScroll;
        }

        public void dispose()
        {
            this.Dispose();
        }

        private void _onCodeLineChanged()
        {
            if (m_code == null)
                return;

            string[] codes = m_code.text.Split('\n');

            string sline = "";
            for (int i = 0; i < codes.Length; ++i)
                sline += " " + (i + 1) + "                 " + codes[i] + "\r\n";

            this.Text = sline;
            this.scrollPosV = m_code.scrollPosV;
        }

        private void code_VScroll(object sender, EventArgs e)
        {
            this.scrollPosV = m_code.scrollPosV;
        }

        public int scrollPosV {
            get {
                return Win32.GetScrollPos(this.Handle, Win32.SB_VERT);
            }

            set {
                Point pt = new Point(0, value);
                Win32.SendMessage(this.Handle, (int)Win32.EM_SETSCROLLPOS, IntPtr.Zero, ref pt);
            }
        }
    }
}
