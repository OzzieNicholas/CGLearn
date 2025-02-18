using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using System.ComponentModel;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(true)]
    public class BarToggleButton : ToolStripButton
    {
        private String m_imageName = "";
        private String m_imageRevsName = "";

        public BarToggleButton() {
            this.AutoSize = true;
            this.ImageScaling = ToolStripItemImageScaling.None;
        }

        public BarToggleButton(String image) {
            this.imageName = image;
            this.AutoSize = true;
            this.ImageScaling = ToolStripItemImageScaling.None;
        }

        public String imageName {
            get => m_imageName;
            set {
                m_imageName = value;
                m_imageRevsName = m_imageName + "_revs";
                swapImage();
            }
        }

        private void _setImage() {
            if (this.Parent != null)
                this.Parent.SuspendLayout();

            this.Image = Res.dpiImage(m_imageName);

            if (this.Parent != null)
                this.Parent.ResumeLayout();
        }

        private void _setImageRevs() {
            if (this.Parent != null)
                this.Parent.SuspendLayout();

            Image img = Res.dpiImage(m_imageRevsName);
            if (img != null)
                this.Image = img;

            if (this.Parent != null)
                this.Parent.ResumeLayout();
        }

        public void swapImage() {
            if (this.Checked)
                _setImageRevs();
            else
                _setImage();
        }

        protected override void OnCheckedChanged(EventArgs e) {
            base.OnCheckedChanged(e);
            swapImage();
        }

        protected override void OnMouseEnter(EventArgs e) {
            base.OnMouseEnter(e);
            _setImageRevs();
        }

        protected override void OnMouseLeave(EventArgs e) {
            base.OnMouseLeave(e);
            if (!this.Checked)
                _setImage();
        }
    }
}
