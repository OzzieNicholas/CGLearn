import { createContext } from 'react'
import { NodeData } from '@/types/node-graph'
import { NodeGraphStoreType } from '@/stores/node-graph'
import { Parameter, NodeDef, ParameterData, ParamItemType } from '@/commands/index.types'
import { ParamActionType } from '@/commands/definer'

export type ParamChange = {
  action: ParamActionType
  name: string
  dirty?: boolean
}

export type ContextMenuParams = {
  param_name: string
  param: Parameter
  param_def: ParameterData
  pos: { x: number; y: number }
  readonly: boolean
  onAddExpression: () => void
}

export type ParamPanelContextState = {
  node: NodeData
  graph_id: number
  subgraph_id: number
  node_graph: NodeGraphStoreType
  node_def: NodeDef
  param_items: ParamItemType[]
  setChange: (change: ParamChange) => void
  showContextMenu: (params: ContextMenuParams) => void
}

export const ParamPanelContext = createContext<ParamPanelContextState>(null!)
