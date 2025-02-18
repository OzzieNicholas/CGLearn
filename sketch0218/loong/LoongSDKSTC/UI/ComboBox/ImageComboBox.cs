
using System;
using System.Drawing;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using System.Linq;
using LoongCAD.Util;

namespace LoongCAD.UI
{
    [ToolboxItem(true)]
    public class ImageComboBox : ImageComboBoxBase
    {
        public ImageComboBox() {
            
        }

        [DefaultValue(null)]
        public ImageList ImageList {
            get => m_imgList;
            set {
                m_imgList = value;
                if (!STC.Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(null)]
        public Image Image {
            get => m_img;
            set {
                m_img = value;
                if (!STC.Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(0)]
        public Padding ImageBorder {
            get => m_imgBorder;
            set {
                m_imgBorder = value;
                if (!STC.Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

        [DefaultValue(0)]
        public Size ImageItemSize {
            get => m_imgItemSize;
            set {
                m_imgItemSize = value;
                if (!STC.Define.IS_RUNTIME)
                    this.Invalidate();
            }
        }

    }
}
