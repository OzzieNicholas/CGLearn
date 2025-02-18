import { NodeInfo } from '@/types/node-graph'
import { invoke as _invoke } from '@tauri-apps/api/core'
import {
  AddConnectParams,
  AddCustomLayoutParams,
  AddCustomParamParams,
  BatchGetNodeOutputMeshAttrsParams,
  BoolParams,
  Color3Params,
  Color4Params,
  CopyNodesParams,
  CreateNodeParams,
  CreateNodeResult,
  CreateSubgraphFromNodesParams,
  DeleteConnectParams,
  DslLanguageReference,
  ExecuteToNodeParams,
  GetAllMeshesMemoryUsageInfoParams,
  GetAllMeshesMemoryUsageInfoResult,
  GetAllNodeDefsResult,
  GetAllParamsDefsParams,
  GetAllParamsDefsResult,
  GetCurrentGraphResult,
  GetMeshMemoryUsageInfoParams,
  GetNdaFilePathParams,
  GetNodeDataVersionParams,
  GetNodeIdByNameParams,
  GetNodeInfoParams,
  GetNodeInputMeshesAttribsParams,
  GetNodeInputMeshesAttribsResult,
  GetNodeNameByIdParams,
  GetNodeParams,
  GetNodeSpaceDescByPathParams,
  GetNodeSpaceDescByPathResult,
  GetNodesByGraphIdParams,
  GetNodesByGraphIdResult,
  GetParamDefParams,
  GetParamEnumListParams,
  GetParamEnumListResult,
  GetParamExpressionParams,
  GetParamExpressionResult,
  GetParamPathParams,
  GetParamValueByPathParams,
  GetParamValueParams,
  GetParamValueResult,
  GetRampMethodsResult,
  GetRampViewParams,
  GetRampViewResult,
  GetRelativeParamPathParams,
  LoadGraphParams,
  LoadGraphResult,
  MeshMemoryUsageInfo,
  NewGraphResult,
  NodeData,
  NodeDef,
  NumberParams,
  OrbitViewportParams,
  PanViewportParams,
  ParamValue,
  ParameterData,
  PasteNodesParams,
  PublishNdaParams,
  PublishNdaResult,
  RebuildNodeParametersParams,
  RemoveCustomLayoutParams,
  RemoveCustomParamParams,
  RemoveNodeParams,
  SaveGraphParams,
  SaveGraphResult,
  SearchNodesParams,
  SearchNodesResult,
  SearchParamsParams,
  SearchParamsResult,
  SetLayoutCollapsedParams,
  SetNodeBypassParams,
  SetNodeColorParams,
  SetNodeDisplayParams,
  SetNodeEditableParams,
  SetNodeNameParams,
  SetNodePositionParams,
  SetParamCheckedParams,
  SetParamEditableParams,
  SetParamExpressionParams,
  SetParamParams,
  SetParamVisibleParams,
  SetViewportMeshParams,
  ZoonViewportParams,
  getNodeDefParams,
  getNodePathParams
} from './index.types'

export * from './definer'
import { Mutex } from 'async-mutex'

const execute_and_change_node_mutext = new Mutex()

function invoke<T>(funcName: string, args?: (string | number | boolean | object)[]) {
  return _invoke<T>('invoke', {
    func: funcName,
    args: args ? JSON.stringify({ args }) : '[]'
  })
}

export const getAllNodeDefs = async () => {
  const resStr = await invoke<string>('get_all_node_defs', [''])
  // console.log('get_all_node_defs: ', JSON.parse(resStr))
  return (JSON.parse(resStr) as GetAllNodeDefsResult).node_defs
}

export const getNodeDef = async (params: getNodeDefParams) => {
  const resStr = await invoke<string>('get_node_def', params)
  // console.log('get_node_def: ', JSON.parse(resStr))
  const res = JSON.parse(resStr) as NodeDef
  return res
}

export const createNode = async (params: CreateNodeParams) => {
  // console.log('create node ', params)
  const res = JSON.parse(await invoke<string>('create_node', params)) as CreateNodeResult
  // console.log(res)
  return res
}

export const removeNode = (params: RemoveNodeParams) => {
  // console.log('remove node ', params)
  return execute_and_change_node_mutext.runExclusive(async () => {
    return invoke('delete_node', params)
  })
}

export const setNodeDisplay = (params: SetNodeDisplayParams) => invoke('set_display_node', params)

export const setNodeBypass = (params: SetNodeBypassParams) => invoke('set_node_bypass', params)

export const addConnect = (params: AddConnectParams) => {
  // console.log('add link', params)
  return invoke('add_link', params)
}

export const deleteConnect = (params: DeleteConnectParams) => {
  return execute_and_change_node_mutext.runExclusive(async () => {
    return invoke('delete_link', params)
  })
}

export const getNodeNameById = (params: GetNodeNameByIdParams) =>
  invoke<string>('get_node_name_by_address', params)

export const getNodesByGraphId = async (params: GetNodesByGraphIdParams) => {
  const resStr = await invoke<string>('get_all_nodes', params)
  if (resStr.length == 0) return null
  // console.log('get_all_nodes: ', JSON.parse(resStr))
  return JSON.parse(resStr) as GetNodesByGraphIdResult
}

export const getNode = async (params: GetNodeParams) => {
  const resStr = await invoke<string>('get_node', params)
  // console.log('get_node: ', JSON.parse(resStr))
  return JSON.parse(resStr) as NodeData
}

export const getParamDef = async (params: GetParamDefParams) => {
  const resStr = await invoke<string>('get_param_def', params)
  return JSON.parse(resStr) as ParameterData
}

export async function getParamValue(params: GetParamValueParams): Promise<ParamValue> {
  const resStr = await invoke<string>('get_param_value', params)
  // console.log('get_param_value: ', JSON.parse(resStr))
  return (JSON.parse(resStr) as GetParamValueResult).value
}

export async function getParamValueByPath(
  params: GetParamValueByPathParams
): Promise<ParamValue | undefined> {
  const resStr = await invoke<string>('get_param_value_by_path', params)
  if (resStr.length == 0) return undefined
  return (JSON.parse(resStr) as GetParamValueResult).value
}

export async function getParamPath(params: GetParamPathParams): Promise<string> {
  return invoke<string>('get_param_path', params)
}

export async function getRelativeParamPath(params: GetRelativeParamPathParams): Promise<string> {
  return invoke<string>('get_relative_param_path', params)
}

export const getAllParamsDefs = async (
  params: GetAllParamsDefsParams
): Promise<ParameterData[]> => {
  const resStr = await invoke<string>('get_all_param_defs', params)
  // console.log('get_all_param_defs: ', JSON.parse(resStr))
  return (JSON.parse(resStr) as GetAllParamsDefsResult).param_defs
}

export const getNodeIdByName = async (params: GetNodeIdByNameParams) => {
  const res = await invoke<string>('get_node_id_by_name', params)
  return parseInt(res)
}

export const setParam = (params: SetParamParams) => {
  // console.log('set params', params)
  return execute_and_change_node_mutext.runExclusive(async () => {
    return await invoke('set_param', [params[0], params[1], params[2], { value: params[3] }])
  })
}

export const setParamExpression = (params: SetParamExpressionParams) => {
  return invoke('set_param_expression', params)
}

export const setParamChecked = (params: SetParamCheckedParams) => {
  return invoke('set_param_checked', params)
}

export const setParamVisible = (params: SetParamVisibleParams) => {
  return invoke('set_param_visible', params)
}

export const setParamEditable = (params: SetParamEditableParams) => {
  return invoke('set_param_editable', params)
}

export const setLayoutCollapsed = (params: SetLayoutCollapsedParams) => {
  return invoke('set_layout_collapsed', params)
}

export const saveGraph = async (params: SaveGraphParams): Promise<boolean> => {
  const resStr = await invoke<string>('save_graph', params)
  return (JSON.parse(resStr) as SaveGraphResult).result
}

export const loadGraph = async (params: LoadGraphParams) => {
  const resStr = await invoke<string>('load_graph', params)
  return JSON.parse(resStr) as LoadGraphResult
}

export const getParamEnumList = async (params: GetParamEnumListParams): Promise<string[]> => {
  const resStr = await invoke<string>('get_param_enum_list', params)
  if (resStr.length == 0) return []
  // console.log('get_param_enum_list: ', JSON.parse(resStr))
  return (JSON.parse(resStr) as GetParamEnumListResult).enum_list
}

export const getParamExpression = async (params: GetParamExpressionParams): Promise<string> => {
  const resStr = await invoke<string>('get_param_expression', params)
  return (JSON.parse(resStr) as GetParamExpressionResult).expression
}

export const setNodePosition = (params: SetNodePositionParams) =>
  invoke('set_node_position', params)

export const newGraph = async () => {
  // console.log('new_graph')
  const resStr = await invoke<string>('new_graph')
  return JSON.parse(resStr) as NewGraphResult
}

export const getCurrentGraph = async () => {
  // console.log('get_current_graph')
  const resStr = await invoke<string>('get_current_graph')
  return JSON.parse(resStr) as GetCurrentGraphResult
}

export async function rustLog(text: string) {
  await _invoke('rust_log', {
    text
  })
}

export async function writeTextFile(path: string, content: string) {
  await _invoke('write_text_file', {
    path,
    content
  })
}

export async function openProfiler() {
  await _invoke('open_profiler')
}

export async function openTracyProfiler() {
  await _invoke('open_tracy_profiler')
}

export async function openImageViewer() {
  await _invoke('open_image_viewer')
}

export async function openDevtools() {
  await _invoke('open_devtools')
}

export async function connectToServer(port: number) {
  return _invoke<boolean>('connect_to_server', { port })
}

export async function disconnectFromServer() {
  return _invoke<boolean>('disconnect_from_server')
}

export async function getServerDefaultPort() {
  return _invoke<number>('get_server_default_port')
}

export async function getCurrentBackendAddr() {
  return _invoke<string | null>('get_current_backend_addr')
}

export async function reloadAll() {
  _invoke('reload_all')
}

export const executeToNode = (params: ExecuteToNodeParams) => invoke('execute_to_node', params)

export const getNodePath = (params: getNodePathParams) => invoke<string>('get_node_path', params)

export const publishNda = async (params: PublishNdaParams): Promise<boolean> => {
  const resStr = await invoke<string>('publish_nda', params)
  if (resStr.length == 0) return false
  return (JSON.parse(resStr) as PublishNdaResult).result
}

export const setNodeName = (params: SetNodeNameParams) => {
  // console.log('set node name', params)
  return invoke('set_node_name', params) as Promise<string>
}

export const createSubgraphFromNodes = async (params: CreateSubgraphFromNodesParams) => {
  const res = await invoke<string>('create_subgraph_from_nodes', params)
  if (res.length == 0) return 0
  return parseInt(res)
}

export const getRampView = async (params: GetRampViewParams) => {
  const resStr = await invoke<string>('get_ramp_view', [
    { value: params[0] },
    params[1],
    typeof params[0][0].value !== 'number'
  ])
  return (JSON.parse(resStr) as GetRampViewResult).view
}

export const setNodeColor = (params: SetNodeColorParams) => invoke('set_node_color', params)

export const renderFrame = async () => {
  await _invoke('render_frame')
}

export const getNodeSpaceDescByPath = async (params: GetNodeSpaceDescByPathParams) => {
  const resStr = await invoke<string>('get_node_space_desc_by_path', params)
  if (resStr.length == 0) return null
  return JSON.parse(resStr) as GetNodeSpaceDescByPathResult
}

export const setNodeEditable = async (params: SetNodeEditableParams) =>
  invoke('set_node_editable', params)

export const getNdaFilePath = async (params: GetNdaFilePathParams) =>
  invoke<string>('get_nda_file_path', params)

export const copyNodes = async (params: CopyNodesParams) => invoke<string>('copy_nodes', params)

export const pasteNodes = async (params: PasteNodesParams) => {
  const resStr = await invoke<string>('paste_nodes', params)
  if (resStr.length == 0) return null
  return JSON.parse(resStr) as GetNodesByGraphIdResult
}

export const getNodeInfo = async (params: GetNodeInfoParams) => {
  const resStr = await invoke<string>('get_node_info', params)
  if (resStr.length == 0) return null
  return JSON.parse(resStr) as NodeInfo
}

export const GetNodeInputMeshesAttribs = async (params: GetNodeInputMeshesAttribsParams) => {
  const resStr = await invoke<string>('get_node_input_meshes_attribs', params)
  if (resStr.length == 0) return []
  return (JSON.parse(resStr) as GetNodeInputMeshesAttribsResult).data
}

// export const initViewport = async () => await _invoke('init_viewport')

export const resizeViewport = async (width: number, height: number) =>
  _invoke('resize_viewport', {
    width,
    height
  })

export const orbitViewport = async (params: OrbitViewportParams) => await invoke('orbit', params)

export const panViewport = async (params: PanViewportParams) => await invoke('pan', params)

export const zoomViewport = async (params: ZoonViewportParams) => await invoke('zoom', params)

export const setViewportMesh = async (params: SetViewportMeshParams) => {
  execute_and_change_node_mutext.runExclusive(async () => {
    // console.log('execute, ', params[0])
    await invoke('set_node_mesh', [params[0], params[1], params[2] || false])
  })
}

export const viewportFitView = async () => await invoke('zoom_curr_mesh_to_fit')

export const viewportShowWireframe = async (params: BoolParams) =>
  await invoke('show_wireframe', params)

export const viewportShowPointIndex = async (params: BoolParams) =>
  await invoke('show_point_index_label', params)

export const viewportShowPointNormal = async (params: BoolParams) =>
  await invoke('show_point_normal', params)

export const viewportShowPrimIndex = async (params: BoolParams) =>
  await invoke('show_prim_index_label', params)

export const viewportShowPrimNormal = async (params: BoolParams) =>
  await invoke('show_prim_normal', params)

export const viewportSetIndexSize = async (param: NumberParams) =>
  await invoke('set_label_font_size', param)
export const viewportSetPointNormalLength = async (param: NumberParams) =>
  invoke('set_point_normal_length', param)

export const viewportSetPrimNormalLength = async (param: NumberParams) =>
  invoke('set_prim_normal_length', param)

export const viewportSetPointSize = async (param: NumberParams) => {
  invoke('set_point_size', param)
}

export const viewportSetPointColor = async (param: Color4Params) =>
  invoke('set_point_color', [param[0], param[1], param[2]])

export const setParallelExecutionNodes = async (param: BoolParams) =>
  invoke('set_parallel_execution_nodes', param)

export const viewportSetWireframeColor = async (param: Color4Params) =>
  invoke('set_wireframe_color', [param[0], param[1], param[2], param[3] || 1.0])

export const viewportSetPointIndexColor = async (param: Color4Params) =>
  invoke('set_point_index_label_font_color', [param[0], param[1], param[2], param[3] || 1.0])

export const viewportSetPrimIndexColor = async (param: Color4Params) =>
  invoke('set_prim_index_label_font_color', [param[0], param[1], param[2], param[3] || 1.0])

export const viewportSetPointNormalColor = async (param: Color3Params) =>
  invoke('set_point_normal_color', param)

export const viewportSetPrimNormalColor = async (param: Color3Params) =>
  invoke('set_prim_normal_color', param)

export const viewportSetBackgroundColor = async (param: Color4Params) =>
  invoke('set_background_color', [param[0], param[1], param[2], param[3] || 1.0])

export const getRampMethods = async (): Promise<string[]> => {
  const resStr = await invoke<string>('get_ramp_methods')
  return (JSON.parse(resStr) as GetRampMethodsResult).methods
}

export const getDslLanguageReference = async () => {
  const resStr = await invoke<string>('get_dsl_language_reference')
  if (resStr.length == 0) return null
  return JSON.parse(resStr) as DslLanguageReference
}

export const batchGetNodeOutputMeshAttrs = async (params: BatchGetNodeOutputMeshAttrsParams) => {
  const resStr = await invoke<string>('batch_get_node_output_mesh_attrs', [
    params[0],
    params[1],
    params[2],
    params[3],
    JSON.stringify(params[4])
  ])
  if (resStr.length == 0) return null
  return JSON.parse(resStr).batch_attribs as string[]
}

export const addCustomParam = async (params: AddCustomParamParams) => {
  await invoke('add_custom_param', params)
}

export const removeCustomParam = async (params: RemoveCustomParamParams) => {
  await invoke('remove_custom_param', params)
}

export const addCustomLayout = async (params: AddCustomLayoutParams) => {
  await invoke('add_custom_layout', params)
}

export const removeCustomLayout = async (params: RemoveCustomLayoutParams) => {
  await invoke('remove_custom_layout', params)
}

export const rebuildNodeParameters = async (params: RebuildNodeParametersParams) => {
  await invoke('rebuild_node_parameters', params)
}

export const getNodeDataVersion = async (params: GetNodeDataVersionParams) => {
  const res = await invoke<string>('get_node_data_version', params)
  return parseInt(res)
}

export const searchNodes = async (params: SearchNodesParams) => {
  const resStr = await invoke<string>('search_nodes', params)
  if (resStr.length == 0) {
    return []
  }
  return (JSON.parse(resStr) as SearchNodesResult).result
}

export const searchParams = async (params: SearchParamsParams) => {
  const resStr = await invoke<string>('search_params', params)
  if (resStr.length == 0) {
    return []
  }
  return (JSON.parse(resStr) as SearchParamsResult).result
}

export const getMeshMemoryUsageInfo = async (params: GetMeshMemoryUsageInfoParams) => {
  const resStr = await invoke<string>('get_mesh_memory_usage', params)
  if (resStr.length == 0) {
    return null
  }
  return JSON.parse(resStr) as MeshMemoryUsageInfo
}

export const getAllMeshesMemoryUsageInfo = async (params: GetAllMeshesMemoryUsageInfoParams) => {
  const resStr = await invoke<string>('get_all_meshes_memory_usage', params)
  if (resStr.length == 0) {
    return null
  }
  return (JSON.parse(resStr) as GetAllMeshesMemoryUsageInfoResult).result
}

export const collectMemory = async (force: boolean) => await invoke('collect_memory', [force])

export const setMemoryProfiling = async (enable: boolean) =>
  await invoke('set_memory_profiling', [enable])

export const toggleConsoleWindow = async () => {
  await _invoke('toggle_console_window')
}

export const disableLightForPolygon = async (enable: BoolParams) => {
  await invoke('enable_light_for_polygon', [!enable[0]])
}

export const alwaysShowIsolatePoint = async (enable: BoolParams) => {
  await invoke('only_show_isolate_point', [!enable[0]])
}

export const forceShowPointsByDefaultColor = async (enable: BoolParams) => {
  await invoke('force_show_points_by_default_color', enable)
}

export const viewportSetNearPlane = async (param: NumberParams) => {
  invoke('set_near_plane', param)
}

export const viewportSetFarPlane = async (param: NumberParams) => {
  invoke('set_far_plane', param)
}
