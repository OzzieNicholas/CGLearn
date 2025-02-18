import { NodeGraphStoreType } from '@/stores/node-graph'
import { NodeData } from '@/types/node-graph'
import { createSelectors } from '@/utils/create-selectors'
import { create } from 'zustand'

export interface NodeGraphBridgeState {
  node: NodeData | null
  graph_id?: number
  subgraph_id?: number
  node_graph?: NodeGraphStoreType
}

export const useNodeGraphBridgeStore = createSelectors(
  // eslint-disable-next-line no-unused-vars
  create<NodeGraphBridgeState>()((_) => ({
    node: null,
    graph_id: undefined,
    subgraph_id: undefined,
    node_graph: undefined
  }))
)

export const useNodeGraphBridgeStoreApi = () => {
  return useNodeGraphBridgeStore
}

export const setNode = (
  node: NodeData | null,
  graph_id?: number,
  subgraph_id?: number,
  node_graph?: NodeGraphStoreType
) => useNodeGraphBridgeStore.setState({ node, graph_id, subgraph_id, node_graph })
