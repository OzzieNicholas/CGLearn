import {
  ConnectionLineComponentProps,
  getStraightPath,
  getBezierPath,
  getSmoothStepPath
} from '@xyflow/react'
import { SettingsState, useSettingsStore } from '@/stores/settings'
import { EdgeStyle } from '@/types/node-graph'

const edgeStyleStoreSelector = (state: SettingsState) => state.node_graph.edge_style

export default function ConnectionLine({
  fromX,
  fromY,

  fromPosition,
  toX,
  toY,
  toPosition
}: ConnectionLineComponentProps) {
  const edgeStyle = useSettingsStore(edgeStyleStoreSelector)

  let edgePath: string = ''

  switch (edgeStyle) {
    case EdgeStyle.Straight: {
      ;[edgePath] = getStraightPath({
        sourceX: fromX,
        sourceY: fromY,
        targetX: toX,
        targetY: toY
      })
      break
    }
    case EdgeStyle.Bezier: {
      ;[edgePath] = getBezierPath({
        sourceX: fromX,
        sourceY: fromY,
        sourcePosition: fromPosition,
        targetX: toX,
        targetY: toY,
        targetPosition: toPosition
      })
      break
    }
    case EdgeStyle.Step: {
      ;[edgePath] = getSmoothStepPath({
        sourceX: fromX,
        sourceY: fromY,
        sourcePosition: fromPosition,
        targetX: toX,
        targetY: toY,
        targetPosition: toPosition,
        borderRadius: 0
      })
      break
    }
    case EdgeStyle.SmoothStep: {
      ;[edgePath] = getSmoothStepPath({
        sourceX: fromX,
        sourceY: fromY,
        sourcePosition: fromPosition,
        targetX: toX,
        targetY: toY,
        targetPosition: toPosition,
        borderRadius: 120
      })
      break
    }
  }

  return (
    <g>
      <path
        fill="none"
        stroke={'rgb(250 204 21)'}
        strokeWidth={1.5}
        className="animated"
        d={edgePath}
      />
      <circle cx={toX} cy={toY} fill="#fff" r={2} stroke={'rgb(250 204 21)'} strokeWidth={1.5} />
    </g>
  )
}
