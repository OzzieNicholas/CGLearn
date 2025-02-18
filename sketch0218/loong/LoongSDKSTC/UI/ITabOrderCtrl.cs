using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LoongCAD.UI
{
    public interface ITabOrderCtrl
    {
        string TabGroup { get; set; }
    }
}
