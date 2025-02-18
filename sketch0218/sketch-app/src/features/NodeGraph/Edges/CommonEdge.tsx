import { Edge, EdgeStyle } from '@/types/node-graph'
import { Transition } from '@headlessui/react'
import {
  BaseEdge,
  EdgeLabelRenderer,
  EdgeProps,
  useReactFlow,
  getStraightPath,
  getBezierPath,
  getSmoothStepPath
  // ReactFlowState,
  // useStore
} from '@xyflow/react'
import React, { useCallback, useContext, useMemo } from 'react'
import { InteractiveContext } from '../context'
import { SettingsState, useSettingsStore } from '@/stores/settings'
// import { shallow } from 'zustand/shallow'

import { IoMdAddCircle } from 'react-icons/io'
import { TiDelete } from 'react-icons/ti'

// const storeSelector = (state: ReactFlowState) => ({
//   zoom: state.transform[2]
// })

const edgeStyleStoreSelector = (state: SettingsState) => state.node_graph.edge_style

export default function CustomEdge(edge: EdgeProps<Edge>) {
  const {
    id,
    sourceX,
    sourceY,
    targetX,
    targetY,
    sourcePosition,
    targetPosition,
    style = {},
    markerEnd,
    data
  } = edge
  const { showContextMenu, hideElementToolbar } = useContext(InteractiveContext)
  const { deleteElements, flowToScreenPosition } = useReactFlow()
  const edgeStyle = useSettingsStore(edgeStyleStoreSelector)
  // const { zoom } = useStore(storeSelector, shallow)
  const zoom = 1.0

  let edgePath: string = ''
  let labelX = 0
  let labelY = 0

  switch (edgeStyle) {
    case EdgeStyle.Straight: {
      ;[edgePath, labelX, labelY] = getStraightPath({
        sourceX,
        sourceY,
        targetX,
        targetY
      })
      break
    }
    case EdgeStyle.Bezier: {
      ;[edgePath, labelX, labelY] = getBezierPath({
        sourceX,
        sourceY,
        sourcePosition,
        targetX,
        targetY,
        targetPosition
      })
      break
    }
    case EdgeStyle.Step: {
      ;[edgePath, labelX, labelY] = getSmoothStepPath({
        sourceX,
        sourceY,
        sourcePosition,
        targetX,
        targetY,
        targetPosition,
        borderRadius: 0
      })
      break
    }
    case EdgeStyle.SmoothStep: {
      ;[edgePath, labelX, labelY] = getSmoothStepPath({
        sourceX,
        sourceY,
        sourcePosition,
        targetX,
        targetY,
        targetPosition,
        borderRadius: 120
      })
      break
    }
  }

  const onDeleteEdge = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault()
      e.stopPropagation()
      deleteElements({ edges: [{ id }] })
    },
    [id]
  )

  const onAddNode = (e: React.MouseEvent) => {
    e.preventDefault()
    e.stopPropagation()
    const node_pos = {
      x: (sourceX + targetX) / 2 - 100,
      y: (sourceY + targetY) / 2
    }
    const menu_pos = flowToScreenPosition(node_pos)
    showContextMenu(menu_pos, node_pos, undefined, {
      ...edge,
      sourceHandle: edge.sourceHandleId,
      targetHandle: edge.targetHandleId
    })
  }
  // console.log('render egde ', id)
  const size = useMemo(() => `${1.5 / zoom}rem`, [zoom])

  const showToolbar = useMemo(() => {
    return !hideElementToolbar && data?.hovered === true
  }, [data?.hovered])

  return (
    <>
      <BaseEdge path={edgePath} markerEnd={markerEnd} style={style} />
      <EdgeLabelRenderer>
        <Transition
          show={showToolbar}
          enter="transition-opacity duration-100"
          enterFrom="opacity-0"
          enterTo="opacity-100"
          leave="transition-opacity duration-150"
          leaveFrom="opacity-100"
          leaveTo="opacity-0"
        >
          <div
            style={{
              position: 'absolute',
              transform: `translate(-50%, -50%) translate(${labelX}px,${labelY}px)`,

              // everything inside EdgeLabelRenderer has no pointer events by default
              // if you have an interactive element, set pointer-events: all
              pointerEvents: 'all'
            }}
            className="nodrag nopan"
          >
            <div
              className="flex"
              style={{
                gap: size
              }}
            >
              <div
                className="btn btn-circle btn-ghost m-0 min-h-0 p-0 text-white/40 hover:text-white/80"
                style={{
                  height: size,
                  width: size
                }}
                onClick={onAddNode}
              >
                <IoMdAddCircle className="m-0 size-full p-0" />
              </div>
              <div
                className=" btn btn-circle btn-ghost m-0 min-h-0 p-0 text-white/40 hover:text-white/80"
                style={{
                  height: size,
                  width: size
                }}
                onClick={onDeleteEdge}
              >
                <TiDelete
                  className="m-0 size-full p-0"
                  style={{
                    scale: '120%'
                  }}
                />
              </div>
            </div>
          </div>
        </Transition>
      </EdgeLabelRenderer>
    </>
  )
}
