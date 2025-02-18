using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using System.Drawing;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;


namespace UI
{
    class FU_Tool_Bar_Next : FuncUnit
    {
        public FU_Tool_Bar_Next()
        {
            m_name = "tool_next_bar";
            m_group = "tool";
            m_icon = "res:tb_icon_bar_next";
            m_barStyle = "button";
            m_tip = "下一个工具栏";

        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            if (sender.GetType() == typeof(ToolFuncButton))
            {
                BarNode barProv = (sender as ToolFuncButton).ButtonNode.Parent as BarNode;
                barProv.Provisioned = false;
                BarNode nextBarProv = CustomBarsProvision.instance.GetNextSlideBar(barProv.Name);
                nextBarProv.Provisioned = true;

                string nextLocation = nextBarProv.Location;
                LayoutNode layoutNode = barProv.Parent as LayoutNode;

                CustomToolBar bar = LayoutManager.instance.getBar(barProv);
                CustomToolBar nextBar = LayoutManager.instance.getBar(nextBarProv);

                if (barProv.Location == CustomBarsProvision.BAR_LOCATION_FLOATY)
                {
                    bar.hideFloaty();
                    nextBar.showFloaty(barProv.x, barProv.y);
                } 
                else
                {
                    nextBarProv.moveBarBefore(barProv);
                    MainForm.instance.barRoot.SuspendLayout();
                    LayoutManager.instance.refreshLayoutBarByLocation(barProv.Location, layoutNode);
                    if (nextLocation != CustomBarsProvision.BAR_LOCATION_TOP)
                        LayoutManager.instance.refreshLayoutBarByLocation(nextLocation, layoutNode);
                    MainForm.instance.barRoot.ResumeLayout();

                    if (nextLocation == CustomBarsProvision.BAR_LOCATION_FLOATY)
                        nextBar.hideFloaty();
                }

                if (nextBar.Parent != null && nextBar.Parent.GetType() == typeof(ToolBarPanel)) {
                    (nextBar.Parent as ToolBarPanel).relayout();
                }
            }
        }
    }
}
