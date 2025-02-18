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
    public class UCPanel : System.Windows.Forms.Panel
    {
        public UCPanel() {
            this.DoubleBuffered = true;
        }

    }
}
