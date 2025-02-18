import {
  renderFrame,
  resizeViewport,
} from '@/commands'
import { listen } from '@tauri-apps/api/event'
import { Mutex } from 'async-mutex'
import React, { useCallback, useRef, useState } from 'react'
import { useDebounce, useEffectOnce } from 'react-use'
import { ViewportToolBar } from './Toolbar'
import { useGlobalVariablesStoreApi } from '@/stores/global-variables'
import { isWindows } from '@/utils/env'
import { invoke } from '@tauri-apps/api/core'

const rendererMutex = new Mutex()

type ViewportSize = {
  width: number
  height: number
}

export function Viewport() {
  const window = useRef<HTMLDivElement>(null)
  const canvas = useRef<HTMLCanvasElement>(null)
  const [viewportSize, setViewportSize] = useState<ViewportSize>({
    width: 0,
    height: 0
  })
  const viewportSizeRef = useRef<ViewportSize>({
    width: 0,
    height: 0
  })

  const onSharedBufferReceived = useCallback((e: any) => {
    if (!window.current || !canvas.current) return

    const buffer: ArrayBuffer = e.getBuffer()

    if (buffer.byteLength !== Math.ceil(viewportSizeRef.current.width / 256.0) * 256 * viewportSizeRef.current.height * 4) {
      // when resizing not finished
      console.error('buffer size not match')
      return
    }

    const ctx = canvas.current.getContext('2d', { colorSpace: 'srgb' })
    if (ctx && buffer) {
      const clamped_array = new Uint8ClampedArray(buffer)
      const width = buffer.byteLength / (viewportSizeRef.current.height * 4)
      const height = viewportSizeRef.current.height

      const img_data = new ImageData(clamped_array, width, height)
      ctx.putImageData(img_data, 0, 0)
    }
  }, [])

  const onResize = useCallback(() => {
    if (!window.current) return
    if (window.current.clientWidth < 2 || window.current.clientHeight < 2) return
    viewportSizeRef.current = {
      width: window.current.clientWidth,
      height: window.current.clientHeight
    }
    invoke('sketch_core_tick')
    setViewportSize(viewportSizeRef.current)
  }, [setViewportSize])

  const [,] = useDebounce(
    () => {
      return rendererMutex.runExclusive(() => {
        resizeViewport(viewportSizeRef.current.width, viewportSizeRef.current.height)
      })
    },
    300,
    [viewportSize]
  )

  useEffectOnce(() => {
    // setParallelExecutionNodes([
    //   useSettingsStoreApi().getState().node_graph.parallel_execution_nodes
    // ])
    useGlobalVariablesStoreApi().setState(() => ({ viewport_visible: true }))

    // const unlisten1 = useSettingsStore.subscribe(
    //   (state) => state.node_graph.parallel_execution_nodes,
    //   (parallel_execution_nodes) => setParallelExecutionNodes([parallel_execution_nodes])
    // )

    if (isWindows()) {
      // @ts-ignore:next-line
      chrome.webview.addEventListener('sharedbufferreceived', onSharedBufferReceived)
    }

    const unlisten2 = listen('frame', () => {
      if (rendererMutex.isLocked()) return
      rendererMutex.runExclusive(renderFrame)
    })

    onResize()

    let resize_observer: ResizeObserver | null = null
    if (window.current) {
      resize_observer = new ResizeObserver(() => {
        onResize()
      })
      resize_observer.observe(window.current)
    }
    return () => {
      useGlobalVariablesStoreApi().setState(() => ({ viewport_visible: false }))
      if (isWindows()) {
        // @ts-ignore:next-line
        chrome.webview.removeEventListener('sharedbufferreceived', onSharedBufferReceived)
      }
      if (resize_observer !== null) {
        resize_observer.disconnect()
      }
      // unlisten1()
      unlisten2.then((fn) => fn())
    }
  })

  const onMouseDown = useCallback((e: React.MouseEvent<HTMLCanvasElement>) => {
    e.preventDefault()
    const x = e.nativeEvent.offsetX
    const y = viewportSizeRef.current.height - e.nativeEvent.offsetY
    invoke("set_mouse_button_down_event", { x: x, y: y, button: e.button, ctrl: e.nativeEvent.ctrlKey, alt: e.altKey, shift: e.shiftKey })
    invoke('sketch_core_tick')
  }, [])

  const onMouseUp = useCallback((e: React.MouseEvent<HTMLCanvasElement>) => {
    e.preventDefault()
    const x = e.nativeEvent.offsetX
    const y = viewportSizeRef.current.height - e.nativeEvent.offsetY
    invoke("set_mouse_button_up_event", { x: x, y: y, button: e.button, ctrl: e.nativeEvent.ctrlKey, alt: e.altKey, shift: e.shiftKey })
    invoke('sketch_core_tick')
  }, [])

  const onMouseMove = useCallback((e: React.MouseEvent<HTMLCanvasElement>) => {
    e.preventDefault()
    const x = e.nativeEvent.offsetX
    const y = viewportSizeRef.current.height - e.nativeEvent.offsetY
    invoke("set_mouse_move_event", { x: x, y: y, button: e.button, ctrl: e.nativeEvent.ctrlKey, alt: e.altKey, shift: e.shiftKey })
    invoke('sketch_core_tick')
  }, [])

  return (
    <div ref={window} className="z-10 m-0 size-full p-0">
      <ViewportToolBar className="absolute right-1 top-1" />
      <canvas
        ref={canvas}
        className="z-10 m-0 p-0"
        onMouseMove={onMouseMove}
        onMouseDown={onMouseDown}
        onMouseUp={onMouseUp}
        width={viewportSize.width}
        height={viewportSize.height}
        onContextMenu={(e) => e.preventDefault()}
      />
    </div>
  )
}
