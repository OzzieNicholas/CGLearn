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
using System.Diagnostics;
using System.IO;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class ProfileTipImage : PictureBox
    {
        protected static Dictionary<string, Image> m_imgs = new Dictionary<string, Image>();
        protected string m_type = "";
        protected string m_subType = "";

        public ProfileTipImage() {
            this.AutoSize = false;
            this.BackgroundImageLayout = ImageLayout.Center;
        }

        public void modify(string type, string subType) {
            m_type = type;
            m_subType = subType;
            this._updateImage();
        }

        protected void _updateImage() {
            if (String.IsNullOrEmpty(m_type)
                || m_subType == null) { //subType有可能为"", 所以不能用isNullOrEmpty判断
                this.BackgroundImage = null;
                return;
            }

            string key = m_type + "$" + m_subType;

            Image img = null;
            if (m_imgs.TryGetValue(key, out img)) {
                this.BackgroundImage = img;
                return;
            }

            //string file = Native.profileTipImageFile(m_type, m_subType, DPI.Dpi);
            //if (String.IsNullOrEmpty(file)) {
            //    this.BackgroundImage = null;
            //    return;
            //}

            //using (var fs = new FileStream(file, FileMode.Open, FileAccess.Read)) {
            //    img = Image.FromStream(fs);
            //    if (img != null) {
            //        m_imgs[key] = img;
            //        this.BackgroundImage = img;
            //    }
            //}
        }


    }
}
