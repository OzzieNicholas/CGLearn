using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    public class CellTabHeader
    {
        private CellTab m_tab = null;
        private Define.LayoutCellType m_type = Define.LayoutCellType.CELL_NONE;
        private long m_id = -1;
        private string m_name = "";
        private string m_dispName = "";
        private int m_width = 0;

        public CellTabHeader(CellTab tab, Define.LayoutCellType type, long id, string name) {
            m_type = type;
            m_tab = tab;
            m_id = id;
            this.name = name;
        }

        public Define.LayoutCellType type {
            get => m_type;
        }

        public long id {
            get => m_id;
        }

        public string name {
            get => m_name;
            set {
                m_name = value;
                m_width = _getTabHeaderWidth(m_name, m_tab.Font);
                m_dispName = _stripTabText(m_name, m_tab.Font);
            }
        }

        public string dispName {
            get => m_dispName;
        }

        public int width {
            get => m_width;
        }

        public Rectangle rect(int x) {
            return new Rectangle(x, CellTab.s_headerTopMagin, m_width, m_tab.itemHeight);
        }

        public Rectangle closeBtnRect(int x) {
            int btnSize = DPI.Scale(12);
            return new Rectangle(x + m_width - DPI.Scale(6) - btnSize, CellTab.s_headerTopMagin + (m_tab.itemHeight - btnSize) / 2, btnSize, btnSize);
        }

        private int _getTabHeaderWidth(string name, Font font) {
            Size s = TextRenderer.MeasureText(name.Replace(" ", "　"), font);
            return (s.Width < m_tab.itemWidthMin ? m_tab.itemWidthMin :
                (s.Width > m_tab.itemWidthMax ? m_tab.itemWidthMax : s.Width));
        }

        private string _stripTabText(string text, Font font) {
            string measText = text.Replace(" ", "　");
            Size s = TextRenderer.MeasureText(measText, font);
            if (s.Width <= m_width)
                return text;

            int n = measText.Length;
            while (s.Width > m_width) {
                n -= 1;
                s = TextRenderer.MeasureText(measText.Substring(0, n) + "..", font);
            }

            return text.Substring(0, n) + "..";
        }
    }
}
