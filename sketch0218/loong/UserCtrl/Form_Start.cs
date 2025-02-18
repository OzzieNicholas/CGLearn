using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Util;
using LoongCAD.Geometry;
using LoongCAD.STC;

namespace UserCtrl
{
    public partial class Form_Start : Form
    {
        protected static Form_Start m_inst = null; 

        public static Form_Start instance {
            get {
                if (m_inst == null)
                    m_inst = new Form_Start();

                return m_inst;
            }
        }

        public Form_Start() {
            InitializeComponent();

            this.TopMost = true;

            pic_back.BackgroundImageLayout = ImageLayout.Stretch;

            if (DPI.Dpi > 144.0f)
                pic_back.BackgroundImage = Properties.Resources.start_page_192;
            else if (DPI.Dpi > 120.0f)
                pic_back.BackgroundImage = Properties.Resources.start_page_144;
            else if (DPI.Dpi > 96.0f)
                pic_back.BackgroundImage = Properties.Resources.start_page_120;
            else
                pic_back.BackgroundImage = Properties.Resources.start_page_96;

            lb_tips.Text = "";

            this.progress = 0;
        }

        public string tips {
            get => lb_tips.Text;
            set {
                lb_tips.Text = value;
                lb_tips.Refresh();
            }
        }

        public int progress {
            get {
                if (pan_progress.Visible == false)
                    return 0;

                return 100 * pan_progress.Width / this.Width;
            }

            set {
                if (value == 0) {
                    pan_progress.Visible = false;
                    return;
                }

                pan_progress.Width = Math.Min(100, value) * this.Width / 100;
                pan_progress.Visible = true;
            }
        }
    }
}
