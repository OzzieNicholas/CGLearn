using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(false)]
    public partial class SysImgs : UserControl
    {
        private static SysImgs m_instance = new SysImgs();
        public static SysImgs instance => m_instance;

        private static readonly string[] DPI_POSTFIX = new string[] { "_96", "_120", "_144", "_168", "_192" };

        public SysImgs() {
            InitializeComponent();
        }

        public Image getMiniBarIcon(string name)
        {
            return _getDpiImage(name, mini_bar_icons_24x24_96, mini_bar_icons_24x24_120,
                mini_bar_icons_24x24_144, mini_bar_icons_24x24_168, mini_bar_icons_24x24_192);
        }

        public Image getMiniBarIcon11x11(string name)
        {
            return _getDpiImage(name, mini_bar_icons_11x11_96, mini_bar_icons_11x11_120, 
                mini_bar_icons_11x11_144, mini_bar_icons_11x11_168, mini_bar_icons_11x11_192);
        }

        protected Image _getDpiImage(string name, ImageList dpi96, ImageList dpi120,
                                ImageList dpi144, ImageList dpi168, ImageList dpi192) {
            var dpi = DPI.Dpi;

            if (String.IsNullOrEmpty(name))
                return null;

            string main = name;
            string ext = "";
            int dot = name.LastIndexOf('.');
            if (dot >= 0)
            {
                main = name.Substring(0, dot);
                ext = name.Substring(dot);
            }

            if (dpi >= 192)
            {
                int p = dpi192.Images.IndexOfKey(main + "_192" + ext);
                if (p >= 0)
                    return dpi192.Images[p];
            }

            if (dpi >= 168)
            {
                int p = dpi168.Images.IndexOfKey(main + "_168" + ext);
                if (p >= 0)
                    return dpi168.Images[p];
            }

            if (dpi >= 144)
            {
                int p = dpi144.Images.IndexOfKey(main + "_144" + ext);
                if (p >= 0)
                    return dpi144.Images[p];
            }

            if (dpi >= 120)
            {
                int p = dpi120.Images.IndexOfKey(main + "_120" + ext);
                if (p >= 0)
                    return dpi120.Images[p];
            }

            if (dpi >= 96)
            {
                int p = dpi96.Images.IndexOfKey(main + "_96" + ext);
                if (p >= 0)
                    return dpi96.Images[p];
            }

            return dpi96.Images[name];
        }
    }
}
