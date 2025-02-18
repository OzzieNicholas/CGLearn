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
    public class CodeInput : RichTextBox
    {
        private void SetTabDistance(RichTextBox txt, int[] tabs)
        {
            Win32.SendMessage(txt.Handle, Win32.EM_SETTABSTOPS, tabs.Length, tabs);
        }

        private string m_lastCode = "";

        protected Action m_onLineChanged = null;

        public CodeInput()
        {
            this.AutoSize = false;
            this.Multiline = true;
            this.WordWrap = false;
            this.ScrollBars = RichTextBoxScrollBars.Both;
            this.AcceptsTab = true;
            this.Font = new Font(new FontFamily("Courier New"), this.Font.Size);
            this.BorderStyle = BorderStyle.None;

            Size chSize = TextRenderer.MeasureText("A", this.Font);
            int pixPerUnits = 4;
            SetTabDistance(this, new int[] { chSize.Width * 4 / pixPerUnits });
        }

        public void dispose()
        {
            this.Dispose();
        }

        //protected override void OnPreviewKeyDown(PreviewKeyDownEventArgs e)
        //{
        //    if (e.KeyCode == Keys.Tab)
        //    {
        //        e.IsInputKey = true;
        //    }

        //    base.OnPreviewKeyDown(e);
        //}

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (e.KeyCode == Keys.Home) {
                int head = _returnLineHead(this.Text, this.SelectionStart);
                int pos = head;
                while (this.Text[pos] == '\t')
                    pos++;

                if (this.SelectionStart == pos) //在有效行首按HOME，应按默认行为跳到绝对行首
                {
                    pos = head;
                }

                if (e.Shift)
                    this.Select(pos, this.SelectionStart + this.SelectionLength - pos);
                else
                    this.SelectionStart = pos;

                e.Handled = true;
            }
            else if (e.KeyCode == Keys.End) {
                int end = this.Text.IndexOf('\n', this.SelectionStart + this.SelectionLength);
                if (e.Shift) {
                    if (end >= this.SelectionStart)
                        this.SelectionLength = end - this.SelectionStart;

                    e.Handled = true;
                }
            }
            else if (e.KeyCode == Keys.Tab) {
                if (e.Shift) //Shift + Tab
                {
                    int off = this.SelectionStart;
                    int len = this.SelectionLength;
                    int end = off + len;
                    int newOff = off;
                    int newLen = len;

                    int pos = _returnLineHead(this.Text, off);
                    string result = this.Text.Substring(0, pos);
                    string line;
                    int posnext = pos;
                    while (pos < end && _parseLine(this.Text, ref posnext, out line)) {
                        if (line[0] == '\t' || line[0] == ' ') {
                            if (pos < off)
                                newOff -= 1;
                            else
                                newLen -= 1;

                            result += line.Substring(1);
                        }
                        else
                            result += line;

                        pos = posnext;
                    }

                    if (pos < this.Text.Length)
                        result += this.Text.Substring(pos);

                    this._updateText(result, newOff, newLen);
                }
                else //Tab
                {
                    int off = this.SelectionStart;
                    int len = this.SelectionLength;
                    int newLen = len;

                    string result = this.Text.Substring(0, off);
                    if (len > 0) {
                        int pos = off;
                        string line;
                        while (pos < off + len && _parseLine(this.Text, ref pos, out line)) {
                            if (result.Length > off) //not first line
                            {
                                newLen += 1;
                            }

                            result += '\t';
                            result += line;
                        }

                        if (pos < this.Text.Length)
                            result += this.Text.Substring(pos);
                    }
                    else {
                        result += '\t';
                        result += this.Text.Substring(off);
                    }

                    this._updateText(result, off + 1, newLen);
                }

                e.Handled = true;
            }

            if (e.KeyCode == Keys.Enter) {
                if (m_onLineChanged != null)
                    m_onLineChanged();
            }
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            base.OnKeyPress(e);

            if (e.KeyChar == '\t') {
                e.Handled = true;
            }
        }

        public int firstLine {
            get {
                int firstVisibleChar = this.GetCharIndexFromPosition(new Point(0, this.Font.Height / 2));

                int line = 1;
                string text = this.Text;
                for (int i = 0; i < firstVisibleChar; ++i) {
                    if (text[i] == '\n')
                        line += 1;
                }

                return line;
            }
        }

        protected void _updateText(string text, int selStart, int selLen)
        {
            Win32.SendMessage(this.Handle, Win32.WM_SETREDRAW, false, 0);

            int firstVisibleChar = this.GetCharIndexFromPosition(new Point(0, this.Font.Height / 2));
            this.Text = text;
            this.SelectionStart = firstVisibleChar;
            this.ScrollToCaret();
            this.Select(selStart, selLen);

            Win32.SendMessage(this.Handle, Win32.WM_SETREDRAW, true, 0);
            this.Refresh();
        }

        protected int _returnLineHead(string s, int off)
        {
            off -= 1;

            while (off >= 0 && s[off] != '\n')
                off--;

            off += 1;

            return off;
        }

        protected bool _parseLine(string s, ref int off, out string retLine)
        {
            retLine = "";

            if (off >= s.Length)
                return false;

            int begin = off;
            while (s[off] != '\n' && off < s.Length) {
                off++;
            }

            retLine = s.Substring(begin, off - begin);

            if (off < s.Length) {
                retLine += "\n";
                off++;
            }

            return true;
        }

        public string text {
            get => this.Text;
            set {
                this.Text = value;
                if (m_onLineChanged != null)
                    m_onLineChanged();
            }
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            if (m_onLineChanged != null)
                m_onLineChanged();
        }

        public bool isModified {
            get => m_lastCode != this.Text;
            set {
                if (value == true)
                    m_lastCode = value ? null : this.Text;
            }
        }

        public Action onLineChanged {
            get => m_onLineChanged;
            set => m_onLineChanged = value;
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
