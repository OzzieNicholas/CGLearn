using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public partial class TabPanel : UserControl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private TabPanelPage[] m_pages = new TabPanelPage[0];
        private int m_selectedIndex = -1;

        public TabPanel() {
            InitializeComponent();
            this.DoubleBuffered = true;
            this.TabStop = false;
        }

        [Category("Custom"), Browsable(true), Description("Pages")]
        public TabPanelPage[] Pages {
            get => m_pages;
            set {
                this.Controls.Clear();

                m_pages = value;

                if (m_pages != null) {
                    for (int i = 0; i < m_pages.Length; ++i) {
                        var page = m_pages[i];
                        page.Visible = false;
                        if (page.Parent != this) {
                            this.Controls.Add(page);
                            page.Location = new Point(5, 5);
                            page.Size = new Size(this.Width - 5, this.Height - 5);
                        }
                    }
                }

                this.SelectedIndex = m_selectedIndex; //force update
            }
        }

        [Category("Custom"), Browsable(true), Description("Selected Index")]
        public int SelectedIndex {
            get => m_selectedIndex;
            set {
                m_selectedIndex = value;

                if (m_pages == null || m_selectedIndex >= m_pages.Length)
                    m_selectedIndex = -1;

                if (m_pages != null) {
                    for (int i = 0; i < m_pages.Length; ++i) {
                        m_pages[i].Visible = (i == m_selectedIndex);
                    }
                }
            }
        }

        [Category("Custom"), Browsable(true), Description("Selected Page")]
        public TabPanelPage SelectedPage {
            get {
                if (m_pages == null)
                    return null;

                if (m_selectedIndex < 0 || m_selectedIndex >= m_pages.Length)
                    return null;

                return m_pages[m_selectedIndex];
            }

            set {
                int idx = -1;
                if (m_pages != null) {
                    for (int i = 0; i < m_pages.Length; ++i) {
                        if (value == m_pages[i])
                            idx = i;
                    }
                }
                this.SelectedIndex = idx;
            }
        }

        [Description("Prop Name")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [Description("Check Name")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }
    }

}
