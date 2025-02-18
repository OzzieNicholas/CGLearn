using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(false)]
    public class ToolBarPanelCenter : Panel
    {
        private CellTab m_headerTab = null; //View的顶部名称Tab栏
        private MainView m_viewPanel = null; //主绘制窗口

        public ToolBarPanelCenter() {
            m_headerTab = new CellTab();
            m_headerTab.Dock = DockStyle.Top;
            m_headerTab.Height = DPI.Scale(CellTab.ORG_HEIGHT);

            m_viewPanel = new MainView();
            m_viewPanel.BackColor = Color.DarkGray;
            m_viewPanel.Location = new Point(0, m_headerTab.Height);
            m_viewPanel.Dock = DockStyle.Fill;
                        
            this.Controls.AddRange(new Control[] { m_viewPanel, m_headerTab });
        }

        public CellTab headerTab {
            get => m_headerTab;
        }

        public MainView viewPanel {
            get => m_viewPanel;
        }
    }
}
