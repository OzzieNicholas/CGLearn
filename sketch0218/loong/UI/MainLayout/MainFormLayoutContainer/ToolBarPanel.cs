using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace UI
{
    [ToolboxItem(false)]
    public class ToolBarPanel : Panel
    {
        protected List<List<CustomToolBar>> m_bars = new List<List<CustomToolBar>>();
        protected ToolStripLayoutStyle m_barLayout = ToolStripLayoutStyle.HorizontalStackWithOverflow;
        protected string m_panelPosition = "undefined";

        public ToolBarPanel() {
            this.Padding = new Padding(0);
            this.Margin = new Padding(0);
            this.BorderStyle = BorderStyle.None;
            this.BackColor = SystemColors.Control;
            this.DoubleBuffered = true;
        }

        public string position
        {
            get => m_panelPosition;
        }

        public void clearBars() {
            m_bars.Clear();
            Controls.Clear();
            this.relayout();
        }

        public int lineCount {
            get => m_bars.Count;
        }

        public List<List<CustomToolBar>> lines {
            get => m_bars;
        }

        public ToolStripLayoutStyle barLayout {
            get => m_barLayout;
        }

        public void addToolBar(CustomToolBar tb, int line, bool isAppared) {
            tb.LayoutStyle = m_barLayout;

            if (isAppared)
            {
                if (this.Controls.Contains(tb))
                {
                    foreach (var li in m_bars)
                    {
                        if (li.Contains(tb))
                            li.Remove(tb);
                    }
                }
                else
                    this.Controls.Add(tb);
            }

            //Console.WriteLine("*************** m_insertionIndex: " + m_insertionIndex);
            while (m_bars.Count <= line)
                m_bars.Add(new List<CustomToolBar>());

            m_bars[line].Add(tb);

            //Console.WriteLine("*************** find index: " + m_bars[line].FindIndex(item => item == tb));

            /*var barProv = CustomBarsProvision.instance.getBar(tb.BarName);
            if (m_panelPosition != barProv.Location) {
                Console.WriteLine("addToolBar: m_panelPosition (" + m_panelPosition + "), barProv.Location (" + barProv.Location + ")");
                barProv.changeLocation(m_panelPosition);
            }

            if (line != barProv.Row)
            {
                Console.WriteLine("addToolBar: line (" + line.ToString() + "), barProv.Row (" + barProv.Row.ToString() + "), name is " + barProv.Name);
                barProv.changeRow(line);
            }*/
        }

        public void addToolBar(CustomToolBar tb, int line, int idx) {
            tb.LayoutStyle = m_barLayout;

            if (this.Controls.Contains(tb)) {
                foreach (var li in m_bars) {
                    if (li.Contains(tb))
                        li.Remove(tb);
                }
            }
            else
                this.Controls.Add(tb);

            while (m_bars.Count <= line)
                m_bars.Add(new List<CustomToolBar>());

            {
                var li = m_bars[line];
                if (idx >= li.Count)
                    li.Add(tb);
                else
                    li.Insert(idx, tb);
            }
        }

        public void removeToolBar(CustomToolBar tb) {
            if (!this.Controls.Contains(tb))
                return;

            foreach (var li in m_bars) {
                if (li.Contains(tb))
                    li.Remove(tb);
            }

            this.Controls.Remove(tb);
        }

        public virtual bool indexOf(Rectangle insertRect, out int insertLine, out int insertIdx) {
            insertLine = -1;
            insertIdx = -1;
            return false;
        }

        public void relayout() {
            this.relayout(Rectangle.Empty);
        }

        public virtual void relayout(Rectangle reserve) { //<x: line, y: idx, w: width, h: height>
            
        }
    }
}
