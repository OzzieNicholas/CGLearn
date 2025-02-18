using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;

namespace UI
{
    [ToolboxItem(false)]
    public class PalleteColorGrid : DataGridView
    {
        public PalleteColorGrid()
        {
            this.DoubleBuffered = true;   
        }


    }
    
}
