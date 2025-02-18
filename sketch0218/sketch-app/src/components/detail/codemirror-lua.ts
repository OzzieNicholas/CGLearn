import { completeFromList, Completion } from '@codemirror/autocomplete'
import { lua } from '@codemirror/legacy-modes/mode/lua'
import { hoverTooltip } from '@codemirror/view'

let _luaLanguage = { ...lua }

const completions: Completion[] = [
  {
    label: 'value',
    info: 'value(param_path)\n\nReturns the value of a parameter',
    type: 'function'
  },
  {
    label: 'editable',
    info: 'editable(param_path)\n\nReturns the editable of a parameter',
    type: 'function'
  },
  {
    label: 'checked',
    info: 'checked(param_path)\n\nReturns the checked of a parameter',
    type: 'function'
  },
  {
    label: 'visible',
    info: 'visible(param_path)\n\nReturns the visible of a parameter',
    type: 'function'
  },
  {
    label: 'set_value',
    info: 'set_value(value)\n\nSet value of this parameter',
    type: 'function'
  },
  {
    label: 'set_editable',
    info: 'set_editable(value)\n\nSet editable of this parameter',
    type: 'function'
  },
  {
    label: 'set_checked',
    info: 'set_checked(value)\n\nSet checked of this parameter',
    type: 'function'
  },
  {
    label: 'set_visible',
    info: 'set_visible(value)\n\nSet visible of this parameter',
    type: 'function'
  },
  {
    label: 'in_data',
    info: 'in_data(node_path, port_index)\n\nGets the input data of a node',
    type: 'function'
  },
  {
    label: 'out_data',
    info: 'out_data(node_path, port_index)\n\nGets the output data of a node',
    type: 'function'
  },
  {
    label: 'has_in',
    info: 'has_in(node_path, port_index)\n\nReturns whether the specified port has input data',
    type: 'function'
  },
  {
    label: 'point',
    info: 'point(node_path, is_input, port_idx, attrib_name, element_idx)\n\nGets the point attribute value of a mesh',
    type: 'function'
  },
  {
    label: 'vertex',
    info: 'vertex(node_path, is_input, port_idx, attrib_name, element_idx)\n\nGets the vertex attribute value of a mesh',
    type: 'function'
  },
  {
    label: 'prim',
    info: 'prim(node_path, is_inpu, port_idxt, attrib_name, element_idx)\n\nGets the primitive attribute value of a mesh',
    type: 'function'
  },
  {
    label: 'global',
    info: 'global(node_path, is_input, port_idx, attrib_name)\n\nGets the global attribute value of a mesh',
    type: 'function'
  },
  {
    label: 'has_point_group',
    info: 'has_point_group(node_path, is_input, port_idx, group_name)\n\nReturns whether the specified point group exists',
    type: 'function'
  },
  {
    label: 'has_vertex_group',
    info: 'has_vertex_group(node_path, is_input, port_idx, group_name)\n\nReturns whether the specified vertex group exists',
    type: 'function'
  },
  {
    label: 'has_prim_group',
    info: 'has_prim_group(node_path, is_input, port_idx, group_name)\n\nReturns whether the specified primitive group exists',
    type: 'function'
  },
  {
    label: 'has_edge_group',
    info: 'has_edge_group(node_path, is_input, port_idx, group_name)\n\nReturns whether the specified edge group exists',
    type: 'function'
  },
  {
    label: 'has_point_attrib',
    info: 'has_point_attrib(node_path, is_input, port_idx, attrib_name)\n\nReturns whether the specified point attribute exists',
    type: 'function'
  },
  {
    label: 'has_vertex_attrib',
    info: 'has_vertex_attrib(node_path, is_input, port_idx, attrib_name)\n\nReturns whether the specified vertex attribute exists',
    type: 'function'
  },
  {
    label: 'has_prim_attrib',
    info: 'has_prim_attrib(node_path, is_input, port_idx, attrib_name)\n\nReturns whether the specified prim attribute exists',
    type: 'function'
  },
  {
    label: 'has_global_attrib',
    info: 'has_global_attrib(node_path, is_input, port_idx, attrib_name)\n\nReturns whether the specified global attribute exists',
    type: 'function'
  },
  {
    label: 'num_points',
    info: 'num_points(node_path, is_input, port_idx)\n\nGets number of points in a mesh',
    type: 'function'
  },
  {
    label: 'num_vertices',
    info: 'num_vertices(node_path, is_input, port_idx)\n\nGets number of vertices in a mesh',
    type: 'function'
  },
  {
    label: 'num_prims',
    info: 'num_prims(node_path, is_inpu, port_idx)\n\nGets number of prims in a mesh',
    type: 'function'
  }
]

_luaLanguage.languageData = {
  ...(_luaLanguage.languageData ? _luaLanguage.languageData : {}),
  commentTokens: { line: '--' },
  // eslint-disable-next-line quotes
  closeBrackets: { brackets: ['(', '[', '{', "'", '"'] }, // prettier-ignore
  autocomplete: completeFromList(completions)
}

export const luaLanguage = () => {
  const functions = completions.filter((c) => c.type === 'function')
  const functionRegex = new RegExp(`\\b(${functions.map((f) => f.label).join('|')})\\s*\\(`)
  _luaLanguage.token = (stream, state) => {
    if (stream.match(functionRegex)) {
      stream.backUp(1)
      return 'function'
    }
    return lua.token(stream, state)
  }
  return _luaLanguage
}

export const luaWordHover = hoverTooltip((view, pos, side) => {
  let { from, to, text } = view.state.doc.lineAt(pos)
  let start = pos,
    end = pos
  while (start > from && /\w/.test(text[start - from - 1])) start--
  while (end < to && /\w/.test(text[end - from])) end++
  if ((start == pos && side < 0) || (end == pos && side > 0)) return null
  const hover = text.slice(start - from, end - from)
  const func = completions.find((f) => f.label === hover)
  if (!func || !func.info) return null

  return {
    pos: start,
    end,
    above: true,
    create() {
      let dom = document.createElement('div')
      dom.textContent = func.info as string
      return { dom }
    }
  }
})
