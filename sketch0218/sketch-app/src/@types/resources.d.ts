export default interface Resources {
  common: {
    click: 'Click'
    menu: 'Menu'
    file: 'File'
    edit: 'Edit'
    settings: 'Settings'
    help: 'Help'
    new: 'New'
    open: 'Open'
    save: 'Save'
    'save-as': 'Save As'
    quit: 'Quit'
    about: 'About'
    delete: 'Delete'
    undo: 'Undo'
    redo: 'Redo'
    cut: 'Cut'
    copy: 'Copy'
    paste: 'Paste'
    'not-editable': 'Not Editable'
    unlock: 'Unlock'
    ok: 'Ok'
    cancel: 'Cancel'
    'save-path': 'Save path'
    untitled: 'untitled'
    color: 'Color'
    path: 'Path'
    'type-search-here': 'Type search here'
  }
  window: {
    'node-graph': 'Node Graph'
    '3d-viewport': '3D Viewport'
    'parameter-panel': 'Node Parameters'
    'attribute-sheet': 'Attribute Sheet'
    'performance-profiler': 'Performance Profiler'
  }
  menu: {
    'open-recent': 'Open Recent'
    'no-items': 'No items'
    'reset-layout': 'Reset layout'
    extract: 'Extract & Isolate'
    'reload-nda': 'Reload from disk'
    'unlock-nda': 'Unlock'
    'publish-node': 'Publish node'
    'paste-as-expr': 'Paste as expression'
    'reset-value': 'Reset value'
    'add-expr': 'Add expression'
    'delete-expr': 'Delete expression'
    'copy-path': 'Copy path'
    'copy-name': 'Copy name'
    execute: 'Execute'
  }
  settings: {
    interface: 'Interface'
    translation: 'Translation'
    editing: 'Editing'
    language: 'Language'
    editor: 'Editor'
    languages: {
      'zh.cn': '简体中文'
      en: 'English'
    }
    appearance: 'Appearance'
    'background-color': 'App background'
    'undo-history-max-size': 'Undo history max size'
    'node-graph': {
      'parallel-execution-nodes': 'Parallel execution nodes'
      'snap-to-grid': 'Snap to grid'
      'selection-mode': {
        key: 'Selection mode'
        partial: 'Partial'
        full: 'Full'
      }
      background: {
        key: 'Background'
        lines: 'Lines'
        dots: 'Dots'
        cross: 'Cross'
        none: 'None'
      }
      'edge-style': {
        key: 'Edge style'
        straight: 'Straight'
        bezier: 'Bezier'
        step: 'Step'
        'smooth-step': 'Smooth Step'
      }
      'background-color': 'Node graph background'
    }
  }
  tooltip: {
    'node-graph': {
      fitview: 'Move view to display all nodes'
      layout: 'Auto layout node graph'
      'unlock-graph': 'Unlock current graph'
      'toggle-minimap': 'Toggle mini map'
      'auto-execution': 'Toggle auto execution'
    }
    viewport: {
      fitview: 'Move camera to fully display the scene'
      'toggle-light-for-polygon': 'Toggle disable light for polygon'
      'toggle-wireframe': 'Toggle wireframe'
      'toggle-point-index': 'Toggle point index'
      'toggle-show-point': 'Always show point'
      'toggle-point-normal': 'Toggle point normal'
      'toggle-prim-index': 'Toggle primitive index'
      'toggle-prim-normal': 'Toggle primitive normal'
      'more-settings': 'More settings'
    }
    'attribute-sheet': {
      'no-data-to-display': 'No data to display'
      'no-attributes': 'There is $NUM_ELEMENTS $ATTRIB_CLASS in total but with no attributes'
    }
  }
  message: {
    'confirm-save-before-quit': 'Save project before quit?'
    'confirm-save-before-open': 'Save project before open?'
    'confirm-save-before-new': 'Save project before new?'
    'save-and-open': 'Save and open'
    'save-and-quit': 'Save and quit'
    'save-and-new': 'Save and new'
    'quit-anyway': 'Quit anyway'
    'open-anyway': 'Open anyway'
    'new-anyway': 'New anyway'
  }
  'node-graph': {
    'node-name': 'Node name'
    'node-version': 'Node version'
    category: 'Category'
    'pack-nodes': 'Create subgraph from selected nodes'
    'publish-node': 'Publish node'
    'create-backdrop-from-selected-nodes': 'Create backdrop from selected nodes'
    'node-info': {
      'execution-time': 'Execution time'
      'no-data': 'No data'
      point: 'Point'
      vertex: 'Vertex'
      prim: 'Primitive'
      global: 'Global'
      'num-points': 'Points'
      'num-vertices': 'Vertices'
      'num-prims': 'Primitives'
      compressed: 'Compressed'
      shared: 'Shared'
      major: 'Major'
      minor: 'Minor'
      attributes: 'Attributes'
    }
  }
  viewport: {
    'background-color': 'Background color'
    'wireframe-color': 'Wireframe color'
    'point-size': 'Point size'
    'point-color': 'Point color'
    'near-plane': 'Near Plane'
    'far-plane': 'Far Plane'
    'index-size': 'Index size'
    'point-index-color': 'Point index color'
    'point-normal-color': 'Point normal color'
    'point-normal-length': 'Point normal length'
    'prim-index-color': 'Primitive index color'
    'prim-normal-color': 'Primitive normal color'
    'prim-normal-length': 'Primitive normal length'
  }
  time: {
    s: 's'
    ms: 'ms'
  }
  parameter: {
    type: 'Type'
    name: 'Name'
    label: 'Label'
    visible: 'Visible'
    editable: 'Editable'
    collapsible: 'Collapsible'
    expanded: 'Expanded'
    checkable: 'Checkable'
    checked: 'Checked'
    expression: 'Expression'
    tooltip: 'Tooltip'
    'enum-items': 'Enum Items'
    'default-value': 'Default Value'
    'min-value': 'Min Value'
    'max-value': 'Max Value'
    'min-ui-value': 'Min UI Value'
    'max-ui-value': 'Max UI Value'
    'label-visible': 'Label Visible'
    'value-action': 'Value Action'
    'visible-action': 'Visible Action'
    'editable-action': 'Editable Action'
    'checked-action': 'Checked Action'
  }
  'command-palette': {
    node: 'Node'
    parameter: 'Parameter'
    command: 'Command'
    line: 'Line To'
    'type-name': 'Type name'
    'display-name': 'Name'
  }
  command: {
    'export-graph-debug-info': 'Export graph debug info'
    'collect-memory': 'Collect memory'
    'toggle-console-window': 'Toggle console window'
    'open-performance-profiler': 'Open performance profiler'
    'open-tracy-profiler': 'Open tracy profiler'
    'enable-memory-profiling': 'Enable memory profiling'
    'disable-memory-profiling': 'Disable memory profiling'
    'open-image-viewer': 'Open image viewer'
    'open-devtools': 'Open devtools'
    'connect-to-server': 'Connect to server'
    'disconnect-from-server': 'Disconnect from server'
  }
  builder: {
    'clone-node': 'Clone Node'
  }
}