using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace UserCtrl
{
    public interface IDGridCell
    {
        DGrid.CellType type { get;  }
        object data { get; set; }
        bool ReadOnly { get; set; }
        object items { get; set; }
        bool clickEdit { get; set; }
        bool doubleClickEdit { get; set; }

        DGrid grid { get; }
        DataGridViewCell origin { get; }

        IDGridCell mergePivot { get;}
        Point mergeRange { get; set; }

        Rectangle bounds { get; }

        void setMergePivot(IDGridCell cell);
        void setMergeRange(Point range);

        int RowIndex { get; }
        int ColumnIndex { get; }
    }
}
