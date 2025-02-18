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
    public interface IUserCtrl
    {
        bool Visible { get; set; }
        Point Location { get; set; }
        Rectangle ClientRectangle { get;  }
        Rectangle DisplayRectangle { get;  }
        Control.ControlCollection Controls { get;  }

        int Width { get; set; }
        int Height { get; set; }
        
        string PropName { get; set; }
        string CheckName { get; set; }
        string TabGroup { get; set; }

        Rectangle RectangleToScreen(Rectangle rect);
        Rectangle RectangleToClient(Rectangle rect);
        Point PointToScreen(Point pt);
        Point PointToClient(Point pt);
        void Refresh();

        void Dispose();
        
    }
}
