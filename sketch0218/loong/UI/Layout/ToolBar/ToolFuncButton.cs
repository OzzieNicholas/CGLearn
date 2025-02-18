using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    public class ToolFuncButton : ToolStripButton
    {
        private FuncUnit m_unit = null;
        private ButtonNode m_toolBthNode = null;


        public ToolFuncButton() {
            this.AutoSize = true;
            //this.Size = new Size(32, 32);
            this.ImageScaling = ToolStripItemImageScaling.None;
        }

        public ButtonNode ButtonNode {
            get => m_toolBthNode;
            set => m_toolBthNode = value;
        }

        public FuncUnit funcUnit {
            get => m_unit;
            set {
                m_unit = value;

                if (m_unit != null)
                    this.DoubleClickEnabled = true;

                this.refresh();
            }
        }

        protected override void OnMouseEnter(EventArgs e) {
            base.OnMouseEnter(e);
            if (m_unit != null && m_unit.iconImgRevs != null)
                this._image = m_unit.iconImgRevs;
        }

        protected override void OnMouseLeave(EventArgs e) {
            base.OnMouseLeave(e);
            if (m_unit != null)
                this._image = m_unit.iconImg;
        }

        protected Image _image {
            get => this.Image;

            set {
                if (this.Parent != null)
                    this.Parent.SuspendLayout();

                this.Image = value;

                if (this.Parent != null)
                    this.Parent.ResumeLayout();
            }
        }

        protected override void OnClick(EventArgs e) {
            base.OnClick(e);

            if (m_unit != null)
                FuncUnit.invokeClick(m_unit, this, e, Control.ModifierKeys, null);
        }

        protected override void OnDoubleClick(EventArgs e) {
            base.OnDoubleClick(e);

            if (m_unit != null)
                FuncUnit.invokeDoubleClick(m_unit, this, e, Control.ModifierKeys, null, false); //DoubleClick不记录，将来有需求再说
        }

        public virtual void refresh() {
            if (this.Parent != null)
                this.Parent.SuspendLayout();

            if (m_unit != null) {
                if (this.Image == null)
                    this.Image = m_unit.iconImg;

                string tip = m_unit.tip;
                string sc = m_unit.shortCutTip;
                if (sc.Length > 0)
                    tip += " (" + sc + ")";
                this.ToolTipText = tip;
            }
            else {
                this.Image = null;
                this.ToolTipText = "";
            }

            if (this.Parent != null)
                this.Parent.ResumeLayout();
        }
    }
}
