using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;


namespace UI
{
    class FU_Main : FuncUnit
    {
        public FU_Main() {
            
        }

        public override Define.LayoutCellType cellType => Define.LayoutCellType.CELL_VIEW;
    }
}
