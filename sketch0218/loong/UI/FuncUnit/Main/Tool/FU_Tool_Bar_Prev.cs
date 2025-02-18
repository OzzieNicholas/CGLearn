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
    class FU_Tool_Bar_Prev : FuncUnit
    {
        public FU_Tool_Bar_Prev()
        {
            m_name = "tool_previous_bar";
            m_group = "tool";
            m_icon = "res:tb_icon_bar_previous";
            m_barStyle = "button";
            m_tip = "上一个工具栏";
            
        }

        public override void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras)
        {
            if (sender.GetType() == typeof(ToolFuncButton))
            {
                BarNode barProv = (sender as ToolFuncButton).ButtonNode.Parent as BarNode;
                barProv.Provisioned = false;
                BarNode prevBarProv = CustomBarsProvision.instance.GetPreviousSlideBar(barProv.Name);
                prevBarProv.Provisioned = true;

                string prevLocation = prevBarProv.Location;
                LayoutNode layoutNode = barProv.Parent as LayoutNode;

                CustomToolBar bar = LayoutManager.instance.getBar(barProv);
                CustomToolBar prevBar = LayoutManager.instance.getBar(prevBarProv);

                if (barProv.Location == CustomBarsProvision.BAR_LOCATION_FLOATY)
                {
                    bar.hideFloaty();
                    prevBar.showFloaty(barProv.x, barProv.y);
                }
                else
                {
                    prevBarProv.moveBarBefore(barProv);
                    MainForm.instance.barRoot.SuspendLayout();
                    LayoutManager.instance.refreshLayoutBarByLocation(barProv.Location, layoutNode);
                    if (prevLocation != CustomBarsProvision.BAR_LOCATION_TOP)
                        LayoutManager.instance.refreshLayoutBarByLocation(prevLocation, layoutNode);
                    MainForm.instance.barRoot.ResumeLayout();

                    if (prevLocation == CustomBarsProvision.BAR_LOCATION_FLOATY)
                        prevBar.hideFloaty();
                }

                if (prevBar.Parent != null && prevBar.Parent.GetType() == typeof(ToolBarPanel))
                {
                    (prevBar.Parent as ToolBarPanel).relayout();
                }
            }
        }
    }
}
