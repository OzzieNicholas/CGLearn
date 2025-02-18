import { getCurrentGraph } from '@/commands'
import { NodeKind } from '@/types/node-def'
import { Edge, NNode, NodeSpaceDesc } from '@/types/node-graph'
import { UndoRedo } from '@/types/undo-redo'
import { loadSubGraph } from '@/utils/node-graph'
import { Mutex } from 'async-mutex'
import { PersistStorage, StorageValue } from 'zustand/middleware'

export type LocalState = {
  graph: number
  root_graph: number
  undo_redo: UndoRedo
}

type PersistState = LocalState & {
  nodes: NNode[]
  edges: Edge[]
  node_space: NodeSpaceDesc
  url?: string
}

class NodeGraphLocalProvider implements PersistStorage<PersistState> {
  graphs: Record<string, PersistState>
  mutex: Mutex

  constructor() {
    this.graphs = {}
    this.mutex = new Mutex()
  }

  async loadState(name: string): Promise<PersistState> {
    let data = this.graphs[name]
    if (data) {
      if (data.node_space.id == 0) {
        data.node_space = { id: data.root_graph, editable: true, kind: NodeKind.Subgraph }
      }

      if (data.node_space.id != 0) {
        const { nodes, edges } = await loadSubGraph(data.graph, data.node_space.id)
        data.nodes = nodes
        data.edges = edges
      }

      if (data.undo_redo.unbind === null) {
        data.undo_redo = new UndoRedo(name)
      }
      return data
    }

    const current_graph = await getCurrentGraph()
    const graph_id = current_graph.graph_id
    const root_graph_id = current_graph.subgraph_id
    const { nodes, edges } = await loadSubGraph(graph_id, root_graph_id)

    data = {
      graph: graph_id,
      root_graph: root_graph_id,
      node_space: { id: root_graph_id, editable: true, kind: NodeKind.Subgraph },
      url: current_graph.graph_url.length > 0 ? current_graph.graph_url : undefined,
      nodes,
      edges,
      undo_redo: new UndoRedo(name)
    }

    this.graphs[name] = data

    return data
  }

  async getItem(name: string) {
    const data = await this.mutex.runExclusive(() => this.loadState(name))
    return {
      state: data
    }
  }

  setItem(name: string, value: StorageValue<PersistState>) {
    this.graphs[name] = value.state

    // let data = this.graphs[name]
    // if (data.current_subgraph != value.state.current_subgraph) {
    //   data.current_subgraph = value.state.current_subgraph
    // }
    // if (data.edges != value.state.edges) {
    //   data.edges = value.state.edges
    // }
    // if (data.nodes != value.state.nodes) {
    //   data.nodes = value.state.nodes
    // }
  }

  // eslint-disable-next-line @typescript-eslint/no-unused-vars, no-unused-vars
  removeItem(name: string) {
    let data = this.graphs[name]
    if (data !== undefined && data.undo_redo.unbind !== null) {
      data.node_space = { id: data.root_graph, editable: true, kind: NodeKind.Subgraph }
      data.undo_redo.unbind()
      data.undo_redo.unbind = null
      data.undo_redo.clear()
    }
  }
}

const nodeGraphLocalProvider = new NodeGraphLocalProvider()

export { nodeGraphLocalProvider }
