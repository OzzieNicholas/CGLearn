using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LoongCAD.UI
{
    public interface IPlaceholderCtrl
    {
        object Placeholder { get; set; }
        void ClearPlaceholder();
    }
}
