import Tooltip from '@/components/Tooltip'
import { categoryTheme, iconThemes } from '@/constant/node-resource'
import { useI18n } from '@/locales'
import { listen as listenGlobalAction } from '@/stores/global-actions'
import { useNodeGraphStore, useNodeGraphStoreApi } from '@/stores/node-graph'
import {
  SettingsState,
  setMinimapVisible,
  setNodeGraphAutoExecution,
  useSettingsStore
} from '@/stores/settings'
import { closeWindow } from '@/utils/window-state'
import { Edge, NNode, NodeGraphState, NodeLog } from '@/types/node-graph'
import { cn } from '@/utils/cn'
import { isMacOS } from '@/utils/env'
import { delay } from '@/utils/functions'
import useLayoutedElements from '@/utils/layout'
import {
  getNodeSpacePathItems,
  handleCompare,
  handleIndex,
  isSubgraphOrNda,
  isSubgraphOrNdaEditable
} from '@/utils/node-graph'
import uuid from '@/utils/uuid'
import NiceModal from '@ebay/nice-modal-react'
import { listen } from '@tauri-apps/api/event'
import {
  // clear as clipboardClear,
  readText as clipboardRead,
  writeText as clipboardWrite
} from '@tauri-apps/plugin-clipboard-manager'
import {
  Background,
  Connection,
  DefaultEdgeOptions,
  FitViewOptions,
  InternalNode,
  MiniMap,
  Node as Node_,
  OnConnectStartParams,
  Panel,
  ReactFlow,
  ReactFlowProvider,
  Viewport,
  XYPosition,
  getConnectedEdges,
  getOutgoers,
  useReactFlow,
  useStoreApi
} from '@xyflow/react'
import { produce } from 'immer'
import { checkIntersection } from 'line-intersect'
import React, { useCallback, useMemo, useRef, useState } from 'react'
import { useHotkeys } from 'react-hotkeys-hook'
import { useAsync, useEffectOnce, useKeyPressEvent } from 'react-use'
import { useShallow } from 'zustand/react/shallow'
import ContextMenu, { createNode } from './ContextMenu'
import NodeContextMenu from './NodeContextMenu'
import PopupDialog, { PopupDialogType } from './PopupDialog'
import ConnectionLine from './ConnectionLine'
import {
  ContextMenuParam,
  InteractiveContext,
  InteractiveContextState,
  edgeTypes,
  nodeTypes,
  EdgeInfo
} from './context'

import { BsFullscreen, BsViewStacked } from 'react-icons/bs'
import { CiMap } from 'react-icons/ci'
import { FaRegPlayCircle } from 'react-icons/fa'
import { FaRegCircleStop } from 'react-icons/fa6'
import { GoUnlock } from 'react-icons/go'
import { PiPackageLight, PiSelectionBackground } from 'react-icons/pi'
import { VscRootFolderOpened } from 'react-icons/vsc'

import '@xyflow/react/dist/style.css'

import '@/css/node-graph.css'

const defaultEdgeOptions: DefaultEdgeOptions = {
  animated: false,
  focusable: true,
  style: { strokeWidth: 2 }
}

const fitViewOptions: FitViewOptions = {
  padding: 0.5,
  includeHiddenNodes: false
}

function miniMapNodeColor(node: NNode): string {
  if (!node.data.color.startsWith('#FFF')) {
    return node.data.color
  }
  let idx = categoryTheme[node.data.category]
  if (idx === undefined) {
    idx = 0
  }
  return iconThemes[idx][1]
}

const settingsStoreSelector = (state: SettingsState) => ({
  snapToGrid: state.node_graph.snap_to_grid,
  selectionMode: state.node_graph.selection_mode,
  background: state.node_graph.background,
  minimap: state.node_graph.minimap,
  autoExecution: state.node_graph.auto_execution,
  backgroundColor: state.node_graph.background_color
})

const nodeGraphStoreSelector = (state: NodeGraphState) => ({
  nodes: state.nodes,
  edges: state.edges,
  nodeSpace: state.node_space,
  onNodesChange_: state.onNodesChange,
  onEdgesChange_: state.onEdgesChange,
  onEdgesDelete_: state.onEdgesDelete,
  setEdges_: state.setEdges
})

function Flow() {
  const { nodes, edges, nodeSpace, onNodesChange_, onEdgesChange_, onEdgesDelete_, setEdges_ } =
    useNodeGraphStore(useShallow(nodeGraphStoreSelector))
  const { snapToGrid, selectionMode, background, minimap, autoExecution, backgroundColor } =
    useSettingsStore(settingsStoreSelector)
  const {
    fitView,
    screenToFlowPosition,
    deleteElements,
    getNodes,
    getEdges,
    setViewport,
    getViewport,
    setNodes
  } = useReactFlow()
  const flowStoreApi = useStoreApi()
  const storeApi = useNodeGraphStoreApi()
  const { t } = useI18n()
  const { layoutedElements } = useLayoutedElements()
  const pathItems = useAsync(
    async () => await getNodeSpacePathItems(nodeSpace ? nodeSpace.id : 0),
    [nodeSpace]
  )
  const isEditable = useMemo(
    () => (nodeSpace ? isSubgraphOrNdaEditable(nodeSpace.editable) : true),
    [nodeSpace]
  )

  //console.log('render NodeGraph')
  const nodeSpaceViewportCache = useRef<Record<string, Viewport>>({})
  const edgeUpdateSuccessful = useRef(true)
  const connectingStart = useRef<OnConnectStartParams | null>(null)
  const intersectingEdge = useRef<Edge | null>(null)
  const [contextMenuParam, setContextMenuParam] = useState<ContextMenuParam>({
    show: false
  })
  const ref = useRef(null)
  const nodePositionChanged = useRef(false)
  const oldPositions = useRef<Record<string, XYPosition>>({})
  const lastClickPos = useRef<XYPosition>({ x: 0, y: 0 })

  const [hideElementToolbar, setHideElementToolbar] = useState(false)
  const preContextRef = useRef<InteractiveContextState | null>(null)

  const _showElementToolbar = useCallback(() => {
    setHideElementToolbar(false)
  }, [setHideElementToolbar])

  const _hideElementToolbar = useCallback(() => {
    setHideElementToolbar(true)
  }, [setHideElementToolbar])

  useKeyPressEvent('Control', _hideElementToolbar, _showElementToolbar)

  const setNodeSpace = useCallback(
    async (node?: NNode, idx?: number) => {
      if (idx !== undefined) {
        if (!pathItems.value || idx == pathItems.value.length - 1) {
          return
        }
      } else if (node === undefined || !isSubgraphOrNda(node.data.kind)) {
        return
      }

      const current = getViewport()
      nodeSpaceViewportCache.current[storeApi.getState().node_space.id] = {
        x: current.x,
        y: current.y,
        zoom: current.zoom
      }

      let new_space_id = 0
      if (node !== undefined) {
        new_space_id = await storeApi.getState().setNodeSpace(node)
      } else if (idx !== undefined) {
        new_space_id = await storeApi
          .getState()
          .setNodeSpace('/' + pathItems.value!.slice(1, idx + 1).join('/'))
      }

      const viewport = nodeSpaceViewportCache.current[new_space_id]
      if (viewport) {
        setViewport(viewport)
      } else {
        fitView()
      }
    },
    [pathItems, getViewport]
  )

  const showContextMenu = useCallback(
    async (
      menu_pos: XYPosition,
      node_pos?: XYPosition,
      connection?: OnConnectStartParams,
      edge?: EdgeInfo,
      node?: NNode
    ) => {
      const pane = (ref.current as any).getBoundingClientRect()
      const X = menu_pos.x - pane.x
      const Y = menu_pos.y - pane.y

      if (node) {
        if (contextMenuParam.show) {
          // force update menu position
          setContextMenuParam({ show: false })
          await delay(50)
        }

        setContextMenuParam({
          show: true,
          top: Y < pane.height - 200 ? Math.max(Y - 15, 0) : undefined,
          left: X < pane.width - 200 ? Math.max(X + 100, 0) : undefined,
          right: X >= pane.width - 200 ? pane.width - X : undefined,
          bottom: Y >= pane.height - 200 ? pane.height - Y : undefined,
          node
        })
      } else {
        if (!node_pos) {
          node_pos = screenToFlowPosition({ x: menu_pos.x, y: menu_pos.y })
        }
        setContextMenuParam({
          show: true,
          top: Y < pane.height - 200 ? Math.max(Y - 15, 0) : undefined,
          left: X < pane.width - 200 ? Math.max(X - 85, 0) : undefined,
          right: X >= pane.width - 200 ? pane.width - X : undefined,
          bottom: Y >= pane.height - 200 ? pane.height - Y : undefined,
          node_pos,
          connection,
          edge
        })
      }
    },
    [contextMenuParam, screenToFlowPosition]
  )

  const onPaneContextMenu = useCallback(
    (event: MouseEvent | React.MouseEvent<Element, MouseEvent>) => {
      event.preventDefault()
      showContextMenu({ x: event.clientX, y: event.clientY })
    },
    [showContextMenu]
  )

  const onNodeContextMenu = useCallback(
    (event: React.MouseEvent<Element, MouseEvent>, node: NNode) => {
      event.preventDefault()
      showContextMenu({ x: event.clientX, y: event.clientY }, undefined, undefined, undefined, node)
    },
    [showContextMenu]
  )

  const onPaneClick = useCallback(
    (e?: React.MouseEvent<Element, MouseEvent>) => {
      if (e) {
        lastClickPos.current = { x: e.clientX, y: e.clientY }
      }
      if (contextMenuParam.show && !connectingStart.current) {
        if (!isEditable) return
        setContextMenuParam({
          show: false
        })
      }
    },
    [contextMenuParam, isEditable]
  )

  const onNodeMouseEnter = useCallback(
    (_: any, node: Node_) => {
      if (!flowStoreApi.getState().userSelectionActive) {
        storeApi.getState().setNodeHovered(node.id, true)
      }
    },
    [nodes]
  )

  const onNodeMouseLeave = useCallback(
    (_: any, node: Node_) => {
      if (!flowStoreApi.getState().userSelectionActive) {
        storeApi.getState().setNodeHovered(node.id, false)
      }
    },
    [nodes]
  )

  const getIntersectingEdge = useCallback((node_: NNode) => {
    const { nodeLookup, edges: edges_ } = flowStoreApi.getState()
    const node = nodeLookup.get(node_.id)
    const shrink = 80
    if (
      node &&
      node.measured &&
      node.internals.positionAbsolute &&
      node.measured.width &&
      node.measured.height
    ) {
      for (const edge of edges_) {
        if (edge.source === node.id || edge.target === node.id) {
          continue
        }
        const source = nodeLookup.get(edge.source) as InternalNode<NNode>
        const target = nodeLookup.get(edge.target) as InternalNode<NNode>
        if (
          source &&
          source.measured &&
          source.internals.positionAbsolute &&
          source.measured.width &&
          source.measured.height &&
          target &&
          target.measured &&
          target.internals.positionAbsolute &&
          target.measured.width
        ) {
          const source_offset =
            ((1 + handleIndex(edge.sourceHandle)) * source.measured.width) /
            (1.0 + source.data.outputs.length)
          const target_offset =
            ((1 + handleIndex(edge.targetHandle)) * target.measured.width) /
            (1.0 + target.data.inputs.length)
          const source_pos = {
            x: source.internals.positionAbsolute.x + source_offset,
            y: source.internals.positionAbsolute.y + source.measured.height
          }
          const target_pos = {
            x: target.internals.positionAbsolute.x + target_offset,
            y: target.internals.positionAbsolute.y
          }

          if (
            checkIntersection(
              source_pos.x,
              source_pos.y,
              target_pos.x,
              target_pos.y,
              node.internals.positionAbsolute.x + shrink,
              node.internals.positionAbsolute.y + node.measured.height * 0.5,
              node.internals.positionAbsolute.x + node.measured.width - shrink,
              node.internals.positionAbsolute.y + node.measured.height * 0.5
            ).type === 'intersecting'
          ) {
            return edge
          }
        }
      }
    }
    return null
  }, [])

  const onNodeDrag = useCallback(
    (_: any, node: NNode, nodes: NNode[]) => {
      nodePositionChanged.current = true
      if (nodes.length > 1) return

      const intersected_edge = getIntersectingEdge(node)
      if (intersected_edge) {
        if (!intersectingEdge.current || intersectingEdge.current.id !== intersected_edge.id) {
          let temp_edges = []
          const edges_ = storeApi.getState().edges
          if (node.data.inputs.length > 0) {
            const temp_edge = {
              id: 'temp1',
              source: intersected_edge.source,
              sourceHandle: intersected_edge.sourceHandle,
              target: node.id,
              targetHandle: undefined,
              className: 'temp'
            }
            if (
              !edges_.find(
                (e) =>
                  e.source === temp_edge.source &&
                  e.target === temp_edge.target &&
                  handleCompare(e.sourceHandle, temp_edge.sourceHandle) &&
                  handleCompare(e.targetHandle, temp_edge.targetHandle)
              )
            ) {
              temp_edges.push(temp_edge)
            }
          }
          if (node.data.outputs.length > 0) {
            const temp_edge = {
              id: 'temp2',
              source: node.id,
              sourceHandle: undefined,
              target: intersected_edge.target,
              targetHandle: intersected_edge.targetHandle,
              className: 'temp'
            }
            if (
              !edges_.find(
                (e) =>
                  e.source === temp_edge.source &&
                  e.target === temp_edge.target &&
                  handleCompare(e.sourceHandle, temp_edge.sourceHandle) &&
                  handleCompare(e.targetHandle, temp_edge.targetHandle)
              )
            ) {
              temp_edges.push(temp_edge)
            }
          }
          setEdges_([...edges_.filter((e) => !e.id.startsWith('temp')), ...temp_edges])
          intersectingEdge.current = intersected_edge
        }
      } else if (intersectingEdge.current) {
        setEdges_(storeApi.getState().edges.filter((e) => !e.id.startsWith('temp')))
        intersectingEdge.current = null
      }
    },
    [getIntersectingEdge]
  )

  const onNodeDoubleClick = useCallback(
    (_: any, node: NNode) => {
      setNodeSpace(node, undefined)
    },
    [contextMenuParam, setNodeSpace]
  )

  const onNodeDragStart = useCallback((_: any, _node: NNode, nodes: NNode[]) => {
    nodePositionChanged.current = false
    oldPositions.current = {}
    for (const n of nodes) {
      oldPositions.current[n.id] = n.position
    }
  }, [])

  const onNodeDragStop = useCallback(
    async (_: any, node: NNode) => {
      if (nodePositionChanged.current) {
        nodePositionChanged.current = false
        storeApi.getState().onNodesPositionChange(oldPositions.current)
      }

      if (intersectingEdge.current) {
        let temp_edges = storeApi.getState().edges.filter((e) => e.id.startsWith('temp'))
        if (temp_edges.length > 0) {
          await setEdges_(storeApi.getState().edges.filter((e) => !e.id.startsWith('temp')))

          // begin undo group
          await storeApi.setState(
            produce((state) => {
              state.undo_redo.beginGroup('insert node')
            })
          )

          const edges_to_del = storeApi
            .getState()
            .edges.filter(
              (e) =>
                (e.source == node.id && handleCompare(e.sourceHandle)) ||
                (e.target == node.id && handleCompare(e.targetHandle))
            )
            .concat(intersectingEdge.current)

          await setEdges_(
            storeApi
              .getState()
              .edges.filter((e) => edges_to_del.find((e2) => e2.id === e.id) === undefined)
          )

          await storeApi.getState().onEdgesDelete(edges_to_del)
          // await deleteElements({ edges: edges_to_del })

          const new_edges: Edge[] = temp_edges.map((e) => ({
            id: uuid(),
            source: e.source,
            sourceHandle: e.sourceHandle,
            target: e.target,
            targetHandle: e.targetHandle
          }))

          await storeApi.getState().addEdges(new_edges)

          // end undo group
          await storeApi.setState(
            produce((state) => {
              state.undo_redo.endGroup()
            })
          )
        }
        intersectingEdge.current = null
      }
    },
    [deleteElements]
  )

  const onEdgeMouseEnter = useCallback(
    (_: any, edge: Edge) => {
      if (!flowStoreApi.getState().userSelectionActive) {
        storeApi.getState().setEdgeHovered(edge.id, true)
      }
    },
    [edges]
  )

  const onEdgeMouseLeave = useCallback(
    (_: any, edge: Edge) => {
      if (!flowStoreApi.getState().userSelectionActive) {
        storeApi.getState().setEdgeHovered(edge.id, false)
      }
    },
    [edges]
  )

  const onReconnectStart = useCallback(() => {
    edgeUpdateSuccessful.current = false
    _hideElementToolbar()
  }, [_hideElementToolbar])

  const onReconnect = useCallback(
    (old_edge: Edge, new_connection: Connection) => {
      edgeUpdateSuccessful.current = true
      connectingStart.current = null
      storeApi.getState().onReconnect(old_edge, new_connection)
    },
    [edges]
  )

  const onReconnectEnd = useCallback(
    (_: any, edge: Edge) => {
      if (!edgeUpdateSuccessful.current) {
        deleteElements({ edges: [{ id: edge.id }] })
      }
      edgeUpdateSuccessful.current = true
      _showElementToolbar()
    },
    [edges, _showElementToolbar]
  )

  const onEdgeDoubleClick = useCallback(
    (e: React.MouseEvent, edge: Edge) => {
      let node_pos = screenToFlowPosition({
        x: e.clientX,
        y: e.clientY
      })
      node_pos.x -= 9
      node_pos.y -= 30

      createNode('Dot', storeApi, {
        node_pos,
        edge
      })
    },
    [edges]
  )

  const onConnect = useCallback((connection: Connection) => {
    // reset the start node on connections
    connectingStart.current = null
    storeApi.getState().onConnect(connection)
  }, [])

  const onConnectStart = useCallback(
    (_: any, { nodeId, handleId, handleType }: OnConnectStartParams) => {
      connectingStart.current = { nodeId, handleId, handleType }
      _hideElementToolbar()
    },
    [_hideElementToolbar]
  )

  const onConnectEnd = useCallback(
    (event: any) => {
      _showElementToolbar()
      if (!connectingStart.current) return
      const target_is_pane = (event.target as any)?.classList.contains('react-flow__pane')
      if (target_is_pane) {
        event.preventDefault()
        showContextMenu(
          {
            x: event.clientX,
            y: event.clientY
          },
          undefined,
          connectingStart.current
        )
        delay(100).then(() => {
          connectingStart.current = null
        })
      }
    },
    [showContextMenu]
  )

  const onNodesDelete = useCallback(
    async (deleted: NNode[]) => {
      // begin undo group
      await storeApi.setState(
        produce((state) => {
          state.undo_redo.beginGroup('delete nodes')
        })
      )

      const connected_edges = getConnectedEdges(deleted, edges)
      let new_edges = []

      if (deleted.length == 1) {
        const node = deleted[0]
        if (connected_edges.length > 1) {
          for (const edge of connected_edges) {
            if (edge.target === node.id) {
              const handle_idx = handleIndex(edge.targetHandle)
              const pair = connected_edges.find(
                (e: Edge) => e.source === node.id && handleIndex(e.sourceHandle) == handle_idx
              )
              if (pair) {
                new_edges.push({
                  id: uuid(),
                  source: edge.source,
                  sourceHandle: edge.sourceHandle,
                  target: pair.target,
                  targetHandle: pair.targetHandle
                })
              }
            }
          }
        }
      }

      await storeApi.getState().onNodesDelete(deleted)

      if (new_edges.length > 0) {
        await storeApi.getState().addEdges(new_edges)
      }

      // end undo group
      await storeApi.setState(
        produce((state) => {
          state.undo_redo.endGroup()
        })
      )
    },
    [nodes, edges]
  )

  const onDelete = useCallback(
    async (params: { nodes: NNode[]; edges: Edge[] }) => {
      if (params.nodes.length == 0) {
        return onEdgesDelete_(params.edges)
      } else if (params.edges.length == 0) {
        return onNodesDelete(params.nodes)
      }

      // begin undo group
      await storeApi.setState(
        produce((state) => {
          state.undo_redo.beginGroup('delete elements')
        })
      )
      await onEdgesDelete_(params.edges)
      await onNodesDelete(params.nodes)
      // end undo group
      await storeApi.setState(
        produce((state) => {
          state.undo_redo.endGroup()
        })
      )
    },
    [onNodesDelete, onEdgesDelete_]
  )

  // preventing cycles
  const isValidConnection = useCallback((connection: Edge | Connection) => {
    if (connection.source === connection.target) return false

    // we are using getNodes and getEdges helpers here
    // to make sure we create isValidConnection function only once
    const nodes = getNodes()
    const edges = getEdges()
    const target = nodes.find((node) => node.id === connection.target)
    if (!target) return false
    if (
      edges.findIndex(
        (e: Edge) =>
          e.source == connection.source &&
          e.target === connection.target &&
          handleCompare(e.sourceHandle, connection.sourceHandle) &&
          handleCompare(e.targetHandle, connection.targetHandle)
      ) > -1
    )
      return false

    const hasCycle = (node: Node_, visited = new Set()) => {
      if (visited.has(node.id)) return false

      visited.add(node.id)

      for (const outgoer of getOutgoers(node, nodes, edges)) {
        if (outgoer.id === connection.source) return true
        if (hasCycle(outgoer, visited)) return true
      }
    }

    if (target.id === connection.source) return false
    return !hasCycle(target)
  }, [])

  const toggleMiniMap = useCallback(() => {
    setMinimapVisible(!minimap)
  }, [minimap])

  const toggleAutoExecution = useCallback(() => {
    setNodeGraphAutoExecution(!autoExecution)
  }, [autoExecution])

  const packSelectedNodes = useCallback(() => {
    const { nodes, createSubgraphFromNodes } = storeApi.getState()
    const node_ids = nodes.filter((n) => n.selected).map((n) => n.data.id)
    if (node_ids.length > 0) {
      createSubgraphFromNodes(node_ids)
    }
  }, [])

  const createBackdrop = useCallback(() => {
    const { nodes, createBackdropFromNodes } = storeApi.getState()
    const node_ids = nodes.filter((n) => n.selected).map((n) => n.data.id)
    if (node_ids.length > 0) {
      createBackdropFromNodes(node_ids)
    }
  }, [])

  const unlockGraph = useCallback(() => {
    storeApi.getState().setNodeEditable(storeApi.getState().node_space.id, true)
  }, [])

  const onDisplay = useCallback(() => {
    const selected_node = storeApi.getState().nodes.find((n) => n.selected)
    if (selected_node) {
      storeApi.getState().setDisplayNode(selected_node.id)
    }
  }, [])

  const onExecuteSelected = useCallback(() => {
    const selected_node = storeApi.getState().nodes.find((n) => n.selected)
    if (selected_node) {
      storeApi.getState().executeToNode(selected_node.data.id)
    }
  }, [])

  const onSelectAll = useCallback(() => {
    setNodes((nodes) => {
      return nodes.map((n) => {
        if (!n.selected) {
          return { ...n, selected: true }
        }
        return n
      })
    })
  }, [setNodes])

  const onCopy = useCallback(async () => {
    const all_nodes = storeApi.getState().nodes
    const selected = all_nodes.filter((n) => n.selected)
    if (selected.length == 0) {
      return selected
    }
    const data = await storeApi.getState().copyNodes(selected, true)
    clipboardWrite(data, { label: 'nous-nodes' })
    return selected
  }, [])

  const jumpToNode = useCallback(async (path: string) => {
    const idx = path.lastIndexOf('/')
    const node_space_path = path.substring(0, idx) || '/'
    const node_name = path.substring(idx + 1)
    if (!node_name) return
    const new_space_id = await storeApi.getState().setNodeSpace(node_space_path)
    if (new_space_id <= 0) return
    await delay(100)
    const node = await storeApi.getState().nodes.find((n) => n.data.label === node_name)
    if (!node) {
      const viewport = nodeSpaceViewportCache.current[new_space_id]
      if (viewport) {
        setViewport(viewport)
      } else {
        fitView()
      }
    } else {
      fitView({ nodes: [node] })
      flowStoreApi.getState().resetSelectedElements()
      flowStoreApi.getState().addSelectedNodes([node.id])
    }
  }, [])

  const onPaste = useCallback(async () => {
    const data = await clipboardRead()
    if (data.length == 0) return
    if (data.startsWith('/')) {
      // paste node path, jump to the node
      jumpToNode(data)
    } else {
      storeApi.getState().pasteNodes(
        data,
        screenToFlowPosition({
          x: lastClickPos.current.x,
          y: lastClickPos.current.y
        })
      )
    }
  }, [jumpToNode])

  const onCut = useCallback(async () => {
    const selected = await onCopy()
    if (selected.length == 0) {
      return
    }
    const all_edges = storeApi.getState().edges
    const connected_edges = getConnectedEdges(selected, all_edges)
    deleteElements({ nodes: selected, edges: connected_edges })
  }, [deleteElements])

  const onNew = useCallback(() => {
    const state = storeApi.getState()
    if (state.hasChanges()) {
      NiceModal.show(PopupDialog, { type: PopupDialogType.SaveBeforeNew, storeApi })
    } else {
      state.clear()
    }
  }, [])

  const onOpen = useCallback((url?: string) => {
    const state = storeApi.getState()
    if (state.hasChanges()) {
      NiceModal.show(PopupDialog, { type: PopupDialogType.SaveBeforeOpen, url, storeApi })
    } else {
      state.load(url)
    }
  }, [])

  const onSave = useCallback(() => {
    storeApi.getState().save()
  }, [])

  const onSaveAs = useCallback(() => {
    storeApi.getState().save(undefined, true)
  }, [])

  const onQuit = useCallback(() => {
    if (storeApi.getState().hasChanges()) {
      NiceModal.show(PopupDialog, { type: PopupDialogType.SaveBeforeQuit, storeApi })
    } else {
      closeWindow()
    }
  }, [])

  const meta_key = isMacOS() ? 'meta' : 'ctrl'
  useHotkeys('d', onDisplay)
  useHotkeys('l', () => layoutedElements())
  useHotkeys('f', () => fitView({ duration: 200 }))
  useHotkeys(`${meta_key}+enter`, onExecuteSelected)
  useHotkeys(`${meta_key}+a`, onSelectAll)

  useEffectOnce(() => {
    let handles = []
    // TODO: react when node graph is focused
    handles.push(listenGlobalAction('quit', onQuit))
    handles.push(listenGlobalAction('new', onNew))
    handles.push(listenGlobalAction('open', onOpen))
    handles.push(listenGlobalAction('save', onSave))
    handles.push(listenGlobalAction('saveAs', onSaveAs))
    handles.push(listenGlobalAction('cut', onCut))
    handles.push(listenGlobalAction('copy', onCopy))
    handles.push(listenGlobalAction('paste', onPaste))
    handles.push(listenGlobalAction('jumpToNode', jumpToNode))

    const unlisten1 = listen<string>('NodeChangeEvent', (event) => {
      const node_id = JSON.parse(event.payload).node_id as number
      storeApi.getState().refreshNode(node_id)
    })
    const unlisten2 = listen<string>('NodeLogEvent', (event) => {
      const payload = JSON.parse(event.payload)
      const node_id = payload.node_id as number
      const node_logs = payload.logs as NodeLog[]
      storeApi.getState().setNodeLogs(node_id, node_logs)
    })

    return () => {
      handles.forEach((h) => h())
      unlisten1.then((fn) => fn())
      unlisten2.then((fn) => fn())
    }
  })

  const ctxData = useMemo(() => {
    let new_ctx = false
    if (preContextRef.current === null) {
      new_ctx = true
    } else {
      if (
        showContextMenu !== preContextRef.current.showContextMenu ||
        setNodeSpace !== preContextRef.current.setNodeSpace ||
        isEditable !== preContextRef.current.editable
      ) {
        new_ctx = true
      } else if (hideElementToolbar !== preContextRef.current.hideElementToolbar) {
        preContextRef.current.hideElementToolbar = hideElementToolbar
        new_ctx = false
      }
    }

    if (new_ctx) {
      preContextRef.current = {
        showContextMenu,
        setNodeSpace,
        editable: isEditable,
        hideElementToolbar
      }
    }
    return preContextRef.current!
  }, [showContextMenu, setNodeSpace, isEditable, hideElementToolbar])

  return (
    <InteractiveContext.Provider value={ctxData}>
      <ReactFlow
        nodeTypes={nodeTypes}
        edgeTypes={edgeTypes}
        connectionLineComponent={ConnectionLine}
        nodes={nodes}
        edges={edges}
        onNodesChange={isEditable ? onNodesChange_ : undefined}
        // onNodesDelete={isEditable ? onNodesDelete : undefined}
        onNodeDoubleClick={onNodeDoubleClick}
        onNodeDrag={isEditable ? onNodeDrag : undefined}
        onNodeDragStart={isEditable ? onNodeDragStart : undefined}
        onNodeDragStop={isEditable ? onNodeDragStop : undefined}
        onNodeMouseEnter={isEditable ? onNodeMouseEnter : undefined}
        onNodeMouseLeave={isEditable ? onNodeMouseLeave : undefined}
        onEdgesChange={isEditable ? onEdgesChange_ : undefined}
        // onEdgesDelete={isEditable ? onEdgesDelete_ : undefined}
        onEdgeMouseEnter={isEditable ? onEdgeMouseEnter : undefined}
        onEdgeMouseLeave={isEditable ? onEdgeMouseLeave : undefined}
        onReconnect={isEditable ? onReconnect : undefined}
        onReconnectStart={isEditable ? onReconnectStart : undefined}
        onReconnectEnd={isEditable ? onReconnectEnd : undefined}
        onEdgeDoubleClick={isEditable ? onEdgeDoubleClick : undefined}
        onConnect={isEditable ? onConnect : undefined}
        onDelete={isEditable ? onDelete : undefined}
        onConnectStart={isEditable ? onConnectStart : undefined}
        onConnectEnd={isEditable ? onConnectEnd : undefined}
        onNodeContextMenu={isEditable ? onNodeContextMenu : undefined}
        onPaneContextMenu={isEditable ? onPaneContextMenu : undefined}
        onSelectionStart={_hideElementToolbar}
        onSelectionEnd={_showElementToolbar}
        onPaneClick={onPaneClick}
        isValidConnection={isValidConnection}
        defaultEdgeOptions={defaultEdgeOptions}
        fitViewOptions={fitViewOptions}
        panOnDrag={[1]}
        connectOnClick={true}
        selectionOnDrag={true}
        selectNodesOnDrag={true}
        selectionMode={selectionMode}
        autoPanOnNodeDrag={true}
        autoPanOnConnect={true}
        autoPanSpeed={20}
        fitView
        nodeDragThreshold={1}
        snapToGrid={snapToGrid}
        deleteKeyCode={isEditable ? ['Delete', 'Backspace'] : null}
        multiSelectionKeyCode={isMacOS() ? 'Meta' : 'Shift'}
        minZoom={0.1}
        maxZoom={2}
        ref={ref}
        onClick={onPaneClick}
        nodesDraggable={isEditable}
        nodesConnectable={isEditable}
        edgesFocusable={isEditable}
        className="size-full"
        style={{
          background: isEditable ? backgroundColor : '#8f959933'
        }}
      >
        {/* background */}
        {background !== undefined && <Background color="#ccc" variant={background} />}

        {/* mini map */}
        {minimap && (
          <MiniMap
            nodeStrokeWidth={3}
            zoomable
            pannable
            inversePan={false}
            nodeColor={miniMapNodeColor}
            nodeBorderRadius={10}
            className="bg-menu-bg opacity-50"
            title="MiniMap"
          />
        )}

        {/* context menu */}
        {contextMenuParam.show &&
          (contextMenuParam.node ? (
            <NodeContextMenu {...contextMenuParam} close={onPaneClick} />
          ) : (
            <ContextMenu param={contextMenuParam} close={onPaneClick} />
          ))}

        {/* toolbar */}
        <Panel
          position="top-right"
          className="m-1 mr-3.5 flex select-none gap-0	rounded	bg-menu-bg/50 p-0 text-zinc-400"
        >
          {/* unlock graph */}
          {!isEditable && (
            <Tooltip placement="bottom-end" content="tooltip.node-graph.unlock-graph">
              <div
                className="btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary"
                onClick={unlockGraph}
              >
                <GoUnlock className="size-full p-1" />
              </div>
            </Tooltip>
          )}

          {/* auto execution */}
          <Tooltip placement="bottom-end" content="tooltip.node-graph.auto-execution">
            <div
              className={cn(
                'btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary',
                autoExecution ? ' bg-btn-active/50 text-text-primary' : ''
              )}
              onClick={toggleAutoExecution}
            >
              {autoExecution ? (
                <FaRegPlayCircle className="size-full p-1" />
              ) : (
                <FaRegCircleStop className="size-full p-1" />
              )}
            </div>
          </Tooltip>

          {/* fitview */}
          <Tooltip placement="bottom-end" content="tooltip.node-graph.fitview">
            <div
              className="btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary"
              onClick={() => fitView({ duration: 200 })}
            >
              <BsFullscreen className="size-full p-1.5" />
            </div>
          </Tooltip>

          {/* layout */}
          <Tooltip placement="bottom-end" content="tooltip.node-graph.layout">
            <div
              className="btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary"
              onClick={() => layoutedElements()}
            >
              <BsViewStacked className="size-full p-1.5" />
            </div>
          </Tooltip>

          {/* create backdrop */}
          <Tooltip placement="bottom-end" content="node-graph.create-backdrop-from-selected-nodes">
            <div
              className="btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary"
              onClick={createBackdrop}
            >
              <PiSelectionBackground className="size-full p-1" />
            </div>
          </Tooltip>

          {/* pack selected nodes */}
          <Tooltip placement="bottom-end" content="node-graph.pack-nodes">
            <div
              className="btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary"
              onClick={packSelectedNodes}
            >
              <PiPackageLight className="size-full p-1" />
            </div>
          </Tooltip>

          {/* minimap */}
          <Tooltip placement="bottom-end" content="tooltip.node-graph.toggle-minimap">
            <div
              className={cn(
                'btn btn-square btn-ghost size-7 min-h-0 rounded	hover:text-text-primary',
                minimap ? ' bg-btn-active/50 text-text-primary' : ''
              )}
              onClick={toggleMiniMap}
            >
              <CiMap className="size-full p-1" />
            </div>
          </Tooltip>
        </Panel>

        {/* node space bar */}
        {pathItems.value && pathItems.value.length > 1 && (
          <Panel position="top-left" className="select-none rounded bg-menu-bg/50 text-zinc-300">
            <div className="breadcrumbs m-0 px-4 py-1 text-sm">
              <ul>
                {pathItems.value.map((item, idx) => (
                  <li key={idx}>
                    <span
                      className="cursor-pointer hover:text-text-primary"
                      onClick={() => setNodeSpace(undefined, idx)}
                    >
                      {idx == 0 ? <VscRootFolderOpened /> : item}
                    </span>
                  </li>
                ))}
              </ul>
            </div>
          </Panel>
        )}

        {/* subgraph status */}
        {!isEditable && (
          <Panel position="bottom-left" className="select-none rounded bg-menu-bg/50 text-zinc-400">
            <span className="p-4 text-lg font-bold">{t('common.not-editable')}</span>
          </Panel>
        )}
      </ReactFlow>
    </InteractiveContext.Provider>
  )
}

export function NodeGraph() {
  return (
    <ReactFlowProvider>
      <Flow />
    </ReactFlowProvider>
  )
}
