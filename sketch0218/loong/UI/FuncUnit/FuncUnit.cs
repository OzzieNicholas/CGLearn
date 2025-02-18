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
using IFWrapper;

namespace UI
{
    public class FuncUnit {
        protected static Dictionary<string, FuncUnit> m_units = new Dictionary<string, FuncUnit>();
        protected static FuncUnitCall m_recentCall = null;

        protected string m_name = "";
        protected string m_group = "";
        protected string m_icon = "";
        protected Image m_iconImg = null;
        protected Image m_iconImgRevs = null;
        protected string m_barStyle = "button";
        protected string m_tip = "";
        protected String m_shortCutTip = "";

        public FuncUnit() {

        }

        public static void init() {
            regFuncUnit(new FU_Sys_Switch_Layout_Mode());

            regFuncUnit(new FU_File_New());
            regFuncUnit(new FU_File_Open());
            regFuncUnit(new FU_File_AutoSave());
            regFuncUnit(new FU_File_Save());
            regFuncUnit(new FU_File_SaveAs());
            regFuncUnit(new FU_File_Open_Doc_Directory());
            regFuncUnit(new FU_File_Exit());

            regFuncUnit(new FU_Edit_Undo());
            regFuncUnit(new FU_Edit_Redo());
            regFuncUnit(new FU_Edit_Copy());
            regFuncUnit(new FU_Edit_Copy_Line());
            regFuncUnit(new FU_Edit_Copy_Rotate());
            regFuncUnit(new FU_Edit_Copy_Mirror());
            regFuncUnit(new FU_Edit_Move());
            regFuncUnit(new FU_Edit_Move_Line());
            regFuncUnit(new FU_Edit_Move_Rotate());
            regFuncUnit(new FU_Edit_Move_Mirror());
            regFuncUnit(new FU_Edit_Del());
            regFuncUnit(new FU_Edit_Pick_All());
            regFuncUnit(new FU_Edit_Stop());
            regFuncUnit(new FU_Edit_Props());
            regFuncUnit(new FU_Edit_Revert_Snap());
            regFuncUnit(new FU_Edit_Stop_Snap());
            regFuncUnit(new FU_Edit_Invoke_Recent_Call());

            regFuncUnit(new FU_View_Create_Model_Base());
            regFuncUnit(new FU_View_Create_Model_2P());
            regFuncUnit(new FU_View_Create_Model_3P());
            regFuncUnit(new FU_View_Create_Model_By_WorkPlane());
            regFuncUnit(new FU_View_Create_Model_By_Grid());
            regFuncUnit(new FU_View_Create_Model_By_Part_Front());
            regFuncUnit(new FU_View_Create_Model_By_Part_Top());
            regFuncUnit(new FU_View_Create_Model_By_Part_Back());
            regFuncUnit(new FU_View_Create_Model_By_Part_Bottom());
            regFuncUnit(new FU_View_Create_Obj_3D());
            regFuncUnit(new FU_View_Create_Obj_Def());
            regFuncUnit(new FU_View_Create_Obj_Ortho());
            regFuncUnit(new FU_View_Create_Obj_Front());
            regFuncUnit(new FU_View_Create_Obj_Back());
            regFuncUnit(new FU_View_Create_Obj_Top());
            regFuncUnit(new FU_View_Create_Obj_Bottom());
            regFuncUnit(new FU_View_Create_Obj_Head());
            regFuncUnit(new FU_View_Create_Obj_Tail());
            regFuncUnit(new FU_View_Create_Comp_3D());
            regFuncUnit(new FU_View_Create_Comp_Def());
            regFuncUnit(new FU_View_Create_Comp_Ortho());
            regFuncUnit(new FU_View_Rdmode_Wireframe());
            regFuncUnit(new FU_View_Rdmode_Trans());
            regFuncUnit(new FU_View_Rdmode_Hidden_Lines());
            regFuncUnit(new FU_View_Rdmode_Edgeface());
            regFuncUnit(new FU_View_Rdmode_Sel_Only());
            regFuncUnit(new FU_View_RdCompMode_Wireframe());
            regFuncUnit(new FU_View_RdCompMode_Trans());
            regFuncUnit(new FU_View_RdCompMode_Hidden_Lines());
            regFuncUnit(new FU_View_RdCompMode_Edgeface());
            regFuncUnit(new FU_View_RdCompMode_Sel_Only());
            regFuncUnit(new FU_View_Rdquality_High());
            regFuncUnit(new FU_View_Set_WorkPlane_By_World());
            regFuncUnit(new FU_View_Set_WorkPlane_1P());
            regFuncUnit(new FU_View_Set_WorkPlane_2P());
            regFuncUnit(new FU_View_Set_WorkPlane_3P());
            regFuncUnit(new FU_View_Set_WorkPlane_By_ViewPlane());
            regFuncUnit(new FU_View_Set_WorkPlane_By_Part_Front());
            regFuncUnit(new FU_View_Set_WorkPlane_By_Part_Top());
            regFuncUnit(new FU_View_Set_WorkPlane_By_Part_Back());
            regFuncUnit(new FU_View_Set_WorkPlane_By_Part_Bottom());
            regFuncUnit(new FU_View_Set_Focused_Box_2P());
            regFuncUnit(new FU_View_Set_Focused_Box_By_View());
            regFuncUnit(new FU_View_Set_Focused_Box_By_Model());
            regFuncUnit(new FU_View_Set_Focused_Box_By_Part());
            regFuncUnit(new FU_View_Set_Active_Box_By_Part());
            regFuncUnit(new FU_View_Create_Clip());
            regFuncUnit(new FU_View_Zoom_By_Part());
            regFuncUnit(new FU_View_Zoom_By_WorkBox());
            regFuncUnit(new FU_View_ZoomIn());
            regFuncUnit(new FU_View_ZoomOut());
            regFuncUnit(new FU_View_ZoomIn_Key());
            regFuncUnit(new FU_View_ZoomOut_Key());
            regFuncUnit(new FU_View_Reset_Cam());
            regFuncUnit(new FU_View_Trans_By_Hand());
            regFuncUnit(new FU_View_Move_Left());
            regFuncUnit(new FU_View_Move_Right());
            regFuncUnit(new FU_View_Move_Up());
            regFuncUnit(new FU_View_Move_Down());
            regFuncUnit(new FU_View_Center_By_Cursor());
            regFuncUnit(new FU_View_Pick_Pivot());
            regFuncUnit(new FU_View_Pick_Pivot_Rotate());
            regFuncUnit(new FU_View_Rotate_H15());
            regFuncUnit(new FU_View_Rotate_HN15());
            regFuncUnit(new FU_View_Rotate_V15());
            regFuncUnit(new FU_View_Rotate_VN15());
            regFuncUnit(new FU_View_Rotate_H5());
            regFuncUnit(new FU_View_Rotate_HN5());
            regFuncUnit(new FU_View_Rotate_V5());
            regFuncUnit(new FU_View_Rotate_VN5());
            regFuncUnit(new FU_View_Rotate_Enable());
            regFuncUnit(new FU_View_Free_Walk());
            regFuncUnit(new FU_View_List());
            regFuncUnit(new FU_View_Props());
            regFuncUnit(new FU_View_Redraw());
            regFuncUnit(new FU_View_Reset_Pos());
            regFuncUnit(new FU_View_Switch_Type());
            regFuncUnit(new FU_View_Hide_Objs());
            regFuncUnit(new FU_View_Focus_Objs());
            regFuncUnit(new FU_View_Switch_To_Next());
            regFuncUnit(new FU_View_TopMost());
            regFuncUnit(new FU_View_WorkBoxVis());

            regFuncUnit(new FU_Sel_Cur_Objs_Undo());
            regFuncUnit(new FU_Sel_Cur_Objs_Redo());

            regFuncUnit(new FU_Create_SPBeam());
            regFuncUnit(new FU_Create_SPColumn());
            regFuncUnit(new FU_Create_SPCurveBeam());
            regFuncUnit(new FU_Create_SPPlate());

            regFuncUnit(new FU_Create_SPBeam_Props());
            regFuncUnit(new FU_Create_SPColumn_Props());
            regFuncUnit(new FU_Create_SPPlate_Props());

            regFuncUnit(new FU_Measure_Dist());
            regFuncUnit(new FU_Measure_DistH());
            regFuncUnit(new FU_Measure_DistV());
            regFuncUnit(new FU_Measure_Angl());
            regFuncUnit(new FU_Measure_Arc());
            regFuncUnit(new FU_Measure_Bolt());

            regFuncUnit(new FU_Tool_Bar_Prev());
            regFuncUnit(new FU_Tool_Bar_Next());
            regFuncUnit(new FU_Tool_Pallete_Mgr());
            regFuncUnit(new FU_Tool_ShortCut_Mgr());
            regFuncUnit(new FU_Tool_Options());
            regFuncUnit(new FU_Tool_Settings());
            regFuncUnit(new FU_Tool_Hover_Highlight());
            regFuncUnit(new FU_Tool_Snap_Symbol_Vis());
            regFuncUnit(new FU_Tool_Auto_Occ_Detect());
            regFuncUnit(new FU_Tool_Cut_Color());
            regFuncUnit(new FU_Tool_Cus_Bar());
            regFuncUnit(new FU_Tool_Pick_Toggle_All());
            regFuncUnit(new FU_Tool_Pick_Toggle_Part());
            regFuncUnit(new FU_Tool_Snap_Toggle_Pt_Ref_Point());
            regFuncUnit(new FU_Tool_Snap_Toggle_Pt_Geo_Point());
            regFuncUnit(new FU_Tool_Snap_Toggle_Pt_Line_Nearest());
            regFuncUnit(new FU_Tool_Snap_Toggle_Pt_Free());
            regFuncUnit(new FU_Tool_Snap_Point_By_Input());
            regFuncUnit(new FU_Tool_Snap_Point_By_Input_Global());
            regFuncUnit(new FU_Tool_Snap_Point_By_Input_Absolute());
            regFuncUnit(new FU_Tool_Snap_Point_By_Input_Relative());
            regFuncUnit(new FU_Tool_Snap_Lock_X());
            regFuncUnit(new FU_Tool_Snap_Lock_Y());
            regFuncUnit(new FU_Tool_Snap_Lock_Z());
            regFuncUnit(new FU_Tool_Run_Script());
            regFuncUnit(new FU_Tool_Test_Form());

            regFuncUnit(new FU_Create_StraightLine());
            regFuncUnit(new FU_Create_Rectangle());
            regFuncUnit(new FU_Create_Polygon());
            regFuncUnit(new FU_Create_Circle());
            regFuncUnit(new FU_Create_Arc());
            regFuncUnit(new FU_Create_Spline());
            regFuncUnit(new FU_Extrude_Polygon());
            regFuncUnit(new FU_Extrude_Nurbs());
            regFuncUnit(new FU_Extrude_Path());
            regFuncUnit(new FU_Translate());
            regFuncUnit(new FU_Rotate());
            regFuncUnit(new FU_Scale());
            regFuncUnit(new FU_Buffer());
            regFuncUnit(new FU_TRS());
            regFuncUnit(new FU_Edit_Nurbs());

            updateShortCuts();
        }

        public static Dictionary<string, FuncUnit> units => m_units;

        public static FuncUnit get(string name) {
            if (name == null || !m_units.ContainsKey(name))
                return null;

            return m_units[name];
        }

        public static void regFuncUnit(FuncUnit unit) {
            m_units[unit.name] = unit;

            if (String.IsNullOrEmpty(unit.icon)) 
                unit.icon = "res:tb_icon_unknown_func";
            else if (Res.dpiImage(unit.icon.Substring(4)) == null) 
                unit.icon = "res:tb_icon_unknown_func";
        }

        public static void updateShortCuts() {
            Native.queryShortCuts(out string[] scNames, out uint[] scs);
            foreach (var it in m_units) {
                var fu = it.Value;
                fu.shortCutTip = "";
            }
            
            for (int i = 0; i < scNames.Length; ++i) {
                var name = scNames[i];
                if ((scs[i] & 0xffffff) == 0)
                    continue;

                FuncUnit fu = null;
                if (!m_units.TryGetValue(name, out fu))
                    continue;

                fu.shortCutTip = FuncUnit.shortCut2Str(scs[i]);
            }
        }

        public static string shortCut2Str(uint flag) {
            string val = "";
            if ((flag & (1 << 25)) != 0)
                val += "Ctrl";
            if ((flag & (1 << 24)) != 0)
                val += "Alt";
            if ((flag & (1 << 26)) != 0)
                val += "Shift";

            if ((flag & 0xff0000) != 0) {
                if (val.Length > 0)
                    val += " + ";

                Keys key = (Keys)((flag & 0xff0000) >> 16);
                if (key >= Keys.D0 && key <= Keys.D9) //数字，不采用Key中的D0-D9形式
                    val += key - Keys.D0;
                else //非数字，直接采用Keys名
                    val += key.ToString();
            }

            if ((flag & 0xff00) != 0) {
                if (val.Length > 0)
                    val += " + ";

                Keys key = (Keys)((flag & 0xff00) >> 8);
                if (key >= Keys.D0 && key <= Keys.D9) //数字，不采用Key中的D0-D9形式
                    val += key - Keys.D0;
                else //非数字，直接采用Keys名
                    val += key.ToString();
            }

            if ((flag & 0xff) != 0) {
                if (val.Length > 0)
                    val += " + ";

                Keys key = (Keys)(flag & 0xff);
                if (key >= Keys.D0 && key <= Keys.D9) //数字，不采用Key中的D0-D9形式
                    val += key - Keys.D0;
                else //非数字，直接采用Keys名
                    val += key.ToString();
            }

            return val;
        }

        public string name => m_name;
        public string group => m_group;
        public virtual Define.LayoutCellType cellType => Define.LayoutCellType.CELL_NONE;
        public virtual string shortCutInvokeType => "click";

        public void initShortCut(Keys key, bool ctrl, bool alt, bool shift) {
            Native.initShortCut(m_name, this.cellType, (int)key, ctrl, alt, shift);
        }

        public string shortCutTip {
            get => m_shortCutTip;
            set => m_shortCutTip = value;
        }

        public string icon {
            get => m_icon;
            set {
                m_icon = value != null ? value : "";
                m_iconImg = null;
            }
        }

        public Image iconImg {
            get {
                if (m_iconImg == null) {
                    if (m_icon.IndexOf("res:") == 0) {
                        m_iconImg = Res.dpiImage(m_icon.Substring(4));
                    }
                    else {
                        byte[] bytes = Convert.FromBase64String(m_icon);
                        var ms = new MemoryStream(bytes, 0, bytes.Length);
                        m_iconImg = Image.FromStream(ms, true);
                    }
                }

                return m_iconImg;
            }
        }

        public Image iconImgRevs {
            get {
                if (m_iconImgRevs == null) {
                    if (m_icon.IndexOf("res:") == 0) {
                        m_iconImgRevs = Res.dpiImage(m_icon.Substring(4) + "_revs");
                    }
                    else {
                        byte[] bytes = Convert.FromBase64String(m_icon.Replace(".", "_revs."));
                        var ms = new MemoryStream(bytes, 0, bytes.Length);
                        m_iconImgRevs = Image.FromStream(ms, true);
                    }
                }

                return m_iconImgRevs;
            }
        }

        public string barStyle => m_barStyle;
        public string tip => m_tip;
        public virtual bool userUnit => true;
        public virtual bool isChecked => true;
        public virtual bool recordable => true;

        public virtual bool isValid() {
            return !String.IsNullOrEmpty(Native.documentPath());
        }

        public virtual void onClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {

        }

        public virtual void onDoubleClick(object sender, EventArgs e, Keys ctrlKeys, DObject paras) {
            this.onClick(sender, e, ctrlKeys, paras);
        }

        public virtual void onCheck(bool check, object sender, EventArgs e, Keys ctrlKeys, DObject paras) {

        }

        public static void invokeCheck(string funcName, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            FuncUnit unit = FuncUnit.get(funcName);
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "check", sender, e, ctrlKeys, paras);

            unit.onCheck(!unit.isChecked, sender, e, ctrlKeys, paras);
        }

        public static void invokeCheck(FuncUnit unit, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "check", sender, e, ctrlKeys, paras);

            unit.onCheck(!unit.isChecked, sender, e, ctrlKeys, paras);
        }

        public static void invokeClick(string funcName, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            FuncUnit unit = FuncUnit.get(funcName);
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "click", sender, e, ctrlKeys, paras);
            
            unit.onClick(sender, e, ctrlKeys, paras);
        }

        public static void invokeClick(FuncUnit unit, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "click", sender, e, ctrlKeys, paras);
            
            unit.onClick(sender, e, ctrlKeys, paras);
        }

        public static void invokeDoubleClick(string funcName, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            FuncUnit unit = FuncUnit.get(funcName);
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "double_click", sender, e, ctrlKeys, paras);
            
            unit.onDoubleClick(sender, e, ctrlKeys, paras);
        }

        public static void invokeDoubleClick(FuncUnit unit, object sender, EventArgs e, Keys ctrlKeys, DObject paras, bool record = true) {
            if (unit == null || !unit.isValid())
                return;

            if (!record)
                m_recentCall = null;
            else if (unit.recordable)
                m_recentCall = new FuncUnitCall(unit, "double_click", sender, e, ctrlKeys, paras);
            
            unit.onDoubleClick(sender, e, ctrlKeys, paras);
        }

        public static void invokeRecentCall() {
            if (m_recentCall == null)
                return;

            m_recentCall.invoke();
        }
    }
}
