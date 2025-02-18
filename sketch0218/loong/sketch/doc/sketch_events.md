# 说明

本文档定义了 Sketch 和前端的交互事件，包括输入事件和输出事件

# 可用控件

| 控件名              | 用途                                                         |
| :------------------ | :----------------------------------------------------------- |
| Create_Spline       | 创建样条曲线；默认控制点，按 shift 切换为内插点              |
| Create_Circle       | 创建圆；默认创建 24 等分的离散圆，按 shift 切换 nurbs 参数圆 |
| Create_Rectangle    | 创建矩形；默认创建多边形矩形，按 shift 切换 nurbs 曲面矩形   |
| Create_StraightLine | 创建折线                                                     |
| Common_Buffer       | 多边形偏移工具                                               |
| Edit_Nurbs          | Nurbs 编辑工具                                               |
| Extrude_Curve       | 沿特定方向拉伸曲线构建曲面                                   |
| Extrude_Path        | 沿选中路径方向拉伸对象                                       |
| Extrude_Polygon     | 多边形推拉工具，按 shift 切换 add 和 move 模式               |
| Common_Rotate       | 通用旋转工具                                                 |
| Common_Scale        | 通用缩放工具                                                 |
| Common_Translate    | 通用移动工具                                                 |
| Common_TRS          | 通用移动/缩放/旋转工具                                       |

# 输入事件

| 事件              | 参数    | 参数类型 | 说明     |
| :---------------- | :------ | :------- | :------- |
| controller_input  | value   | wstring  | 输入数值 |
|                   | confirm | bool     | 是否确认 |
| delete            | -       | -        | -        |
| copy              | -       | -        | -        |
| paste             | -       | -        | -        |
| controller_escape | -       | -        | -        |

# 输出事件

| 事件                | 参数          | 参数类型 | 说明             |
| :------------------ | :------------ | :------- | :--------------- |
| controller_active   | name          | wstring  | controller 名称  |
| controller_deactive | name          | wstring  | controller 名称  |
| controller_update   | name          | wstring  | controller 名称  |
|                     | current_param | wstring  | 当前更新的参数名 |
|                     | value         | wstring  | 当前更新的数值   |
| gizmo_enable        | -             | -        | 启动 Gizmo       |
| gizmo_disable       | -             | -        | 关闭 Gizmo       |

<!-- |controller_mouse_lock | -   | -       | controller鼠标锁定 -->
<!-- |controller_mouse_unlock| -  | -       | controller鼠标解锁 -->
