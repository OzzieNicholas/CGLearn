
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    public class ParaForm : UserCtrl.ParaFormBase
    {
        public ParaForm() {

        }

        protected override void OnMouseEnter(EventArgs e) {
            base.OnMouseEnter(e);

            //Native.clearFocusedViewHover();
        }
    }
}
