using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    [ToolboxItem(true)]
    public class ContextForm : ParaFormBase
    {
        public ContextForm() {
            
        }

        public Point evalShowPos(Rectangle rcWork, Rectangle rcBtn) {
            int x = 0;
            int y = 0;
            
            if (rcBtn.X + this.Width <= rcWork.Right)
                x = rcBtn.X;
            else if (rcBtn.Right - this.Width >= rcWork.Left)
                x = rcBtn.Right - this.Width;
            else
                x = rcBtn.X;

            if (rcBtn.Bottom + this.Height < rcWork.Bottom)
                y = rcBtn.Bottom;
            else if (rcBtn.Y - this.Height >= rcWork.Top)
                y = rcBtn.Y - this.Height;
            else if ((rcBtn.Top + rcBtn.Bottom) / 2 < (rcWork.Top + rcWork.Bottom) / 2)
                y = rcBtn.Bottom;
            else
                y = rcBtn.Y - this.Height;
            
            return new Point(x, y);
        }
    }
}
