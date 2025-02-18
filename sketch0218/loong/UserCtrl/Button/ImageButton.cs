using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ImageButton : System.Windows.Forms.PictureBox {
        private Image m_image = null;
        private Image m_imageHL = null;
        private Color m_backColor = Color.Transparent;
        private Color m_backColorHL = Color.Transparent;

        public ImageButton() {
            this.DoubleBuffered = true;
            this.BorderStyle = BorderStyle.None;
            this.BackgroundImageLayout = ImageLayout.Stretch;
        }

        [Browsable(false), DefaultValue(null)]
        public new Image Image {
            get => base.Image;
            set { }
        }

        [Browsable(false), DefaultValue(null)]
        public new Image BackgroundImage {
            get => base.BackgroundImage;
            set {
            }
        }

        [DefaultValue(ImageLayout.Stretch)]
        public new ImageLayout BackgroundImageLayout {
            get => base.BackgroundImageLayout;
            set => base.BackgroundImageLayout = value;
        }


        [Browsable(false), DefaultValue(null)]
        public new Color BackColor {
            get => base.BackColor;
            set {
            }
        }

        [DefaultValue(null)]
        public Image image {
            get => m_image;
            set {
                m_image = value;
                base.BackgroundImage = m_image;
            }
        }

        [DefaultValue(null)]
        public Image imageHL {
            get => m_imageHL;
            set => m_imageHL = value;
        }

        [DefaultValue(null)]
        public Color backColor {
            get => m_backColor;
            set {
                m_backColor = value;
                base.BackColor = m_backColor;
            }
        }

        [DefaultValue(null)]
        public Color backColorHL {
            get => m_backColorHL;
            set => m_backColorHL = value;
        }

        protected override void OnMouseEnter(EventArgs e) {
            base.OnMouseEnter(e);

            base.BackgroundImage = (m_imageHL != null) ? m_imageHL : m_image;
            base.BackColor = m_backColorHL;
        }

        protected override void OnMouseLeave(EventArgs e) {
            base.OnMouseLeave(e);

            base.BackgroundImage = m_image;
            base.BackColor = m_backColor;
        }

    }
}
