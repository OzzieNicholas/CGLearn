using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;

namespace UI
{
    [ToolboxItem(false)]
    public class CustomToolBar : UserCtrl.ToolBar
    {
        private Dictionary<ToolFuncButton, FuncUnit> m_item2Func = new Dictionary<ToolFuncButton, FuncUnit>();
        private BarNode m_barProv = null;
        private string m_barName = "";
        private Floaty m_floaty;

        public CustomToolBar() {
            this.Dock = DockStyle.None;
            this.DoubleBuffered = true;

            m_floaty = new Floaty(this);
            m_floaty.Visible = false;
        }

        virtual public bool IsDynamic {
            get => true;
        }

        virtual public string BarName {
            get => m_barName;
            set => m_barName = value;
        }

        public Floaty floaty {
            get => m_floaty;
        }

        public bool isInGripArea(Point cpos) {
            var grip = this.GripRectangle;
            if (this.LayoutStyle == ToolStripLayoutStyle.HorizontalStackWithOverflow)
                return cpos.X >= 0 && cpos.X <= grip.Right && cpos.Y >= 0 && cpos.Y <= this.Height;
            else
                return cpos.X >= 0 && cpos.X <= this.Width && cpos.Y >= 0 && cpos.Y <= grip.Bottom;
        }

        public BarNode provsion {
            set {
                if (value == null)
                    return;
                m_barProv = value;
                BarName = m_barProv.Name;
                this.refresh();

                //System.Console.WriteLine(" CustomToolBar: bar ( name: " + m_barProv.Name + ", sn: " + m_barProv.SN + ", tip: " + m_barProv.Tip + " )");
                //foreach (var c in m_barProv.Children)
                //{
                //    System.Console.WriteLine("         : btn ( name: " + c.Name + ", sn: " + c.SN + ", tip: " + c.Tip + " )");
                //}
            }
            get => m_barProv;
        }

        public virtual void refresh() {
            //if (m_barProv != null)
            //{
            //    System.Console.WriteLine("  Bar refresh ###: (" + m_barProv.toString() + ")");
            //    foreach (ButtonNode btn in m_barProv.Children)
            //        System.Console.WriteLine("            ### - add btn for bar " + m_barProv.Name + ": (" + btn.toString() + ")");
            //}
            if (!IsDynamic)
                return;

            this.SuspendLayout();

            this.Items.Clear();
            m_item2Func.Clear();

            if (m_barProv == null)
                return;
            //System.Console.WriteLine("  Bar refresh @@@@: top bar (" + m_barProv.toString() + ")");
            //foreach (ButtonNode btn in m_barProv.Children)
            //    System.Console.WriteLine("            @@@@ - add btn for bar " + m_barProv.Name + ": (" + btn.toString() + ")");
            foreach (ButtonNode btn in m_barProv.Children) {
                if (btn.Tip == "|") {
                    var sep = new ToolStripSeparator();
                    sep.AutoSize = false;
                    if (this.Width > this.Height)
                        sep.Height = 28;
                    else
                        sep.Width = 28;
                    this.Items.Add(sep);
                    continue;
                }
                //System.Console.WriteLine("  ---- Bar Layout refresh - add btn for bar " + m_barProv.Name + ": (" + btn.toString() + ")");

                FuncUnit func = FuncUnit.get(btn.Name);
                if (func == null)
                    continue;

                if (func.barStyle == "button") {
                    ToolFuncButton tfBtn = new ToolFuncButton();
                    tfBtn.ButtonNode = btn;
                    tfBtn.funcUnit = func;
                    this.Items.Add(tfBtn);
                    m_item2Func[tfBtn] = func;
                }
            }

            this.ResumeLayout();

        }

        public virtual void updateShortCuts() {
            foreach (var it in m_item2Func) {
                it.Key.refresh();
            }
        }

        public bool isFloatyVisiable()
        {
            return m_floaty.Visible;
        }

        public void showFloaty(int x = -1, int y = -1) {
            m_floaty.show(x, y);
        }

        public void hideFloaty() {
            m_floaty.Controls.Clear();
            m_floaty.hide();
        }

        public ToolBarPanel panel => this.Parent as ToolBarPanel;

        public Point screenPos {
            get {
                if (this.Parent != null)
                    return this.Parent.PointToScreen(this.Location);

                return this.Location;
            }
        }

        public Rectangle screenRect => this.RectangleToScreen(this.ClientRectangle);
    }

}
