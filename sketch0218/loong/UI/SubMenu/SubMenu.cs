using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using LoongCAD.Common;
using LoongCAD.Geometry;
using LoongCAD.Util;
using LoongCAD.STC;
using IFWrapper;

namespace UI
{
    [ToolboxItem(false)]
    public partial class SubMenu : UserCtrl.SubMenuBase
    {
        private static Dictionary<string, SubMenu> m_ctrlMenus = new Dictionary<string, SubMenu>();
        private static Dictionary<Define.ObjectType, SubMenu> m_objMenus = new Dictionary<Define.ObjectType, SubMenu>();
        
        static SubMenu() {
            m_objMenus[Define.ObjectType.OBJ_GRID] = SubMenu_Grid.instance;
            m_objMenus[Define.ObjectType.OBJ_GRID_AXIS] = SubMenu_Grid.instance;

            m_objMenus[Define.ObjectType.OBJ_STEEL_PART_BEAM] = SubMenu_SP.instance;
            m_objMenus[Define.ObjectType.OBJ_STEEL_PART_PLATE] = SubMenu_SP.instance;
            m_objMenus[Define.ObjectType.OBJ_STEEL_PART_POLY_BEAM] = SubMenu_SP.instance;

            m_objMenus[Define.ObjectType.OBJ_CTRL_POINT] = SubMenu_CtrlPoint.instance;
            m_objMenus[Define.ObjectType.OBJ_CTRL_POINT_GRID_AXIS] = SubMenu_CtrlPoint.instance;
            m_objMenus[Define.ObjectType.OBJ_CTRL_POINT_CHAMFER] = SubMenu_CtrlPoint_Chamfer.instance;
        }

        public static SubMenu activeMenu {
            get {
                var cellType = Native.focusedCellType();
                if (cellType == Define.LayoutCellType.CELL_VIEW) {
                    if (Native.isFocusedCellSnappingPoint()) 
                        return SubMenu_Snap_Point.instance;

                    //if (Native.queryCurObjBaseInfos(out long[] ids, out Define.ObjectType[] objTypes, out bool[] cutterFlags)) {
                    //    if (ids.Length == 0) 
                    //        return SubMenu_View.instance;

                    //    //除Grid外都为同一类型，则显示此类型submenu
                    //    //除Grid外多种类型，则显示多类型submenu
                    //    //都为Grid, 则显示Grid submenu
                    //    Define.ObjectType smType = objTypes[0];
                    //    foreach (var objType in objTypes) {
                    //        if (objType == Define.ObjectType.OBJ_GRID) 
                    //            continue;

                    //        if (smType == Define.ObjectType.OBJ_GRID)
                    //            smType = objType;
                    //        else if (smType != objType) {
                    //            smType = Define.ObjectType.OBJ_UNKNOWN;
                    //            break;
                    //        }
                    //    }
                        
                    //    if (m_objMenus.TryGetValue(smType, out SubMenu sm))
                    //        return sm;

                    //    return SubMenu_MultiObjs.instance;
                    //}
                }

                return null;
            }
        }

        public static void showActive(Control parent, int x, int y) {
            SubMenu subMenu = SubMenu.activeMenu;
            if (subMenu == null)
                return;

            if (x < 0 && y < 0) {
                var pt = (parent != null)? parent.PointToClient(MousePosition) : MousePosition;
                subMenu.show(parent, pt.X, pt.Y);
            }
            else
                subMenu.show(parent, x, y);
        }

        protected override void onShown() {
            base.onShown();

            foreach (var item in this.Items) {
                if (item is CustomMenuItem)
                    (item as CustomMenuItem).refresh();
            }
        }

        protected void _updateCheckdItem(ToolStripItem item, FuncUnit fu) {
            
        }

        public virtual void reset() {

        }

        //Click item
        public CustomMenuItem fu(string text, string funcName, DObject paras = null) {
            CustomMenuItem item = new CustomMenuItem("click");
            item.funcUnit = FuncUnit.get(funcName);
            item.paras = paras;
            item.Text = text;
            return item;
        }

        //Checked item
        public CustomMenuItem cfu(string text, string funcName, DObject paras = null) {
            CustomMenuItem item = new CustomMenuItem("check");
            item.funcUnit = FuncUnit.get(funcName);
            item.paras = paras;
            item.Text = text;
            return item;
        }

        public override void addItem(ToolStripItem item) {
            if (item == null)
                return;

            if (item is CustomMenuItem) {
                var citem = item as CustomMenuItem;
                if (citem.funcUnit != null)
                    m_namedItems[citem.funcUnit.name] = item;
            }
            else if (item.Tag != null) {
                m_namedItems[item.Tag as string] = item;
            }

            this.Items.Add(item);
        }

        public override void addItems(ToolStripItem[] items) {
            var stack = new Stack<ToolStripItem>();
            foreach (var item in items)
                stack.Push(item);

            while (stack.Count > 0) {
                var item = stack.Pop();
                if (item == null)
                    continue;

                if (item is CustomMenuItem) {
                    var citem = item as CustomMenuItem;
                    if (citem.funcUnit != null)
                        m_namedItems[citem.funcUnit.name] = item;
                }
                else if (item.Tag != null)
                    m_namedItems[item.Tag as string] = item;

                if (item is ToolStripMenuItem) {
                    var mitem = item as ToolStripMenuItem;
                    foreach (ToolStripItem it in mitem.DropDownItems)
                        stack.Push(it);
                }
            }

            this.Items.AddRange(items);
        }

    }
}
