import { Connection, Edge as Edge_, Node as Node_, XYPosition } from '@xyflow/react'
import { EdgeChange, NodeChange } from '@xyflow/system'
import { NodeKind, NodePortDef } from './node-def'
import { LogLevel } from '@/commands/definer'

export type NodeLog = {
  level: LogLevel
  message: string[]
}

export type NodeData = {
  id: number
  label: string
  type_name: string
  color: string
  icon: string
  inputs: NodePortDef[]
  outputs: NodePortDef[]
  bypassed: boolean
  is_display?: boolean
  logs?: NodeLog[]

  kind: NodeKind
  category: string
  desc: string

  editable?: boolean
  hovered?: boolean

  inline_param?: boolean
  version: number
  show_version: boolean
}

export enum EdgeStyle {
  Straight = 'straignt',
  Bezier = 'bezier',
  Step = 'step',
  SmoothStep = 'smooth-step'
}

export type EdgeData = {
  hovered?: boolean
}

export type NNode = Node_<NodeData>

export type Edge = Edge_<EdgeData>

export type NodeSpaceDesc = {
  id: number
  editable: boolean
  kind: NodeKind
}

export type NdaSaveParams = {
  type_name: string
  category: string
  url: string
  old_type_name?: string
  old_url?: string
  type_version: number
}
export type AttribInfo = {
  name: string
  type: string
  compressed: boolean
  shared: boolean
  major_version: number
  minor_version: number
}

export type NodeInfo = {
  node: {
    execution_time: number //microseconds
    logs?: NodeLog[]
  }
  mesh?: {
    num_points: number
    num_prims: number
    num_vertices: number
    point: AttribInfo[]
    vertex: AttribInfo[]
    prim: AttribInfo[]
    global: AttribInfo[]
  }
}

export type NodeGraphState = {
  id: string
  url?: string
  nodes: NNode[]
  edges: Edge[]
  node_space: NodeSpaceDesc

  setNodes: (nodes: NNode[]) => Promise<void>

  addNode: (node: NNode) => Promise<number>

  addEdges: (edges: Edge[]) => Promise<void>

  setNodeSpace: (node: NNode | string) => Promise<number>

  setNodesPosition: (nodes: NNode[]) => Promise<void>

  setDisplayNode: (node_id: string) => Promise<void>

  setNodeColor: (node_id: string, color: string) => Promise<void>

  setNodeBypassed: (node_id: string, bypassed: boolean) => Promise<void>

  setNodeEditable: (node_id: number, editable: boolean) => Promise<void>

  setNodeHovered: (node_id: string, hovered: boolean) => Promise<void>

  setNodeData: (node_id: string, data: NodeData) => Promise<void>

  renameNode: (node_id: string, label: string) => Promise<void>

  setEdges: (edges: Edge[]) => Promise<void>

  setEdgeHovered: (edge_id: string, hovered: boolean) => Promise<void>

  deleteNode: (node_id: string) => Promise<void>

  deleteEdge: (edge_id: string) => Promise<void>

  deleteInputEdges: (node_id: string, handle?: string | null) => Promise<void>

  onNodesChange: (changes: NodeChange<NNode>[]) => Promise<void>

  onEdgesChange: (changes: EdgeChange<Edge>[]) => Promise<void>

  onConnect: (connection: Connection) => Promise<void>

  onNodesDelete: (nodes: NNode[]) => Promise<void>

  onEdgesDelete: (edges: Edge[]) => Promise<void>

  onReconnect: (old_edge: Edge, new_connection: Connection) => Promise<void>

  onColorChange: (node_id: string, old_value: string) => Promise<void>

  onNodesPositionChange: (old_value: Record<string, XYPosition>) => Promise<void>

  publishNda: (node_id: string, params: NdaSaveParams) => Promise<void>

  getNdaSaveParams: (node_id: string) => Promise<NdaSaveParams | null>

  load: (url?: string) => Promise<void>

  save: (url?: string, save_as?: boolean) => Promise<void>

  clear: () => Promise<void>

  copyNodes: (nodes: NNode[], with_connections: boolean) => Promise<string>

  pasteNodes: (data: string, pos: XYPosition) => Promise<void>

  hasChanges: () => boolean

  createSubgraphFromNodes: (node_ids: number[]) => Promise<void>

  createBackdropFromNodes: (node_ids: number[]) => Promise<void>

  getNodeInfo: (node: NNode | number, port_id: number) => Promise<NodeInfo | null>

  getNodePath: (node: NNode | number) => Promise<string>

  getNodeGraphId: () => number

  setNodeParam: (
    node_id: number,
    param_name: string,
    value: any,
    type?: 'value' | 'check' | 'expr'
  ) => Promise<any>

  onNodeParamEditingFinished: (
    node_name: string,
    param_name: string,
    old_value: any,
    new_value: any,
    type?: 'value' | 'check' | 'expr'
  ) => Promise<any>

  refreshNode: (node_id: number) => Promise<any>

  executeToNode: (node_id: number) => Promise<any>

  setNodeLogs: (node_id: number, logs?: NodeLog[]) => Promise<void>
}
