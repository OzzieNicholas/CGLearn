import { getNode, getNodeDef } from '@/commands'
import { NodeData as NodeDataType, NodeDef } from '@/commands/index.types'
import Empty from '@/components/Empty'
import { getIconTheme, iconTypes } from '@/constant/node-resource'
import {
  setNode,
  useNodeGraphBridgeStore,
  useNodeGraphBridgeStoreApi
} from '@/stores/node-graph-bridge'
import { Nullable } from '@/types/maybe'
import { NodeData } from '@/types/node-graph'
import {
  initDefRootLayout,
  isSubgraphOrNdaEditable,
  mergeItemDefs,
  updateDefEnumList
} from '@/utils/node-graph'
import NiceModal from '@ebay/nice-modal-react'
// eslint-disable-next-line no-redeclare
import { Event, listen } from '@tauri-apps/api/event'
import { Mutex } from 'async-mutex'
import { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { TbSettingsBolt } from 'react-icons/tb'
import { Else, If, Then } from 'react-if'
import { useEffectOnce } from 'react-use'
import { ContextMenuParams, ParamChange, ParamPanelContext } from './context'
import ParameterBuilder from './ParameterBuilder'
import ParameterContextMenu from './ParameterContextMenu'
import { LayoutParameters } from './ParameterItems'
//import { Item } from '@radix-ui/react-dropdown-menu'

export function ParameterPanel() {
  const node = useNodeGraphBridgeStore((state) => state.node)
  const [nodeData, setNodeData] = useState<NodeDataType | null>(null)
  const paramChange = useRef<ParamChange | undefined>(undefined)
  const mutex = useRef(new Mutex())

  const nodeDef = useRef<NodeDef | null>(null)
  const ref = useRef<HTMLDivElement>(null)
  const isValid = node !== null && node.id != 0 && nodeData !== null && nodeDef.current !== null
  const storeApi = useNodeGraphBridgeStoreApi()
  const [contextMenuParams, setContextMenuParams] = useState<ContextMenuParams | null>(null)

  const readonly = useMemo(() => {
    const node_graph = storeApi.getState().node_graph
    if (node_graph) {
      const node_space = node_graph.getState().node_space
      if (node_space) {
        return !isSubgraphOrNdaEditable(node_space.editable)
      }
    }
    return false
  }, [node])

  const updateNodeData = useCallback(
    async (node: Nullable<NodeData>) => {
      if (node) {
        const { graph_id, subgraph_id } = storeApi.getState()
        if (graph_id && subgraph_id) {
          const def = await getNodeDef([graph_id, node.type_name, node.version])

          // TODO: optimize this way out init root layout
          const root_layout_def = initDefRootLayout(def)

          const n = await getNode([graph_id, subgraph_id, node.id])
          def.children![0].children = mergeItemDefs(
            n.custom_defs?.children,
            def.children![0].children
          )

          await updateDefEnumList(root_layout_def[0], graph_id, node)

          setNodeData(n)
          nodeDef.current = def
          return
        }
      }
      setNodeData(null)
      nodeDef.current = null
    },
    [node, setNodeData]
  )

  useEffectOnce(() => {
    const unlisten1 = listen<string>('RemoveNodeEvent', (event) => {
      const current_node = storeApi.getState().node
      if (!current_node) return
      const node_id = parseInt(JSON.parse(event.payload).node_id)
      if (node_id == current_node.id) {
        setNode(null)
      }
    })

    const unlisten2 = listen<string>('ParamChangeEvent', (event: Event<string>) => {
      const current_node = storeApi.getState().node
      if (!current_node) return
      const { node_id, param_name, action_type } = JSON.parse(event.payload)

      // if change this node
      if (parseInt(node_id) == current_node.id) {
        mutex.current.runExclusive(() => {
          if (paramChange.current !== undefined) {
            // if change another param or another data
            if (
              param_name !== paramChange.current.name ||
              action_type != paramChange.current.action
            ) {
              paramChange.current.dirty = true
              return
            }
            // if change this param
            const dirty = paramChange.current.dirty
            paramChange.current = undefined
            if (!dirty) {
              return
            }
          }

          // rebuild panel
          updateNodeData(current_node).catch(console.error)
        })
      }
    })
    const unlisten3 = listen<string>('NodeChangeEvent', (event) => {
      const current_node = storeApi.getState().node
      if (!current_node) return

      // console.log('NodeChangeEvent', event)
      const { node_id } = JSON.parse(event.payload)

      if (current_node.id != parseInt(node_id)) return

      updateNodeData(current_node)
    })

    return () => {
      unlisten1.then((fn) => fn())
      unlisten2.then((fn) => fn())
      unlisten3.then((fn) => fn())
    }
  })

  useEffect(() => {
    updateNodeData(node).catch(console.error)
  }, [node, updateNodeData])

  const setParamChange = useCallback((change: ParamChange) => {
    mutex.current.runExclusive(() => {
      paramChange.current = change
    })
  }, [])

  const iconTheme = useMemo(() => (node !== null ? getIconTheme(node.category) : ''), [node])

  return (
    <If condition={isValid}>
      <Then>
        {() => {
          const NodeIcon = iconTypes[node!.type_name] || iconTypes.default
          return (
            <ParamPanelContext.Provider
              value={{
                node: node!,
                graph_id: storeApi.getState().graph_id!,
                subgraph_id: storeApi.getState().subgraph_id!,
                node_graph: storeApi.getState().node_graph!,
                node_def: nodeDef.current!,
                param_items: nodeData!.param_items,
                setChange: setParamChange,
                showContextMenu: (params) => {
                  const rect = ref.current!.getBoundingClientRect()
                  params.pos.x -= rect.left
                  params.pos.y -= rect.top
                  if (contextMenuParams !== null) {
                    setContextMenuParams(null)
                    setTimeout(() => {
                      setContextMenuParams(params)
                    }, 50)
                  } else {
                    setContextMenuParams(params)
                  }
                }
              }}
            >
              <div ref={ref} className="size-full overflow-auto pt-2 text-sm tracking-wide">
                <div className="mx-1 mb-2 flex h-5 select-none place-items-center gap-2 rounded bg-black/10 selection:bg-text-select/40">
                  <NodeIcon className="ml-1 size-4 p-0" style={{ color: iconTheme[1] }} />
                  <label className="opacity-90">{node?.label}</label>
                  <div className="mx-auto"></div>
                  <div
                    className="btn btn-ghost btn-xs size-5 min-h-0 p-0"
                    onClick={() => {
                      NiceModal.show(ParameterBuilder, {
                        node_id: node!.id,
                        node: node!,
                        node_def: nodeDef.current!,
                        node_data: nodeData!
                      })
                    }}
                  >
                    <TbSettingsBolt className="size-full p-0.5 opacity-80" />
                  </div>
                </div>
                <LayoutParameters
                  layout={{
                    def_name: 'root',
                    visible: true,
                    editable: true,
                    collapsed: false,
                    param_items: nodeData!.param_items
                  }}
                  readonly={readonly}
                />
              </div>

              <If condition={contextMenuParams !== null}>
                <Then>
                  {() => (
                    <ParameterContextMenu
                      name={contextMenuParams!.param_name}
                      param={contextMenuParams!.param}
                      paramDef={contextMenuParams!.param_def}
                      readonly={contextMenuParams!.readonly}
                      left={contextMenuParams!.pos.x}
                      top={contextMenuParams!.pos.y}
                      onAddExpression={contextMenuParams!.onAddExpression}
                      onClick={() => setContextMenuParams(null)}
                    />
                  )}
                </Then>
              </If>
            </ParamPanelContext.Provider>
          )
        }}
      </Then>
      <Else>{() => <Empty />}</Else>
    </If>
  )
}
