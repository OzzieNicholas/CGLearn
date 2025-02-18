import { getNode } from '@/commands'
import { ParamActionType } from '@/commands/definer'
import { ParamItemType, Parameter } from '@/commands/index.types'
import { useNodeGraphStoreApi } from '@/stores/node-graph'
import { NNode } from '@/types/node-graph'
import { rgbafToHexa } from '@/utils/functions'
import { findParamItem } from '@/utils/parameter'
import { listen } from '@tauri-apps/api/event'
import { NodeProps, ReactFlowState, useReactFlow, useStore } from '@xyflow/react'
import React, { memo, useCallback, useEffect, useRef, useState } from 'react'
import { shallow } from 'zustand/shallow'

const storeSelector = (state: ReactFlowState) => ({
  zoom: state.transform[2]
})

function BackdropNode({ id, data, selected }: NodeProps<NNode>) {
  const [params, setParams] = useState<{
    comment: string
    width: number
    height: number
    font_size: number
    font_color: string
    background_color: string
  } | null>(null)
  const { screenToFlowPosition, getIntersectingNodes, setNodes } = useReactFlow()
  const { zoom } = useStore(storeSelector, shallow)
  const storeApi = useNodeGraphStoreApi()

  const ref = useRef<HTMLDivElement>(null)
  const handleRef = useRef<HTMLDivElement>(null)
  const delta = useRef<{ x: number; y: number } | null>(null)
  const dirty = useRef<boolean>(false)

  const setParams_ = useCallback(
    (items: ParamItemType[]) => {
      const size = (findParamItem('Size', items) as Parameter).value as number[]
      setParams({
        comment: (findParamItem('Comment', items) as Parameter).value as string,
        width: size[0],
        height: size[1],
        font_size: (findParamItem('Font Size', items) as Parameter).value as number,
        font_color: rgbafToHexa(
          (findParamItem('Font Color', items) as Parameter).value as number[]
        ),
        background_color: rgbafToHexa(
          (findParamItem('Background Color', items) as Parameter).value as number[]
        )
      })
    },
    [setParams]
  )

  useEffect(() => {
    getNode([
      storeApi.getState().getNodeGraphId(),
      storeApi.getState().node_space.id,
      data.id
    ]).then((n) => {
      setParams_(n.param_items)
    })
  }, [data.id])

  useEffect(() => {
    const unlisten = listen<string>('ParamChangeEvent', (event) => {
      const { node_id, action_type } = JSON.parse(event.payload)
      if (action_type != ParamActionType.Value) {
        return
      }

      // if change this node
      if (parseInt(node_id) == data.id) {
        getNode([
          storeApi.getState().getNodeGraphId(),
          storeApi.getState().node_space.id,
          data.id
        ]).then((n) => {
          setParams_(n.param_items)
        })
      } else {
        unlisten.then((fn) => fn())
      }
    })
    return () => {
      unlisten.then((fn) => fn())
    }
  }, [data.label, selected])

  const selectIntersectingNodes = useCallback(() => {
    if (ref.current === null) return
    const rect = ref.current.getBoundingClientRect()
    const pos1 = screenToFlowPosition({ x: rect.left, y: rect.top })
    const pos2 = screenToFlowPosition({ x: rect.right, y: rect.bottom })
    let intersecting_nodes = new Set<string>()
    getIntersectingNodes(
      {
        x: pos1.x,
        y: pos1.y,
        width: pos2.x - pos1.x,
        height: pos2.y - pos1.y
      },
      true
    ).forEach((n) => {
      intersecting_nodes.add(n.id)
    })
    intersecting_nodes.add(id)

    if (intersecting_nodes.size > 0) {
      setNodes((nodes) => {
        return nodes.map((n) => {
          const v = intersecting_nodes.has(n.id)
          if (n.selected != v) {
            return { ...n, selected: v }
          }
          return n
        })
      })
    }
  }, [id])

  useEffect(() => {
    if (!selected) {
      setNodes((nodes) => {
        return nodes.map((n) => {
          if (n.selected) {
            return { ...n, selected: false }
          }
          return n
        })
      })
    }
  }, [selected])

  const onPointerDown = useCallback(
    (e: React.PointerEvent) => {
      if (e.buttons == 1 && ref.current !== null && handleRef.current !== null) {
        const rect = ref.current.getBoundingClientRect()
        delta.current = {
          x: (rect.right - e.clientX) / zoom,
          y: (rect.bottom - e.clientY) / zoom
        }
        handleRef.current.setPointerCapture(e.pointerId)
      }
    },
    [zoom]
  )

  const onPointerUp = useCallback(
    (e: React.PointerEvent) => {
      if (handleRef.current !== null) {
        handleRef.current.releasePointerCapture(e.pointerId)
        if (dirty.current) {
          dirty.current = false
          storeApi.getState().setNodeParam(data.id, 'Size', [params!.width, params!.height])
        }
      }
    },
    [params]
  )

  const onPointerMove = useCallback(
    (e: React.PointerEvent) => {
      e.preventDefault()
      e.stopPropagation()
      if (e.buttons == 1 && ref.current !== null && delta.current !== null) {
        const rect = ref.current.getBoundingClientRect()
        const width = Math.max((e.clientX - rect.left) / zoom + delta.current.x, 200)
        const height = Math.max((e.clientY - rect.top) / zoom + delta.current.y, 200)
        setParams((pre) => {
          if (width == pre!.width && height == pre!.height) {
            return pre
          }
          dirty.current = true
          return {
            ...pre!,
            width,
            height
          }
        })
      }
    },
    [setParams, zoom]
  )

  if (!params) return undefined

  return (
    <>
      <div
        className="h-4 rounded-t-lg border-1 border-b-0 backdrop-blur-sm"
        style={{
          width: params.width,
          borderColor: selected ? '#FACC15' : '#ffffff33',
          backgroundColor:
            params.background_color.length <= 7
              ? params.background_color
              : params.background_color.substring(0, 7)
        }}
        onPointerDown={selectIntersectingNodes}
      />
      <div
        ref={ref}
        className="nodrag nopan pointer-events-none absolute rounded-b-lg border-1 border-t-0 p-1"
        style={{
          width: params.width,
          height: params.height,
          borderColor: selected ? '#FACC15' : '#ffffff33',
          fontSize: params.font_size,
          color: params.font_color,
          backgroundColor: params.background_color
        }}
      >
        <div className=" size-full overflow-hidden whitespace-pre-wrap break-words leading-tight">
          {params.comment}
        </div>
        <div
          ref={handleRef}
          className="pointer-events-auto absolute bottom-0 right-0 size-8 cursor-nwse-resize"
          onPointerDown={onPointerDown}
          onPointerUp={onPointerUp}
          onPointerMove={onPointerMove}
        />
      </div>
    </>
  )
}

export default memo(BackdropNode)
