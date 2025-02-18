using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public partial class ImageCheckButton : UserControl, IUserCtrl
    {
        private string m_propName = "";
        private string m_checkName = "";
        private string m_tabGroup = "";
        private Image m_img = null;
        private Image m_imgHover = null;
        private Image m_imgChecked = null;
        private float m_imgRot = 0;
        private bool m_checked = false;
        private bool m_hovered = false;
        private bool m_autoChecked = true;
        private Matrix m_matDraw = new Matrix();

        public ImageCheckButton()
        {
            InitializeComponent();
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.ResizeRedraw, true);
        }

        protected override void CreateHandle() {
            if (!IsHandleCreated) {
                try {
                    base.CreateHandle();
                }
                catch { }
                finally {
                    if (!IsHandleCreated) {
                        base.RecreateHandle();
                    }
                }
            }
        }

        [DefaultValue("")]
        public string PropName {
            get => m_propName;
            set => m_propName = value;
        }

        [DefaultValue("")]
        public string CheckName {
            get => m_checkName;
            set => m_checkName = value;
        }

        [DefaultValue("")]
        public string TabGroup {
            get => m_tabGroup;
            set => m_tabGroup = value;
        }

        public Image image {
            get => m_img;
            set {
                m_img = value;
                this.Size = value.Size;
            }
        }

        public Image imageHover {
            get => m_imgHover;
            set => m_imgHover = value;
        }

        public Image imageChecked {
            get => m_imgChecked;
            set => m_imgChecked = value;
        }

        public float imageRotation {
            get => m_imgRot;
            set {
                m_imgRot = value;
                m_matDraw.Reset();
                m_matDraw.RotateAt(-m_imgRot, new Point(this.Width / 2, this.Height / 2));
                this.Invalidate();
            }
        }

        public bool autoChecked {
            get => m_autoChecked;
            set => m_autoChecked = value;
        }

        public bool Checked {
            get => m_checked;
            set {
                m_checked = value;
                this.Invalidate();
            }
        }

        protected override void OnMouseClick(MouseEventArgs e) {
            base.OnMouseClick(e);

            if (m_autoChecked)
                this.Checked = !this.Checked;
        }

        protected override void OnMouseEnter(EventArgs eventargs) {
            base.OnMouseEnter(eventargs);
            m_hovered = true;
            this.Invalidate();
        }

        protected override void OnMouseLeave(EventArgs eventargs) {
            base.OnMouseLeave(eventargs);
            m_hovered = false;
            this.Invalidate();
        }

        protected override void OnSizeChanged(EventArgs e) {
            base.OnSizeChanged(e);

            m_matDraw.Reset();
            m_matDraw.RotateAt(-m_imgRot, new Point(this.Width / 2, this.Height / 2));
        }

        protected override void OnPaint(PaintEventArgs e) {
            e.Graphics.Clear(this.BackColor);

            var img = (m_hovered && m_imgHover != null) ? m_imgHover :
                    (m_checked && m_imgChecked != null) ? m_imgChecked : m_img;
            if (img == null)
                return;

            int x = (this.Width - img.Width) / 2;
            int y = (this.Height - img.Height) / 2;

            e.Graphics.Transform = m_matDraw;
            e.Graphics.DrawImage(img, x, y, img.Width, img.Height);
        }
    }
}
