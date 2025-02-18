import { getNode, getNodeDef } from '@/commands'
import { NodeData as NodeDataType, NodeDef } from '@/commands/index.types'
import { useNodeGraphStore, useNodeGraphStoreApi } from '@/stores/node-graph'
import { NodeData, NodeGraphState } from '@/types/node-graph'
import {
  initDefRootLayout,
  isSubgraphOrNdaEditable,
  mergeItemDefs,
  updateDefEnumList
} from '@/utils/node-graph'
import { listen } from '@tauri-apps/api/event'
import { Mutex } from 'async-mutex'
import { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { Else, If, Then } from 'react-if'
import { ContextMenuParams, ParamChange, ParamPanelContext } from './context'
import ParameterContextMenu from './ParameterContextMenu'
import { LayoutParameters } from './ParameterItems'

const storeSelector = (state: NodeGraphState) => ({
  graph_id: state.getNodeGraphId(),
  subgraph_id: state.node_space.id
})

const setHexColorOpacity = (hex: string, opacity: number) => {
  const r = parseInt(hex.slice(1, 3), 16)
  const g = parseInt(hex.slice(3, 5), 16)
  const b = parseInt(hex.slice(5, 7), 16)
  return `rgba(${r}, ${g}, ${b}, ${opacity})`
}

export function NodeParameters({ node }: { node: NodeData }) {
  const [nodeData, setNodeData] = useState<NodeDataType | null>(null)
  const paramChange = useRef<ParamChange | undefined>(undefined)
  const mutex = useRef(new Mutex())
  const { graph_id, subgraph_id } = useNodeGraphStore(storeSelector)
  const storeApi = useNodeGraphStoreApi()
  const [contextMenuParams, setContextMenuParams] = useState<ContextMenuParams | null>(null)

  const nodeDef = useRef<NodeDef | null>(null)
  const isValid = node.id != 0 && nodeData !== null && nodeDef.current !== null
  const ref = useRef<HTMLDivElement>(null)

  const readonly = useMemo(() => {
    const node_space = storeApi.getState().node_space
    if (node_space) {
      return !isSubgraphOrNdaEditable(node_space.editable)
    }
    return false
  }, [node])

  useEffect(() => {
    const unlisten = listen<string>('ParamChangeEvent', (event) => {
      if (!isValid) return
      const { node_id, param_name, action_type } = JSON.parse(event.payload)

      // if change this node
      if (parseInt(node_id) == node.id) {
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
          updateNodeData(graph_id, subgraph_id, node)
        })
      }
    })

    return () => {
      unlisten.then((fn) => fn())
    }
  }, [node])

  const updateNodeData = useCallback(
    async (graph_id: number, subgraph_id: number, node: NodeData) => {
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
      }
    },
    [node, setNodeData]
  )

  useEffect(() => {
    if (node) {
      updateNodeData(graph_id, subgraph_id, node)
    }
  }, [node.id])

  const setParamChange = useCallback((change: ParamChange) => {
    mutex.current.runExclusive(() => {
      paramChange.current = change
    })
  }, [])

  return (
    <If condition={isValid}>
      <Then>
        {() => (
          <ParamPanelContext.Provider
            value={{
              node: node!,
              graph_id: graph_id,
              subgraph_id: subgraph_id,
              node_graph: storeApi,
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
            <div
              ref={ref}
              className="mt-1 max-h-96 max-w-lg overflow-auto rounded-lg p-2 text-sm tracking-wide shadow-lg"
              style={{
                backgroundColor: setHexColorOpacity(node.color, 0.15)
              }}
              // onMouseDown={(e) => e.stopPropagation()}
              onContextMenu={(e) => e.stopPropagation()}
            >
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
                    onClick={() => setTimeout(() => setContextMenuParams(null), 50)}
                  />
                )}
              </Then>
            </If>
          </ParamPanelContext.Provider>
        )}
      </Then>
      <Else>{() => <div />}</Else>
    </If>
  )
}
