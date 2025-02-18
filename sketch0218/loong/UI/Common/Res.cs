using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Resources;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    public class Res
    {
        private static readonly string[] DPI_POSTFIX = new string[] { "_96", "_120", "_144", "_168", "_192" };
        
        public Res() {
            
        }

        public static Image image(string name) {
            var res = Properties.Resources.ResourceManager.GetObject(name);
            return res as Image;
        }

        public static Image dpiImage(string name) {
            var dpi = DPI.Dpi;

            int ibegin = 0;
            if (dpi >= 192)
                ibegin = 4;
            else if (dpi >= 168)
                ibegin = 3;
            else if (dpi >= 144)
                ibegin = 2;
            else if (dpi >= 120)
                ibegin = 1;
            else
                ibegin = 0;

            for (int i = ibegin; i >= 0; --i) {
                var res = Properties.Resources.ResourceManager.GetObject(name + DPI_POSTFIX[i]);
                if (res is Image)
                    return res as Image;
            }

            return Properties.Resources.ResourceManager.GetObject(name) as Image;
        }

        public static Image dpiImage(string name, int dpi) {
            int ibegin = 0;
            if (dpi >= 192)
                ibegin = 4;
            else if (dpi >= 168)
                ibegin = 3;
            else if (dpi >= 144)
                ibegin = 2;
            else if (dpi >= 120)
                ibegin = 1;
            else
                ibegin = 0;

            for (int i = ibegin; i >= 0; --i) {
                var res = Properties.Resources.ResourceManager.GetObject(name + DPI_POSTFIX[i]);
                if (res is Image)
                    return res as Image;
            }

            return Properties.Resources.ResourceManager.GetObject(name) as Image;
        }
    }

    
}
