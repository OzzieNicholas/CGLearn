using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using UserCtrl;

namespace UserCtrl
{
    public enum CGridCellType
    {
        NONE = 0,
        LABEL,
        CHECK,
        BUTTON,
        IMAGE,
        INPUT_TEXT,
        INPUT_INT,
        INPUT_INT_ARRAY,
        INPUT_LONG,
        INPUT_DOUBLE,
        INPUT_DOUBLE_ARRAY_SCRIPT,
        LIST_TEXT,
        LIST_INT,
        LIST_DOUBLE,
        LIST_IMAGE,
        COMBOBOX,
        COMBOBOX_INPUT,
        COMBOBOX_IMAGE,
        FILE,
        DIRECTORY,
        TEXT_BUTTON,
    }

    
}
