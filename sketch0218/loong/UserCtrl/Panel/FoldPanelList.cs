using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using System.Drawing.Design;
using System.Diagnostics;

namespace UserCtrl
{
    [ToolboxItem(true)]
    [Designer(typeof(FoldPanelListDesigner))]
    public partial class FoldPanelList : UserControl
    {
        private List<FoldPanelContent> m_pans = new List<FoldPanelContent>();

        public FoldPanelList() {
            this.DoubleBuffered = true;

            InitializeComponent();
        }

        protected void _updateCtrls() {
            
        }

        public FoldPanelListGrid grid {
            get => pan_grid;
        }

        [Browsable(true), DefaultValue(1)]
        public int rowCount {
            get => pan_grid.RowCount;
            set {
                pan_grid.RowCount = Math.Max(1, value);
            }
        }
    }

    public class FoldPanelListDesigner : ParentControlDesigner
    {
        //public override void Initialize(IComponent component) {
        //    base.Initialize(component);
        //    this.EnableDesignMode(((FoldPanelList)this.Control).grid, "FoldPanelListGrid");
        //}
    }
}
