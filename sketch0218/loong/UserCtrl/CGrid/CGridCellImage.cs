using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public class CGridCellImage : CGridCell
    {
        public CGridCellImage(CGrid grid) : base(grid) {
            m_type = CGridCellType.IMAGE;

            PictureBox pic = new PictureBox();
            pic.SizeMode = PictureBoxSizeMode.StretchImage;
            pic.Dock = DockStyle.Fill;
            pic.AutoSize = true;
            pic.Anchor = AnchorStyles.None;
            pic.Margin = new Padding(3, 3, 3, 3);
            m_ctrl = pic;
        }

        public override object value {
            get => (m_ctrl as PictureBox).Image;
            set => (m_ctrl as PictureBox).Image = value as Image;
        }

    }


}
