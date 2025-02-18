import { NNode } from '@/types/node-graph'
import { OnConnectStartParams, XYPosition } from '@xyflow/react'
import { createContext } from 'react'
import CommonEdge from './Edges/CommonEdge'
import BackdropNode from './Nodes/BackdropNode'
import CommonNode from './Nodes/CommonNode'
import DotNode from './Nodes/DotNode'

export const nodeTypes = {
  default: CommonNode,
  common: CommonNode,
  Dot: DotNode,
  Backdrop: BackdropNode
}

export const nodeTypesKeys = new Set<string>(Object.keys(nodeTypes))

export const edgeTypes = {
  default: CommonEdge,
  common: CommonEdge
}

export type EdgeInfo = {
  source: string
  sourceHandle?: string | null | undefined
  target: string
  targetHandle?: string | null | undefined
}

export type ContextMenuParam = {
  show: boolean
  node_pos?: XYPosition
  connection?: OnConnectStartParams
  edge?: EdgeInfo
  node?: NNode
  top?: number
  left?: number
  right?: number
  bottom?: number
}

export type InteractiveContextState = {
  editable: boolean
  hideElementToolbar: boolean

  showContextMenu: (
    menu_pos: XYPosition,
    node_pos?: XYPosition,
    connection?: OnConnectStartParams,
    edge?: EdgeInfo
  ) => void

  setNodeSpace: (node?: NNode, idx?: number) => void
}

export const InteractiveContext = createContext<InteractiveContextState>(null!)
