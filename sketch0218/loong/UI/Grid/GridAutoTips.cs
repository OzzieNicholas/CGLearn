using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using UserCtrl;
using IFWrapper;

namespace UI
{
    [ToolboxItem(true)]
    public class GridAutoTips : DataGridView
    {
        public GridAutoTips() {

        }

        [Browsable(true)]
        public new int RowCount {
            get => base.RowCount;
            set => base.RowCount = value;
        }
    }
}
