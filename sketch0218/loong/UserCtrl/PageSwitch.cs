using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;


namespace UserCtrl
{
    public partial class PageSwitch : UserControl
    {
        private int m_pageNum = 1;
        private int m_page = 0;
        private bool m_rangeClamp = false;
        public Action<int, int> pageChanged = null;

        public PageSwitch() {
            InitializeComponent();

            SetStyle(ControlStyles.DoubleBuffer | ControlStyles.ResizeRedraw, true);

            this._updateCtrls();
        }

        [DefaultValue(1)]
        public int pageNum {
            get => m_pageNum;
            set {
                m_pageNum = Math.Max(1, value);
                this._updateCtrls();
            }
        }

        [DefaultValue(0)]
        public int page {
            get => m_page;
            set {
                m_page = Math.Max(0, value);
                this._updateCtrls();
            }
        }

        [DefaultValue(false)]
        public bool rangeClamp {
            get => m_rangeClamp;
            set {
                m_rangeClamp = value;
                this._updateCtrls();
            }
        }

        private void btn_begin_Click(object sender, EventArgs e) {
            if (m_page == 0)
                return;

            int pageBefore = m_page;
            m_page = 0;
            this._updateCtrls();

            if (pageChanged != null)
                pageChanged(pageBefore, m_page);
        }

        private void btn_pre_Click(object sender, EventArgs e) {
            if (m_page <= 0)
                return;

            int pageBefore = m_page;
            m_page -= 1;
            this._updateCtrls();

            if (pageChanged != null)
                pageChanged(pageBefore, m_page);
        }

        private void btn_next_Click(object sender, EventArgs e) {
            if (m_rangeClamp && m_page >= m_pageNum - 1)
                return;

            int pageBefore = m_page;
            m_page += 1;
            this._updateCtrls();

            if (pageChanged != null)
                pageChanged(pageBefore, m_page);
        }

        private void btn_end_Click(object sender, EventArgs e) {
            if (m_page == m_pageNum - 1)
                return;

            int pageBefore = m_page;
            m_page = m_pageNum - 1;
            this._updateCtrls();

            if (pageChanged != null)
                pageChanged(pageBefore, m_page);
        }

        protected void _updateCtrls() {
            if (m_rangeClamp)
                lb_page.Text = "" + (m_page + 1) + '/' + m_pageNum;
            else
                lb_page.Text = "" + (m_page + 1);

            int x = 0;

            btn_begin.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            btn_begin.Location = new Point(x + 3, (this.Height - btn_begin.Height) / 2);
            x = btn_begin.Location.X + btn_begin.Width;

            btn_pre.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            btn_pre.Location = new Point(x + 3, (this.Height - btn_pre.Height) / 2);
            x = btn_pre.Location.X + btn_pre.Width;

            lb_page.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            lb_page.Location = new Point(x + 3, (this.Height - lb_page.Height) / 2);
            x = lb_page.Location.X + lb_page.Width;

            btn_next.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            btn_next.Location = new Point(x + 3, (this.Height - btn_next.Height) / 2);
            x = btn_next.Location.X + btn_next.Width;

            btn_end.Anchor = AnchorStyles.Top | AnchorStyles.Left;
            btn_end.Location = new Point(x + 3, (this.Height - btn_end.Height) / 2);
            x = btn_end.Location.X + btn_end.Width;

            this.Width = x + 3;
        }


    }
}
