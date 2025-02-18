using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;

namespace UI
{
    [ToolboxItem(false)]
    public class MainStatus : StatusStrip
    {
        private ToolStripStatusLabel m_warningAndtips = new ToolStripStatusLabel();
        private ToolStripStatusLabel m_selInfo = new ToolStripStatusLabel();

        private string m_warnings = "";
        private string m_tips = "";
        private string m_objSelInfo = "";
        private string m_cellSelInfo = "";

        public MainStatus()
        {
            this.DoubleBuffered = true;
            this.LayoutStyle = ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.AutoSize = false;
            base.Font = new Font(this.Font.FontFamily, DPI.Scale(13), FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            base.Height = base.Font.Height + 10;
            this.SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint | ControlStyles.DoubleBuffer, true);

            m_warningAndtips.Margin = new Padding(DPI.Scale(30), 3, 0, 0);
            m_warningAndtips.BorderStyle = Border3DStyle.SunkenInner;

            m_selInfo.Alignment = ToolStripItemAlignment.Right;
            m_selInfo.Margin = new Padding(0, 3, DPI.Scale(50), 0);
            m_selInfo.BorderStyle = Border3DStyle.SunkenInner;

            this.Items.Add(m_warningAndtips);
            this.Items.Add(m_selInfo);
        }

        public new Font Font {
            get => base.Font;
            set { }
        }

        public new int Height {
            get => base.Height;
            set { }
        }

        public new Size Size {
            get => base.Size;
            set => this.Width = value.Width;
        }

        public string tips {
            get => m_tips;
            set {
                if (m_tips == value)
                    return;

                m_tips = value;
                
                if (String.IsNullOrEmpty(m_warnings))
                    m_warningAndtips.Text = m_tips;
                else if (String.IsNullOrEmpty(m_tips))
                    m_warningAndtips.Text = m_warnings;
                else
                    m_warningAndtips.Text = m_warnings + " -- " + m_tips;
            }
        }

        public string warnings {
            get => m_warnings;
            set {
                if (m_warnings == value)
                    return;

                m_warnings = value;

                if (String.IsNullOrEmpty(m_warnings))
                    m_warningAndtips.Text = m_tips;
                else if (String.IsNullOrEmpty(m_tips))
                    m_warningAndtips.Text = m_warnings;
                else
                    m_warningAndtips.Text = m_warnings + " -- " + m_tips;
            }
        }

        public string objSelInfo {
            get => m_objSelInfo;
            set {
                if (m_objSelInfo == value)
                    return;

                m_objSelInfo = value;

                if (!String.IsNullOrEmpty(m_objSelInfo) && !String.IsNullOrEmpty(m_cellSelInfo))
                    m_selInfo.Text = "选中 " + m_objSelInfo + " " + m_cellSelInfo;
                else if (!String.IsNullOrEmpty(m_objSelInfo))
                    m_selInfo.Text = "选中 " + m_objSelInfo;
                else if (!String.IsNullOrEmpty(m_cellSelInfo))
                    m_selInfo.Text = "选中 " + m_cellSelInfo;
                else
                    m_selInfo.Text = "";
            }
        }

        public string cellSelInfo {
            get => m_cellSelInfo;
            set {
                if (m_cellSelInfo == value)
                    return;

                m_cellSelInfo = value;

                if (!String.IsNullOrEmpty(m_cellSelInfo))
                    m_selInfo.Text = "选中 " + m_cellSelInfo;
                else if (!String.IsNullOrEmpty(m_objSelInfo))
                    m_selInfo.Text = "选中 " + m_objSelInfo;
                else
                    m_selInfo.Text = "";
            }
        }

        protected override void WndProc(ref Message msg) {
            if (this.DesignMode) {
                base.WndProc(ref msg);
                return;
            }
            
            switch (msg.Msg) {
                case Win32.WM_ERASEBKGND: {
                    msg.Result = IntPtr.Zero;
                    return;
                }
                case Win32.WM_NCHITTEST: {
                    msg.Result = (IntPtr)Win32.HTTRANSPARENT;
                    return;
                }
            }

            base.WndProc(ref msg);
        }
    }


}
