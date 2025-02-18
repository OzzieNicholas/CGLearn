import { NodeDef as NodeDef_, NodeKind, NodePortDef as NodePortDef_ } from '@/types/node-def'
import { NodeLog } from '@/types/node-graph'
import { LayoutTemplate, ParameterTemplate } from '@/types/parameter'
import { LayoutType, ParamType } from './definer'

export type GetAllNodeDefsResult = {
  node_defs: NodeDef[]
}

export type NodeParamItemsDef = {
  children?: ParamItemDef[]
}

export type NodeDef = NodeDef_ & NodeParamItemsDef

export type getNodeDefParams = [graph_id: number, node_type_name: string, version: number]

export type NodePortDef = NodePortDef_

export type GetNodesByGraphIdParams = [graph_id: number, subgraph_id: number]

export type GetNodeParams = [graph_id: number, subgraph_id: number, node_id: number]

export enum ParamItemDefType {
  Parameter = 0,
  Layout = 1
}

export type ParameterData = {
  name: string
  display_name?: string
  name_visible: boolean
  checkable: boolean
  checked?: boolean
  default_value: ParamValue
  editable: boolean
  max_value?: number
  min_value?: number
  min_ui_value?: number
  max_ui_value?: number
  step?: number
  tooltip: string
  type: ParamType
  visible: boolean
  builtin: boolean
  enum_items?: string[]
  expression?: string
  value_action?: string
  visible_action?: string
  editable_action?: string
  checked_action?: string
}

export type LayoutData = {
  name: string
  display_name?: string
  type: LayoutType
  editable: boolean
  visible: boolean
  expanded?: boolean
  builtin: boolean
  visible_action?: string
  editable_action?: string
}

export type ParamItemDefData = {
  item_type: ParamItemDefType
  def: ParameterData | LayoutData
}

export type ParamItemDef = {
  name: string
  data: ParamItemDefData
  children?: ParamItemDef[]
}

export interface Node_ {
  name: string
  type_name: string
  node_data: NodeData
}

export interface NodeData {
  id: number
  name: string
  type_name: string
  bypass: boolean
  editable?: boolean
  color: number[]
  display?: boolean
  input_num: number
  output_num: number
  pos_x: number
  pos_y: number
  custom_defs?: NodeParamItemsDef
  param_items: ParamItemType[]
  logs?: NodeLog[]
  type_version: number
}

export type Layout = {
  def_name: string
  visible: boolean
  editable: boolean
  collapsed: boolean
  param_items?: ParamItemType[]
}

export type Parameter = {
  def_name: string
  name: string
  expression?: string
  editable: boolean
  visible: boolean
  checked: boolean
  value: ParamValue
  param_items?: ParamItemType[]
}

export type ParamItemType = Layout | Parameter

export interface GetNodesByGraphIdResult {
  nodes: Node_[]
  links: NodeLink[]
  name: string
  bypass: boolean
  pos_x: number
  pos_y: number
  input_num: number
  output_num: number
  param_items: ParamItemType[]
}

export type NodeLink = {
  name: string
  node_links_in: Link[]
}

export type Link = {
  node_name: string
  out_port_id: number
  port_id: number
}

export type CreateNodeParams = [
  graph_id: number,
  subgraph_id: number,
  type_name: string,
  x: number,
  y: number
]

export type CreateNodeResult = {
  nodes: { id: number; name: string; color: number[] }[]
  links: NodeLink[]
}

export type RemoveNodeParams = [node_id: number]

export type RemoveNodeResult = {
  graph_id: string
  node_id: string
}

export type AddConnectParams = [
  graph_id: number,
  from_node: number,
  from_port: number,
  to_node: number,
  to_port: number
]

export type AddConnectResult = {
  graph_id: string
  from_node: string
  from_port: number
  to_node: string
  to_port: number
}

export type DeleteConnectParams = [graph_id: number, to_node: number, to_port: number]

export type DeleteConnectResult = {
  graph_id: string
  to_node: string
  to_port: number
}

export type SetNodeDisplayParams = [graph_id: number, subgraph_id: number, node_name: string]

export type SetNodeBypassParams = [graph_id: number, node_id: number, bypass: boolean]

export type GetNodeNameByIdParams = [node_id: number]

export type GetParamDefParams = [graph_id: number, node_id: number, param_name: string]

export type GetParamValueParams = [graph_id: number, node_id: number, param_name: string]

export type GetParamValueByPathParams = [graph_id: number, param_path: string]

export type GetParamPathParams = [graph_id: number, node_id: number, param_name: string]

export type GetRelativeParamPathParams = [
  graph_id: number,
  node_id: number,
  target_param_name: string,
  src_param_path: string
]

export type GetAllParamsDefsParams = [graph_id: number, node_id: number]

export type GetAllParamsDefsResult = {
  param_defs: ParameterData[]
}

export type ParamValue = number | string | boolean | number[] | ColorRampValue[] | ScalarRampValue[]

export type ColorRampValue = {
  pos: number
  value: number[]
  method: string
}

export type ScalarRampValue = {
  pos: number
  value: number
  method: string
}

export type GetParamValueResult = {
  value: ParamValue
}

export type GetNodeIdByNameParams = [graph_id: number, subgraph_id: number, node_name: string]

export type SetParamParams = [
  graph_id: number,
  node_id: number,
  param_name: string,
  value: ParamValue
]

export type SetParamExpressionParams = [
  graph_id: number,
  node_id: number,
  param_name: string,
  expression: string
]

export type SetParamCheckedParams = [
  graph_id: number,
  node_id: number,
  param_name: string,
  checked: boolean
]

export type SetParamVisibleParams = [
  graph_id: number,
  node_id: number,
  param_name: string,
  visible: boolean
]

export type SetParamEditableParams = [
  graph_id: number,
  node_id: number,
  param_name: string,
  editable: boolean
]

export type SetLayoutCollapsedParams = [
  graph_id: number,
  node_id: number,
  layout_name: string,
  collapsed: boolean
]

export type SaveGraphParams = [graph_id: number, file_path: string]

export type SaveGraphResult = {
  result: boolean
}

export type LoadGraphParams = [graph_id: number, file_path: string]

export type LoadGraphResult = {
  graph_id: number
  root_subgraph_id: number
  result: boolean
}

export type GetParamEnumListParams = [graph_id: number, node_id: number, param_name: string]

export type GetParamEnumListResult = {
  enum_list: string[]
}

export type GetParamExpressionParams = [graph_id: number, node_id: number, param_name: string]

export type GetParamExpressionResult = {
  expression: string
}

export type SetNodePositionParams = [graph_id: number, node_id: number, x: number, y: number]

export type AttributeType = 'point' | 'vertex' | 'prim' | 'global'

export type NewGraphResult = {
  graph_id: number
  subgraph_id: number
}

export type GetCurrentGraphResult = {
  graph_id: number
  subgraph_id: number
  graph_url: string
}

export type ExecuteToNodeParams = [graph_id: number, node_id: number, parallel: boolean]

export type getNodePathParams = [graph_id: number, node_id: number]

export type PublishNdaParams = [
  graph_id: number,
  subgraph_id: number,
  type_name: string,
  category: string,
  file_path: string,
  type_version: number
]

export type PublishNdaResult = {
  result: boolean
}

export type SetNodeNameParams = [node_id: number, name: string]

export type CreateSubgraphFromNodesParams = [
  graph_id: number,
  subgraph_id: number,
  node_list: {
    value: number[]
  }
]

export type GetRampViewParams = [value: ColorRampValue[] | ScalarRampValue[], sample_num: number]

export type ScalarRampView = {
  x: number
  y: number
}

export type ColorRampView = {
  x: number
  color: number[]
}
export type GetRampViewResult = {
  view: ScalarRampView[] | ColorRampView[]
}

export type ParamsChangePayload = {
  node_path: string
  param_name: string
  action_type: number
}

export type SetNodeColorParams = [graph_id: number, node_id: number, rgba: object]

export type GetNodeSpaceDescByPathParams = [graph_id: number, node_path: string]

export type GetNodeSpaceDescByPathResult = {
  id: number
  kind: NodeKind
  editable?: boolean
}

export type SetNodeEditableParams = [node_id: number, editable: boolean]

export type GetNdaFilePathParams = [node_id: number]

export type CopyNodesParams = [
  graph_id: number,
  subgraph_id: number,
  node_list: {
    value: number[]
  },
  copy_links: boolean
]

export type PasteNodesParams = [
  graph_id: number,
  subgraph_id: number,
  clipboard: string,
  move_nodes: boolean,
  x: number,
  y: number
]

export type GetNodeInfoParams = [graph_id: number, node_id: number, port_id: number]

export type GetNodeInputMeshesAttribsParams = [graph_id: number, node_id: number]

export type GetNodeInputMeshesAttribsResult = {
  data: Record<AttributeType, { name: string; type: string }[]>[]
}

export type GetRampMethodsResult = {
  methods: string[]
}

export type DslFunctionDesc = {
  name: string
  doc: string
  overloads: string[]
}

export type DslLanguageReference = {
  types: string[]
  operators: string[]
  keywords: string[]
  functions: DslFunctionDesc[]
}

export type BatchGetNodeOutputMeshAttrsParams = [
  graph_id: number,
  node_id: number,
  port_id: number,
  attr_class: 'point' | 'vertex' | 'prim' | 'global',
  batch_desc: {
    begin: number
    end: number
    full_precision: boolean
    attrib_names: string[]
    component_indices: number[]
  }
]

export type AddCustomParamParams = [
  graph_id: number,
  node_id: number,
  parent_layout_name: string,
  param_template: LayoutData
]

export type RemoveCustomParamParams = [graph_id: number, node_id: number, param_name: string]

export type AddCustomLayoutParams = [
  graph_id: number,
  node_id: number,
  parent_layout_name: string,
  layout_template: LayoutData
]

export type RemoveCustomLayoutParams = [graph_id: number, node_id: number, layout_name: string]

export type RebuildNodeParametersParams = [
  graph_id: number,
  node_id: number,
  templates: {
    param_items: (ParameterTemplate | LayoutTemplate)[]
  }
]

export type GetNodeDataVersionParams = [graph_id: number, node_id: number, port_id: number]

export type SearchNodesParams = [graph_id: number, search_text: string, subgraph_id: number]

export type SearchNodeDesc = {
  id: number
  name: string
  type_name: string
  path: string
  category: string
}

export type SearchNodesResult = {
  result: SearchNodeDesc[]
}

export type SearchParamsParams = [graph_id: number, search_text: string, subgraph_id: number]

export type SearchParamDesc = {
  id: number
  name: string
  display_name: string
  type: string
  node_id: number
  node_name: string
  node_type_name: string
  node_path: string
  node_category: string
  match_name: string // name or display_name
}

export type SearchParamsResult = {
  result: SearchParamDesc[]
}

export type AttribMemoryUsageInfo = {
  name: string
  size: number
  version: number
  prims: number
  class: string
  type: string
}

export type MeshMemoryUsageInfo = {
  version: number
  size: number
  points_size: number
  vertices_size: number
  prims_size: number
  num_points: number
  num_vertices: number
  num_prims: number
  point_attribs: AttribMemoryUsageInfo[]
  vertex_attribs: AttribMemoryUsageInfo[]
  prim_attribs: AttribMemoryUsageInfo[]
  global_attribs: AttribMemoryUsageInfo[]
}

export type NodeMeshMemoryUsageInfo = MeshMemoryUsageInfo & {
  path: string
  type: string
  port: number
  execution_time: number
}

export type GetMeshMemoryUsageInfoParams = [graph_id: number, node_id: number, port_id: number]

export type GetAllMeshesMemoryUsageInfoParams = [graph_id: number]

export type GetAllMeshesMemoryUsageInfoResult = {
  result: NodeMeshMemoryUsageInfo[]
}

export type OrbitViewportParams = [x: number, y: number]

export type PanViewportParams = [x: number, y: number]

export type ZoonViewportParams = [delta: number]

export type SetViewportMeshParams = [node_id: number, port_idx: number, force_update?: boolean]

export type BoolParams = [value: boolean]
export type NumberParams = [value: number]
export type Color3Params = [r: number, g: number, b: number]
export type Color4Params = [r: number, g: number, b: number, a?: number]
