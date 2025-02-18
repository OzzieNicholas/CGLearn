import {
  addConnect,
  createNode,
  createSubgraphFromNodes,
  deleteConnect,
  getNdaFilePath,
  getNode,
  getNodeDataVersion,
  getNodeIdByName,
  getNodeInfo,
  getNodeNameById,
  getNodeSpaceDescByPath,
  loadGraph,
  newGraph,
  copyNodes as nousCopyNodes,
  executeToNode as nousExecuteToNode,
  getNodePath as nousGetNodePath,
  publishNda,
  removeNode,
  saveGraph,
  // setAutoExecuteGraph,
  setNodeBypass,
  setNodeColor,
  setNodeDisplay,
  setNodeEditable,
  setNodeName,
  setNodePosition,
  setParam,
  setParamChecked,
  setParamExpression,
  setViewportMesh
} from '@/commands'
import i18n from '@/locales'
import { useGlobalVariablesStoreApi } from '@/stores/global-variables'
import { LocalState, nodeGraphLocalProvider } from '@/providers/node-graph'
import { addNodeDef, getNodeDef, removeNodeDef, useNodeDefsStoreApi } from '@/stores/node-defs'
import {
  setNode as setParameterPanelNode,
  useNodeGraphBridgeStoreApi
} from '@/stores/node-graph-bridge'
import { addRecentProject, useSettingsStoreApi /*, useSettingsStore*/ } from '@/stores/settings'
import { setTitleBarInfo } from '@/stores/title-bar'
import { NodeKind } from '@/types/node-def'
import {
  Edge,
  NNode,
  NdaSaveParams,
  NodeData,
  NodeGraphState,
  NodeSpaceDesc,
  NodeLog
} from '@/types/node-graph'
import { createSelectors } from '@/utils/create-selectors'
import { deepCopy, delay } from '@/utils/functions'
import {
  generateNodeFromData,
  handleCompare,
  handleIndex,
  isSubgraphOrNda,
  isSubgraphOrNdaEditable,
  loadSubGraph,
  pasteNodes as pasteNodes_
} from '@/utils/node-graph'
import uuid from '@/utils/uuid'
import { open as openFileDialog, save as saveFileDialog } from '@tauri-apps/plugin-dialog'
import { hexToRgba, rgbaToHexa } from '@uiw/color-convert'
import {
  Connection,
  XYPosition,
  applyEdgeChanges,
  applyNodeChanges,
  reconnectEdge
} from '@xyflow/react'
import { EdgeChange, NodeChange } from '@xyflow/system'
import { produce, setAutoFreeze } from 'immer'
import { pathEqual } from 'path-equal'
import { create } from 'zustand'
import { persist } from 'zustand/middleware'
import { getNodeGraphStoreApi } from './index'

setAutoFreeze(false)

type State = NodeGraphState &
  LocalState & {
    has_changes: boolean
    privateSetNodeSpace: (node_space: NodeSpaceDesc) => Promise<void>
    privateRenameNode: (old_name: string, new_name: string) => Promise<void>
    privateSetNodesPosition: (
      changes: Record<string, XYPosition[]>,
      data_idx: number
    ) => Promise<void>
    privateSetDisplayNode: (node_label: string) => Promise<void>
    privateSetNodeColor: (node_label: string, color: string) => Promise<void>
    privateSetNodeBypassed: (node_label: string, bypassed: boolean) => Promise<void>
    privateSetNodeEditable: (
      node_path: string,
      editable: boolean | undefined,
      node_label?: string
    ) => Promise<void>
    privateDeleteEdge: (edge: Edge) => Promise<void>
    execute: () => Promise<void>
  }

export const createLocalNodeGraphStore = (id: string) =>
  createSelectors(
    create<State>()(
      persist(
        (set, get) => ({
          id: id,
          url: undefined,

          nodes: [],
          edges: [],

          graph: 0,
          root_graph: 0,
          node_space: null!,
          undo_redo: null!,
          has_changes: false,

          setNodes: async (nodes: NNode[]) => {
            set({ nodes })
          },

          setEdges: async (edges: Edge[]) => {
            set({ edges })
          },

          setNodeSpace: async (node: NNode | string) => {
            const { graph, node_space } = get()
            if (typeof node === 'string') {
              node = {
                data: (await getNodeSpaceDescByPath([graph, node])) as any
              } as NNode
              if (!node.data) {
                return -1
              }
            }

            if (
              isSubgraphOrNda(node.data.kind) &&
              node.data.id != 0 &&
              node.data.id != node_space.id
            ) {
              const new_node_space = {
                id: node.data.id,
                kind: node.data.kind,
                editable: isSubgraphOrNdaEditable(node.data.editable)
              }

              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push(
                    {
                      name: 'change node space',
                      data: { old_value: deepCopy(node_space), new_value: new_node_space },
                      undo: async (data: any) => {
                        await get().privateSetNodeSpace(data.old_value)
                      },
                      redo: async (data: any) => {
                        await get().privateSetNodeSpace(data.new_value)
                      }
                    },
                    true
                  )
                  state.has_changes = true
                })
              )
            }

            return node.data.id
          },

          addNode: async (node: NNode) => {
            const { graph, node_space, nodes } = get()

            const show_version = hasDifferentVersion(nodes, node.data.type_name, node.data.version)

            const [nodes_, links_] = await nousCreateNode(
              graph,
              node_space.id,
              node,
              undefined,
              show_version
            )

            if (show_version) {
              nodes
                .filter((node) => node.data.type_name === nodes_[0].data.type_name)
                .forEach((node) => (node.data.show_version = show_version))
            }

            set((state) => ({
              nodes: [...nodes, ...nodes_],
              edges: [...state.edges, ...links_]
            }))

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'create node',
                  data: deepCopy(node),
                  undo: async (_: NNode) => {
                    const { graph, node_space } = get()
                    const to_remove_node_labels = nodes_.map((node) => node.data.label)

                    for (const label of to_remove_node_labels) {
                      await nousDeleteNodeByName(graph, node_space.id, label)
                      const show_version = hasDifferentVersion(nodes, node.data.type_name)

                      if (!show_version) {
                        const same_name_nodes = nodes.filter(
                          (n) => n.data.type_name === node.data.type_name
                        )

                        same_name_nodes.forEach((n) => (n.data.show_version = show_version))
                      }
                    }

                    set((_) => ({
                      nodes: nodes.filter((n) => to_remove_node_labels.indexOf(n.data.label) < 0)
                    }))
                  },
                  redo: async (data: NNode) => {
                    const { graph, node_space, nodes } = get()

                    const show_version = hasDifferentVersion([...nodes, data], data.data.type_name)

                    const [nodes_, links_] = await nousCreateNode(
                      graph,
                      node_space.id,
                      data,
                      true,
                      show_version
                    )

                    if (show_version) {
                      nodes
                        .filter((node) => node.data.type_name === nodes_[0].data.type_name)
                        .forEach((node) => (node.data.show_version = show_version))
                    }

                    set((state) => ({
                      nodes: [...nodes, ...nodes_],
                      edges: [...state.edges, ...links_]
                    }))
                    data.data.label = nodes_[0].data.label
                  }
                })
                state.has_changes = true
              })
            )

            return nodes_[0].data.id
          },

          addEdges: async (edges: Edge[]) => {
            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'create edges',
                  data: edges,
                  undo: async (data: Edge[]) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: state.edges.filter(
                        (edge) => data.find((e) => e.id === edge.id) === undefined
                      )
                    }))
                    for (const edge of data) {
                      await nousDeleteConnection(graph, edge, nodes)
                    }
                  },
                  redo: async (data: Edge[]) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: [...state.edges, ...data]
                    }))
                    for (const edge of data) {
                      await nousAddConnection(graph, edge, nodes)
                    }
                  }
                })
                state.has_changes = true
              })
            )

            const { graph, nodes } = get()
            set((state) => ({
              edges: [...state.edges, ...edges]
            }))
            for (const edge of edges) {
              await nousAddConnection(graph, edge, nodes)
            }
          },

          setNodesPosition: async (changed_nodes: NNode[]) => {
            const { graph, nodes } = get()

            let changes: Record<string, XYPosition[]> = {}
            changed_nodes.forEach((node) => {
              const old_node = findNodeById(nodes, node.id)
              if (old_node) {
                changes[node.data.label] = [old_node.position, node.position]
              }
            })

            if (changed_nodes.length == nodes.length) {
              set({ nodes: changed_nodes })
            } else {
              set((state) => ({
                nodes: state.nodes.map((node) => {
                  const change = changes[node.data.label]
                  if (change !== undefined) {
                    return {
                      ...node,
                      position: change[1]
                    }
                  }
                  return node
                })
              }))
            }

            for (const node of changed_nodes) {
              nousSetNodePosition(graph, node, node.position)
            }

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'move nodes',
                  data: { changes },
                  block_signal: true,
                  undo: async (data: { changes: Record<string, XYPosition[]> }) => {
                    get().privateSetNodesPosition(data.changes, 0)
                  },
                  redo: async (data: { changes: Record<string, XYPosition[]> }) => {
                    get().privateSetNodesPosition(data.changes, 1)
                  }
                })
                state.has_changes = true
              })
            )
          },

          setDisplayNode: async (node_id: string) => {
            let old_value = ''
            let new_value = ''

            const { nodes, privateSetDisplayNode } = get()
            for (const node of nodes) {
              if (node.id === node_id) {
                new_value = node.data.label
                if (old_value.length > 0) {
                  break
                }
              } else if (node.data.is_display) {
                old_value = node.data.label
                if (new_value.length > 0) {
                  break
                }
              }
            }

            await privateSetDisplayNode(new_value)

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'set display node',
                  data: { old_value, new_value },
                  undo: async (data: any) => {
                    await get().privateSetDisplayNode(data.old_value)
                  },
                  redo: async (data: any) => {
                    await get().privateSetDisplayNode(data.new_value)
                  }
                })
                state.has_changes = true
              })
            )
          },

          setNodeColor: async (node_id: string, color: string) => {
            const { nodes, privateSetNodeColor } = get()
            const node = findNodeById(nodes, node_id)
            if (node) {
              privateSetNodeColor(node?.data.label, color)
            }
          },

          setNodeBypassed: async (node_id: string, bypassed: boolean) => {
            const { nodes, privateSetNodeBypassed } = get()
            const node = findNodeById(nodes, node_id)
            if (node) {
              const old_value = node.data.bypassed
              await privateSetNodeBypassed(node.data.label, bypassed)

              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'set display bypassed',
                    data: { old_value, new_value: bypassed, label: node.data.label },
                    undo: async (data: any) => {
                      await get().privateSetNodeBypassed(data.label, data.old_value)
                    },
                    redo: async (data: any) => {
                      await get().privateSetNodeBypassed(data.label, data.new_value)
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          setNodeEditable: async (node_id: number, editable: boolean) => {
            const { graph, nodes, privateSetNodeEditable } = get()
            const node_path = await nousGetNodePath([graph, node_id])

            if (node_path.length > 0) {
              const node = nodes.find((n) => n.data.id === node_id)
              const node_label = node ? node.data.label : undefined
              await privateSetNodeEditable(node_path, editable, node_label)

              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'set node editable',
                    data: { value: editable, node_path, node_label },
                    undo: async (data: any) => {
                      await get().privateSetNodeEditable(
                        data.node_path,
                        !data.value,
                        data.node_label
                      )
                    },
                    redo: async (data: any) => {
                      await get().privateSetNodeEditable(
                        data.node_path,
                        data.value,
                        data.node_label
                      )
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          setNodeHovered: async (node_id: string, hovered: boolean) => {
            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.id === node_id) {
                  return {
                    ...node,
                    data: {
                      ...node.data,
                      hovered
                    }
                  }
                }
                return node
              })
            }))
          },

          setNodeData: async (node_id: string, data: NodeData) => {
            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.id === node_id) {
                  return {
                    ...node,
                    data
                  }
                }
                return node
              })
            }))
          },

          setEdgeHovered: async (edge_id: string, hovered: boolean) => {
            set((state) => ({
              edges: state.edges.map((edge) => {
                if (edge.id === edge_id) {
                  return {
                    ...edge,
                    data: {
                      ...edge.data,
                      hovered
                    }
                  }
                }
                return edge
              })
            }))
          },

          renameNode: async (node_id: string, label: string) => {
            const node = findNodeById(get().nodes, node_id)
            if (node) {
              const new_value = (await nousSetNodeName(node, label)) || label

              let old_value = ''

              set((state) => ({
                nodes: state.nodes.map((node) => {
                  if (node.id === node_id) {
                    old_value = node.data.label
                    return {
                      ...node,
                      data: {
                        ...node.data,
                        label: new_value
                      }
                    }
                  }
                  return node
                })
              }))

              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'rename node',
                    data: {
                      old_value,
                      new_value
                    },
                    block_signal: true,
                    undo: async (data: any) => {
                      return get().privateRenameNode(data.new_value, data.old_value)
                    },
                    redo: async (data: any) => {
                      return get().privateRenameNode(data.old_value, data.new_value)
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          deleteNode: async (node_id: string) => {
            const node = findNodeById(get().nodes, node_id)
            if (node) {
              set((state) => ({
                nodes: state.nodes.filter((node) => node.id !== node_id)
              }))
              const { graph, node_space } = get()
              const ser = await nousCopyNodes([
                graph,
                node_space.id,
                {
                  value: [node.data.id]
                },
                false
              ])
              await nousDeleteNode(node)

              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'delete node',
                    data: { node: deepCopy(node), ser },
                    undo: async (data: { node: NNode; ser: string }) => {
                      const { graph, node_space, nodes } = get()

                      const has_different_version = hasDifferentVersion(nodes, node.data.type_name)

                      const [nodes_, links_] = await nousCreateNodeFromJson(
                        graph,
                        node_space.id,
                        data.node,
                        data.ser,
                        true,
                        has_different_version
                      )
                      set((state) => ({
                        nodes: [...state.nodes, ...nodes_],
                        edges: [...state.edges, ...(links_ || [])]
                      }))
                      data.node.data.label = nodes_[0].data.label
                    },
                    redo: async (data: { node: NNode; ser: string }) => {
                      const { graph, node_space } = get()
                      set((state) => ({
                        nodes: state.nodes.filter(
                          (node) => node.data.label !== data.node.data.label
                        )
                      }))
                      await nousDeleteNodeByName(graph, node_space.id, data.node.data.label)
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          deleteInputEdges: async (node_id: string, handle?: string | null) => {
            const { edges, privateDeleteEdge } = get()
            const edge_to_delete = edges.find(
              (edge) => edge.target === node_id && handleCompare(edge.targetHandle, handle)
            )

            if (edge_to_delete) {
              await privateDeleteEdge(edge_to_delete)
            }
          },

          deleteEdge: async (edge_id: string) => {
            const { edges, privateDeleteEdge } = get()
            const edge_to_delete = edges.find((edge) => edge.id === edge_id)
            if (edge_to_delete) {
              await privateDeleteEdge(edge_to_delete)
            }
          },

          onNodesChange: async (changes: NodeChange<NNode>[]) => {
            const { graph, nodes, node_space, id } = get()

            let new_nodes: NNode[] = []

            let current_selected_node = null
            let current_selected_node_updated = false
            if (changes[0].type === 'select') {
              current_selected_node = useNodeGraphBridgeStoreApi().getState().node
              changes.sort((a, b) => {
                if (a.type === 'select' && b.type === 'select') {
                  return a.selected < b.selected ? -1 : 1
                }
                return 0
              })
            }

            for (let change of changes) {
              switch (change.type) {
                case 'add': {
                  if (change.item.data.id === 0) {
                    const show_version = hasDifferentVersion(nodes, change.item.data.type_name)
                    const [new_nodes_] = await nousCreateNode(
                      graph,
                      node_space.id,
                      change.item,
                      undefined,
                      show_version
                    )
                    new_nodes.push(...new_nodes_)
                  }
                  break
                }
                case 'select': {
                  if (change.selected) {
                    if (!current_selected_node_updated) {
                      const change_node = findNodeById(nodes, change.id)
                      if (change_node) {
                        if (!current_selected_node) {
                          setParameterPanelNode(
                            change_node.data,
                            graph,
                            node_space.id,
                            getNodeGraphStoreApi(id)
                          )
                          current_selected_node_updated = true
                        } else {
                          const n = nodes.find((n) => n.data.id === current_selected_node!.id)
                          if (n?.selected) {
                            current_selected_node_updated = true
                          } else if (current_selected_node.id !== change_node.data.id) {
                            setParameterPanelNode(
                              change_node.data,
                              graph,
                              node_space.id,
                              getNodeGraphStoreApi(id)
                            )
                            current_selected_node_updated = true
                          }
                        }
                      }
                    }
                  } else {
                    if (current_selected_node) {
                      const change_node = findNodeById(nodes, change.id)
                      if (change_node?.data.id === current_selected_node.id) {
                        current_selected_node = null
                      }
                    }
                  }
                  break
                }
              }
            }

            set((state) => ({
              nodes: applyNodeChanges(changes, state.nodes)
            }))

            if (new_nodes.length > 0) {
              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'create nodes',
                    data: new_nodes,
                    undo: async (data: NNode[]) => {
                      const { graph, node_space } = get()
                      set((state) => ({
                        nodes: state.nodes.filter(
                          (node) => data.find((n) => n.data.label === node.data.label) === undefined
                        )
                      }))
                      for (const node of data) {
                        await nousDeleteNodeByName(graph, node_space.id, node.data.label)
                      }
                    },
                    redo: async (data: NNode[]) => {
                      const { graph, node_space, nodes } = get()

                      let new_nodes = []
                      let new_links = []

                      for (const node of data) {
                        const show_version = hasDifferentVersion(nodes, node.data.type_name)
                        const [new_nodes_, links_] = await nousCreateNode(
                          graph,
                          node_space.id,
                          node,
                          true,
                          show_version
                        )
                        new_nodes.push(...new_nodes_)
                        new_links.push(...(links_ || []))
                      }
                      data.length = 0
                      data.push(...new_nodes)

                      set((state) => ({
                        nodes: [...state.nodes, ...new_nodes],
                        edges: [...state.edges, ...new_links]
                      }))
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          onEdgesChange: async (changes: EdgeChange<Edge>[]) => {
            const all_change_is_select =
              changes.map((change) => change.type === 'select').filter((is) => is === true)
                .length === changes.length

            if (all_change_is_select) {
              return
            }

            const { graph, nodes, edges, deleteInputEdges } = get()

            let new_edges: Edge[] = []

            // begin undo group
            set(
              produce((state: State) => {
                state.undo_redo.beginGroup('change edges')
              })
            )

            for (let change of changes) {
              switch (change.type) {
                case 'add': {
                  new_edges.push(change.item)
                  await deleteInputEdges(change.item.target, change.item.targetHandle)
                  await nousAddConnection(graph, change.item, nodes)
                  break
                }
              }
            }

            set(() => ({
              edges: applyEdgeChanges(changes, edges)
            }))

            if (new_edges.length > 0) {
              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'create edges',
                    data: new_edges,
                    undo: async (data: Edge[]) => {
                      const { graph, nodes } = get()
                      set((state) => ({
                        edges: state.edges.filter(
                          (edge) => data.find((e) => e.id === edge.id) === undefined
                        )
                      }))
                      for (const edge of data) {
                        await nousDeleteConnection(graph, edge, nodes)
                      }
                    },
                    redo: async (data: Edge[]) => {
                      const { graph, nodes } = get()
                      set((state) => ({
                        edges: [...state.edges, ...data]
                      }))
                      for (const edge of data) {
                        await nousAddConnection(graph, edge, nodes)
                      }
                    }
                  })
                  state.undo_redo.endGroup()
                  state.has_changes = true
                })
              )
            } else {
              set(
                produce((state: State) => {
                  state.undo_redo.endGroup()
                  state.has_changes = true
                })
              )
            }
          },

          onConnect: async (connection: Connection) => {
            const { graph, nodes, deleteInputEdges } = get()

            // begin undo group
            set(
              produce((state: State) => {
                state.undo_redo.beginGroup('connect edge')
              })
            )

            if (connection.target) {
              await deleteInputEdges(connection.target, connection.targetHandle)
            }

            let new_edge: Edge = null!

            set((state) => {
              new_edge = {
                id: uuid(),
                source: connection.source,
                sourceHandle: connection.sourceHandle,
                target: connection.target,
                targetHandle: connection.targetHandle
              }
              return {
                edges: state.edges.concat(new_edge)
              }
            })

            await nousAddConnection(graph, connection, nodes)

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'create edge',
                  data: deepCopy(new_edge),
                  undo: async (data: Edge) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: state.edges.filter((e) => e.id !== data.id)
                    }))
                    await nousDeleteConnection(graph, data, nodes)
                  },
                  redo: async (data: Edge) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: state.edges.concat(data)
                    }))
                    await nousAddConnection(graph, data, nodes)
                  }
                })
                state.undo_redo.endGroup()
                state.has_changes = true
              })
            )
          },

          onReconnect: async (old_edge: Edge, new_connection: Connection) => {
            const { graph, nodes, deleteInputEdges } = get()

            // begin undo group
            set(
              produce((state: State) => {
                state.undo_redo.beginGroup('update edge')
              })
            )

            if (
              new_connection.target &&
              !(
                old_edge.target === new_connection.target &&
                handleCompare(old_edge.targetHandle, new_connection.targetHandle)
              )
            ) {
              await deleteInputEdges(new_connection.target, new_connection.targetHandle)
            }

            let new_edge: Edge = null!
            set((state) => {
              const edges = reconnectEdge(old_edge, new_connection, state.edges)
              new_edge = edges[edges.length - 1]
              return {
                edges
              }
            })

            await nousDeleteConnection(graph, old_edge, nodes)
            await nousAddConnection(graph, new_connection, nodes)

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'update edge',
                  data: {
                    old_value: deepCopy(old_edge),
                    new_value: deepCopy(new_edge)
                  },
                  undo: async (data: any) => {
                    const { graph, nodes } = get()

                    set((state) => ({
                      edges: state.edges
                        .filter((e) => e.id !== data.new_edge.id)
                        .concat(data.old_edge)
                    }))
                    await nousDeleteConnection(graph, data.new_edge, nodes)
                    await nousAddConnection(graph, data.old_edge, nodes)
                  },
                  redo: async (data: any) => {
                    const { graph, nodes } = get()

                    set((state) => ({
                      edges: state.edges
                        .filter((e) => e.id !== data.old_value.id)
                        .concat(data.new_edge)
                    }))

                    await nousDeleteConnection(graph, data.old_value, nodes)
                    await nousAddConnection(graph, data.new_edge, nodes)
                  }
                })
                state.undo_redo.endGroup()
                state.has_changes = true
              })
            )
          },

          onNodesDelete: async (nodes_: NNode[]) => {
            const { graph, node_space, nodes } = get()
            const ser = await nousCopyNodes([
              graph,
              node_space.id,
              {
                value: nodes_.map((n) => n.data.id)
              },
              false
            ])

            const modified_nodes: NNode[] = []

            for (const node of nodes_) {
              await nousDeleteNode(node)
              const show_version = hasDifferentVersion(nodes, node.data.type_name)

              if (!show_version) {
                const same_name_nodes = nodes.filter(
                  (n) => n.data.type_name === node.data.type_name
                )

                same_name_nodes.forEach((n) => {
                  modified_nodes.push(n)
                  n.data.show_version = show_version
                })
              }
            }

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'delete nodes',
                  data: { nodes: nodes_, ser },
                  undo: async (data: { nodes: NNode[]; ser: string }) => {
                    const { graph, node_space } = get()
                    const [new_nodes, new_links] = await nousCreateNodesFromJson(
                      graph,
                      node_space.id,
                      data.nodes,
                      data.ser,
                      true
                    )
                    data.nodes.length = 0
                    data.nodes.push(...new_nodes)

                    modified_nodes.forEach((n) => (n.data.show_version = true))

                    set((state) => ({
                      nodes: [...state.nodes, ...new_nodes, ...modified_nodes],
                      edges: [...state.edges, ...new_links]
                    }))
                  },
                  redo: async (data: { nodes: NNode[]; ser: string }) => {
                    const { graph, node_space } = get()

                    modified_nodes.forEach((n) => (n.data.show_version = false))

                    set((state) => ({
                      nodes: [
                        ...state.nodes.filter(
                          (node) => data.nodes.find((n) => n.data.id === node.data.id) === undefined
                        ),
                        ...modified_nodes
                      ]
                    }))
                    for (const node of data.nodes) {
                      await nousDeleteNodeByName(graph, node_space.id, node.data.label)
                    }
                  }
                })
                state.has_changes = true
              })
            )
          },

          onEdgesDelete: async (edges: Edge[]) => {
            const { graph, nodes } = get()
            for (const edge of edges) {
              await nousDeleteConnection(graph, edge, nodes)
            }

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'delete edges',
                  data: edges,
                  undo: async (data: Edge[]) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: [...state.edges, ...data]
                    }))
                    for (const edge of data) {
                      await nousAddConnection(graph, edge, nodes)
                    }
                  },
                  redo: async (data: Edge[]) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: state.edges.filter(
                        (edge) => data.find((e) => e.id === edge.id) === undefined
                      )
                    }))
                    for (const edge of data) {
                      await nousDeleteConnection(graph, edge, nodes)
                    }
                  }
                })
                state.has_changes = true
              })
            )
          },

          onColorChange: async (node_id: string, old_value: string) => {
            const { nodes } = get()
            const node = findNodeById(nodes, node_id)
            if (node) {
              // undo/redo
              set(
                produce((state: State) => {
                  state.undo_redo.push({
                    name: 'set node color',
                    data: { old_value, new_value: node.data.color, label: node.data.label },
                    block_signal: true,
                    undo: async (data: any) => {
                      get().privateSetNodeColor(data.label, data.old_value)
                    },
                    redo: async (data: any) => {
                      get().privateSetNodeColor(data.label, data.new_value)
                    }
                  })
                  state.has_changes = true
                })
              )
            }
          },

          onNodesPositionChange: async (old_value: Record<string, XYPosition>) => {
            let changes: Record<string, XYPosition[]> = {}
            const { graph, nodes } = get()

            for (const node_id in old_value) {
              const node = findNodeById(nodes, node_id)
              if (node) {
                const old_pos = old_value[node_id]
                changes[node.data.label] = [old_pos, node.position]
                nousSetNodePosition(graph, node, node.position)
              }
            }

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'move nodes',
                  data: changes,
                  block_signal: true,
                  undo: async (data: Record<string, XYPosition[]>) => {
                    get().privateSetNodesPosition(data, 0)
                  },
                  redo: async (data: Record<string, XYPosition[]>) => {
                    get().privateSetNodesPosition(data, 1)
                  }
                })
                state.has_changes = true
              })
            )
          },

          publishNda: async (node_id: string, params: NdaSaveParams) => {
            const { graph, nodes } = get()
            const node = findNodeById(nodes, node_id)
            if (node && isSubgraphOrNda(node.data.kind)) {
              const def = getNodeDef(params.type_name)
              if (def) {
                if (def.kind !== NodeKind.Nda) {
                  console.error('Can not override built-in node: ', params.type_name)
                  alert(`Can not override built-in node: '${params.type_name}'`)
                  return
                }
              }

              const result = await publishNda([
                graph,
                node.data.id,
                params.type_name,
                params.category,
                params.url,
                params.type_version
              ])
              if (!result) {
                console.error('Failed to save nda')
                return
              }

              if (params.old_type_name && params.old_url) {
                if (pathEqual(params.old_url, params.url)) {
                  removeNodeDef(params.old_type_name)
                }
              }

              addNodeDef({
                type_name: params.type_name,
                category: params.category,
                kind: NodeKind.Nda,
                inputs: node.data.inputs,
                outputs: node.data.outputs,
                type_version: params.type_version
              })
            }
          },

          getNdaSaveParams: async (node_id: string) => {
            const node = findNodeById(get().nodes, node_id)
            if (node) {
              if (node.data.kind == NodeKind.Nda) {
                const file_path = await getNdaFilePath([node.data.id])
                const def = getNodeDef(node.data.type_name)
                if (def) {
                  return {
                    type_name: def.type_name,
                    category: def.category,
                    url: file_path,
                    old_type_name: node.data.type_name,
                    old_url: file_path,
                    type_version: def.type_version
                  }
                }
                return {
                  type_name: node.data.type_name,
                  category: 'Custom',
                  url: file_path,
                  old_type_name: node.data.type_name,
                  old_url: file_path,
                  type_version: 1
                }
              } else if (node.data.kind === NodeKind.Subgraph) {
                return {
                  type_name: node.data.label,
                  category: 'Custom',
                  url: '',
                  type_version: 1
                }
              }
            }
            return null
          },

          load: async (url?: string) => {
            if (!url || url.length == 0) {
              // open a dialog
              const file = await openFileDialog({
                multiple: false,
                directory: false,
                filters: [
                  {
                    name: 'nous file',
                    extensions: ['nous', 'jnous']
                  }
                ]
              })
              if (!file) {
                return
              }
              url = file.path as string
            }

            // parameter panel
            setParameterPanelNode(null)

            // new graph
            const new_graph = await newGraph()
            let graph_id = new_graph.graph_id
            let root_graph_id = new_graph.subgraph_id

            // load from file
            const result = await loadGraph([graph_id, url])
            if (result.result) {
              graph_id = result.graph_id
              root_graph_id = result.root_subgraph_id

              // load new graph
              const { nodes, edges } = await loadSubGraph(graph_id, root_graph_id)
              set(() => ({
                graph: graph_id,
                root_graph: root_graph_id,
                node_space: { id: root_graph_id, editable: true, kind: NodeKind.Subgraph },
                nodes: nodes,
                edges: edges,
                has_changes: false,
                url: url
              }))
            } else {
              // when load failed
              set(() => ({
                graph: graph_id,
                root_graph: root_graph_id,
                node_space: { id: root_graph_id, editable: true, kind: NodeKind.Subgraph },
                nodes: [],
                edges: [],
                has_changes: false,
                url: url
              }))
            }

            // clear undo stack
            set(
              produce((state: State) => {
                state.undo_redo.clear()
              })
            )

            // update app title
            setTitleBarInfo(url as string)

            // update recent projects
            addRecentProject(url)

            // bridge
            useNodeGraphBridgeStoreApi().setState({
              graph_id,
              subgraph_id: root_graph_id,
              node_graph: getNodeGraphStoreApi(id)
            })
          },

          save: async (url?: string, save_as?: boolean) => {
            url = get().url

            if (save_as || !url || url.length == 0) {
              // open a dialog
              const path = await saveFileDialog({
                filters: [
                  {
                    name: 'nous binary file',
                    extensions: ['nous']
                  },
                  {
                    name: 'nous debug file',
                    extensions: ['jnous']
                  }
                ]
              })
              if (!path) {
                return
              }
              url = path as string

              set(() => ({
                url: url
              }))

              // update app title
              setTitleBarInfo(url)

              // update recent projects
              addRecentProject(url)
            }

            const { graph } = get()
            await saveGraph([graph, url])
            set(() => ({
              has_changes: false
            }))
          },

          clear: async () => {
            // parameter panel
            setParameterPanelNode(null)

            // new graph
            const new_graph = await newGraph()
            const graph_id = new_graph.graph_id
            const root_graph_id = new_graph.subgraph_id

            //
            set(() => ({
              graph: graph_id,
              root_graph: root_graph_id,
              node_space: { id: root_graph_id, editable: true, kind: NodeKind.Subgraph },
              nodes: [],
              edges: [],
              has_changes: false,
              url: undefined
            }))

            // clear undo stack
            set(
              produce((state: State) => {
                state.undo_redo.clear()
              })
            )

            // update app title
            setTitleBarInfo(i18n.t('common.untitled'))

            // bridge
            useNodeGraphBridgeStoreApi().setState({
              graph_id,
              subgraph_id: root_graph_id,
              node_graph: getNodeGraphStoreApi(id)
            })
          },

          copyNodes: async (nodes: NNode[], with_connections: boolean) => {
            const { graph, node_space } = get()
            return nousCopyNodes([
              graph,
              node_space.id,
              {
                value: nodes.map((n) => n.data.id)
              },
              with_connections
            ])
          },

          pasteNodes: async (clipboard: string, pos: XYPosition) => {
            const redo = async (data: { clipboard: string; pos: XYPosition; nodes?: NNode[] }) => {
              const { graph, node_space } = get()
              const { nodes, edges } = await pasteNodes_(
                graph,
                node_space.id,
                data.clipboard,
                true,
                data.pos
              )

              data.nodes = nodes

              set((state) => ({
                nodes: state.nodes.concat(nodes),
                edges: state.edges.concat(edges.map((e) => ({ ...e, id: uuid() })))
              }))
            }

            const undo = async (data: { clipboard: string; pos: XYPosition; nodes: NNode[] }) => {
              set((state) => ({
                nodes: state.nodes.filter((n) => findNodeById(data.nodes, n.id) === undefined),
                edges: state.edges.filter(
                  (e) =>
                    findNodeById(data.nodes, e.source) === undefined &&
                    findNodeById(data.nodes, e.target) === undefined
                )
              }))
            }

            let data = { clipboard, pos }
            redo(data)

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'paste node',
                  data,
                  undo,
                  redo
                })
                state.has_changes = true
              })
            )
          },

          hasChanges: () => {
            return get().has_changes
          },

          createSubgraphFromNodes: async (node_ids: number[]) => {
            const def = getNodeDef('Subgraph')
            if (!def) {
              console.error('can not find def: Subgraph')
              return
            }
            const { graph, node_space, nodes, edges } = get()
            const ser = await nousCopyNodes([
              graph,
              node_space.id,
              {
                value: node_ids
              },
              false
            ])

            const node_id = await createSubgraphFromNodes([
              graph,
              node_space.id,
              { value: node_ids }
            ])

            if (node_id == 0) {
              return
            }

            const source_nodes = nodes.filter(
              (n) => node_ids.find((id) => id == n.data.id) !== undefined
            )
            const source_edges = edges.filter(
              (e) => source_nodes.find((n) => n.id === e.target) !== undefined
            )

            const node_data = await getNode([graph, node_space.id, node_id])
            const show_version = hasDifferentVersion(nodes, node_data.type_name)
            const new_node = generateNodeFromData(node_id, node_data, def, show_version)

            set((state) => ({
              nodes: state.nodes
                .filter((n) => node_ids.find((id) => id == n.data.id) === undefined)
                .concat(new_node),
              edges: state.edges.filter(
                (e1) => source_edges.find((e2) => e2.id == e1.id) === undefined
              )
            }))

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'pack nodes',
                  data: {
                    nodes: deepCopy(source_nodes),
                    edges: deepCopy(source_edges),
                    subgraph: node_data.name,
                    ser
                  },
                  undo: async (data: {
                    nodes: NNode[]
                    edges: Edge[]
                    subgraph: string
                    ser: string
                  }) => {
                    const { graph, node_space, nodes } = get()

                    await nousDeleteNodeByName(graph, node_space.id, data.subgraph)

                    const [new_nodes] = await nousCreateNodesFromJson(
                      graph,
                      node_space.id,
                      data.nodes,
                      data.ser,
                      true
                    )

                    const all_nodes: NNode[] = nodes
                      .filter((n) => n.data.label !== data.subgraph)
                      .concat(new_nodes)

                    for (const edge of data.edges) {
                      await nousAddConnection(graph, edge, all_nodes)
                    }

                    set((state) => ({
                      nodes: all_nodes,
                      edges: state.edges.concat(data.edges)
                    }))

                    data.nodes = new_nodes
                  },
                  redo: async (data: {
                    nodes: NNode[]
                    edges: Edge[]
                    subgraph: string
                    ser: string
                  }) => {
                    const { graph, node_space, nodes, edges } = get()

                    const source_nodes = nodes.filter(
                      (n1) => data.nodes.find((n2) => n1.data.label === n2.data.label) !== undefined
                    )

                    const source_edges = edges.filter(
                      (e) => source_nodes.find((n) => n.id === e.target) !== undefined
                    )

                    const node_ids = source_nodes.map((n) => n.data.id)

                    const node_id = await createSubgraphFromNodes([
                      graph,
                      node_space.id,
                      { value: node_ids }
                    ])

                    if (node_id == 0) {
                      return
                    }

                    let node_data = await getNode([graph, node_space.id, node_id])
                    node_data.name = await setNodeName([node_id, data.subgraph])
                    const show_version = hasDifferentVersion(nodes, node_data.type_name)
                    const new_node = generateNodeFromData(node_id, node_data, def, show_version)
                    // rustLog('redo3')

                    set((state) => ({
                      nodes: state.nodes
                        .filter((n) => node_ids.find((id) => id == n.data.id) === undefined)
                        .concat(new_node),
                      edges: state.edges.filter(
                        (e1) => source_edges.find((e2) => e2.id == e1.id) === undefined
                      )
                    }))

                    data.subgraph = node_data.name
                    data.edges = source_edges
                  }
                })
                state.has_changes = true
              })
            )
          },

          createBackdropFromNodes: async (node_ids: number[]) => {
            const def = getNodeDef('Backdrop')
            if (!def) {
              console.error('can not find def: Backdrop')
              return
            }

            const {
              id,
              graph,
              node_space,
              nodes,
              addNode,
              setNodeParam,
              onNodeParamEditingFinished
            } = get()

            const source_nodes = nodes.filter(
              (n) => node_ids.find((id) => id == n.data.id) !== undefined
            )

            let minPosX = 0
            let minPosY = 0
            let maxPosX = 0
            let maxPosY = 0

            let idx = 0
            for (const source_node of source_nodes) {
              const nodeMinPosX = source_node.position.x
              const nodeMinPosY = source_node.position.y
              const nodeMaxPosX =
                nodeMinPosX +
                (source_node.measured?.width === undefined
                  ? source_node.width || 0
                  : source_node.measured.width)
              const nodeMaxPosY =
                nodeMinPosY +
                (source_node.measured?.height === undefined
                  ? source_node.height || 0
                  : source_node.measured.height)

              if (idx == 0) {
                minPosX = nodeMinPosX
                minPosY = nodeMinPosY
                maxPosX = nodeMaxPosX
                maxPosY = nodeMaxPosY
              } else {
                minPosX = Math.min(minPosX, nodeMinPosX)
                minPosY = Math.min(minPosY, nodeMinPosY)
                maxPosX = Math.max(maxPosX, nodeMaxPosX)
                maxPosY = Math.max(maxPosY, nodeMaxPosY)
              }
              ++idx
            }
            const extend = 40

            minPosX -= extend
            minPosY -= extend
            maxPosX += extend
            maxPosY += extend

            set(
              produce((state: State) => {
                state.undo_redo.beginGroup('create backdrop')
              })
            )

            const backdropNode = {
              id: uuid(),
              type: 'Backdrop',
              position: { x: minPosX, y: minPosY },
              selected: true,
              data: {
                id: 0,
                label: def.type_name,
                type_name: def.type_name,
                color: '#FFF',
                icon: def.type_name,
                inputs: [],
                outputs: [],
                bypassed: false,
                is_display: false,
                kind: def.kind,
                category: def.category,
                desc: 'No description',

                version: def.type_version,
                show_version: false
              }
            }

            const size = [maxPosX - minPosX, maxPosY - minPosY]
            const nodeId = await addNode(backdropNode)
            await delay(20)
            await setNodeParam(nodeId, 'Size', size)
            const newNode = get().nodes.find((n) => n.data.id === nodeId)
            if (newNode) {
              await onNodeParamEditingFinished(newNode.data.label, 'Size', [100, 100], size)
              setParameterPanelNode(newNode.data, graph, node_space.id, getNodeGraphStoreApi(id))
            }

            set(
              produce((state: State) => {
                state.undo_redo.endGroup()
                state.has_changes = true
              })
            )
          },

          getNodeInfo: async (node: NNode | number, port_id: number) => {
            if (port_id < 0) return null
            if (typeof node === 'number') {
              return getNodeInfo([get().graph, node, port_id])
            }
            return getNodeInfo([get().graph, node.data.id, port_id])
          },

          getNodePath: async (node: NNode | number) => {
            if (typeof node === 'number') {
              return nousGetNodePath([get().graph, node])
            }
            return nousGetNodePath([get().graph, node.data.id])
          },

          getNodeGraphId: () => {
            return get().graph
          },

          setNodeParam: async (
            node_id: number,
            param_name: string,
            value: any,
            type?: 'value' | 'check' | 'expr'
          ) => {
            const { graph, execute } = get()

            if (!type || type === 'value') {
              await setParam([graph, node_id, param_name, value])
            } else if (type === 'check') {
              await setParamChecked([graph, node_id, param_name, value as boolean])
            } else if (type === 'expr') {
              await setParamExpression([graph, node_id, param_name, value as string])
            }
            execute()
          },

          onNodeParamEditingFinished: async (
            node_name: string,
            param_name: string,
            old_value: any,
            new_value: any,
            type?: 'value' | 'check' | 'expr'
          ) => {
            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'edit parameter',
                  data: { node_name, param_name, old_value, new_value, type },
                  undo: async (data) => {
                    const { nodes, setNodeParam } = get()
                    const node = nodes.find((n) => n.data.label === data.node_name)
                    if (!node) return
                    return setNodeParam(node.data.id, data.param_name, data.old_value, data.type)
                  },
                  redo: async (data) => {
                    const { nodes, setNodeParam } = get()
                    const node = nodes.find((n) => n.data.label === data.node_name)
                    if (!node) return
                    return setNodeParam(node.data.id, data.param_name, data.new_value, data.type)
                  }
                })
                state.has_changes = true
              })
            )
          },

          refreshNode: async (node_id: number) => {
            const { graph, node_space, nodes } = get()
            const node = nodes.find((n) => n.data.id === node_id)
            if (node) {
              const node_data = await getNode([graph, node_space.id, node_id])
              if (!node_data) {
                console.error(`can not find node data: ${node.data.label}`)
                return
              }

              const def = getNodeDef(node.data.type_name)
              if (!def) {
                console.error(`can not find def: ${node.data.type_name}`)
                return
              }
              const src_node = generateNodeFromData(node_id, node_data, def, node.data.show_version)
              const new_node = {
                ...node,
                type: src_node.type,
                data: {
                  ...src_node.data,
                  hovered: node.data.hovered,
                  inline_param: node.data.inline_param
                }
              }
              set((state) => ({
                nodes: state.nodes.map((n) => (n.data.id === node_id ? new_node : n)),
                edges: state.edges
                  .filter((e) => {
                    if (e.source === node.id) {
                      if (handleIndex(e.sourceHandle) >= new_node.data.outputs.length) {
                        return false
                      }
                    } else if (e.target === node.id) {
                      if (handleIndex(e.targetHandle) >= new_node.data.inputs.length) {
                        return false
                      }
                    }
                    return true
                  })
                  .map((e) => {
                    if (e.source === node.id) {
                      e.source = new_node.id
                    } else if (e.target === node.id) {
                      e.target = new_node.id
                    }
                    return e
                  })
              }))
            }
          },

          executeToNode: async (node_id: number) => {
            if (node_id == 0) {
              return
            }

            const { graph, nodes } = get()
            const display_node = nodes.find((n) => n.data.is_display)

            const parallel_execution_nodes =
              useSettingsStoreApi().getState().node_graph.parallel_execution_nodes === true

            const viewport_visible = useGlobalVariablesStoreApi().getState().viewport_visible
            if (!viewport_visible) {
              setViewportMesh([0, 0])
            }

            if (display_node !== undefined) {
              if (display_node.data.id === node_id) {
                if (viewport_visible) {
                  await setViewportMesh([node_id, 0])
                } else {
                  await nousExecuteToNode([graph, node_id, parallel_execution_nodes])
                }
              } else {
                const old_version = viewport_visible
                  ? await getNodeDataVersion([graph, display_node.data.id, 0])
                  : 0

                await nousExecuteToNode([graph, node_id, parallel_execution_nodes])

                if (viewport_visible) {
                  const new_version = await getNodeDataVersion([graph, display_node.data.id, 0])
                  if (old_version !== new_version) {
                    await setViewportMesh([display_node.data.id, 0, true])
                  }
                }
              }
            } else {
              await nousExecuteToNode([graph, node_id, parallel_execution_nodes])
            }
          },

          setNodeLogs: async (node_id: number, logs?: NodeLog[]) => {
            const { nodes } = get()
            const node = nodes.find((n) => n.data.id === node_id)
            if (node) {
              const new_node = {
                ...node,
                data: {
                  ...node.data,
                  logs: logs
                }
              }
              set((state) => ({
                nodes: state.nodes.map((n) => (n.data.id === node_id ? new_node : n))
              }))
            }
          },

          privateSetNodeSpace: async (node_space: NodeSpaceDesc) => {
            const { graph } = get()
            const { nodes, edges } = await loadSubGraph(graph, node_space.id)
            // eslint-disable-next-line no-unused-vars
            set((_) => ({
              nodes,
              edges,
              node_space
            }))
          },

          privateSetNodesPosition: async (
            changes: Record<string, XYPosition[]>,
            data_idx: number
          ) => {
            const { graph, node_space } = get()

            set((state) => ({
              nodes: state.nodes.map((node) => {
                const change = changes[node.data.label]
                if (change !== undefined) {
                  const new_pos = change[data_idx]
                  nousSetNodePositionByName(graph, node_space.id, node.data.label, new_pos)
                  return {
                    ...node,
                    position: new_pos
                  }
                }
                return node
              })
            }))
          },

          privateSetDisplayNode: async (node_label: string) => {
            let display_node_exists = false

            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.data.label === node_label) {
                  display_node_exists = true

                  return {
                    ...node,
                    data: {
                      ...node.data,
                      is_display: true
                    }
                  }
                } else if (node.data.is_display) {
                  return {
                    ...node,
                    data: {
                      ...node.data,
                      is_display: false
                    }
                  }
                }
                return node
              })
            }))

            if (display_node_exists) {
              const { graph, node_space } = get()
              await setNodeDisplay([graph, node_space.id, node_label])
            }
          },

          privateSetNodeColor: async (node_label: string, color: string) => {
            let target_node: NNode | null = null

            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.data.label === node_label) {
                  target_node = node
                  return {
                    ...node,
                    data: {
                      ...node.data,
                      color
                    }
                  }
                }
                return node
              })
            }))

            if (target_node !== null) {
              const { graph } = get()
              await nousSetNodeColor(graph, target_node, color)
            }
          },

          privateSetNodeBypassed: async (node_label: string, bypassed: boolean) => {
            let target_node_id: number | null = null

            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.data.label === node_label) {
                  target_node_id = node.data.id
                  return {
                    ...node,
                    data: {
                      ...node.data,
                      bypassed
                    }
                  }
                }
                return node
              })
            }))

            if (target_node_id !== null) {
              const { graph } = get()
              await setNodeBypass([graph, target_node_id, bypassed])
            }
          },

          privateSetNodeEditable: async (
            node_path: string,
            editable: boolean | undefined,
            node_label?: string
          ) => {
            if (node_label !== undefined) {
              set((state) => ({
                nodes: state.nodes.map((node) => {
                  if (node.data.label === node_label) {
                    return {
                      ...node,
                      data: {
                        ...node.data,
                        editable: editable
                      }
                    }
                  }
                  return node
                })
              }))
            }

            const { graph, node_space } = get()
            const target_node_id = (await getNodeSpaceDescByPath([graph, node_path]))!.id

            if (target_node_id == node_space.id) {
              set((state) => ({
                node_space: {
                  ...state.node_space,
                  editable: editable || false
                }
              }))
            }
            await setNodeEditable([target_node_id, editable || false])
          },

          privateRenameNode: async (old_name: string, new_name: string) => {
            const { graph, node_space } = get()
            await nousSetNodeNameByName(graph, node_space.id, old_name, new_name)
            set((state) => ({
              nodes: state.nodes.map((node) => {
                if (node.data.label === old_name) {
                  return {
                    ...node,
                    data: {
                      ...node.data,
                      label: new_name
                    }
                  }
                }
                return node
              })
            }))
          },

          privateDeleteEdge: async (edge: Edge) => {
            const { graph, nodes } = get()
            set((state) => ({
              edges: state.edges.filter((e) => e.id !== edge.id)
            }))
            await nousDeleteConnection(graph, edge, nodes)

            // undo/redo
            set(
              produce((state: State) => {
                state.undo_redo.push({
                  name: 'delete edge',
                  data: deepCopy(edge),
                  undo: async (data: Edge) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: [...state.edges, data]
                    }))
                    await nousAddConnection(graph, data, nodes)
                  },
                  redo: async (data: Edge) => {
                    const { graph, nodes } = get()
                    set((state) => ({
                      edges: state.edges.filter((edge) => edge.id !== data.id)
                    }))
                    await nousDeleteConnection(graph, data, nodes)
                  }
                })
                state.has_changes = true
              })
            )
          },

          execute: async () => {
            if (!useSettingsStoreApi().getState().node_graph.auto_execution) return

            const { nodes, undo_redo } = get()
            if (undo_redo.isGrouping()) return
            const display_node = nodes.find((n) => n.data.is_display)
            const viewport_visible = useGlobalVariablesStoreApi().getState().viewport_visible

            if (display_node) {
              if (viewport_visible) {
                await setViewportMesh([display_node.data.id, 0])
              } else {
                const { graph } = get()
                const parallel_execution_nodes =
                  useSettingsStoreApi().getState().node_graph.parallel_execution_nodes === true
                await nousExecuteToNode([graph, display_node.data.id, parallel_execution_nodes])
              }
            } else {
              await setViewportMesh([0, 0])
            }
          }
        }),
        {
          name: id,
          storage: nodeGraphLocalProvider,
          partialize: (state) => ({
            url: state.url,
            nodes: state.nodes,
            edges: state.edges,
            graph: state.graph,
            root_graph: state.root_graph,
            node_space: state.node_space,
            undo_redo: state.undo_redo
          }),
          onRehydrateStorage: () => (state) => {
            if (!state) return
            state.undo_redo.unlistenAll()
            state.undo_redo.listen('clear', state.execute)
            state.undo_redo.listen('push', state.execute)
            state.undo_redo.listen('redo', state.execute)
            state.undo_redo.listen('undo', state.execute)
            state.undo_redo.listen('end-grouping', state.execute)

            useNodeGraphBridgeStoreApi().setState({
              graph_id: state.graph,
              node_graph: getNodeGraphStoreApi(id)
            })

            if (state.url !== undefined) {
              setTitleBarInfo(state.url)
            }
          }
        }
      )
    )
  )

const findNodeById = (nodes: NNode[], id: string | null) => {
  return nodes.find((n) => n.id === id)
}

const nousAddConnection = async (
  graph_id: number,
  edge: {
    source: string | null
    target: string | null
    sourceHandle?: string | null
    targetHandle?: string | null
  },
  nodes: NNode[]
) => {
  const source = findNodeById(nodes, edge.source)
  const target = findNodeById(nodes, edge.target)
  if (source && target && source.data.id !== 0 && target.data.id !== 0) {
    return addConnect([
      graph_id,
      source.data.id,
      parseInt(edge.sourceHandle || '0'),
      target.data.id,
      parseInt(edge.targetHandle || '0')
    ])
  }
}

const nousDeleteConnection = async (
  graph_id: number,
  edge: { target: string | null; targetHandle?: string | null },
  nodes: NNode[]
) => {
  const target = findNodeById(nodes, edge.target)
  if (target) {
    return deleteConnect([graph_id, target.data.id, parseInt(edge.targetHandle || '0')])
  }
}

const nousDeleteNode = async (node: NNode) => {
  const node_id = node.data.id
  if (node_id !== 0) {
    return removeNode([node_id])
  }
}

const nousDeleteNodeByName = async (graph_id: number, subgraph_id: number, node_name: string) => {
  const node_id = await getNodeIdByName([graph_id, subgraph_id, node_name])
  if (node_id !== 0) {
    return removeNode([node_id])
  }
}

const nousSetNodeColor = async (graph_id: number, node: NNode, color: string) => {
  if (node.data.id != 0) {
    const rgba = hexToRgba(color)
    setNodeColor([
      graph_id,
      node.data.id,
      { value: [rgba.r / 255.0, rgba.g / 255.0, rgba.b / 255.0, rgba.a] }
    ])
  }
}

const nousSetNodePosition = async (graph_id: number, node: NNode, position: XYPosition) => {
  if (node.data.id != 0) {
    setNodePosition([graph_id, node.data.id, position.x, position.y])
  }
}

const nousSetNodePositionByName = async (
  graph_id: number,
  subgraph_id: number,
  node_name: string,
  position: XYPosition
) => {
  const node_id = await getNodeIdByName([graph_id, subgraph_id, node_name])
  if (node_id != 0) {
    setNodePosition([graph_id, node_id, position.x, position.y])
  }
}

const nousSetNodeName = async (node: NNode, name: string) => {
  if (node.data.id != 0) {
    return setNodeName([node.data.id, name])
  }
}

const nousSetNodeNameByName = async (
  graph_id: number,
  subgraph_id: number,
  node_name: string,
  new_name: string
) => {
  const node_id = await getNodeIdByName([graph_id, subgraph_id, node_name])
  if (node_id != 0) {
    return setNodeName([node_id, new_name])
  }
}

const nousCreateNode = async (
  graph_id: number,
  subgraph_id: number,
  node: NNode,
  override_name?: boolean,
  show_version: boolean = false
): Promise<[NNode[], Edge[]]> => {
  let { nodes: new_nodes, links: new_links } = await createNode([
    graph_id,
    subgraph_id,
    node.data.type_name,
    node.position.x,
    node.position.y
  ])

  const nodes = []
  const links: Edge[] = []

  if (new_nodes.length === 1) {
    const new_node = new_nodes[0]

    if (override_name && new_node.name !== node.data.label) {
      await setNodeName([new_node.id, node.data.label])
      new_node.name = await getNodeNameById([new_node.id])
    }

    let node_ = {
      ...node,
      data: {
        ...node.data,
        id: new_node.id,
        label: new_node.name,
        color: rgbaToHexa({
          r: new_node.color[0] * 255.0,
          g: new_node.color[1] * 255.0,
          b: new_node.color[2] * 255.0,
          a: new_node.color[3]
        }),
        show_version
      }
    }
    if (node.data.kind == NodeKind.Nda && node_.data.editable === undefined) {
      node_.data.editable = false
    }

    nodes.push(node_)
  } else {
    for (const new_node of new_nodes) {
      const new_node_ = await getNode([graph_id, subgraph_id, new_node.id])
      const nodeDefs = useNodeDefsStoreApi().defs
      const def = nodeDefs.find((d) => d.type_name === new_node_.type_name)

      if (!def) continue

      let node_ = {
        id: uuid(),
        type: node.type,
        position: { x: new_node_.pos_x, y: new_node_.pos_y },

        data: {
          id: new_node_.id,
          label: new_node_.name,
          type_name: new_node_.type_name,
          color: '#FFF',
          icon: new_node_.type_name,
          inputs: def.inputs.map((i) => ({
            name: i.name,
            type: i.type
          })),
          outputs: def.outputs.map((i) => ({
            name: i.name,
            type: i.type
          })),
          bypassed: false,
          kind: def.kind,

          category: def.category,
          desc: 'No description',

          version: def.type_version,
          show_version
        }
      }

      nodes.push(node_)
    }

    for (const new_link of new_links) {
      const node = nodes.find((node) => node.data.label === new_link.name)
      if (!node) continue

      for (const link_in of new_link.node_links_in) {
        const node_in = nodes.find((node) => node.data.label === link_in.node_name)
        if (!node_in) continue

        const link = {
          id: uuid(),
          source: node_in.id,
          sourceHandle: link_in.out_port_id.toString(),
          target: node.id,
          targetHandle: link_in.port_id.toString()
        }

        links.push(link)
      }
    }
  }

  return [nodes, links]
}

const nousCreateNodeFromJson = async (
  graph_id: number,
  subgraph_id: number,
  node: NNode,
  json: string,
  override_name?: boolean,
  show_version: boolean = false
): Promise<[NNode[], Edge[] | undefined]> => {
  const { nodes } = await pasteNodes_(graph_id, subgraph_id, json, false)

  if (!nodes || nodes.length == 0) {
    return nousCreateNode(graph_id, subgraph_id, node, override_name, show_version)
  }
  let new_node = nodes[0]
  if (override_name && node.data.label !== new_node.data.label) {
    await setNodeName([new_node.data.id, node.data.label])
    new_node.data.label = await getNodeNameById([new_node.data.id])
  }
  new_node = {
    ...node,
    data: {
      ...node.data,
      id: new_node.data.id,
      label: new_node.data.label,
      color: new_node.data.color
    }
  }
  return [[new_node], undefined]
}

const nousCreateNodesFromJson = async (
  graph_id: number,
  subgraph_id: number,
  in_nodes: NNode[],
  json: string,
  override_name?: boolean
): Promise<[NNode[], Edge[]]> => {
  let { nodes } = await pasteNodes_(graph_id, subgraph_id, json, false)
  if (!nodes || nodes.length == 0) {
    let new_nodes = []
    let new_links = []

    for (const in_node of in_nodes) {
      const [nodes, links] = await nousCreateNode(
        graph_id,
        subgraph_id,
        in_node,
        override_name,
        in_node.data.show_version
      )

      new_nodes.push(...nodes)
      new_links.push(...links)
    }
    return [new_nodes, new_links]
  } else {
    nodes = nodes.map((n) => {
      const node = in_nodes.find((in_node) => in_node.data.label === n.data.label)
      if (!node) {
        return n
      }
      return {
        ...node,
        data: {
          ...node.data,
          id: n.data.id,
          label: n.data.label,
          color: n.data.color
        }
      }
    })
  }
  return [nodes, []]
}

export function hasDifferentVersion(
  nodes: NNode[],
  type_name: string,
  new_version?: number
): boolean {
  let has_different_version = false
  let current_version = new_version ?? -1

  for (const node of nodes) {
    if (node.data.type_name === type_name) {
      if (current_version === -1) {
        current_version = node.data.version
      } else if (current_version !== node.data.version) {
        has_different_version = true
        break
      }
    }
  }

  return has_different_version
}
