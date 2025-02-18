export default {
  common: {
    click: '点击',
    menu: '菜单',
    file: '文件',
    edit: '编辑',
    settings: '设置',
    help: '帮助',
    new: '新建',
    open: '打开',
    save: '保存',
    'save-as': '另存为',
    quit: '退出',
    about: '关于',
    delete: '删除',
    undo: '撤销',
    redo: '重做',
    cut: '剪切',
    copy: '复制',
    paste: '粘贴',
    'not-editable': '不可编辑',
    unlock: '解锁',
    ok: '确定',
    cancel: '取消',
    'save-path': '保存路径',
    untitled: '未命名',
    color: '颜色',
    path: '路径',
    'type-search-here': '在此处键入搜索'
  },
  window: {
    'node-graph': '节点编辑器',
    '3d-viewport': '3D 视窗',
    'parameter-panel': '节点参数',
    'attribute-sheet': '属性数据表',
    'performance-profiler': '性能分析器'
  },
  menu: {
    'open-recent': '打开最近',
    'no-items': '无',
    'reset-layout': '重置布局',
    extract: '提取 & 分离',
    'reload-nda': '重载',
    'unlock-nda': '解锁',
    'publish-node': '发布节点',
    'paste-as-expr': '粘贴为表达式',
    'reset-value': '重置',
    'add-expr': '添加表达式',
    'delete-expr': '删除表达式',
    'copy-path': '复制路径',
    'copy-name': '复制名字',
    execute: '运行'
  },
  settings: {
    interface: '界面',
    translation: '翻译',
    editing: '编辑',
    editor: '编辑器',
    language: '语言',
    appearance: '外观',
    'background-color': '应用背景',
    'undo-history-max-size': '最大撤销步数',
    'node-graph': {
      'parallel-execution-nodes': '并行运行节点',
      'snap-to-grid': '网格吸附',
      'selection-mode': {
        key: '选择模式',
        partial: '部分',
        full: '全部'
      },
      background: {
        key: '背景',
        lines: '线',
        dots: '点',
        cross: '叉',
        none: '无'
      },
      'edge-style': {
        key: '连线样式',
        straight: '直线',
        bezier: '贝塞尔曲线',
        step: '阶梯曲线',
        'smooth-step': '平滑阶梯曲线'
      },
      'background-color': '节点编辑器背景'
    }
  },
  tooltip: {
    'node-graph': {
      fitview: '移动视图以显示所有节点',
      layout: '自动排列节点图',
      'unlock-graph': '解锁当前节点图',
      'toggle-minimap': '显示/隐藏 小地图',
      'auto-execution': '启用/禁用 自动执行'
    },
    viewport: {
      fitview: '移动摄像机以显示完整场景',
      'toggle-wireframe': '显示/隐藏 线框',
      'toggle-light-for-polygon': '启用/禁用 无光照模式',
      'toggle-point-index': '显示/隐藏 点序号',
      'toggle-show-point': '启用/禁用 始终显示点',
      'toggle-point-normal': '显示/隐藏 点法线',
      'toggle-prim-index': '显示/隐藏 图元序号',
      'toggle-prim-normal': '显示/隐藏 图元法线',
      'more-settings': '更多设置'
    },
    'attribute-sheet': {
      'no-data-to-display': '无数据',
      'no-attributes': '共有 $NUM_ELEMENTS 个没有 Attribute 的 $ATTRIB_CLASS'
    }
  },
  message: {
    'confirm-save-before-quit': '退出前是否先保存?',
    'confirm-save-before-open': '打开前是否先保存?',
    'confirm-save-before-new': '新建前是否先保存?',
    'save-and-open': '保存并打开',
    'save-and-quit': '保存并退出',
    'save-and-new': '保存并新建',
    'quit-anyway': '直接关闭',
    'open-anyway': '直接打开',
    'new-anyway': '直接新建'
  },
  'node-graph': {
    'node-name': '节点名字',
    'node-version': '节点版本',
    category: '节点类别',
    'pack-nodes': '根据选中的节点创建子图',
    'publish-node': '发布节点',
    'create-backdrop-from-selected-nodes': '根据所选节点创建背景',
    'node-info': {
      'execution-time': '运行耗时',
      'no-data': '无数据',
      point: '点',
      vertex: '顶点',
      prim: '图元',
      global: '全局',
      'num-points': '点',
      'num-vertices': '顶点',
      'num-prims': '图元',
      compressed: '压缩',
      shared: '共享',
      major: '主版本',
      minor: '次版本',
      attributes: '属性'
    }
  },
  viewport: {
    'background-color': '背景颜色',
    'wireframe-color': '线框颜色',
    'point-size': '点尺寸',
    'point-color': '点颜色',
    'index-size': '序号尺寸',
    'point-index-color': '点序号颜色',
    'point-normal-color': '点法线颜色',
    'point-normal-length': '点法线长度',
    'prim-index-color': '图元序号颜色',
    'prim-normal-color': '图元法线颜色',
    'prim-normal-length': '图元法线长度',
    'near-plane': '近平面',
    'far-plane': '远平面'
  },
  time: {
    s: '秒',
    ms: '毫秒'
  },
  parameter: {
    type: '类型',
    name: '名字',
    label: '标签',
    visible: '可见',
    editable: '可编辑',
    collapsible: '可折叠',
    expanded: '默认展开',
    checkable: '可勾选',
    checked: '默认勾选',
    expression: '表达式',
    tooltip: '提示',
    'enum-items': '菜单元素',
    'default-value': '默认值',
    'min-value': '最小值',
    'max-value': '最大值',
    'min-ui-value': 'UI最小值',
    'max-ui-value': 'UI最大值',
    'label-visible': '标签可见',
    'value-action': '值操作',
    'visible-action': '可见操作',
    'editable-action': '可编辑操作',
    'checked-action': '勾选操作'
  },
  'command-palette': {
    node: '节点',
    parameter: '参数',
    command: '命令',
    line: '行数',
    'type-name': '类型',
    'display-name': '名称'
  },
  command: {
    'export-graph-debug-info': '导出Graph调试信息',
    'collect-memory': '清理内存',
    'toggle-console-window': '显式/隐藏控制台窗口',
    'open-performance-profiler': '打开性能分析器',
    'open-tracy-profiler': '打开Tracy分析器',
    'enable-memory-profiling': '启用内存分析',
    'disable-memory-profiling': '关闭内存分析',
    'open-image-viewer': '打开图像查看器',
    'open-devtools': '打开开发者工具',
    'connect-to-server': '连接到服务器',
    'disconnect-from-server': '断开服务器连接'
  },
  builder: {
    'clone-node': '克隆节点'
  }
}