import { getRampMethods, getRampView } from '@/commands'
import { ColorRampValue, ColorRampView } from '@/commands/index.types'
import { cn } from '@/utils/cn'
import { hexToRgbaf, isNumberSame, rgbafToHexa } from '@/utils/functions'
import { isColorRampSame } from '@/utils/parameter'
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { useDebounce, useEffectOnce } from 'react-use'
import ColorPicker from './ColorPicker'
import Select from './Select'
import { Slider, SliderValueType } from './Slider'

let methods: string[] = []
let default_stops: any = []
getRampMethods().then((v) => {
  methods = v
  default_stops = [
    { pos: 0, color: '#fff', method: methods[0], selected: true },
    { pos: 1, color: '#000', method: methods[0] }
  ]
})
const sample_num = 100

const convertRamp = (ramp?: ColorRampValue[]) => {
  if (!ramp || ramp.length == 0) return undefined
  return ramp.map((v, idx) => ({
    pos: v.pos,
    method: v.method,
    color: rgbafToHexa(v.value),
    selected: idx == 0
  }))
}

const convertToRamp = (ramp: { pos: number; color: string; method: string }[]) => {
  return [...ramp]
    .sort((a, b) => a.pos - b.pos)
    .map((v) => ({
      pos: v.pos,
      method: v.method,
      value: hexToRgbaf(v.color)
    }))
}

const ColorStop = ({
  color,
  pos,
  width,
  selected,
  handlePressed
}: {
  color: string
  pos: number
  width: number
  handlePressed: () => void
  selected?: boolean
}) => {
  return (
    <div
      onMouseMove={(e) => e.preventDefault()}
      onMouseDown={handlePressed}
      onContextMenu={(e) => {
        e.stopPropagation()
        e.preventDefault()
      }}
      className={cn(
        'rounded-full size-[20px] border-2 bottom-0 cursor-pointer brightness-90',
        selected ? 'border-yellow-200' : 'border-white/80'
      )}
      style={{
        position: 'absolute',
        left: `${pos * width - 10}px`,
        backgroundColor: color,
        zIndex: selected ? '20' : '10'
      }}
    />
  )
}

export default function ColorRamp({
  onChange,
  defaultValue,
  value,
  onEditingFinished,
  className
}: {
  onChange: (value: ColorRampValue[]) => void
  defaultValue?: ColorRampValue[]
  value?: ColorRampValue[]
  onEditingFinished?: (old_value: ColorRampValue[], new_value: ColorRampValue[]) => void
  className?: string
}) {
  const ref = useRef<HTMLDivElement>(null)
  const canvasRef = useRef<HTMLCanvasElement>(null!)
  const [width, setWidth] = useState(0)
  const [colorStops, setColorStops] = useState(
    convertRamp(value) || convertRamp(defaultValue) || [...default_stops]
  )

  const dragging = useRef(-1)
  const oldValueRef = useRef(0)

  useEffectOnce(() => {
    if (!ref.current) return
    const resizeObserver = new ResizeObserver(() => {
      if (ref.current) {
        setWidth(ref.current.clientWidth)
      }
    })
    resizeObserver.observe(ref.current)
    return () => resizeObserver.disconnect() // clean up
  })

  useEffect(() => {
    if (value !== undefined && dragging.current < 0) {
      const selected = colorStops.find((stop) => stop.selected)
      let new_stops = convertRamp(value) || [...default_stops]
      if (selected !== undefined) {
        let idx = 0
        for (let stop of new_stops) {
          if (Math.abs(stop.pos - selected.pos) < 1e-3) {
            stop.selected = true
            if (idx !== 0) {
              delete new_stops[0].selected
            }
            break
          }
          ++idx
        }
      }
      setColorStops(new_stops)
    }
  }, [value])

  const [,] = useDebounce(
    () => {
      if (onChange) {
        const new_ramp = convertToRamp(colorStops)
        if (value) {
          if (!isColorRampSame(new_ramp, value)) {
            onChange(new_ramp)
          }
        } else {
          onChange(new_ramp)
        }
      }
    },
    10,
    [colorStops, onChange]
  )

  const updateColorStops = useCallback(
    (new_stops: { pos: number; color: string; method: string }[], finised?: boolean) => {
      const old_stops = finised && onEditingFinished !== undefined ? [...colorStops] : undefined
      setColorStops(new_stops)
      if (finised && onEditingFinished !== undefined) {
        onEditingFinished(convertToRamp(old_stops!), convertToRamp(new_stops))
      }
    },
    [value, colorStops, onEditingFinished]
  )

  const updateSelectedColorStop = useCallback(
    (pos?: number, color?: string, method?: string, finished?: boolean) => {
      updateColorStops(
        colorStops.map((stop) =>
          stop.selected
            ? {
                ...stop,
                pos: pos !== undefined ? pos : stop.pos,
                color: color !== undefined ? color : stop.color,
                method: method !== undefined ? method : stop.method
              }
            : stop
        ),
        finished
      )
    },
    [colorStops, updateColorStops]
  )

  const selectedStop = useMemo(() => {
    return colorStops.find((e) => e.selected) || colorStops[0]
  }, [colorStops])

  const selecteColorStop = useCallback(
    (idx: number) => {
      setColorStops(
        colorStops.map((stop, i) =>
          idx === i
            ? {
                ...stop,
                selected: true
              }
            : stop.selected
              ? {
                  pos: stop.pos,
                  color: stop.color,
                  method: stop.method
                }
              : stop
        )
      )
    },
    [colorStops]
  )

  const onCanvas = (canvas: HTMLCanvasElement) => {
    if (!canvas || !ref.current) return
    canvasRef.current = canvas
    const context = canvas.getContext('2d')
    if (!context) return
    const height = ref.current.clientHeight

    if (width < 1e-6) return
    const gradient = context.createLinearGradient(0, 0, width, 0)

    if (colorStops.every((stop) => stop.method === methods[0])) {
      colorStops.forEach((stop) => {
        gradient.addColorStop(Math.max(Math.min(stop.pos, 1.0), 0), stop.color)
      })

      context.fillStyle = gradient
      context.fillRect(0, 0, width, height)
    } else {
      getRampView([convertToRamp(colorStops), sample_num]).then((view) => {
        ;(view as ColorRampView[]).forEach((v) => {
          gradient.addColorStop(Math.max(Math.min(v.x, 1.0), 0), rgbafToHexa(v.color))
        })
        context.fillStyle = gradient
        context.fillRect(0, 0, width, height)
      })
    }
  }

  const EditingFinished = onEditingFinished
    ? (old_value: number | string) => {
        let old_stops = undefined
        if (typeof old_value === 'number') {
          old_stops = colorStops.map((stop) =>
            stop.selected
              ? {
                  ...stop,
                  pos: old_value
                }
              : stop
          )
        } else {
          old_stops = colorStops.map((stop) =>
            stop.selected
              ? {
                  ...stop,
                  color: old_value
                }
              : stop
          )
        }

        onEditingFinished(convertToRamp(old_stops), convertToRamp(colorStops))
      }
    : undefined

  const handleMouseUp = useCallback(
    (event: React.MouseEvent) => {
      if (dragging.current >= 0) {
        const stop = colorStops[dragging.current]
        if (isNumberSame(stop.pos, oldValueRef.current)) {
          if (event.button == 2 && colorStops.length > 1) {
            const selected = colorStops[dragging.current].selected
            const new_stopes = colorStops.filter((_, i) => i !== dragging.current)
            if (selected) new_stopes[0].selected = true
            updateColorStops(new_stopes, true)
          }
          event.preventDefault()
        } else if (EditingFinished) {
          EditingFinished(oldValueRef.current)
        }
        dragging.current = -1
        event.stopPropagation()
      }
    },
    [updateColorStops, colorStops]
  )

  const handleMouseMove = (event: React.MouseEvent) => {
    if (dragging.current >= 0 && ref.current && event.buttons !== 0) {
      const rect = ref.current.getBoundingClientRect()
      let new_pos = (event.clientX - rect.left) / width
      if (new_pos < 0) new_pos = 0
      else if (new_pos > 1) new_pos = 1
      if (!isNumberSame(new_pos, colorStops[dragging.current].pos)) {
        updateColorStops(
          colorStops.map((stop, i) => (i === dragging.current ? { ...stop, pos: new_pos } : stop))
        )
      }
      event.stopPropagation()
    }
  }

  const handleColorStopPressed = useCallback(
    (idx: number) => {
      if (dragging.current < 0) {
        oldValueRef.current = colorStops[idx].pos
        selecteColorStop(idx)
        dragging.current = idx
      }
    },
    [colorStops]
  )

  return (
    <div
      className={cn('flex w-full gap-4', className)}
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onMouseLeave={handleMouseUp}
    >
      <div ref={ref} style={{ position: 'relative' }} className="m-0 h-12 grow rounded p-0">
        {colorStops.map((stop, idx) => (
          <ColorStop
            key={idx}
            color={stop.color}
            pos={stop.pos}
            width={width}
            selected={stop.selected}
            handlePressed={() => handleColorStopPressed(idx)}
          />
        ))}
        <canvas
          className="m-0 h-12 w-full rounded p-0"
          width={ref.current?.offsetWidth}
          height={ref.current?.offsetHeight}
          ref={onCanvas}
          onContextMenu={(e) => {
            e.preventDefault()
            e.stopPropagation()
          }}
          onDoubleClick={(e) => {
            e.preventDefault()
            e.stopPropagation()
            const rect = e.currentTarget.getBoundingClientRect()
            const x = e.clientX - rect.left
            const pos = x / width

            const old_color_stops = onEditingFinished ? convertToRamp(colorStops) : undefined
            const new_color_stops = [
              ...colorStops.map((stop) => {
                return stop.selected
                  ? {
                      pos: stop.pos,
                      color: stop.color,
                      method: stop.method
                    }
                  : stop
              }),
              { pos, color: '#fff', method: colorStops[0].method, selected: true }
            ]
            updateColorStops(new_color_stops)

            if (old_color_stops) {
              onEditingFinished!(old_color_stops, convertToRamp(new_color_stops))
            }
          }}
        />
      </div>
      <div className="flex flex-col gap-1">
        <div className="flex flex-row gap-1">
          <Slider
            valueType={SliderValueType.Float}
            value={selectedStop.pos}
            rangeMin={0}
            rangeMax={1}
            limitMin={0}
            limitMax={1}
            onValueChange={(v) => {
              updateSelectedColorStop(v, undefined, undefined)
            }}
            onEditingFinished={EditingFinished}
            className="w-20"
          />
          <Select
            className="select-accent bg-white/20 text-xs"
            onChange={(v) => updateSelectedColorStop(undefined, undefined, v, true)}
            value={selectedStop.method}
            options={methods}
          />
        </div>

        <ColorPicker
          placement="bottom"
          color={selectedStop.color}
          onChange={(c) => {
            updateSelectedColorStop(undefined, c.hex)
          }}
          onEditingFinished={EditingFinished}
        >
          <button
            className="btn btn-ghost h-5 min-h-0 w-full rounded-md border-1 border-black/50"
            style={{
              backgroundColor: selectedStop.color
            }}
          />
        </ColorPicker>
      </div>
    </div>
  )
}
