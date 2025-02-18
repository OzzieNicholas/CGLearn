using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UI
{

    public delegate void OnApplyPropsDelegate(LoongCAD.Util.DObject srcProps);

    public delegate void OnModifyPropsDelegate(LoongCAD.Util.DObject srcProps);

    public delegate LoongCAD.Util.DObject OnPickPropsDelegate();

}
