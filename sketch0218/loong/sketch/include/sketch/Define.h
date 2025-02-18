#pragma once

#include "Core.h"
#include <map>
#include <string>

namespace sketch
{
class CORE_DLL_DECLSPEC Define
{
public:
    static void init();

    static const int MAINFORM_MIN_X = 800;
    static const int MAINFORM_MIN_Y = 600;
    static const int MAINFORM_BORDER_MARGIN = 4;

    enum ErrorCode
    {
        ERR_NONE = 0,
        ERR_DOC_LOCK,
        ERR_COMP_EDITOR_INVALID,
        ERR_EXPORT_FAILED,
        ERR_IMPORT_FAILED,
        ERR_DIFFERENT_PARENT_ASS
    };

    enum Message
    {
        MSG_NONE = 0,
        MSG_SCENE_CREATE,
        MSG_SCENE_DELETE,
        MSG_OBJ_CREATE,
        MSG_OBJ_DISPOSE,
        MSG_OBJ_DELETE,
        MSG_OBJ_UPDATE,
        MSG_CUR_OBJS_UPDATE,
        MSG_VIEW_CREATE,
        MSG_VIEW_DELETE,
    };

    enum RevertableType
    {
        RVT_NONE = 0,
        RVT_BEGIN,
        RVT_END,
        // 3d
        RVT_OBJ_CREATE,
        RVT_OBJ_DELETE,
        RVT_OBJ_MODIFY,
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
        // drawing
        RVT_DRAWING_CREATE,
        RVT_DRAWING_DELETE,
        RVT_DRAWING_MODIFY,
        RVT_DRELE_CREATE,
        RVT_DRELE_DELETE,
        RVT_DRELE_MODIFY,
        RVT_DRELE_MATRIX_MODIFY,
        RVT_DRELE_MATRIX_LOCAL_MODIFY,
        RVT_DRELE_PARENT_MODIFY,
        // view
        RVT_VIEW_HIDE
    };

    static const std::wstring& toStr(RevertableType type);

    enum DisplayRatio
    {
        DISP_RATIO_NONE = 0,
        DISP_RATIO_AUTO,
        DISP_RATIO_100,
        DISP_RATIO_125,
        DISP_RATIO_150,
        DISP_RATIO_175,
        DISP_RATIO_200,
    };

    enum DocumentSaveResult
    {
        DOC_SAVE_NONE = 0,
        DOC_SAVE_SUCCESS,
        DOC_SAVE_FAILED,
        DOC_SAVE_LOCKED
    };

    enum ObjectType : short
    {
        OBJ_UNKNOWN = 0,

        OBJ_TF_BEGIN = 1,

        OBJ_D5_GROUP,
        OBJ_D5_MESH,
        OBJ_D5_NURBS,

        OBJ_TYPE_END
    };

    static const std::wstring& toStr(ObjectType type);
    static ObjectType toObjectType(const std::wstring& str);

    // Geo Tri Flags
    enum GeoTriFlags : uint32_t
    {
        TRI_FLAG_OUT = 1u << 0,                // 外部
        TRI_FLAG_IN = 1u << 1,                 // 内部
        TRI_FLAG_BOUND = 1u << 2,              // 外包围面
        TRI_FLAG_END = 1u << 3,                // 端部
        TRI_FLAG_TOP = 1u << 4,                // 顶面
        TRI_FLAG_BOTTOM = 1u << 5,             // 底面
        TRI_FLAG_FRONT = 1u << 6,              // 前面
        TRI_FLAG_BACK = 1u << 7,               // 后面
        TRI_FLAG_HEAD = 1u << 8,               // 头部
        TRI_FLAG_TAIL = 1u << 9,               // 尾部
        TRI_FLAG_PLANE_CUT = 1u << 10,         // 切割面
        TRI_FLAG_WEB = 1u << 11,               // 腹板
        TRI_FLAG_FLANGE = 1u << 12,            // 翼板
        TRI_FLAG_EDGE_FOLD = 1u << 13,         // 弯折
        TRI_FLAG_WELD_GROOVE = 1u << 14,       // 焊接坡口
        TRI_FLAG_GEO_CUT = 1u << 15,           // 几何切割
        TRI_FLAG_CAHMFER_LINE = 1u << 16,      // 直线倒角
        TRI_FLAG_CAHMFER_ROUND = 1u << 17,     // 圆角
        TRI_FLAG_CAHMFER_INV_ROUND = 1u << 18, // 凹角
        TRI_FLAG_CAHMFER_ARC = 1u << 19,       // 弧线
        TRI_FLAG_CAHMFER_ORTHO = 1u << 20,     // 垂直倒角
        TRI_FLAG_CAHMFER_PARA = 1u << 21,      // 平行倒角
        TRI_FLAG_CAHMFER_PARA_ROUND = 1u << 22,// 平行圆角倒角
        TRI_FLAG_HOLE = 1u << 23,              // 洞
        TRI_FLAG_LEFT = 1u << 24,
        TRI_FLAG_RIGHT = 1u << 25,
        TRI_FLAG_CIRCULAR = 1u << 26,
        TRI_FLAG_SECTION = 1u << 27,
        TRI_FLAG_DISABLE_OPER = 1u << 28,
    };

    // Object Common Flags
    static const uint32_t OBJ_FLAG_ENABLE = 1;
    static const uint32_t OBJ_FLAG_SAVE_ENABLE = 1 << 1;
    static const uint32_t OBJ_FLAG_USER_DEL_ENABLE = 1 << 2;
    static const uint32_t OBJ_FLAG_USER_COPY_ENABLE = 1 << 3;
    static const uint32_t OBJ_FLAG_USER_MOVE_ENABLE = 1 << 4;
    static const uint32_t OBJ_FLAG_MAG = 1 << 5;
    static const uint32_t OBJ_FLAG_REVERTABLE = 1 << 6;
    static const uint32_t OBJ_FLAG_LOCK = 1 << 7;
    static const uint32_t OBJ_FLAG_COMP_TEMP = 1 << 8;

    // Object Side
    enum ObjectSide : char
    {
        SIDE_LEFT = 0,
        SIDE_RIGHT,
        SIDE_TOP,
        SIDE_BOTTOM,
        SIDE_FRONT,
        SIDE_BACK,
        SIDE_UP,
        SIDE_DOWN,
        SIDE_MIDDLE,
        SIDE_HEAD,
        SIDE_TAIL,
        SIDE_MIDDLE_H,
        SIDE_MIDDLE_V,
        SIDE_MIDDLE_DEPTH,
        SIDE_NONE
    };

    static const std::wstring& toStr(ObjectSide type);
    static ObjectSide toObjectSide(const std::wstring& str);

    // Space
    enum SpacePlane : char
    {
        PLANE_XY = 0,
        PLANE_YZ,
        PLANE_ZX
    };
    enum SpaceAxis : char
    {
        AXIS_X = 0,
        AXIS_Y,
        AXIS_Z,
        AXIS_NX,
        AXIS_NY,
        AXIS_NZ,
        AXIS_NONE
    };

    // Unit
    enum Unit : char
    {
        UNIT_AUTO = 0,
        UNIT_MM,
        UNIT_CM,
        UNIT_M,
        UNIT_CM_M,
        UNIT_INCH,
        UNIT_FT,
        UNIT_FT_IN,
        UNIT_DEG,
        UNIT_RAD
    };
    static const std::wstring& toStr(Unit type);
    static Unit toUnit(const std::wstring& str);

    // Direction
    enum Direction : char
    {
        DIR_NONE = 0,
        DIR_N,
        DIR_W,
        DIR_S,
        DIR_E,
        DIR_NW,
        DIR_NE,
        DIR_SW,
        DIR_SE
    };

    // ObjectRole
    enum ObjectRole : char
    {
        OBJ_ROLE_NONE = 0,
        OBJ_ROLE_MAIN,
        OBJ_ROLE_SUB,
    };

    // Part Usage
    enum PartUsage : char
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

    // Mouse
    enum MouseCursor : char
    {
        CURSOR_NONE = 0,
        CURSOR_ARROW,
        CURSOR_HAND,
        CURSOR_CROSS,
        CURSOR_MOVE,
        CURSOR_SIZE_WE,
        CURSOR_SIZE_NS,
        CURSOR_SIZE_NWSE,
        CURSOR_SIZE_NESW,
        CURSOR_SIZE_ALL
    };

    // Var type
    enum VarType : char
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
    };

    // Surf Treatment
    enum SurfTreatType : char
    {
        SURF_NONE = 0,
        SURF_CONCRETE_FINISH,
        SURF_SPECIAL_MIX,
        SURF_TILE_SURFACE,
        SURF_STEEL_FINISH
    };

    enum SurfPattern : char
    {
        SURF_PATTERN_NONE = 0,
        SURF_PATTERN_1_1,
        SURF_PATTERN_1_2,
        SURF_PATTERN_1_3,
        SURF_PATTERN_1_4,
        SURF_PATTERN_BASKETWEAVE,
        SURF_PATTERN_HERRINGBONE
    };

    // Corner
    enum CornerCutType : char
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

    // Component Detail Type
    enum CompDetailType : char
    {
        COMP_DETAIL_NONE = 0,
        COMP_DETAIL_END,
        COMP_DETAIL_INTERMEDIATE,
        COMP_DETAIL_INTERMEDIATE_REV,
    };

    // Component Detail Position
    enum CompDetailPos : char
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

    // Component UI Usage
    enum CompUIUsage : int
    {
        COMP_UI_USAGE_NONE = 0,
        COMP_UI_USAGE_SET_DEFAULT,
        COMP_UI_USAGE_SET_INSTANCE,
    };

    // Component Ctrl Defaults
    static const double COMP_UI_DEF_DOUBLE;
    static const long long COMP_UI_DEF_LONG;
    static const int COMP_UI_DEF_INT;

    // layout
    enum LayoutCellType : char
    {
        CELL_NONE = 0,
        CELL_VIEW,
        CELL_DRAWING,
        CELL_DR_TEMPLATE,
        CELL_SYMBOL
    };

    // view
    enum ViewType : char
    {
        VIEW_3D = 0,
        VIEW_2D
    };
    enum ViewProjMode : char
    {
        VIEWPROJ_PERSPECTIVE = 0,
        VIEWPROJ_ORTHO
    };
    enum ViewRenderMode : char
    {
        RENDERMODE_NONE = 0,
        RENDERMODE_EDGEFACE,
        RENDERMODE_WIREFRAME,
        RENDERMODE_SHADED_WIREFRAME,
        RENDERMODE_HIDDEN_LINES,
        RENDERMODE_SEL_ONLY
    };
    enum ViewRenderQuality : char
    {
        RENDERQUALITY_NONE = 0,
        RENDERQUALITY_FAST,
        RENDERQUALITY_EXACT,
        RENDERQUALITY_DETAIL,
        RENDERQUALITY_REF_LINE,
        RENDERQUALITY_SLOT_HOLE
    };
    enum ViewNoRange : char
    {
        VIEWNORANGE_ALL = 0,
        VIEWNORANGE_FIRST,
        VIEWNORANGE_LAST
    };
    enum ViewLod : char
    {
        VIEWLOD_NONE = 0,
        VIEWLOD_LOW,
        VIEWLOD_MIDDLE,
        VIEWLOD_HIGH
    };

    // Selected Only Mode, KEEP_STYLE: 选中的保留，其它半透; VIS_ONLY: 选中保留，其它消失(Shift按下时)
    enum ViewFocusMode : char
    {
        VIEW_FOCUS_NONE = 0,
        VIEW_FOCUS_KEEP_STYLE,
        VIEW_FOCUS_VIS_ONLY
    };

    // view manager
    enum ViewManagerMode : char
    {
        VIEWMGR_NONE = 0,
        VIEWMGR_3D,
        VIEWMGR_DRAWING,
        VIEWMGR_DRAWING_TEMPLATE
    };

    // view layout type
    enum ViewLayoutType : char
    {
        VIEWLAYOUT_NONE = 0,
        VIEWLAYOUT_SINGLE_VIEW,
        VIEWLAYOUT_TILED_VIEW,
        VIEWLAYOUT_WINDOWED_VIEW,
        VIEWLAYOUT_SINGLE_DRAWING,
        VIEWLAYOUT_TILED_DRAWING,
        VIEWLAYOUT_WINDOWED_DRAWING
    };

    // edit
    enum MoveResult : char
    {
        MOVE_RESULT_NONE = 0,
        MOVE_RESULT_SUCCESS,
        MOVE_RESULT_ENV_FAILED,
        MOVE_RESULT_OBJ_LOCKED,
        MOVE_RESULT_USER_CANCELED
    };

    enum CopyResult : char
    {
        COPY_RESULT_NONE = 0,
        COPY_RESULT_SUCCESS,
        COPY_RESULT_FAILED,
        COPY_RESULT_ENV_FAILED,
        COPY_RESULT_OBJ_LOCKED,
        COPY_RESULT_USER_CANCELED
    };

    // object filter
    enum ObjectFilterType : char
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

    enum CompareOper : char
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

    enum LogicOper : char
    {
        LOGIC_NONE = 0,
        LOGIC_AND,
        LOGIC_OR
    };

    enum DRViewFilterType : char
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

    // CutPlane mode
    enum CutPlaneMode : char
    {
        CUT_PLANE_NONE = 0,
        CUT_PLANE_CUT,
        CUT_PLANE_ALIGNMENT
    };

    // Clash
    enum ClashStatus : char
    {
        CLASH_STATUS_NONE = 0,
        CLASH_STATUS_OK,
        CLASH_STATUS_CLASH,
    };

    enum ClashType : char
    {
        CLASH_TYPE_NONE = 0,
        CLASH_TYPE_CUT_THROUGH,
        CLASH_TYPE_EXACT_MATCH,
        CLASH_TYPE_CONTAIN,
        CLASH_TYPE_CLASH,
        CLASH_TYPE_SELF_CLASH
    };

    enum ClashStep : char
    {
        CLASH_STEP_NONE = 0,
        CLASH_STEP_ASSIGN,
        CLASH_STEP_FIX,
        CLASH_STEP_APPROVE,
        CLASH_STEP_IGNORE,
        CLASH_STEP_REOPEN
    };

    enum ClashPriority : char
    {
        CLASH_PRIORITY_NONE = 0,
        CLASH_PRIORITY_LOW,
        CLASH_PRIORITY_MEDIUM,
        CLASH_PRIORITY_HIGH
    };

    // Number
    enum NumberMode : char
    {
        NUMBER_MODE_NONE = 0,
        NUMBER_MODE_COMPARE_OLD,
        NUMBER_MODE_ASSIGN_NEW,
        NUMBER_MODE_KEEP_UNCHANGED,
    };

    enum NumberAssSort : char
    {
        NUMBER_ASS_SORT_NONE = 0,
        NUMBER_ASS_SORT_X,
        NUMBER_ASS_SORT_Y,
        NUMBER_ASS_SORT_Z,
        NUMBER_ASS_SORT_MAIN_USR,
        NUMBER_ASS_SORT_USR,
    };

    enum ArraySortOrder : char
    {
        SORT_ORDER_NONE = 0,
        SORT_ORDER_ASCEND,
        SORT_ORDER_DESCEND,
    };

    // drawing
    enum DrawingType : char
    {
        DRAW_UNKNOWN = 0,
        DRAW_SINGLE_PART,
        DRAW_ASSEMBLY,
        DRAW_MULTIPLE,
        DRAW_ARRANGEMENT,
        DRAW_TEMPLATE
    };
    static const std::wstring& toStr(DrawingType type);
    static DrawingType toDrawingType(const std::wstring& str);

    // master drawing
    enum MasterDrawingType : char
    {
        MASTER_DRAWING_TYPE_UNKNOWN = 0,
        MASTER_DRAWING_TYPE_SAVED = 1,
        MASTER_DRAWING_TYPE_CLONE,
        MASTER_DRAWING_TYPE_RULE_SET,
    };
    static const std::wstring& toStr(MasterDrawingType type);
    static MasterDrawingType toMasterDrawingType(const std::wstring& str);

    enum DRElementType : int
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
        DRELE_CTRL_POINT,        // Endpoint control point.
        DRELE_CTRL_POINT_DIAMOND,// Arc, circle, bulge control point.
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
        DRELE_REF_RESERVED1,
        DRELE_REF_RESERVED2,
        DRELE_REF_RESERVED3,
        DRELE_REF_RESERVED4,
        DRELE_REF_RESERVED5,
        DRELE_REF_END,
    };

    static const std::wstring& toStr(DRElementType type);
    static DRElementType toDRElementType(const std::wstring& str);

    enum DRElementAnchor : char
    {
        DRELE_ANCHOR_NONE = 0,
        DRELE_ANCHOR_LEFT,
        DRELE_ANCHOR_MIDDLE,
        DRELE_ANCHOR_RIGHT,
        DRELE_ANCHOR_BOTTOM,
        DRELE_ANCHOR_TOP
    };

    enum DRViewType : char
    {
        DRVIEW_NONE = 0,
        DRVIEW_2D,
        DRVIEW_3D
    };
    static const std::wstring& toStr(DRViewType type);
    static DRViewType toDRViewType(const std::wstring& str);

    enum DRCoordSystemType : char
    {
        DRCOORDSYS_TYPE_LOCAL = 0,
        DRCOORDSYS_TYPE_MODEL = 1,
        DRCOORDSYS_TYPE_ORIENT = 2,
        DRCOORDSYS_TYPE_HORIZON_BRACE = 3,
        DRCOORDSYS_TYPE_VERTICAL_BRACE = 4,
        DRCOORDSYS_TYPE_FIXED = 5,
    };

    enum DRViewSide : char
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

    static const std::wstring& toStr(DRViewSide type);
    static DRViewSide toDRViewSide(const std::wstring& str);

    enum DRViewUnfoldType : char
    {
        DRVIEW_UNFOLD_NONE = 0,
        DRVIEW_UNFOLD_HORIZON = 1,
        DRVIEW_UNFOLD_VERTICAL = 2,
        DRVIEW_UNFOLD_BOTH = 3
    };

    enum DRAssociateType : char
    {
        DRASSOCIATE_TYPE_NONE = 0,

        DRASSOCIATE_TYPE_BOLT_BEGIN,
        DRASSOCIATE_TYPE_BOLT_ORIGIN,
        DRASSOCIATE_TYPE_BOLT_TOP,
        DRASSOCIATE_TYPE_BOLT_BOTTOM,
        DRASSOCIATE_TYPE_BOLT_OUTSIDE,
        DRASSOCIATE_TYPE_BOLT_RESERVED1,
        DRASSOCIATE_TYPE_BOLT_RESERVED2,
        DRASSOCIATE_TYPE_BOLT_RESERVED3,
        DRASSOCIATE_TYPE_BOLT_END,

        DRASSOCIATE_TYPE_PART_BEGIN,
        DRASSOCIATE_TYPE_PART_BOX,
        DRASSOCIATE_TYPE_PART_END_POINT,
        DRASSOCIATE_TYPE_PART_MID_POINT,
        DRASSOCIATE_TYPE_PART_LINE,
        DRASSOCIATE_TYPE_PART_BODY,
        DRASSOCIATE_TYPE_PART_OUTSIDE,
        DRASSOCIATE_TYPE_PART_RESERVED1,
        DRASSOCIATE_TYPE_PART_RESERVED2,
        DRASSOCIATE_TYPE_PART_RESERVED3,
        DRASSOCIATE_TYPE_PART_END,
    };

    enum DRPartRepresentation : char
    {
        DRPART_REPRESENT_OUTLINE = 0,
        DRPART_REPRESENT_EXACT,
        DRPART_REPRESENT_WORKSHOP_FORM,
        DRPART_REPRESENT_SYMBOL,
        DRPART_REPRESENT_SYMBOL_WITH_PARTIAL_PROFILE,
        DRPART_REPRESENT_BOUNDING_BOX,
        DRPART_REPRESENT_BASE_BOX
    };

    enum DRBoltStyle : char
    {
        DRBOLT_NONE = 0,
        DRBOLT_SOLID,
        DRBOLT_EXACT,
        DRBOLT_SYMBOL1,
        DRBOLT_SYMBOL2,
        DRBOLT_SYMBOL3,
    };
    static const std::wstring& toStr(DRBoltStyle type);
    static DRBoltStyle toDRBoltStyle(const std::wstring& str);

    enum DRMarkVisible : char
    {
        DRMARK_VISIBLE_NONE = 0,
        DRMARK_VISIBLE_DISTRIBUTED,
        DRMARK_VISIBLE_PREFERRED,
        DRMARK_VISIBLE_ALWAYS,
    };
    static const std::wstring toStr(DRMarkVisible type);
    static DRMarkVisible toDRMarkVisible(const std::wstring& str);

    enum DRLineType : char
    {
        DRLINE_TYPE0 = 0,//____________________
        DRLINE_TYPE1,    //_ _ _ _ _
        DRLINE_TYPE2,    //__ __ __ __
        DRLINE_TYPE3,    //__ _ __ _ __ _
        DRLINE_TYPE4     //........
    };
    static const std::wstring& toStr(DRLineType type);
    static DRLineType toDRLineType(const std::wstring& str);

    enum DRFillType : char
    {
        DRFILL_TYPE0 = 0,// None
        DRFILL_TYPE1,    // ███████
        DRFILL_TYPE2,    // [brick tile]
        DRFILL_TYPE3,    // [chess tile]
        DRFILL_TYPE4     // [slash tile]
    };

    enum DRDimLinearStyle : char
    {
        DRDIM_LINEAR_STYLE0 = 0,//|--htext--|
        DRDIM_LINEAR_STYLE1,    //|-----vtext|
        DRDIM_LINEAR_STYLE2,    //|--htext--vtext|
        DRDIM_LINEAR_STYLE3,    //|<-htext   htext->|htext->|   htext->|htext->|
        DRDIM_LINEAR_STYLE4,    //|<-htext   htext->|--htext--|   htext->|--htext--|
        DRDIM_LINEAR_STYLE5,    //|    vtext|--htext--vtext|   vtext|--htext--vtext|
        DRDIM_LINEAR_STYLE6,    //|--htext------|--htext--|--htext--|--htext--|
                                //|<-htext vtext|    vtext|    vtext|    vtext|
        DRDIM_LINEAR_STYLE7,    //      vtext|
                                //     --------
    };

    static const std::wstring& toStr(DRDimLinearStyle type);
    static DRDimLinearStyle toDRDimLinearStyle(const std::wstring& str);

    enum DRDimAnglStyle : char
    {
        DRDIM_ANGL_STYLE_ANGL = 0,
        DRDIM_ANGL_STYLE_ANGL_BASIC,
        DRDIM_ANGL_STYLE_TRI,
        DRDIM_ANGL_STYLE_TRI_ANGL,
    };

    static const std::wstring& toStr(DRDimAnglStyle type);
    static DRDimAnglStyle toDRDimAnglStyle(const std::wstring& str);

    enum DRDimSideMarkType : char
    {
        DRDIM_SIDE_MARK_TYPE_NONE = 0,
        DRDIM_SIDE_MARK_TYPE_SPEC = 1,
        DRDIM_SIDE_MARK_TYPE_AUTO = 2,
    };

    enum DRDimSideMarkShape : char
    {
        DRDIM_SIDE_MARK_SHAPE_NONE = 0,
        DRDIM_SIDE_MARK_SHAPE_LEFT = 1,
        DRDIM_SIDE_MARK_SHAPE_BOTH = 2,
        DRDIM_SIDE_MARK_SHAPE_RIGHT = 3,
    };

    enum DRTextType : char
    {
        DRTEXT_TYPE_0 = 0,// Only text.
        DRTEXT_TYPE_1,    // With ref line.
        DRTEXT_TYPE_2,    // Parallel text.
        DRTEXT_TYPE_3,    // With standalone arrow-line.
    };

    enum DRTextFrame : char
    {
        DRTEXT_FRAME0 = 0,
        DRTEXT_FRAME1,//____
        DRTEXT_FRAME2,//[]
        DRTEXT_FRAME3,//()
        DRTEXT_FRAME4,//<>
        DRTEXT_FRAME5,// Circle
        DRTEXT_FRAME6,// Triangle
    };

    static const std::wstring& toStr(DRTextFrame type);
    static DRTextFrame toDRTextFrame(const std::wstring& str);

    enum DRTextAlignment : char
    {
        DRTEXT_ALIGN_NONE = 0,
        DRTEXT_ALIGN_LEFT,
        DRTEXT_ALIGN_RIGHT,
        DRTEXT_ALIGN_TOP,
        DRTEXT_ALIGN_BASELINE,
        DRTEXT_ALIGN_BOTTOM,
        DRTEXT_ALIGN_MIDDLE
    };

    static const std::wstring& toStr(DRTextAlignment type);
    static DRTextAlignment toDRTextAlignment(const std::wstring& str);

    enum DRFrameType : char
    {
        DRFRAME_TYPE0 = 0,
        DRFRAME_TYPE1,//____
        DRFRAME_TYPE2,//[]
        DRFRAME_TYPE3,//()
        DRFRAME_TYPE4,//<>
        DRFRAME_TYPE5,// Circle
        DRFRAME_TYPE6 // Triangle
    };

    enum DRArrowShape : char
    {
        DRARROW_NONE = 0,    // -
        DRARROW_SLASH,       // -/
        DRARROW_ARROW,       // ->|
        DRARROW_SOLID_ARROW, // -|>
        DRARROW_SPHERE,      // -o
        DRARROW_SOLID_SPHERE,// -(*)
        DRARROW_X,           // -X
    };

    static const std::wstring& toStr(DRArrowShape type);
    static DRArrowShape toDRArrowShape(const std::wstring& str);

    enum DRMarkTagType : char
    {
        DRMARK_TAG_NONE = 0,
        DRMARK_TAG_FRAME_BEGIN,
        DRMARK_TAG_FRAME_END,
        DRMARK_TAG_PROPS,
        DRMARK_TAG_WRAP_LINE,
        DRMARK_TAG_SPACE,
        DRMARK_TAG_BACKSPACE,
    };

    static const std::wstring& toStr(DRMarkTagType type);
    static DRMarkTagType toDRMarkTagType(const std::wstring& str);

    enum DRMarkLeaderLineType : char
    {
        DRMARK_LEADER_TYPE_NONE = 0,
        DRMARK_LEADER_TYPE_WITH_LINE,
        DRMARK_LEADER_TYPE_TRY_TOP_LEFT,
        DRMARK_LEADER_TYPE_TOP_LEFT,
        DRMARK_LEADER_TYPE_MIDDLE_HORIZON,
        DRMARK_LEADER_TYPE_MIDDLE_PARALLEL,
        DRMARK_LEADER_TYPE_TRY_MIDDLE_HORIZON,
        DRMARK_LEADER_TYPE_TRY_MIDDLE_PARALLEL,
        DRMARK_LEADER_TYPE_RESERVED8,
        DRMARK_LEADER_TYPE_TOP_CENTER,
    };

    enum DRNoteLeaderLineType : char
    {
        DRNOTE_LEADER_LINE_NONE = 0,
        DRNOTE_LEADER_WITH_LINE = 1,
        DRNOTE_LEADER_PARA_LINE = 2,
    };

    enum DRMarkSymbolShape : char
    {
        DRMARK_SYMBOL_SHAPE_NONE = 0,
        DRMARK_SYMBOL_SHAPE_CIRCLE,
        DRMARK_SYMBOL_SHAPE_CIRCLE_LINE
    };
    static const std::wstring& toStr(DRMarkSymbolShape type);
    static DRMarkSymbolShape toDRMarkSymbolShape(const std::wstring& str);

    enum DRSectionMarkSymbolShape : char
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
    };

    enum DRMarkPosition : char
    {
        DRMARK_POSITION_ABOVE_LINE = 0,             ///< 线上
        DRMARK_POSITION_BELOW_LINE = 1,             ///< 线下
        DRMARK_POSITION_SYMBOL_MIDDLE = 2,          ///< 符号中间
        DRMARK_POSITION_ABOVE_SYMBOL_CENTERLINE = 3,///< 符号中心线之上
        DRMARK_POSITION_BELOW_SYMBOL_CENTERLINE = 4 ///< 符号中心线之下
    };

    enum DRMarkSide : char
    {
        DRMARK_SIDE_BOTH = 0,
        DRMARK_SIDE_LEFT = 1,
        DRMARK_SIDE_RIGHT = 2,
    };

    enum DRMarkAutoRotate : char
    {
        DRMARK_AUTO_ROTATE_HORIZON = 0,    ///< 始终水平 (Always horizon)
        DRMARK_AUTO_ROTATE_ALWAYS = 1,     ///< 始终旋转 (Always rotate)
        DRMARK_AUTO_ROTATE_NO_VERTICAL = 2,///< 不要垂直旋转 (No rotate vertical)
        DRMARK_AUTO_ROTATE_ORTHO = 3       ///< 始终正交 (Always orthogonal)
    };

    enum DRWeldMarkVisible : char
    {
        DRMARK_WELD_VISIBLE_NONE = 0,
        DRMARK_WELD_VISIBLE_SITE,
        DRMARK_WELD_VISIBLE_WORKSHOP,
        DRMARK_WELD_VISIBLE_BOTH
    };
    static const std::wstring& toStr(DRWeldMarkVisible type);
    static DRWeldMarkVisible toDRWeldMarkVisible(const std::wstring& str);

    enum DRDetailSymbolFrame : char
    {
        DRDETAIL_SYMBOL_FRAME_NONE = 0,
        DRDETAIL_SYMBOL_FRAME_CIRCLE = 1,
        DRDETAIL_SYMBOL_FRAME_RECT = 2,
    };

    enum DRAxisVisible : char
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

    enum DRUpdateStatus : char
    {
        DRUPDATE_NONE = 0,
        DRUPDATE_UPDATED,
        DRUPDATE_CLONE,
        DRUPDATE_DIRTY,
        DRUPDATE_DELETE,
    };
    static const std::wstring& toStr(DRUpdateStatus val);
    static DRUpdateStatus toDRUpdateStatus(const std::wstring& val);

    enum DRChangeType : char
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
    static const std::wstring& toStr(DRChangeType val);
    static DRChangeType toDRChangeType(const std::wstring& val);

    enum DRNeighborPartVisible : char
    {
        DRNEIGHBOR_PART_VISIBLE_NONE = 0,
        DRNEIGHBOR_PART_VISIBLE_CONNECTED,
        DRNEIGHBOR_PART_VISIBLE_CONNECTING,
        DRNEIGHBOR_PART_VISIBLE_ALL_COMPONENTS,
        DRNEIGHBOR_PART_VISIBLE_BY_EXTREME,
    };

    enum DRNeighborMainSecondVisible : char
    {
        DRNEIGHBOR_MAIN_SECOND_VISIBLE_MAIN = 0,
        DRNEIGHBOR_MAIN_SECOND_VISIBLE_SECOND,
        DRNEIGHBOR_MAIN_SECOND_VISIBLE_BOTH,
    };

    // Protected area
    enum DRElementArea : char
    {
        DRELE_AREA_UNKNOWN = 0,
        DRELE_AREA_PART_CORNER,
        DRELE_AREA_PART_EDGE,
        DRELE_AREA_PART,
        DRELE_AREA_MARK,
        DRELE_AREA_DIM_ARROW,
        DRELE_AREA_DIM_LINE,
        DRELE_AREA_DIM_VALUE,
        DRELE_AREA_SECTION_LINE,
        DRELE_AREA_SECTION_MARK,
        DRELE_AREA_WELD_ARROW,
        DRELE_AREA_WELD_MARK,
    };

    enum DRPlacingMode : char
    {
        DRPLACING_MODE_FREE = 0,
        DRPLACING_MODE_FIX = 1,
    };

    enum DRProtectedAreaFlag : int
    {
        DRPROTECTED_AREA_UNKNOWN = 0,
        DRPROTECTED_AREA_PART_POINT = 1 << 0,
        DRPROTECTED_AREA_PART_LINE = 1 << 1,
        DRPROTECTED_AREA_PART_BODY = 1 << 2,
        DRPROTECTED_AREA_AXIS_LINE = 1 << 3,
        DRPROTECTED_AREA_AXIS_TEXT = 1 << 4,
        DRPROTECTED_AREA_MARK_TEXT = 1 << 5,
        DRPROTECTED_AREA_DIM_ARROW = 1 << 6,
        DRPROTECTED_AREA_DIM_LINE = 1 << 7,
        DRPROTECTED_AREA_DIM_VALUE = 1 << 8,
        DRPROTECTED_AREA_SECTION_LINE = 1 << 9,
        DRPROTECTED_AREA_SECTION_TEXT = 1 << 10,
        DRPROTECTED_AREA_WELD_ARROW = 1 << 11,
        DRPROTECTED_AREA_WELD_TEXT = 1 << 12,
    };

    static const int ORIENTATION_MARK_VIEW_DIR_EAST = 1 << 0;
    static const int ORIENTATION_MARK_VIEW_DIR_WEST = 1 << 1;
    static const int ORIENTATION_MARK_VIEW_DIR_SOUTH = 1 << 2;
    static const int ORIENTATION_MARK_VIEW_DIR_NORTH = 1 << 3;

    enum SymElementType : char
    {
        SYM_ELEMENT_UNKNOWN = 0,
        SYM_ELEMENT_LINE,
        SYM_ELEMENT_CIRCLE,
        SYM_ELEMENT_FILLED_AREA,
        SYM_ELEMENT_TEXT,
    };

    // Dynamic Point
    enum DPointMode : char
    {
        DPOINT_NONE = 0,
        DPOINT_XYZ,
        DPOINT_CTRL_POINT,
        DPOINT_OBJ_POINT
    };

    // Selector
    enum SelectorType : char
    {
        SEL_TYPE_NONE = 0,
        SEL_TYPE_3D,
        SEL_TYPE_DRAWING,
        SEL_TYPE_SYMBOL
    };
    enum SelectorMode : char
    {
        SEL_NONE = 0,
        SEL_SNAP,
        SEL_PICK
    };

    enum SelectorCaller : char
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

    enum NumericInputType : char
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

    // Snap Target
    enum SnapTarget : char
    {
        SNAP_TARGET_NONE = 0,
        SNAP_TARGET_POINT,
        SNAP_TARGET_PLANE
    };

    enum SnapMode : char
    {
        SNAP_MODE_NONE = 0,
        SNAP_MODE_PLANE,
        SNAP_MODE_3D,
        SNAP_MODE_AUTO
    };

    enum SnapRefPlane : char
    {
        SNAP_REF_PLANE_NONE = 0,
        SNAP_REF_PLANE_VIEW,
        SNAP_REF_PLANE_WORK,
    };

    // Snap flag
    static const uint32_t SNAP_NONE = 0u;
    static const uint32_t SNAP_PT_REF_POINT = (1u << 1);
    static const uint32_t SNAP_PT_GEO_POINT = (1u << 2);
    static const uint32_t SNAP_PT_LINE = (1u << 3);
    static const uint32_t SNAP_PT_CTRL_POINT = (1u << 6);
    static const uint32_t SNAP_PT_END_POINT = (1u << 7);
    static const uint32_t SNAP_PT_HELPER_POINT = (1u << 8);
    static const uint32_t SNAP_PT_GRID_CROSS_POINT = (1u << 9);
    static const uint32_t SNAP_PT_CENTER_POINT = (1u << 10);
    static const uint32_t SNAP_PT_MID_POINT = (1u << 11);
    static const uint32_t SNAP_PT_CROSS_POINT = (1u << 12);
    static const uint32_t SNAP_PT_FOOT_POINT = (1u << 13);
    static const uint32_t SNAP_PT_LINE_EXT = (1u << 14);
    static const uint32_t SNAP_PT_LINE_NEAREST = (1u << 15);
    static const uint32_t SNAP_PT_FREE = (1u << 16);
    static const uint32_t SNAP_PT_CENTER_FLAG = (1u << 17);
    static const uint32_t SNAP_PT_REF_FLAG = (1u << 18);
    static const uint32_t SNAP_PT_WORK_X_FLAG = (1u << 19);
    static const uint32_t SNAP_PT_WORK_Y_FLAG = (1u << 20);
    static const uint32_t SNAP_PT_WORK_Z_FLAG = (1u << 21);

    static const uint32_t SNAP_PT_WORK_AXIS_FLAGS = SNAP_PT_WORK_X_FLAG | SNAP_PT_WORK_Y_FLAG | SNAP_PT_WORK_Z_FLAG;

    static const uint32_t SNAP_PT_FLAGS = SNAP_PT_CENTER_FLAG | SNAP_PT_REF_FLAG | SNAP_PT_WORK_AXIS_FLAGS;

    static const uint32_t SNAP_PT_ALL = SNAP_PT_REF_POINT | SNAP_PT_GEO_POINT | SNAP_PT_LINE | SNAP_PT_END_POINT | SNAP_PT_HELPER_POINT |
                                        SNAP_PT_CENTER_POINT | SNAP_PT_MID_POINT | SNAP_PT_CROSS_POINT | SNAP_PT_FOOT_POINT | SNAP_PT_LINE_EXT |
                                        SNAP_PT_LINE_NEAREST | SNAP_PT_FREE;

    static const uint32_t SNAP_PL_GEO = (1u << 1);
    static const uint32_t SNAP_PL_TOP = (1u << 2);
    static const uint32_t SNAP_PL_BOTTOM = (1u << 3);
    static const uint32_t SNAP_PL_FRONT = (1u << 4);
    static const uint32_t SNAP_PL_BACK = (1u << 5);
    static const uint32_t SNAP_PL_HEAD = (1u << 6);
    static const uint32_t SNAP_PL_TAIL = (1u << 7);
    static const uint32_t SNAP_PL_CENTER = (1u << 8);
    static const uint32_t SNAP_PL_CUT = (1u << 9);
    static const uint32_t SNAP_PL_AXIS = (1u << 10);
    static const uint32_t SNAP_PL_COMP_AXIS = (1u << 11);

    static const uint32_t SNAP_PL_BOUND = SNAP_PL_TOP | SNAP_PL_BOTTOM | SNAP_PL_FRONT | SNAP_PL_BACK | SNAP_PL_HEAD | SNAP_PL_TAIL;

    static const uint32_t SNAP_PL_MAIN = SNAP_PL_BOUND | SNAP_PL_CENTER | SNAP_PL_CUT | SNAP_PL_AXIS | SNAP_PL_COMP_AXIS;

    static const uint32_t SNAP_PL_ALL = SNAP_PL_GEO | SNAP_PL_BOUND | SNAP_PL_CENTER | SNAP_PL_CUT | SNAP_PL_AXIS | SNAP_PL_COMP_AXIS | SNAP_PL_MAIN;

    // Pick Mode
    enum PickMode : char
    {
        PICK_MODE_NONE = 0,
        PICK_MODE_COMPONENT,
        PICK_MODE_COMPONENT_OBJ,
        PICK_MODE_ASSEMBLE,
        PICK_MODE_ASSEMBLE_OBJ
    };

    // Pick Mode
    static const uint32_t PICK_NONE = 0u;
    static const uint32_t PICK_ALL = 0xffffffffu;
    static const uint32_t PICK_CONNECTION = (1u << 1);
    static const uint32_t PICK_PART = (1u << 2);
    static const uint32_t PICK_SURF_TREATMENT = (1u << 3);
    static const uint32_t PICK_POINT = (1u << 4);
    static const uint32_t PICK_GRID = (1u << 5);
    static const uint32_t PICK_GRID_LINE = (1u << 6);
    static const uint32_t PICK_WELD = (1u << 7);
    static const uint32_t PICK_CUT = (1u << 8);
    static const uint32_t PICK_VIEW = (1u << 9);
    static const uint32_t PICK_HELPER = (1u << 10);
    static const uint32_t PICK_BOLTS = (1u << 11);
    static const uint32_t PICK_SINGLE_BOLT = (1u << 12);
    static const uint32_t PICK_RFBAR = (1u << 13);
    static const uint32_t PICK_PLANE = (1u << 14);
    static const uint32_t PICK_CTRL_POINT = (1u << 15);
    static const uint32_t PICK_DISTANCE = (1u << 16);

    static const uint32_t PICK_BAR_ALL = PICK_ALL & ~(PICK_GRID_LINE | PICK_SINGLE_BOLT);

    static const uint32_t PICK_DR_LINE = (1u << 17);
    static const uint32_t PICK_DR_TEXT = (1u << 18);
    static const uint32_t PICK_DR_MARK = (1u << 19);
    static const uint32_t PICK_DR_PART = (1u << 20);
    static const uint32_t PICK_DR_SECT_SYMBOL = (1u << 21);
    static const uint32_t PICK_DR_WELD = (1u << 22);
    static const uint32_t PICK_DR_VIEW = (1u << 23);
    static const uint32_t PICK_DR_DIM = (1u << 24);
    static const uint32_t PICK_DR_SINGLE_DIM = (1u << 25);
    static const uint32_t PICK_DR_GRID = (1u << 26);
    static const uint32_t PICK_DR_GRID_LINE = (1u << 27);
    static const uint32_t PICK_DR_DETAIL_SYMBOL = (1u << 28);
    static const uint32_t PICK_DR_PLUGIN = (1u << 29);

    static const uint32_t PICK_DR_BAR_ALL = PICK_ALL & ~(PICK_DR_SINGLE_DIM | PICK_DR_GRID_LINE);

    // Edit Handle Pick Mode
    static const uint32_t PICK_HANDLE_NONE = 0u;
    static const uint32_t PICK_HANDLE_HOVERING = (1u << 0);
    static const uint32_t PICK_HANDLE_CTRLPOINT = (1u << 1);
    static const uint32_t PICK_HANDLE_DIST_TAG = (1u << 2);
    static const uint32_t PICK_HANDLE_PLATE_EDGE_MID = (1u << 3);
    static const uint32_t PICK_HANDLE_PLATE_EDGE = (1u << 4);
    static const uint32_t PICK_HANDLE_PICK_ALL = 0xffffffffu & ~PICK_HANDLE_HOVERING;
    static const uint32_t PICK_HANDLE_HOVER_ALL = 0xffffffffu;

    // SteelPart
    enum SPTweenMode : char
    {
        SP_TWEEN_MODE_0 = 0,
        SP_TWEEN_MODE_1,
        SP_TWEEN_MODE_2,
        SP_TWEEN_MODE_3,
        SP_TWEEN_MODE_4,
        SP_TWEEN_MODE_5,
        SP_TWEEN_MODE_6
    };

    // for Bolt/Weld
    enum PartMadeType : char
    {
        PART_MADE_WORKSHOP = 0,
        PART_MADE_SITE
    };

    // RFBar
    enum RFBarOffsetBase : char
    {
        RFBAR_OFF_NONE = 0,
        RFBAR_OFF_COVER_THICKNESS,
        RFBAR_OFF_LEG_LENGTH
    };
    static const std::wstring& toStr(RFBarOffsetBase type);
    static RFBarOffsetBase toRFBarOffsetBase(const std::wstring& str);

    enum RFBarGroupMethod : char
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

    enum RFBarGroupIgnore : char
    {
        RFGROUP_IGNORE_NONE = 0,
        RFGROUP_IGNORE_FIRST,
        RFGROUP_IGNORE_LAST,
        RFGROUP_IGNORE_FIRST_LAST,
    };

    enum RFBarGroupShape : char
    {
        RFGROUP_SHAPE_NONE = 0,
        RFGROUP_SHAPE_NORMAL,
        RFGROUP_SHAPE_TAPERED,
        RFGROUP_SHAPE_TAPERED_RIDGE,
        RFGROUP_SHAPE_TAPERED_CURVED,
        RFGROUP_SHAPE_TAPERED_N,
        RFGROUP_SHAPE_SPIRAL
    };

    enum RFBarMeshType : char
    {
        RFBAR_MESH_NONE = 0,
        RFBAR_MESH_POLYGON,
        RFBAR_MESH_RECTANGLE,
        RFBAR_MESH_BENT
    };
    static const std::wstring& toStr(RFBarMeshType type);
    static RFBarMeshType toRFBarMeshType(const std::wstring& str);

    // Bolt
    enum BoltType : char
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

    // Bolt Part Flag
    static const uint32_t BOLT_SECT_WASHER_UP = 1u << 1;
    static const uint32_t BOLT_SECT_WASHER_DOWN0 = 1u << 2;
    static const uint32_t BOLT_SECT_WASHER_DOWN1 = 1u << 3;
    static const uint32_t BOLT_SECT_WASHER_DOWN2 = 1u << 4;
    static const uint32_t BOLT_SECT_WASHER_DOWN3 = 1u << 5;
    static const uint32_t BOLT_SECT_WASHER_DOWN4 = 1u << 6;
    static const uint32_t BOLT_SECT_NUT0 = 1u << 7;
    static const uint32_t BOLT_SECT_NUT1 = 1u << 8;
    static const uint32_t BOLT_SECT_NUT2 = 1u << 9;
    static const uint32_t BOLT_SECT_NUT3 = 1u << 10;
    static const uint32_t BOLT_SECT_NUT4 = 1u << 11;
    static const uint32_t BOLT_SECT_PARTS = (1u << 0)// FIXME: 向下兼容
                                            | BOLT_SECT_WASHER_UP | BOLT_SECT_WASHER_DOWN0 | BOLT_SECT_WASHER_DOWN1 | BOLT_SECT_WASHER_DOWN2 |
                                            BOLT_SECT_WASHER_DOWN3 | BOLT_SECT_WASHER_DOWN4 | BOLT_SECT_NUT0 | BOLT_SECT_NUT1 | BOLT_SECT_NUT2 |
                                            BOLT_SECT_NUT3 | BOLT_SECT_NUT4;

    static const uint32_t BOLT_SECT_OVERSIZED_HOLE0 = 1u << 16;
    static const uint32_t BOLT_SECT_OVERSIZED_HOLE1 = 1u << 17;
    static const uint32_t BOLT_SECT_OVERSIZED_HOLE2 = 1u << 18;
    static const uint32_t BOLT_SECT_OVERSIZED_HOLE3 = 1u << 19;
    static const uint32_t BOLT_SECT_OVERSIZED_HOLE4 = 1u << 20;

    enum BoltHoleType : char
    {
        BOLT_HOLE_OVERSIZED = 0,
        BOLT_HOLE_SLOTTED
    };
    enum BoltRotateSlot : char
    {
        BOLT_SLOT_PARALLEL = 0,
        BOLT_SLOT_ODD,
        BOLT_SLOT_EVEN
    };
    enum BoltGroupShape : char
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

    enum BoltEdgeDistTarget : char
    {
        BOLT_EDGE_DIST_TARGET_NONE = 0,
        BOLT_EDGE_DIST_TARGET_BOLT_D,
        BOLT_EDGE_DIST_TARGET_HOLE_D
    };

    // Weld
    enum WeldMode : char
    {
        WELD_MODE_NONE = 0,
        WELD_MODE_POLYGON,
        WELD_MODE_AUTO
    };

    enum WeldType : char
    {
        WELD_TYPE_NONE = 0,
        WELD_TYPE_EDGE_FLANGE,                                 // 1
        WELD_TYPE_SQUARE_GROOVE_SQUARE_BUTT,                   // 2
        WELD_TYPE_BEVEL_GROOVE_SINGLE_V_BUTT,                  // 3
        WELD_TYPE_BEVEL_GROOVE_BEVEL_BUTT,                     // 4
        WELD_TYPE_SINGLE_V_BUTT_WITH_BROAD_ROOT_FACE,          // 5
        WELD_TYPE_SINGLE_BEVEL_BUTT_WITH_BROAD_ROOT_FACE,      // 6
        WELD_TYPE_U_GROOVE_SINGLE_U_BUTT,                      // 7
        WELD_TYPE_J_GROOVE_J_BUTT,                             // 8
        WELD_TYPE_BEVEL_BACKING,                               // 9
        WELD_TYPE_FILLET,                                      // 10
        WELD_TYPE_PLUG,                                        // 11
        WELD_TYPE_SPOT,                                        // 12
        WELD_TYPE_SEAM,                                        // 13
        WELD_TYPE_SLOT,                                        // 14
        WELD_TYPE_FLARE_BEVEL_GROOVE,                          // 15
        WELD_TYPE_FLARE_V_GROOVE,                              // 16
        WELD_TYPE_CORNER_FLANGE,                               // 17
        WELD_TYPE_PARTIAL_PENET_SINGLE_BEVEL_BUTT_PLUS_FILLET, // 18
        WELD_TYPE_PARTIAL_PENET_SQUARE_GROOVE_PLUS_FILLET,     // 19
        WELD_TYPE_MELT_THROUGHT,                               // 20
        WELD_TYPE_STEEP_FLANKED_BEVEL_GROOVE_SINGLE_V_BUTT,    // 21
        WELD_TYPE_STEEP_FLANKED_BEVEL_GROOVE_SINGLE_BEVEL_BUTT,// 22
        WELD_TYPE_EDGE,                                        // 23
        WELD_TYPE_ISO_SURFACING,                               // 24
        WELD_TYPE_FOLD,                                        // 25
        WELD_TYPE_INCLINED                                     // 26
    };

    enum WeldAutoGroove : char
    {
        WELD_AUTO_GROOVE_NONE = 0,
        WELD_AUTO_GROOVE_TRUE,
        WELD_AUTO_GROOVE_FALSE,
        WELD_AUTO_GROOVE_AS_MAIN,
    };

    enum WeldContour : char
    {
        WELD_CONTOUR_NONE = 0,
        WELD_CONTOUR_1,
        WELD_CONTOUR_2,
        WELD_CONTOUR_3
    };

    enum WeldFinish : char
    {
        WELD_FINISH_NONE = 0,
        WELD_FINISH_G,
        WELD_FINISH_M,
        WELD_FINISH_C,
        WELD_FINISH_V,
        WELD_FINISH_W
    };

    enum WeldEdgeAround : char
    {
        WELD_EDGE = 0,
        WELD_AROUND
    };

    enum NDTInspecLevel : char
    {
        WELD_NDT_NONE = 0,
        WELD_NDT_A,
        WELD_NDT_B,
        WELD_NDT_C,
        WELD_NDT_D,
        WELD_NDT_E
    };

    enum DetailConnectType : char
    {
        DETAIL_CONNECT_AS_PART = 0,
        DETAIL_CONNECT_AS_SUB_ASSEMBLY
    };

    enum WeldElecClass : char
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

    enum WeldProcessType : char
    {
        WELD_PROCESS_NONE = 0,
        WELD_PROCESS_SMAW,
        WELD_PROCESS_SAW,
        WELD_PROCESS_GMAW,
        WELD_PROCESS_FCAW,
        WELD_PROCESS_ESW,
        WELD_PROCESS_EGW
    };

    // Comp Origin Type
    enum CompOriginType : char
    {
        COMP_ORIGIN_NONE = 0,
        COMP_ORIGIN_MIDDLE
    };

    // Comp UI Ctrl Type
    enum CompUIType : int
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
        COMP_CTRL_COMPOSITE_COMBO_IMAGE,
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
        COMP_CTRL_COMPOSITE_END
    };
};

}// namespace sketch
