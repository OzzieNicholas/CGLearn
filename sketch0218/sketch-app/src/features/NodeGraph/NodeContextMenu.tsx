import RecursiveMenu from '@/components/RecursiveMenu'
import { useNodeGraphStoreApi } from '@/stores/node-graph'
import { NodeKind } from '@/types/node-def'
import { NNode } from '@/types/node-graph'
import { handleIndex, isSubgraphOrNda, isSubgraphOrNdaEditable } from '@/utils/node-graph'
import uuid from '@/utils/uuid'
import NiceModal from '@ebay/nice-modal-react'
import { writeText as clipboardWrite } from '@tauri-apps/plugin-clipboard-manager'
import { getConnectedEdges, useReactFlow } from '@xyflow/react'
import { produce } from 'immer'
import { memo, useMemo } from 'react'
import NodePublisher from './NodePublisher'

const NodeContextMenu = ({
  node,
  top,
  left,
  right,
  bottom,
  close
}: {
  node?: NNode
  top?: number
  left?: number
  right?: number
  bottom?: number
  close: () => void
}) => {
  if (node === undefined) {
    return <></>
  }
  const { deleteElements } = useReactFlow()
  const storeApi = useNodeGraphStoreApi()

  const menus = useMemo(
    () => [
      {
        label: 'menu.extract',
        action: async () => {
          const connected_edges = getConnectedEdges([node], storeApi.getState().edges)
          if (connected_edges.length == 0) {
            return
          }

          let new_edges = []

          // begin undo group
          storeApi.setState(
            produce((state) => {
              state.undo_redo.beginGroup('extract node')
            })
          )

          if (connected_edges.length > 1) {
            for (const edge of connected_edges) {
              if (edge.target === node.id) {
                const handle_idx = handleIndex(edge.targetHandle)
                const pair = connected_edges.find(
                  (e) => e.source === node.id && handleIndex(e.sourceHandle) == handle_idx
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

          await deleteElements({ edges: connected_edges })

          if (new_edges.length > 0) {
            await storeApi.getState().addEdges(new_edges)
          }

          // end undo group
          storeApi.setState(
            produce((state) => {
              state.undo_redo.endGroup()
            })
          )
        }
      },
      ...(isSubgraphOrNda(node.data.kind)
        ? [
            {
              label: '',
              divider: true
            },
            {
              label: 'menu.publish-node',
              action: () => {
                NiceModal.show(NodePublisher, { node, storeApi })
              }
            },
            ...(node.data.kind === NodeKind.Nda
              ? [
                  isSubgraphOrNdaEditable(node.data.editable)
                    ? {
                        label: 'menu.reload-nda',
                        action: () => {
                          storeApi.getState().setNodeEditable(node.data.id, false)
                        }
                      }
                    : {
                        label: 'menu.unlock-nda',
                        action: () => {
                          storeApi.getState().setNodeEditable(node.data.id, true)
                        }
                      }
                ]
              : []),
            {
              label: '',
              divider: true
            }
          ]
        : []),
      {
        label: 'menu.execute',
        action: () => {
          storeApi.getState().executeToNode(node.data.id)
        }
      },
      {
        label: 'menu.copy-path',
        action: () => {
          storeApi
            .getState()
            .getNodePath(node)
            .then((path) => {
              clipboardWrite(path)
            })
        }
      },
      {
        label: 'common.delete',
        action: () => {
          deleteElements({ nodes: [{ id: node.id }] })
        }
      }
    ],
    [node]
  )

  return (
    <div style={{ top, left, right, bottom }} className="absolute z-10 w-48" onClick={close}>
      <RecursiveMenu menus={menus} defaultOpen={true} asChild className="w-48 animate-fade-in">
        <span className="m-0 h-0 p-0 text-[0]"></span>
      </RecursiveMenu>
    </div>
  )
}

export default memo(NodeContextMenu)
