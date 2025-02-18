using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace LoongCAD.STC {
    public class Define
    {
        public static string ToStr(object enumVal) {
            return enumVal.ToString();
        }

        public static bool IS_RUNTIME = false;

        public const int MAINFORM_MIN_X = 800;
        public const int MAINFORM_MIN_Y = 600;
        public const int MAINFORM_BORDER_MARGIN = 4;

        public static readonly int[] PrimeNumbers = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
                                                73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
                                                179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
                                                283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
                                                419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
                                                547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
                                                661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809,
                                                811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941,
                                                947, 953, 967, 971, 977, 983, 991, 997 };

        public enum ErrorCode
        {
            ERR_NONE = 0,
            ERR_DOC_LOCK,
            ERR_COMP_EDITOR_INVALID,
            ERR_EXPORT_FAILED,
            ERR_IMPORT_FAILED,
            ERR_DIFFERENT_PARENT_ASS
        };

        public enum RevertableType
        {
            RVT_NONE = 0,
            RVT_BEGIN, RVT_END,
            //3d
            RVT_OBJ_CREATE, RVT_OBJ_DELETE,
            RVT_OBJ_MODIFY,
            RVT_MULTI_OBJS_MODIFY,
            RVT_MULTI_OBJS_TRANSFORM,
            RVT_OBJ_GUID_MODIFY,
            RVT_OBJ_CPTS_MODIFY,
            RVT_OBJ_MATRIX_MODIFY,
            RVT_OBJ_PARENT_MODIFY,
            RVT_OBJ_PREBUILT_MODIFY,
            RVT_OBJ_TMP_SHOW_IN_VIS_DEPTH,
            RVT_PREBUILT_MAIN_MODIFY,
            RVT_PREBUILT_ROOT_ADD,
            RVT_PREBUILT_ROOT_REMOVE,
            RVT_CTRLPT_POS_MODIFY,
            RVT_CTRLPT_OWNER_MODIFY,
            RVT_PART_ASSEMLBY_MODIFY,
            RVT_COMPEDIT_VARS_MODIFY,
            //drawing
            RVT_DRAWING_CREATE,
            RVT_DRAWING_DELETE,
            RVT_DRAWING_MODIFY,
            RVT_DRELE_CREATE,
            RVT_DRELE_DELETE,
            RVT_DRELE_MODIFY,
            RVT_DRELE_MATRIX_MODIFY,
            RVT_DRELE_MATRIX_LOCAL_MODIFY,
            RVT_DRELE_PARENT_MODIFY,
            //view
            RVT_VIEW_HIDE
        };

        public enum DisplayRatio
        {
            DISP_RATIO_NONE = 0,
            DISP_RATIO_AUTO,
            DISP_RATIO_100,
            DISP_RATIO_125,
            DISP_RATIO_150,
            DISP_RATIO_175,
            DISP_RATIO_200
        };

        public enum DocumentSaveResult
        {
            DOC_SAVE_NONE = 0,
            DOC_SAVE_SUCCESS,
            DOC_SAVE_FAILED,
            DOC_SAVE_LOCKED
        };

        public enum ObjectType : short
        {
            OBJ_UNKNOWN = 0,

            OBJ_CTRL_POINT_BEGIN,
            OBJ_CTRL_POINT,
            OBJ_CTRL_POINT_GRID_AXIS,
            OBJ_CTRL_POINT_CHAMFER,
            OBJ_CTRL_POINT_TZPLATE,
            OBJ_CTRL_POINT_END,

            OBJ_TF_BEGIN = 14,
            OBJ_GIZMO3D,
            OBJ_VIEW_CLIP_PLANE,

            OBJ_GRID_AXIS,

            OBJ_CUTTER_BEGIN,
            OBJ_CUT_PLANE,
            OBJ_PART_BEGIN,

            OBJ_STEEL_PART_BEGIN,
            OBJ_STEEL_PART_BEAM, OBJ_STEEL_PART_POLY_BEAM, OBJ_STEEL_PART_PLATE,
            OBJ_STEEL_PART_COLUMN, OBJ_STEEL_PART_TZPLATE, OBJ_STEEL_PART_TWIST_BEAM, OBJ_STEEL_PART_RESERVED3,
            OBJ_STEEL_PART_RESERVED4, OBJ_STEEL_PART_RESERVED5, OBJ_STEEL_PART_RESERVED6, OBJ_STEEL_PART_RESERVED7,
            OBJ_STEEL_PART_END,

            OBJ_CONCRETE_PART_BEGIN,
            OBJ_CONCRETE_PART_PAD_FOOTING, OBJ_CONCRETE_PART_BEAM,
            OBJ_CONCRETE_PART_STRIP_BEAM, OBJ_CONCRETE_PART_SLAB, OBJ_CONCRETE_PART_PANEL,
            OBJ_CONCRETE_PART_COLUMN, OBJ_CONCRETE_PART_RESERVED1, OBJ_CONCRETE_PART_RESERVED2, OBJ_CONCRETE_PART_RESERVED3,
            OBJ_CONCRETE_PART_RESERVED4, OBJ_CONCRETE_PART_RESERVED5, OBJ_CONCRETE_PART_RESERVED6, OBJ_CONCRETE_PART_RESERVED7,
            OBJ_CONCRETE_PART_END,

            OBJ_PART_RESERVED0, OBJ_PART_RESERVED1, OBJ_PART_RESERVED2, OBJ_PART_RESERVED3,
            OBJ_PART_RESERVED4, OBJ_PART_RESERVED5, OBJ_PART_RESERVED6, OBJ_PART_RESERVED7,

            OBJ_PART_END,

            OBJ_CUTTER_RESERVED0, OBJ_CUTTER_RESERVED1, OBJ_CUTTER_RESERVED2, OBJ_CUTTER_RESERVED3,
            OBJ_CUTTER_RESERVED4, OBJ_CUTTER_RESERVED5, OBJ_CUTTER_RESERVED6, OBJ_CUTTER_RESERVED7,

            OBJ_CUTTER_END,

            OBJ_DETAIL_BEGIN,
            OBJ_BOLT,
            OBJ_WELD,
            OBJ_SURF_TREATMENT,

            OBJ_RFBAR_BEGIN,
            OBJ_RFBAR_POLY,
            OBJ_RFBAR_CURVE,
            OBJ_RFBAR_CIRCLE,
            OBJ_RFBAR_RESERVED0,
            OBJ_RFBAR_RESERVED1,
            OBJ_RFBAR_END,

            OBJ_DETAIL_RESERVED2, OBJ_DETAIL_RESERVED3,
            OBJ_DETAIL_RESERVED4, OBJ_DETAIL_RESERVED5, OBJ_DETAIL_RESERVED6, OBJ_DETAIL_RESERVED7,

            OBJ_DETAIL_END,

            OBJ_HELPER_BEGIN,
            OBJ_LINE_HELPER, OBJ_CIRCLE_HELPER, OBJ_QUAD_HELPER, OBJ_POINT_HELPER,
            OBJ_CAD_REF,
            OBJ_HELPER_RESERVED0, OBJ_HELPER_RESERVED1, OBJ_HELPER_RESERVED2, OBJ_HELPER_RESERVED3,
            OBJ_HELPER_RESERVED4, OBJ_HELPER_RESERVED5, OBJ_HELPER_RESERVED6, OBJ_HELPER_RESERVED7,
            OBJ_HELPER_END,

            OBJ_MEASURE_BEGIN,
            OBJ_MEASURE_DIST,
            OBJ_MEASURE_ANGL,
            OBJ_MEASURE_ARC,
            OBJ_MEASURE_END,

            OBJ_RESERVED8,
            OBJ_GRID,
            OBJ_BOLT_GROUP,
            OBJ_ASSEMBLY_OLD,

            OBJ_COMPONENT_BEGIN,
            OBJ_COMPONENT_CONNECTION,
            OBJ_COMPONENT_DETAIL,
            OBJ_COMPONENT_SEAM,
            OBJ_COMPONENT_CUSTOM,
            OBJ_COMPONENT_RESERVED0, OBJ_COMPONENT_RESERVED1, OBJ_COMPONENT_RESERVED2, OBJ_COMPONENT_RESERVED3,
            OBJ_COMPONENT_RESERVED4, OBJ_COMPONENT_RESERVED5, OBJ_COMPONENT_RESERVED6, OBJ_COMPONENT_RESERVED7,
            OBJ_COMPONENT_END,

            OBJ_RFBAR_GROUP_BEGIN,
            OBJ_RFBAR_POLY_GROUP,
            OBJ_RFBAR_CURVE_GROUP,
            OBJ_RFBAR_CIRCLE_GROUP,
            OBJ_RFBAR_MESH_GROUP,
            OBJ_RFBAR_GROUP_RESERVED0,
            OBJ_RFBAR_GROUP_END,

            OBJ_RESERVED9,
            OBJ_RESERVED10,
            OBJ_RESERVED11,
            OBJ_RESERVED12,
            OBJ_RESERVED13,
            OBJ_RESERVED14,

            OBJ_ASSEMBLY,
            OBJ_CAST_UNIT,

            OBJ_PART_TWIST_BEAM_SLICE,
            OBJ_PART_CUSTOM_BEAM_SLICE,

            OBJ_VAR_PROXY_BEGIN = 30000,

            OBJ_VAR_PROXY_FIX_DIST,
            OBJ_VAR_PROXY_SNAP_PLANE,

            OBJ_VAR_PROXY_END,

            OBJ_TF_END,

            OBJ_TYPE_END
        };

        //Geo Tri Flags
        [Flags]
        public enum GeoTriFlags : uint {
            NONE = 0,
            OUT = 1u << 0,//外部
            IN = 1u << 1,//内部
            BOUND = 1u << 2, //外包围面
            END = 1u << 3,//端部
            TOP = 1u << 4, //顶面
            BOTTOM = 1u << 5, //底面
            FRONT = 1u << 6,//前面
            BACK = 1u << 7,//后面
            HEAD = 1u << 8,//头部
            TAIL = 1u << 9, //尾部
            PLANE_CUT = 1u << 10, //切割面
            WEB = 1u << 11,//腹板
            FLANGE = 1u << 12, //翼板
            EDGE_FOLD = 1u << 13,//弯折
            WELD_GROOVE = 1u << 14, //焊接坡口
            GEO_CUT = 1u << 15, //几何切割
            CAHMFER_LINE = 1u << 16, //直线倒角
            CAHMFER_ROUND = 1u << 17, //圆角
            CAHMFER_INV_ROUND = 1u << 18, //凹角
            CAHMFER_ARC = 1u << 19, //弧线
            CAHMFER_ORTHO = 1u << 20, //垂直倒角
            CAHMFER_PARA = 1u << 21, //平行倒角
            CAHMFER_PARA_ROUND = 1u << 22, //平行圆角倒角
            HOLE = 1u << 23, //洞
            LEFT = 1u << 24,
            RIGHT = 1u << 25,
            CIRCULAR = 1u << 26,
            SECTION = 1u << 27,
            DISABLE_OPER = 1u << 28,
        };

        //Object Common Flags
        public const uint OBJ_FLAG_ENABLE = 1;
        public const uint OBJ_FLAG_SAVE_ENABLE = 1 << 1;
        public const uint OBJ_FLAG_USER_DEL_ENABLE = 1 << 2;
        public const uint OBJ_FLAG_USER_COPY_ENABLE = 1 << 3;
        public const uint OBJ_FLAG_USER_MOVE_ENABLE = 1 << 4;
        public const uint OBJ_FLAG_MAG = 1 << 5;
        public const uint OBJ_FLAG_REVERTABLE = 1 << 6;
        public const uint OBJ_FLAG_LOCK = 1 << 7;
        public const uint OBJ_FLAG_COMP_TEMP = 1 << 8;

        //Object Side
        public enum ObjectSide : sbyte { SIDE_LEFT = 0, SIDE_RIGHT, SIDE_TOP, SIDE_BOTTOM, SIDE_FRONT, SIDE_BACK, SIDE_UP, SIDE_DOWN, SIDE_MIDDLE, SIDE_HEAD, SIDE_TAIL, SIDE_MIDDLE_H, SIDE_MIDDLE_V, SIDE_MIDDLE_DEPTH, SIDE_NONE };

        // Away side. Far side / near side.
        [Flags]
        public enum AwaySide : sbyte { NONE = 0, NEAR = 1, FAR = 2, BOTH = 3 };

        //Space
        public enum SpacePlane : sbyte { PLANE_XY = 0, PLANE_YZ, PLANE_ZX };
        public enum SpaceAxis : sbyte { AXIS_X = 0, AXIS_Y, AXIS_Z, AXIS_NX, AXIS_NY, AXIS_NZ, AXIS_NONE };

        //Unit
        public enum Unit { UNIT_AUTO = 0, UNIT_MM, UNIT_CM, UNIT_M, UNIT_CM_M, UNIT_INCH, UNIT_FT, UNIT_FT_IN, UNIT_DEG, UNIT_RAD };

        //Direction
        public enum Direction { DIR_NONE = 0, DIR_N, DIR_W, DIR_S, DIR_E, DIR_NW, DIR_NE, DIR_SW, DIR_SE };

        //ObjectRole
        public enum ObjectRole
        {
            OBJ_ROLE_NONE = 0,
            OBJ_ROLE_MAIN,
            OBJ_ROLE_SUB,
        };

        //Part Usage
        public enum PartUsage
        {
            PART_USAGE_NONE,
            PART_USAGE_BOTTOM,
            PART_USAGE_KNEEBRACE,
            PART_USAGE_SARKING,
            PART_USAGE_BACK,
            PART_USAGE_CLAMP,
            PART_USAGE_WEB,
            PART_USAGE_RESERVED0,
            PART_USAGE_RESERVED1,
            PART_USAGE_RESERVED2,
            PART_USAGE_RESERVED3,
            PART_USAGE_RESERVED4,
            PART_USAGE_RESERVED5,
            PART_USAGE_RESERVED6,
            PART_USAGE_RESERVED7,
            PART_USAGE_OTHER1,
            PART_USAGE_OTHER2,
            PART_USAGE_OTHER3,
        };

        //Mouse
        public enum MouseCursor
        {
            CURSOR_NONE = 0, CURSOR_ARROW, CURSOR_HAND, CURSOR_CROSS, CURSOR_MOVE,
            CURSOR_SIZE_WE, CURSOR_SIZE_NS, CURSOR_SIZE_NWSE, CURSOR_SIZE_NESW, CURSOR_SIZE_ALL
        };

        //Var type
        public enum VarType
        {
            VAR_NONE = 0,
            VAR_TEXT,
            VAR_DOUBLE,
            VAR_LONG,
            VAR_TEXT_ARRAY,
            VAR_DOUBLE_ARRAY,
            VAR_LONG_ARRAY,
            VAR_FIX_DIST,
            VAR_REF_DIST,
            VAR_SNAP_PLANE,
            VAR_MTRL,
            VAR_PROFILE,
            VAR_BOLT_SIZE,
            VAR_FIX_DIST_TO_PLANE
        };

        //Surf Treatment
        public enum SurfTreatType
        {
            SURF_NONE = 0,
            SURF_CONCRETE_FINISH,
            SURF_SPECIAL_MIX,
            SURF_TILE_SURFACE,
            SURF_STEEL_FINISH
        };

        public enum SurfPattern
        {
            SURF_PATTERN_NONE = 0,
            SURF_PATTERN_1_1,
            SURF_PATTERN_1_2,
            SURF_PATTERN_1_3,
            SURF_PATTERN_1_4,
            SURF_PATTERN_BASKETWEAVE,
            SURF_PATTERN_HERRINGBONE
        };

        //Corner
        public enum CornerCutType : sbyte
        {
            CORNER_CUT_NONE = 0,
            CORNER_CUT_LINE,
            CORNER_CUT_ROUND,
            CORNER_CUT_INV_ROUND,
            CORNER_CUT_ARC,
            CORNER_CUT_ORTHO,
            CORNER_CUT_PARA,
            CORNER_CUT_PARA_ROUND
        };

        //Corner position
        public enum CornerPosition : sbyte
        {
            CORNER_TOP_LEFT = 0,
            CORNER_TOP_RIGHT = 1,
            CORNER_BOTTOM_LEFT = 2,
            CORNER_BOTTOM_RIGHT = 3,
        };

        //Component Detail Type
        public enum CompDetailType : sbyte
        {
            COMP_DETAIL_NONE = 0,
            COMP_DETAIL_END,
            COMP_DETAIL_INTERMEDIATE,
            COMP_DETAIL_INTERMEDIATE_REV,
        };

        //Component Detail Position
        public enum CompDetailPos : sbyte
        {
            COMP_DETAIL_POS_NONE = 0,
            COMP_DETAIL_POS_CENTER,
            COMP_DETAIL_POS_LEFT,
            COMP_DETAIL_POS_RIGHT,
            COMP_DETAIL_POS_BOTTOM,
            COMP_DETAIL_POS_TOP,
            COMP_DETAIL_POS_BOTTOMLEFT,
            COMP_DETAIL_POS_BOTTOMRIGHT,
            COMP_DETAIL_POS_TOPLEFT,
            COMP_DETAIL_POS_TOPRIGHT,
            COMP_DETAIL_POS_OVER_CENTER,
            COMP_DETAIL_POS_OVER_LEFT,
            COMP_DETAIL_POS_OVER_RIGHT,
        };

        //Component UI Usage
        public enum CompUIUsage : int
        {
            COMP_UI_USAGE_NONE = 0,
            COMP_UI_USAGE_SET_DEFAULT,
            COMP_UI_USAGE_SET_INSTANCE,
        }

        //Component Ctrl Defaults
        public const double COMP_UI_DEF_DOUBLE = double.MaxValue;
        public const long COMP_UI_DEF_LONG = long.MinValue;
        public const int COMP_UI_DEF_INT = int.MinValue;
        public const int COMP_UI_DEF_ENUM = int.MinValue;
        public const string COMP_UI_DEF_OTHER = "$default";
        //layout
        public enum LayoutCellType { CELL_NONE = 0, CELL_VIEW, CELL_DRAWING, CELL_DR_TEMPLATE };

        //view
        public enum ViewType { VIEW_3D = 0, VIEW_2D };
        public enum ViewProjMode { VIEWPROJ_PERSPECTIVE = 0, VIEWPROJ_ORTHO };
        public enum ViewRenderMode
        {
            RENDERMODE_NONE = 0, RENDERMODE_EDGEFACE,
            RENDERMODE_WIREFRAME, RENDERMODE_SHADED_WIREFRAME,
            RENDERMODE_HIDDEN_LINES, RENDERMODE_SEL_ONLY
        };
        public enum ViewRenderQuality : sbyte
        {
            RENDERQUALITY_NONE = 0, RENDERQUALITY_FAST, RENDERQUALITY_EXACT, RENDERQUALITY_DETAIL,
            RENDERQUALITY_REF_LINE, RENDERQUALITY_SLOT_HOLE
        };
        public enum ViewNoRange { VIEWNORANGE_ALL = 0, VIEWNORANGE_FIRST, VIEWNORANGE_LAST };
        public enum ViewLod { VIEWLOD_NONE = 0, VIEWLOD_LOW, VIEWLOD_MIDDLE, VIEWLOD_HIGH };
        public enum ViewFocusMode { VIEW_FOCUS_NONE = 0, VIEW_FOCUS_KEEP_STYLE, VIEW_FOCUS_VIS_ONLY };

        //view manager
        public enum ViewManagerMode { VIEWMGR_NONE = 0, VIEWMGR_3D, VIEWMGR_DRAWING, VIEWMGR_DRAWING_TEMPLATE };

        //view layout type
        public enum ViewLayoutType
        {
            VIEWLAYOUT_NONE = 0,
            VIEWLAYOUT_SINGLE_VIEW,
            VIEWLAYOUT_TILED_VIEW,
            VIEWLAYOUT_WINDOWED_VIEW,
            VIEWLAYOUT_SINGLE_DRAWING,
            VIEWLAYOUT_TILED_DRAWING,
            VIEWLAYOUT_WINDOWED_DRAWING
        };

        //edit
        public enum MoveResult
        {
            MOVE_RESULT_NONE = 0,
            MOVE_RESULT_SUCCESS,
            MOVE_RESULT_ENV_FAILED,
            MOVE_RESULT_OBJ_LOCKED,
            MOVE_RESULT_USER_CANCELED
        };

        public enum CopyResult
        {
            COPY_RESULT_NONE = 0,
            COPY_RESULT_SUCCESS,
            COPY_RESULT_FAILED,
            COPY_RESULT_ENV_FAILED,
            COPY_RESULT_OBJ_LOCKED,
            COPY_RESULT_USER_CANCELED
        };

        //view display filter item type
        public enum ObjectFilterType
        {
            OBJ_FILTER_TYPE_NONE = 0,
            OBJ_FILTER_TYPE_PART,
            OBJ_FILTER_TYPE_COMPONENT,
            OBJ_FILTER_TYPE_BOLT,
            OBJ_FILTER_TYPE_WELD,
            OBJ_FILTER_TYPE_RFBAR,
            OBJ_FILTER_TYPE_ASSEMBLY,
            OBJ_FILTER_TYPE_OBJECT
        };

        public enum DRViewFilterType
        {
            DRVIEW_FILTER_TYPE_NONE = 0,
            DRVIEW_FILTER_TYPE_PART,
            DRVIEW_FILTER_TYPE_COMPONENT,
            DRVIEW_FILTER_TYPE_BOLT,
            DRVIEW_FILTER_TYPE_WELD,
            DRVIEW_FILTER_TYPE_RFBAR,
            DRVIEW_FILTER_TYPE_ASSEMBLY,
            DRVIEW_FILTER_TYPE_TEMPLATE,
            DRVIEW_FILTER_TYPE_REF_OBJECT,
            DRVIEW_FILTER_TYPE_LEGAL_REGION,
            DRVIEW_FILTER_TYPE_OBJECT_TYPE,
            DRVIEW_FILTER_TYPE_OBJECT
        };

        public enum CompareOper
        {
            COMPARE_NONE = 0,
            COMPARE_EQUAL,
            COMPARE_NOT_EQUAL,
            COMPARE_BEGIN_WITH,
            COMPARE_BEGIN_NOT_WITH,
            COMPARE_END_WITH,
            COMPARE_END_NOT_WITH,
            COMPARE_CONTAIN,
            COMPARE_NOT_CONTAIN,
            COMPARE_GREATER,
            COMPARE_GREATER_EQUAL,
            COMPARE_LESS,
            COMPARE_LESS_EQUAL
        };

        public enum LogicOper
        {
            LOGIC_NONE = 0,
            LOGIC_AND,
            LOGIC_OR
        };

        //CutPlane mode
        public enum CutPlaneMode : sbyte
        {
            CUT_PLANE_NONE = 0,
            CUT_PLANE_CUT,
            CUT_PLANE_ALIGNMENT
        };

        //Clash
        public enum ClashStatus
        {
            CLASH_STATUS_NONE = 0,
            CLASH_STATUS_OK,
            CLASH_STATUS_CLASH,
        };

        public enum ClashType
        {
            CLASH_TYPE_NONE = 0,
            CLASH_TYPE_CUT_THROUGH,
            CLASH_TYPE_EXACT_MATCH,
            CLASH_TYPE_CONTAIN,
            CLASH_TYPE_CLASH,
            CLASH_TYPE_SELF_CLASH
        };

        public enum ClashStep
        {
            CLASH_STEP_NONE = 0,
            CLASH_STEP_ASSIGN,
            CLASH_STEP_FIX,
            CLASH_STEP_APPROVE,
            CLASH_STEP_IGNORE,
            CLASH_STEP_REOPEN
        };

        public enum ClashPriority
        {
            CLASH_PRIORITY_NONE = 0,
            CLASH_PRIORITY_LOW,
            CLASH_PRIORITY_MEDIUM,
            CLASH_PRIORITY_HIGH
        };

        //Number
        public enum NumberMode
        {
            NUMBER_MODE_NONE = 0,
            NUMBER_MODE_COMPARE_OLD,
            NUMBER_MODE_ASSIGN_NEW,
            NUMBER_MODE_KEEP_UNCHANGED,
        };

        public enum NumberAssSort
        {
            NUMBER_ASS_SORT_NONE = 0,
            NUMBER_ASS_SORT_X,
            NUMBER_ASS_SORT_Y,
            NUMBER_ASS_SORT_Z,
            NUMBER_ASS_SORT_MAIN_USR,
            NUMBER_ASS_SORT_USR,
        };

        public enum ArraySortOrder
        {
            SORT_ORDER_NONE = 0,
            SORT_ORDER_ASCEND,
            SORT_ORDER_DESCEND,
        };


        //drawing
        public enum DrawingType { DRAW_UNKNOWN = 0, DRAW_SINGLE_PART, DRAW_ASSEMBLY, DRAW_MULTIPLE, DRAW_ARRANGEMENT };

        public enum MasterDrawingType
        {
            MASTER_DRAWING_TYPE_UNKNOWN = 0,
            MASTER_DRAWING_TYPE_SAVED = 1,
            MASTER_DRAWING_TYPE_CLONE,
            MASTER_DRAWING_TYPE_RULE_SET,
        };

        public enum DRViewType
        {
            DRVIEW_NONE = 0,
            DRVIEW_2D,
            DRVIEW_3D
        };

        public enum DRCoordSystemType
        {
            DRCOORDSYS_TYPE_LOCAL = 0,
            DRCOORDSYS_TYPE_MODEL = 1,
            DRCOORDSYS_TYPE_ORIENT = 2,
            DRCOORDSYS_TYPE_HORIZON_BRACE = 3,
            DRCOORDSYS_TYPE_VERTICAL_BRACE = 4,
            DRCOORDSYS_TYPE_FIXED = 5,
        };

        public enum DRViewSide
        {
            DRVIEW_SIDE_NONE = 0,
            DRVIEW_SIDE_TOP,
            DRVIEW_SIDE_BOTTOM,
            DRVIEW_SIDE_FRONT,
            DRVIEW_SIDE_BACK,
            DRVIEW_SIDE_HEAD,
            DRVIEW_SIDE_END,
            DRVIEW_SIDE_SECTION,
            DRVIEW_SIDE_DETAIL,
            DRVIEW_SIDE_CURVE_SECTION,
            DRVIEW_SIDE_3D,
            DRVIEW_SIDE_RESERVED2,
            DRVIEW_SIDE_RESERVED3,
        };

        public enum DRViewUnfoldType
        {
            DRVIEW_UNFOLD_NONE = 0,
            DRVIEW_UNFOLD_HORIZON = 1,
            DRVIEW_UNFOLD_VERTICAL = 2,
            DRVIEW_UNFOLD_BOTH = 3
        };

        public enum DRPartRepresentation
        {
            DRPART_REPRESENT_OUTLINE = 0,
            DRPART_REPRESENT_EXACT,
            DRPART_REPRESENT_WORKSHOP_FORM,
            DRPART_REPRESENT_SYMBOL,
            DRPART_REPRESENT_SYMBOL_WITH_PARTIAL_PROFILE,
            DRPART_REPRESENT_BOUNDING_BOX,
            DRPART_REPRESENT_BASE_BOX
        };

        public enum DRBoltStyle
        {
            DRBOLT_NONE = 0,
            DRBOLT_SOLID,
            DRBOLT_EXACT,
            DRBOLT_SYMBOL1,
            DRBOLT_SYMBOL2,
            DRBOLT_SYMBOL3,
        };

        public enum DRMarkVisible
        {
            DRMARK_VISIBLE_NONE = 0,
            DRMARK_VISIBLE_DISTRIBUTED,
            DRMARK_VISIBLE_PREFERRED,
            DRMARK_VISIBLE_ALWAYS,
        };

        public enum DRElementType
        {
            DRELE_UNKNOWN = 0,
            DRELE_ELEMENT,

            DRELE_VIEW_BEGIN,
            DRELE_VIEW_PART,
            DRELE_VIEW_ASSEMBLY,
            DRELE_VIEW_DRAWING,
            DRELE_VIEW_ARRANGEMENT,
            DRELE_VIEW_LAYOUT,
            DRELE_VIEW_RESERVED1,
            DRELE_VIEW_RESERVED2,
            DRELE_VIEW_RESERVED3,
            DRELE_VIEW_END,

            DRELE_PART,

            DRELE_DETAIL_BEGIN,
            DRELE_BOLT_GROUP,
            DRELE_WELD,
            DRELE_DETAIL_RESREVED1,
            DRELE_DETAIL_RESREVED2,
            DRELE_DETAIL_RESREVED3,
            DRELE_DETAIL_END,

            DRELE_DIM_DEF_POINT,

            DRELE_DIM_BEGIN,
            DRELE_DIM_NORMAL,
            DRELE_DIM_ANGL,
            DRELE_DIM_ARC_RADIUS,
            DRELE_DIM_ARC_LEN,
            DRELE_DIM_ARC_LEN_RADIATE,
            DRELE_DIM_COG,
            DRELE_DIM_RESERVED1,
            DRELE_DIM_RESERVED2,
            DRELE_DIM_RESERVED3,
            DRELE_DIM_END,

            DRELE_SHAPE_BEGIN,
            DRELE_SHAPE_LINE,
            DRELE_SHAPE_RECT,
            DRELE_SHAPE_CIRCLE,
            DRELE_SHAPE_ARC,
            DRELE_SHAPE_POLY_LINE,
            DRELE_SHAPE_POLYGON,
            DRELE_SHAPE_CLOUD,
            DRELE_SHAPE_RESERVED0,
            DRELE_SHAPE_RESERVED1,
            DRELE_SHAPE_RESERVED2,
            DRELE_SHAPE_RESERVED3,
            DRELE_SHAPE_END,

            DRELE_TEXT,

            DRELE_SYMBOL_BEGIN,
            DRELE_SYMBOL_SECTION,
            DRELE_SYMBOL_DETAIL,
            DRELE_SYMBOL_CURVE_SECTION,
            DRELE_SYMBOL_TAG,
            DRELE_SYMBOL_RESERVED2,
            DRELE_SYMBOL_RESERVED3,
            DRELE_SYMBOL_END,

            DRELE_MARK_BEGIN,
            DRELE_PART_MARK,
            DRELE_BOLT_MARK,
            DRELE_SURF_TREATMENT_MARK,
            DRELE_CONNECTION_MARK,
            DRELE_REBAR_MARK,
            DRELE_LEVEL_MARK,
            DRELE_ASSOCIATE_NOTE,
            DRELE_MARK_RESERVED3,
            DRELE_MARK_END,

            DRELE_TABLE,
            DRELE_VALUE,
            DRELE_TEMPLATE,
            DRELE_WELD_MARK,
            DRELE_RESERVED0,
            DRELE_RESERVED1,
            DRELE_GRID_AXIS,
            DRELE_GRID,
            DRELE_POINT,
            DRELE_RESERVED5,
            DRELE_RESERVED6,
            DRELE_RESERVED7,

            DRELE_CTRL_POINT_BEGIN,
            DRELE_CTRL_POINT, // Endpoint control point.
            DRELE_CTRL_POINT_DIAMOND, // Arc, circle, bulge control point.
            DRELE_CTRL_POINT_RESERVE0,
            DRELE_CTRL_POINT_RESERVE1,
            DRELE_CTRL_POINT_RESERVE2,
            DRELE_CTRL_POINT_RESERVE3,
            DRELE_CTRL_POINT_END,

            DRELE_HELPER_BEGIN,
            DRELE_HELPER_POINTS,
            DRELE_HELPER_RESERVED0,
            DRELE_HELPER_RESERVED1,
            DRELE_HELPER_RESERVED2,
            DRELE_HELPER_RESERVED3,
            DRELE_HELPER_END,

            DRELE_REF_BEGIN,
            DRELE_REF_CAD,
            DRELE_LINK_URL,
            DRELE_LINK_DRAWING,
            DRELE_REF_RESERVED2,
            DRELE_REF_RESERVED3,
            DRELE_REF_RESERVED4,
            DRELE_REF_RESERVED5,
            DRELE_REF_END,
        };

        public enum DRElementAnchor
        {
            DRELE_ANCHOR_NONE = 0,
            DRELE_ANCHOR_LEFT,
            DRELE_ANCHOR_MIDDLE,
            DRELE_ANCHOR_RIGHT,
            DRELE_ANCHOR_BOTTOM,
            DRELE_ANCHOR_TOP
        };

        public enum DRLineType
        {
            DRLINE_TYPE0 = 0, //____________________
            DRLINE_TYPE1, //_ _ _ _ _
            DRLINE_TYPE2, //__ __ __ __
            DRLINE_TYPE3, //__ _ __ _ __ _
            DRLINE_TYPE4 //........
        };

        public enum DRFillType
        {
            DRFILL_TYPE0 = 0, // None
            DRFILL_TYPE1, // ███████
            DRFILL_TYPE2, // [brick tile]
            DRFILL_TYPE3, // [snow tile]
            DRFILL_TYPE4  // [slash tile]
        };

        public enum DRDimLinearStyle
        {
            DRDIM_LINEAR_STYLE_NONE = -1,
            DRDIM_LINEAR_STYLE0 = 0, //|--htext--|
            DRDIM_LINEAR_STYLE1,  //|-----vtext|
            DRDIM_LINEAR_STYLE2, //|--htext--vtext|
            DRDIM_LINEAR_STYLE3, //|<-htext   htext->|htext->|   htext->|htext->|
            DRDIM_LINEAR_STYLE4, //|<-htext   htext->|--htext--|   htext->|--htext--|
            DRDIM_LINEAR_STYLE5, //|    vtext|--htext--vtext|   vtext|--htext--vtext|
            DRDIM_LINEAR_STYLE6, //|--htext------|--htext--|--htext--|--htext--|
                                    //|<-htext vtext|    vtext|    vtext|    vtext|
            DRDIM_LINEAR_STYLE7, //      vtext|
                                    //     --------
        };

        public enum DRDimAnglStyle
        {
            DRDIM_ANGL_STYLE_ANGL = 0,
            DRDIM_ANGL_STYLE_ANGL_BASIC,
            DRDIM_ANGL_STYLE_TRI,
            DRDIM_ANGL_STYLE_TRI_ANGL,
        };

        public enum DRDimSideMarkType
        {
            DRDIM_SIDE_MARK_TYPE_NONE = 0,
            DRDIM_SIDE_MARK_TYPE_SPEC = 1,
            DRDIM_SIDE_MARK_TYPE_AUTO = 2,
        }

        public enum DRDimSideMarkShape
        {
            DRDIM_SIDE_MARK_SHAPE_NONE = 0,
            DRDIM_SIDE_MARK_SHAPE_LEFT = 1,
            DRDIM_SIDE_MARK_SHAPE_BOTH = 2,
            DRDIM_SIDE_MARK_SHAPE_RIGHT = 3,
        };

        public enum DRTextType
        {
            DRTEXT_TYPE_0 = 0,  // Only text.
            DRTEXT_TYPE_1,      // With ref line.
            DRTEXT_TYPE_2,      // Parallel text.
            DRTEXT_TYPE_3,      // With standalone arrow-line.
        };

        public enum DRTextFrame
        {
            DRTEXT_FRAME0 = 0,
            DRTEXT_FRAME1, //____
            DRTEXT_FRAME2, //[]
            DRTEXT_FRAME3, //()
            DRTEXT_FRAME4,  //<>
            DRTEXT_FRAME5, //Circle
            DRTEXT_FRAME6, //Triangle
        };

        public enum DRTextAlignment
        {
            DRTEXT_ALIGN_NONE = 0,
            DRTEXT_ALIGN_LEFT,
            DRTEXT_ALIGN_RIGHT,
            DRTEXT_ALIGN_TOP,
            DRTEXT_ALIGN_BASELINE,
            DRTEXT_ALIGN_BOTTOM,
            DRTEXT_ALIGN_MIDDLE
        };

        public enum DRFrameType
        {
            DRFRAME_TYPE0 = 0,
            DRFRAME_TYPE1, //____
            DRFRAME_TYPE2, //[]
            DRFRAME_TYPE3, //()
            DRFRAME_TYPE4, //<>
            DRFRAME_TYPE5, //Circle
            DRFRAME_TYPE6, //Triangle
        };

        public enum DRArrowShape
        {
            DRARROW_NONE = 0, // -
            DRARROW_SLASH, // -/
            DRARROW_ARROW, // ->|
            DRARROW_SOLID_ARROW, // -|>
            DRARROW_SPHERE, // -o
            DRARROW_SOLID_SPHERE, // -(*)
            DRARROW_X, // -X
        };

        public enum DRMarkTagType
        {
            DRMARK_TAG_NONE = 0,
            DRMARK_TAG_FRAME_BEGIN,
            DRMARK_TAG_FRAME_END,
            DRMARK_TAG_PROPS,
            DRMARK_TAG_WRAP_LINE,
            DRMARK_TAG_SPACE,
            DRMARK_TAG_BACKSPACE,
        };

        public enum DRMarkLeaderLineType
        {
            DRMARK_LEADER_LINE_TYPE0 = 0,
            DRMARK_LEADER_LINE_TYPE1,
            DRMARK_LEADER_LINE_TYPE2,
            DRMARK_LEADER_LINE_TYPE3,
            DRMARK_LEADER_LINE_TYPE4,
            DRMARK_LEADER_LINE_TYPE5,
            DRMARK_LEADER_LINE_TYPE6,
            DRMARK_LEADER_LINE_TYPE7,
            DRMARK_LEADER_LINE_TYPE8,
            DRMARK_LEADER_LINE_TYPE9,
        };

        public enum DRNoteLeaderLineType
        {
            DRNOTE_LEADER_LINE_NONE = 0,
            DRNOTE_LEADER_WITH_LINE = 1,
            DRNOTE_LEADER_PARA_LINE = 2,
        };

        public enum DRMarkSymbolShape
        {
            DRMARK_SYMBOL_SHAPE_NONE = 0,
            DRMARK_SYMBOL_SHAPE_CIRCLE,
            DRMARK_SYMBOL_SHAPE_CIRCLE_LINE
        };

        public enum DRSectionMarkSymbolShape
        {
            DRSECTION_MARK_SYMBOL0 = 0,
            DRSECTION_MARK_SYMBOL1 = 1,
            DRSECTION_MARK_SYMBOL2 = 2,
            DRSECTION_MARK_SYMBOL3 = 3,
            DRSECTION_MARK_SYMBOL4 = 4,
            DRSECTION_MARK_SYMBOL5 = 5,
            DRSECTION_MARK_SYMBOL6 = 6,
            DRSECTION_MARK_SYMBOL7 = 7,
            DRSECTION_MARK_SYMBOL8 = 8,
        }

        public enum DRMarkPosition
        {
            DRMARK_POSITION_ABOVE_LINE = 0,                 ///< 线上
            DRMARK_POSITION_BELOW_LINE = 1,                 ///< 线下
            DRMARK_POSITION_SYMBOL_MIDDLE = 2,              ///< 符号中间
            DRMARK_POSITION_ABOVE_SYMBOL_CENTERLINE = 3,    ///< 符号中心线之上
            DRMARK_POSITION_BELOW_SYMBOL_CENTERLINE = 4     ///< 符号中心线之下
        }

        public enum DRMarkSide
        {
            DRMARK_SIDE_BOTH = 0,
            DRMARK_SIDE_LEFT = 1,
            DRMARK_SIDE_RIGHT = 2,
        }

        public enum DRMarkAutoRotate
        {
            DRMARK_AUTO_ROTATE_HORIZON = 0,     ///< 始终水平 (Always horizon)
            DRMARK_AUTO_ROTATE_ALWAYS = 1,      ///< 始终旋转 (Always rotate)
            DRMARK_AUTO_ROTATE_NO_VERTICAL = 2,  ///< 不要垂直旋转 (No rotate vertical)
            DRMARK_AUTO_ROTATE_ORTHO = 3        ///< 始终正交 (Always orthogonal)
        }

        public enum DRWeldMarkVisible
        {
            DRMARK_WELD_VISIBLE_NONE = 0,
            DRMARK_WELD_VISIBLE_SITE,
            DRMARK_WELD_VISIBLE_WORKSHOP,
            DRMARK_WELD_VISIBLE_BOTH
        }

        public enum DRAxisVisible
        {
            /// 不可见，删除视图中的轴线元素
            /// 注：仅对图纸层级、视图层级轴线属性有效
            DRAXIS_VISIBLE_OFF = 0,
            /// 对图纸层级、视图层级、对象层级轴线属性有效
            DRAXIS_VISIBLE_ON = 1,
            /// 仅将主视图中的轴线属性改为可见，非主视图中的轴线属性改为不可见（剖面视图、端部视图、细部视图为非主视图）
            /// 注：仅对视图层级、对象层级轴线属性有效
            DRAXIS_VISIBLE_ONLY_LABELS_ON = 2,
            /// 将所有视图中的轴线可见属性改为可见
            /// 注：仅对图纸层级轴线属性有效
            DRAXIS_VISIBLE_ALL_VIEWS_ON = 3,
        };

        public enum DRDetailSymbolFrame
        {
            DRDETAIL_SYMBOL_FRAME_NONE = 0,
            DRDETAIL_SYMBOL_FRAME_CIRCLE = 1,
            DRDETAIL_SYMBOL_FRAME_RECT = 2,
        }

        public enum DRUpdateStatus
        {
            DRUPDATE_NONE = 0,
            DRUPDATE_UPDATED,
            DRUPDATE_CLONE,
            DRUPDATE_DIRTY,
            DRUPDATE_DELETE,
        };

        public enum DRChangeType
        {
            DRCHANGE_NONE = 0,
            DRCHANGE_AMOUNT_INCREASE,
            DRCHANGE_AMOUNT_DECREASE,
            DRCHANGE_PARTS_DELETED,
            DRCHANGE_PARTS_MODIFIED,
            DRCHANGE_LINK_MODIFIED,
            DRCHANGE_COPY_MODIFIED,
            DRCHANGE_UPDATED,
            DRCHANGE_CLONE,
        };

        /// <summary>
        /// 图纸相邻元素可见性
        /// </summary>
        public enum DRNeighborVisible
        {
            DRNEIGHBOR_VISIBLE_NONE = 0,
            DRNEIGHBOR_VISIBLE_CONNECTED = 1,
            DRNEIGHBOR_VISIBLE_CONNECTING = 2,
            DRNEIGHBOR_VISIBLE_ALL_CONNECTION = 3,
            DRNEIGHBOR_VISIBLE_BY_EXTREME = 4,
        };

        /// <summary>
        /// 图纸相邻零件元素可见性
        /// </summary>
        public enum DRNeighborPartVisible
        {
            DRNEIGHBOR_PART_VISIBLE_NONE = 0,
            DRNEIGHBOR_PART_VISIBLE_MAIN = 1,
            DRNEIGHBOR_PART_VISIBLE_SECOND = 2,
            DRNEIGHBOR_PART_VISIBLE_BOTH = 3,
        };

        public const int ORIENTATION_MARK_VIEW_DIR_EAST = 1 << 0;
        public const int ORIENTATION_MARK_VIEW_DIR_WEST = 1 << 1;
        public const int ORIENTATION_MARK_VIEW_DIR_SOUTH = 1 << 2;
        public const int ORIENTATION_MARK_VIEW_DIR_NORTH = 1 << 3;

        enum SymElementType
        {
            SYM_ELEMENT_UNKNOWN = 0,
            SYM_ELEMENT_LINE,
            SYM_ELEMENT_CIRCLE,
            SYM_ELEMENT_FILLED_AREA,
            SYM_ELEMENT_TEXT,
        };

        //Dynamic Point
        enum DPointMode { DPOINT_NONE = 0, DPOINT_XYZ, DPOINT_CTRL_POINT, DPOINT_OBJ_POINT };

        //Selector
        public enum SelectorType { SEL_TYPE_NONE = 0, SEL_TYPE_3D, SEL_TYPE_DRAWING };
        public enum SelectorMode { SEL_NONE = 0, SEL_SNAP, SEL_SELECT };

        public enum NumericInputType
        {
            NUM_INPUT_NONE = 0,
            NUM_INPUT_ALONG_DIR,
            NUM_INPUT_CARTESIAN_GLOBAL,
            NUM_INPUT_CARTESIAN_ABSOLUTE,
            NUM_INPUT_CARTESIAN_RELATIVE,
            NUM_INPUT_POLAR_GLOBAL,
            NUM_INPUT_POLAR_ABSOLUTE,
            NUM_INPUT_POLAR_RELATIVE,
        };

        public enum SelectorCaller
        {
            SEL_CALLER_NONE = 0,
            SEL_CALLER_KEY_DOWN,
            SEL_CALLER_KEY_UP,
            SEL_CALLER_MOUSE_DOWN,
            SEL_CALLER_MOUSE_MOVE,
            SEL_CALLER_MOUSE_UP,
            SEL_CALLER_MOUSE_WHEEL,
            SEL_CALLER_MOUSE_CLICK,
            SEL_CALLER_MOUSE_DOUBLE_CLICK,
            SEL_CALLER_RENDER,
        };

        //Snap Target
        public enum SnapTarget
        {
            SNAP_TARGET_NONE = 0,
            SNAP_TARGET_POINT,
            SNAP_TARGET_PLANE
        };

        public enum SnapMode
        {
            SNAP_MODE_NONE = 0,
            SNAP_MODE_PLANE,
            SNAP_MODE_3D,
            SNAP_MODE_AUTO
        };

        public enum SnapRefPlane
        {
            SNAP_REF_PLANE_NONE = 0,
            SNAP_REF_PLANE_VIEW,
            SNAP_REF_PLANE_WORK
        };

        //Snap flag
        public const uint SNAP_NONE = 0u;
        public const uint SNAP_PT_REF_POINT = (1u << 1);
        public const uint SNAP_PT_GEO_POINT = (1u << 2);
        public const uint SNAP_PT_LINE = (1u << 3);
        public const uint SNAP_PT_CTRL_POINT = (1u << 6);
        public const uint SNAP_PT_END_POINT = (1u << 7);
        public const uint SNAP_PT_HELPER_POINT = (1u << 8);
        public const uint SNAP_PT_GRID_CROSS_POINT = (1u << 9);
        public const uint SNAP_PT_CENTER_POINT = (1u << 10);
        public const uint SNAP_PT_MID_POINT = (1u << 11);
        public const uint SNAP_PT_CROSS_POINT = (1u << 12);
        public const uint SNAP_PT_FOOT_POINT = (1u << 13);
        public const uint SNAP_PT_LINE_EXT = (1u << 14);
        public const uint SNAP_PT_LINE_NEAREST = (1u << 15);
        public const uint SNAP_PT_FREE = (1u << 16);
        public const uint SNAP_PT_CENTER_FLAG = (1u << 17);
        public const uint SNAP_PT_REF_FLAG = (1u << 18);
        public const uint SNAP_PT_WORK_X_FLAG = (1u << 19);
        public const uint SNAP_PT_WORK_Y_FLAG = (1u << 20);
        public const uint SNAP_PT_WORK_Z_FLAG = (1u << 21);

        public const uint SNAP_PT_WORK_AXIS_FLAGS = SNAP_PT_WORK_X_FLAG
                                                | SNAP_PT_WORK_Y_FLAG
                                                | SNAP_PT_WORK_Z_FLAG;


        public const uint SNAP_PT_ALL = SNAP_PT_REF_POINT
                                        | SNAP_PT_GEO_POINT
                                        | SNAP_PT_LINE
                                        | SNAP_PT_END_POINT
                                        | SNAP_PT_HELPER_POINT
                                        | SNAP_PT_CENTER_POINT
                                        | SNAP_PT_MID_POINT
                                        | SNAP_PT_CROSS_POINT
                                        | SNAP_PT_FOOT_POINT
                                        | SNAP_PT_LINE_EXT
                                        | SNAP_PT_LINE_NEAREST
                                        | SNAP_PT_FREE;

        public const uint SNAP_PL_GEO = (1u << 1);
        public const uint SNAP_PL_TOP = (1u << 2);
        public const uint SNAP_PL_BOTTOM = (1u << 3);
        public const uint SNAP_PL_FRONT = (1u << 4);
        public const uint SNAP_PL_BACK = (1u << 5);
        public const uint SNAP_PL_HEAD = (1u << 6);
        public const uint SNAP_PL_TAIL = (1u << 7);
        public const uint SNAP_PL_CENTER = (1u << 8);
        public const uint SNAP_PL_CUT = (1u << 9);
        public const uint SNAP_PL_AXIS = (1u << 10);
        public const uint SNAP_PL_COMP_AXIS = (1u << 11);

        public const uint SNAP_PL_BOUND = SNAP_PL_TOP
                                    | SNAP_PL_BOTTOM
                                    | SNAP_PL_FRONT
                                    | SNAP_PL_BACK
                                    | SNAP_PL_HEAD
                                    | SNAP_PL_TAIL;

        public const uint SNAP_PL_MAIN = SNAP_PL_BOUND
                                                | SNAP_PL_CENTER
                                            | SNAP_PL_CUT
                                            | SNAP_PL_AXIS
                                            | SNAP_PL_COMP_AXIS;

        public const uint SNAP_PL_ALL = SNAP_PL_GEO
                                            | SNAP_PL_BOUND
                                            | SNAP_PL_CENTER
                                            | SNAP_PL_CUT
                                            | SNAP_PL_AXIS
                                            | SNAP_PL_COMP_AXIS
                                            | SNAP_PL_MAIN;


        //Pick Mode
        public enum PickMode
        {
            PICK_MODE_NONE = 0,
            PICK_MODE_COMPONENT, PICK_MODE_COMPONENT_OBJ,
            PICK_MODE_ASSEMBLE, PICK_MODE_ASSEMBLE_OBJ
        };

        //Pick Mode
        public const uint PICK_NONE = 0u;
        public const uint PICK_ALL = 0xffffffffu;
        public const uint PICK_ASSEMBLY = (1u << 0);
        public const uint PICK_CONNECTION = (1u << 1);
        public const uint PICK_PART = (1u << 2);
        public const uint PICK_SURF_TREATMENT = (1u << 3);
        public const uint PICK_POINT = (1u << 4);
        public const uint PICK_GRID = (1u << 5);
        public const uint PICK_GRID_LINE = (1u << 6);
        public const uint PICK_WELD = (1u << 7);
        public const uint PICK_CUT = (1u << 8);
        public const uint PICK_VIEW = (1u << 9);
        public const uint PICK_HELPER = (1u << 10);
        public const uint PICK_BOLTS = (1u << 11);
        public const uint PICK_SINGLE_BOLT = (1u << 12);
        public const uint PICK_RFBAR = (1u << 13);
        public const uint PICK_PLANE = (1u << 14);
        public const uint PICK_CTRL_POINT = (1u << 15);
        public const uint PICK_DISTANCE = (1u << 16);

        //for ui tool bar all button
        public const uint PICK_BAR_ALL = PICK_ALL & ~(PICK_ASSEMBLY | PICK_GRID_LINE | PICK_SINGLE_BOLT);

        public const uint PICK_DR_LINES = (1u << 17);
        public const uint PICK_DR_TEXTS = (1u << 18);
        public const uint PICK_DR_MARKS = (1u << 19);
        public const uint PICK_DR_PARTS = (1u << 20);
        public const uint PICK_DR_SECT_SYMBOLS = (1u << 21);
        public const uint PICK_DR_WELDS = (1u << 22);
        public const uint PICK_DR_VIEWS = (1u << 23);
        public const uint PICK_DR_DIMS = (1u << 24);
        public const uint PICK_DR_SINGLE_DIM = (1u << 25);
        public const uint PICK_DR_GRIDS = (1u << 26);
        public const uint PICK_DR_GRID_LINES = (1u << 27);
        public const uint PICK_DR_DETAIL_MARKS = (1u << 28);
        public const uint PICK_DR_PLUGINS = (1u << 29);

        //for ui tool bar all button
        public const uint PICK_DR_BAR_ALL = PICK_ALL & ~(PICK_DR_SINGLE_DIM | PICK_DR_GRID_LINES);

        //Edit Handle Pick Mode
        public const uint PICK_HANDLE_NONE = 0u;
        public const uint PICK_HANDLE_HOVERING = (1u << 0);
        public const uint PICK_HANDLE_CTRLPOINT = (1u << 1);
        public const uint PICK_HANDLE_DIST_TAG = (1u << 2);
        public const uint PICK_HANDLE_PLATE_EDGE_MID = (1u << 3);
        public const uint PICK_HANDLE_PLATE_EDGE = (1u << 4);
        public const uint PICK_HANDLE_PICK_ALL = 0xffffffffu & ~PICK_HANDLE_HOVERING;
        public const uint PICK_HANDLE_HOVER_ALL = 0xffffffffu;

        //SteelPart
        public enum SPTweenMode : sbyte
        {
            SP_TWEEN_MODE_0 = 0,
            SP_TWEEN_MODE_1,
            SP_TWEEN_MODE_2,
            SP_TWEEN_MODE_3,
            SP_TWEEN_MODE_4,
            SP_TWEEN_MODE_5,
            SP_TWEEN_MODE_6
        };

        //for Bolt/Weld
        public enum PartMadeType : sbyte { PART_MADE_WORKSHOP = 0, PART_MADE_SITE };

        //RFBar
        public enum RFBarOffsetBase : sbyte
        {
            RFBAR_OFF_NONE = 0,
            RFBAR_OFF_COVER_THICKNESS,
            RFBAR_OFF_LEG_LENGTH
        };
        public enum RFBarGroupMethod
        {
            RFGROUP_METHOD_NONE = 0,
            RFGROUP_METHOD_BAR_COUNT,
            RFGROUP_METHOD_TARGET_SPACING,
            RFGROUP_METHOD_STEP,
            RFGROUP_METHOD_STEP_WITH_FLEX_FIRST_BLANK,
            RFGROUP_METHOD_STEP_WITH_FLEX_LAST_BLANK,
            RFGROUP_METHOD_STEP_WITH_FLEX_MID_BLANK,
            RFGROUP_METHOD_STEP_WITH_FLEX_FIRST_LAST_BLANK,
        };

        public enum RFBarGroupIgnore
        {
            RFGROUP_IGNORE_NONE = 0,
            RFGROUP_IGNORE_FIRST,
            RFGROUP_IGNORE_LAST,
            RFGROUP_IGNORE_FIRST_LAST,
        };

        public enum RFBarGroupShape
        {
            RFGROUP_SHAPE_NONE = 0,
            RFGROUP_SHAPE_NORMAL,
            RFGROUP_SHAPE_TAPERED,
            RFGROUP_SHAPE_TAPERED_RIDGE,
            RFGROUP_SHAPE_TAPERED_CURVED,
            RFGROUP_SHAPE_TAPERED_N,
            RFGROUP_SHAPE_SPIRAL
        };

        public enum RFBarMeshType : sbyte
        {
            RFBAR_MESH_NONE = 0,
            RFBAR_MESH_POLYGON,
            RFBAR_MESH_RECTANGLE,
            RFBAR_MESH_BENT
        };

        //Bolt
        public enum BoltType
        {
            BOLT_NONE = 0,
            BOLT_HEX,
            BOLT_ROUND,
            BOLT_PLOW,
            BOLT_NUT,
            BOLT_WASHER,
            BOLT_STUD,
            BOLT_THREADED_ROD
        };

        //Bolt Part Flag
        public const uint BOLT_SECT_WASHER_UP = 1u << 1;
        public const uint BOLT_SECT_WASHER_DOWN0 = 1u << 2;
        public const uint BOLT_SECT_WASHER_DOWN1 = 1u << 3;
        public const uint BOLT_SECT_WASHER_DOWN2 = 1u << 4;
        public const uint BOLT_SECT_WASHER_DOWN3 = 1u << 5;
        public const uint BOLT_SECT_WASHER_DOWN4 = 1u << 6;
        public const uint BOLT_SECT_NUT0 = 1u << 7;
        public const uint BOLT_SECT_NUT1 = 1u << 8;
        public const uint BOLT_SECT_NUT2 = 1u << 9;
        public const uint BOLT_SECT_NUT3 = 1u << 10;
        public const uint BOLT_SECT_NUT4 = 1u << 11;
        public const uint BOLT_SECT_PARTS = (1u << 0) //FIXME: 向下兼容
                                            | BOLT_SECT_WASHER_UP
                                            | BOLT_SECT_WASHER_DOWN0
                                            | BOLT_SECT_WASHER_DOWN1
                                            | BOLT_SECT_WASHER_DOWN2
                                            | BOLT_SECT_WASHER_DOWN3
                                            | BOLT_SECT_WASHER_DOWN4
                                            | BOLT_SECT_NUT0
                                            | BOLT_SECT_NUT1
                                            | BOLT_SECT_NUT2
                                            | BOLT_SECT_NUT3
                                            | BOLT_SECT_NUT4;

        public const uint BOLT_SECT_OVERSIZED_HOLE0 = 1u << 16;
        public const uint BOLT_SECT_OVERSIZED_HOLE1 = 1u << 17;
        public const uint BOLT_SECT_OVERSIZED_HOLE2 = 1u << 18;
        public const uint BOLT_SECT_OVERSIZED_HOLE3 = 1u << 19;
        public const uint BOLT_SECT_OVERSIZED_HOLE4 = 1u << 20;

        public enum BoltHoleType : sbyte { BOLT_HOLE_OVERSIZED = 0, BOLT_HOLE_SLOTTED };
        public enum BoltRotateSlot : sbyte { BOLT_SLOT_PARALLEL = 0, BOLT_SLOT_ODD, BOLT_SLOT_EVEN };
        public enum BoltGroupShape : sbyte
        {
            BOLT_GROUPSHAPE_NONE = 0,
            BOLT_GROUPSHAPE_ARRAY,
            BOLT_GROUPSHAPE_CIRCLE,
            BOLT_GROUPSHAPE_LIST,
            BOLT_GROUPSHAPE_RADIAL_OUT,
            BOLT_GROUPSHAPE_RADIAL_IN,
            BOLT_GROUPSHAPE_ALTER_X_ODD,
            BOLT_GROUPSHAPE_ALTER_X_EVEN,
            BOLT_GROUPSHAPE_ALTER_Y_ODD,
            BOLT_GROUPSHAPE_ALTER_Y_EVEN
        };

        public enum BoltEdgeDistTarget
        {
            BOLT_EDGE_DIST_TARGET_NONE = 0,
            BOLT_EDGE_DIST_TARGET_BOLT_D,
            BOLT_EDGE_DIST_TARGET_HOLE_D
        };

        //Weld
        public enum WeldMode : sbyte
        {
            WELD_MODE_NONE = 0,
            WELD_MODE_POLYGON,
            WELD_MODE_AUTO
        };

        public enum WeldType : sbyte
        {
            WELD_TYPE_NONE = 0,
            WELD_TYPE_EDGE_FLANGE, //1
            WELD_TYPE_SQUARE_GROOVE_SQUARE_BUTT, //2
            WELD_TYPE_BEVEL_GROOVE_SINGLE_V_BUTT, //3
            WELD_TYPE_BEVEL_GROOVE_BEVEL_BUTT, //4
            WELD_TYPE_SINGLE_V_BUTT_WITH_BROAD_ROOT_FACE, //5
            WELD_TYPE_SINGLE_BEVEL_BUTT_WITH_BROAD_ROOT_FACE, //6
            WELD_TYPE_U_GROOVE_SINGLE_U_BUTT, //7
            WELD_TYPE_J_GROOVE_J_BUTT, //8
            WELD_TYPE_BEVEL_BACKING, //9
            WELD_TYPE_FILLET, //10
            WELD_TYPE_PLUG, //11
            WELD_TYPE_SPOT, //12
            WELD_TYPE_SEAM, //13
            WELD_TYPE_SLOT, //14
            WELD_TYPE_FLARE_BEVEL_GROOVE, //15
            WELD_TYPE_FLARE_V_GROOVE, //16
            WELD_TYPE_CORNER_FLANGE, //17
            WELD_TYPE_PARTIAL_PENET_SINGLE_BEVEL_BUTT_PLUS_FILLET, //18
            WELD_TYPE_PARTIAL_PENET_SQUARE_GROOVE_PLUS_FILLET, //19
            WELD_TYPE_MELT_THROUGHT, //20
            WELD_TYPE_STEEP_FLANKED_BEVEL_GROOVE_SINGLE_V_BUTT, //21
            WELD_TYPE_STEEP_FLANKED_BEVEL_GROOVE_SINGLE_BEVEL_BUTT, //22
            WELD_TYPE_EDGE, //23
            WELD_TYPE_ISO_SURFACING, //24
            WELD_TYPE_FOLD, //25
            WELD_TYPE_INCLINED //26
        };

        public enum WeldAutoGroove : sbyte
        {
            WELD_AUTO_GROOVE_NONE = 0,
            WELD_AUTO_GROOVE_TRUE,
            WELD_AUTO_GROOVE_FALSE,
            WELD_AUTO_GROOVE_AS_MAIN,
        };


        public enum WeldContour : sbyte
        {
            WELD_CONTOUR_NONE = 0,
            WELD_CONTOUR_1,
            WELD_CONTOUR_2,
            WELD_CONTOUR_3
        };

        public enum WeldFinish : sbyte
        {
            WELD_FINISH_NONE = 0,
            WELD_FINISH_G,
            WELD_FINISH_M,
            WELD_FINISH_C,
            WELD_FINISH_V,
            WELD_FINISH_W
        };

        public enum WeldEdgeAround : sbyte
        {
            WELD_EDGE = 0,
            WELD_AROUND
        };

        public enum NDTInspecLevel : sbyte
        {
            WELD_NDT_NONE = 0,
            WELD_NDT_A,
            WELD_NDT_B,
            WELD_NDT_C,
            WELD_NDT_D,
            WELD_NDT_E
        };

        public enum DetailConnectType : sbyte
        {
            DETAIL_CONNECT_AS_PART = 0,
            DETAIL_CONNECT_AS_SUB_ASSEMBLY
        };

        public enum WeldElecClass : sbyte
        {
            WELD_ELEC_CLASS_NONE = 0,
            WELD_ELEC_CLASS_35,
            WELD_ELEC_CLASS_42,
            WELD_ELEC_CLASS_50,
            WELD_ELEC_CLASS_E60XX,
            WELD_ELEC_CLASS_E70XX,
            WELD_ELEC_CLASS_E80XX,
            WELD_ELEC_CLASS_E90XX
        };

        public enum WeldProcessType : sbyte
        {
            WELD_PROCESS_NONE = 0,
            WELD_PROCESS_SMAW,
            WELD_PROCESS_SAW,
            WELD_PROCESS_GMAW,
            WELD_PROCESS_FCAW,
            WELD_PROCESS_ESW,
            WELD_PROCESS_EGW
        };

        //Comp Origin Type
        public enum CompOriginType { COMP_ORIGIN_NONE = 0, COMP_ORIGIN_MIDDLE };

        //Comp UI Ctrl Type
        public enum CompUIType
        {
            COMP_CTRL_NONE = 0,
            COMP_CTRL_ROOT_TAB,
            COMP_CTRL_ROOT_TAB_PAGE,
            COMP_CTRL_LABEL,
            COMP_CTRL_INPUT_TEXT,
            COMP_CTRL_INPUT_DOUBLE,
            COMP_CTRL_INPUT_INT,
            COMP_CTRL_INPUT_LONG,
            COMP_CTRL_CHECK,
            COMP_CTRL_RADIO,
            COMP_CTRL_COMBO,
            COMP_CTRL_COMBO_IMAGE,
            COMP_CTRL_LIST,
            COMP_CTRL_IMAGE,
            COMP_CTRL_TAB,
            COMP_CTRL_TAB_PAGE,
            COMP_CTRL_GROUP,
            COMP_CTRL_BUTTON,
            COMP_CTRL_RESERVED1,
            COMP_CTRL_RESERVED2,
            COMP_CTRL_RESERVED3,
            COMP_CTRL_RESERVED4,
            COMP_CTRL_RESERVED5,
            COMP_CTRL_RESERVED6,
            COMP_CTRL_RESERVED7,
            COMP_CTRL_COMPOSITE_BEGIN,
            COMP_CTRL_COMPOSITE_TEXT,
            COMP_CTRL_COMPOSITE_DOUBLE,
            COMP_CTRL_COMPOSITE_INT,
            COMP_CTRL_COMPOSITE_LONG,
            COMP_CTRL_COMPOSITE_COMBO,
            COMP_CTRL_COMPOSITE_COMBO_IMAGE_REMOVED,
            COMP_CTRL_COMPOSITE_CHECK,
            COMP_CTRL_COMPOSITE_RADIO,
            COMP_CTRL_COMPOSITE_POINT_CHAMFER,
            COMP_CTRL_COMPOSITE_MTRL,
            COMP_CTRL_COMPOSITE_PROFILE,
            COMP_CTRL_COMPOSITE_MADE_TYPE,
            COMP_CTRL_COMPOSITE_BOLT_SIZE,
            COMP_CTRL_COMPOSITE_BOLT_ASSEMBLY,
            COMP_CTRL_COMPOSITE_WELD_TYPE,
            COMP_CTRL_COMPOSITE_DOUBLE_ARRAY_SCRIPT,
            COMP_CTRL_COMPOSITE_WELD_CONTOUR,
            COMP_CTRL_COMPOSITE_WELD_FINISH,
            COMP_CTRL_COMPOSITE_WELD_EDGE_AROUND,
            COMP_CTRL_COMPOSITE_BOLT_PANEL,
            COMP_CTRL_COMPOSITE_WELD_PANEL,
            COMP_CTRL_COMPOSITE_INT_ARRAY_SCRIPT,
            COMP_CTRL_COMPOSITE_LONG_ARRAY_SCRIPT,
            COMP_CTRL_COMPOSITE_INT_ARRAY,
            COMP_CTRL_COMPOSITE_LONG_ARRAY,
            COMP_CTRL_COMPOSITE_DOUBLE_ARRAY,
            COMP_CTRL_COMPOSITE_PLATE_PANEL,
            COMP_CTRL_COMPOSITE_PLATE_SIDE,
            COMP_CTRL_COMPOSITE_COMBO_IMAGE_NEW,
            COMP_CTRL_COMPOSITE_BOLT_SHAPE,
            COMP_CTRL_COMPOSITE_BOLT_DIR,
            COMP_CTRL_COMPOSITE_BEAM_CUT,
            COMP_CTRL_COMPOSITE_BEAM_CUT_WEB,
            COMP_CTRL_COMPOSITE_BEAM_CUT_FLANGE,
            COMP_CTRL_COMPOSITE_BEAM_CUT_FLANGE_COLLISION,
            COMP_CTRL_COMPOSITE_STIFF_MODE,
            COMP_CTRL_COMPOSITE_STIFF_DIR,
            COMP_CTRL_COMPOSITE_HAUNCH_MODE,
            COMP_CTRL_COMPOSITE_HAUNCH_CORNER,
            COMP_CTRL_COMPOSITE_NOTCH_SHAPE,
            COMP_CTRL_COMPOSITE_NOTCH_POS,
            COMP_CTRL_COMPOSITE_NOTCH_CORNER_TYPE,
            COMP_CTRL_COMPOSITE_NOTCH_FLANGE_SHAPE,
            COMP_CTRL_COMPOSITE_NOTCH_FLANGE_SIDE,
            COMP_CTRL_COMPOSITE_NOTCH_SIZE,
            COMP_CTRL_COMPOSITE_NOTCH_SIZE_ROUND,
            COMP_CTRL_COMPOSITE_NOTCH_FLANGE_SIZE,
            COMP_CTRL_COMPOSITE_ANGLBOX_POS,
            COMP_CTRL_COMPOSITE_ANGLBOX_ROT,
            COMP_CTRL_COMPOSITE_ANGLBOX_DIR,
            COMP_CTRL_COMPOSITE_ANGLBOX_DETAIL,
            COMP_CTRL_COMPOSITE_ANGLBOX_STIFF_SHAPE,
            COMP_CTRL_COMPOSITE_ANGLBOX_STIFF_SIDE_POS,
            COMP_CTRL_COMPOSITE_ANGLBOX_STIFF_MID_POS,
            COMP_CTRL_COMPOSITE_ASSEMBLE_TYPE,
            COMP_CTRL_COMPOSITE_CORNER_PANELL,
            COMP_CTRL_COMPOSITE_CORNER_PANELR,
            COMP_CTRL_COMPOSITE_BOLT_DIST_V_MODE,
            COMP_CTRL_COMPOSITE_BOLT_DIST_H_MODE,
            COMP_CTRL_COMPOSITE_BEAM_GROOVE_SIZE,
            COMP_CTRL_COMPOSITE_BEAM_GROOVE_SHAPE,
            COMP_CTRL_COMPOSITE_BEAM_GROOVE_POS,
            COMP_CTRL_COMPOSITE_BEAM_FLANGE_GROOVE_POS,
            COMP_CTRL_COMPOSITE_BEAM_BACKBAR_POS,
            COMP_CTRL_COMPOSITE_BEAM_BACKBAR_LENGTH,
            COMP_CTRL_COMPOSITE_BEAM_BACKBAR_OVER_DIST,
            COMP_CTRL_COMPOSITE_ANGLBOX_GAP,
            COMP_CTRL_COMPOSITE_ANGLBOX_PBOLT_PANEL,
            COMP_CTRL_COMPOSITE_ANGLBOX_SBOLT_PANEL,
            COMP_CTRL_COMPOSITE_WELD_GRID,
            COMP_CTRL_COMPOSITE_BOLT_INCIDENT_DIR,
            COMP_CTRL_COMPOSITE_BOLT_EXT_LEN,
            COMP_CTRL_COMPOSITE_BOLT_PARTS,
            COMP_CTRL_COMPOSITE_STUD_PANEL,
            COMP_CTRL_COMPOSITE_END,
        };
    }
}
