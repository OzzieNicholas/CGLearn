import { getRampMethods, getRampView } from '@/commands'
import { ScalarRampValue, ScalarRampView } from '@/commands/index.types'
import { cn } from '@/utils/cn'
import { isNumberSame } from '@/utils/functions'
import { isScalarRampSame } from '@/utils/parameter'
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { useDebounce, useEffectOnce } from 'react-use'
import Select from './Select'
import { Slider, SliderValueType } from './Slider'

type Point = ScalarRampValue

let methods: string[] = []
let default_points: Point[] = []
getRampMethods().then((v) => {
  methods = v
  default_points = [
    { pos: 0, value: 0, method: methods[0] },
    { pos: 1, value: 1, method: methods[0] }
  ]
})

const sample_num = 500
const bg_rows = 10
const bg_cols = 5

const isSamePoint = (a: Point, b: Point) => {
  return isNumberSame(a.pos, b.pos) && isNumberSame(a.value, b.value) && a.method === b.method
}

const sortRamp = (ramp: ScalarRampValue[]) => {
  return [...ramp].sort((a, b) => a.pos - b.pos)
}

function SvgPoint({
  point,
  onMouseDown,
  onMouseUp,
  width,
  height,
  selected
}: {
  point: Point
  onMouseDown: () => void
  onMouseUp: (e: React.MouseEvent) => void
  width: number
  height: number
  selected: boolean
}) {
  return (
    <circle
      cx={point.pos * width}
      cy={height - point.value * height}
      r={6}
      onMouseDown={onMouseDown}
      onMouseUp={onMouseUp}
      fill={selected ? 'yellow' : '#00BFA5'}
    />
  )
}

function SvgLine({ points, width, height }: { points: Point[]; width: number; height: number }) {
  const [view, setView] = useState<{ pos: number; value: number }[]>(points)

  useEffect(() => {
    const sorted = sortRamp(points)
    if (!sorted.every((point) => point.method === methods[0])) {
      getRampView([sorted, sample_num]).then((ramp_view) => {
        setView([
          {
            pos: -1,
            value: sorted[0].value
          },
          ...(ramp_view as ScalarRampView[]).map((point) => ({ pos: point.x, value: point.y })),
          {
            pos: 2,
            value: sorted[sorted.length - 1].value
          }
        ])
      })
    } else {
      setView([
        {
          pos: -1,
          value: sorted[0].value
        },
        ...sorted,
        {
          pos: 2,
          value: sorted[sorted.length - 1].value
        }
      ])
    }
  }, [points])

  const pathData =
    'M ' + view.map((point) => `${point.pos * width} ${height - point.value * height}`).join(' L ')
  return <path d={pathData} fill="none" stroke="white" strokeWidth={1} opacity={0.8} />
}

function SvgBackground({ width, height }: { width: number; height: number }) {
  return Array.from(Array(bg_rows).keys())
    .map((i) => (
      <path
        key={i}
        d={`M ${(width * i) / bg_rows} ${-1} L ${(width * i) / bg_rows} ${height + 1}`}
        fill="none"
        stroke="white"
        strokeWidth={1}
        opacity={0.1}
      />
    ))
    .concat(
      Array.from(Array(bg_cols).keys()).map((i) => (
        <path
          key={bg_rows + i}
          d={`M ${-1} ${(height * i) / bg_cols} L ${width + 1} ${(height * i) / bg_cols}`}
          fill="none"
          stroke="white"
          strokeWidth={1}
          opacity={0.1}
        />
      ))
    )

  //return <path d={pathData} fill="none" stroke="white" strokeWidth={1} opacity={0.8} />
}

export default function ScalarRamp({
  onChange,
  defaultValue,
  value,
  className,
  onEditingFinished
}: {
  onChange: (value: ScalarRampValue[]) => void
  defaultValue?: ScalarRampValue[]
  value?: ScalarRampValue[]
  className?: string
  onEditingFinished?: (old_value: ScalarRampValue[], new_value: ScalarRampValue[]) => void
}) {
  const [points, setPoints] = useState(value || defaultValue || default_points)
  const [width, setWidth] = useState(0)
  const [height, setHeight] = useState(0)
  const [selected, setSelected] = useState(0)
  const oldValueRef = useRef<Point | null>(null)

  const svgRef = useRef<SVGSVGElement>(null)

  useEffectOnce(() => {
    if (!svgRef.current) return
    const resizeObserver = new ResizeObserver(() => {
      if (svgRef.current) {
        setWidth(svgRef.current.clientWidth)
        setHeight(svgRef.current.clientHeight)
      }
    })
    resizeObserver.observe(svgRef.current)
    return () => resizeObserver.disconnect() // clean up
  })

  useEffect(() => {
    if (
      value !== undefined &&
      oldValueRef.current === null &&
      !isScalarRampSame(value, sortRamp(points))
    ) {
      const new_points = value.length != 0 ? value : default_points
      setSelected(0)
      setPoints(new_points)
    }
  }, [value])

  const [,] = useDebounce(
    () => {
      if (onChange) {
        const new_points = sortRamp(points)
        if (value) {
          if (!isScalarRampSame(new_points, value)) {
            onChange(new_points)
          }
        } else {
          onChange(new_points)
        }
      }
    },
    10,
    [points, onChange]
  )

  const getSvgCoords = (e: React.MouseEvent) => {
    if (!svgRef.current) return { pos: 0.5, value: 0.5 }
    var pt = svgRef.current.createSVGPoint()
    pt.x = e.clientX
    pt.y = e.clientY
    var svgP = pt.matrixTransform(svgRef.current.getScreenCTM()!.inverse())
    return { pos: svgP.x / width, value: 1 - svgP.y / height }
  }

  const addPoint = useCallback(
    (new_point: Point) => {
      const idx = points.length
      const old_points = onEditingFinished ? [...points] : undefined
      const new_points = [...points, new_point]
      setPoints(new_points)
      setSelected(idx)
      if (old_points && onEditingFinished) {
        onEditingFinished(sortRamp(old_points), sortRamp(new_points))
      }
    },
    [points, onEditingFinished]
  )

  const removePoint = useCallback(
    (idx: number) => {
      if (points.length == 1) return
      const old_points = onEditingFinished ? [...points] : undefined
      const new_points = points.filter((_, i) => i != idx)
      if (idx != 0) {
        setSelected(0)
      }
      setPoints(new_points)
      if (old_points && onEditingFinished) {
        onEditingFinished(sortRamp(old_points), sortRamp(new_points))
      }
    },
    [points, onEditingFinished]
  )

  const updatePoint = useCallback(
    (updated_point: Point, idx: number, finish?: boolean) => {
      updated_point.pos = Math.max(0, Math.min(1, Math.max(0, updated_point.pos)))
      updated_point.value = Math.max(0, Math.min(1, Math.max(0, updated_point.value)))
      if (isSamePoint(updated_point, points[idx])) {
        return
      }
      const old_points = finish && onEditingFinished ? [...points] : undefined
      const new_points = points.map((point, i) => (i === idx ? updated_point : point))
      setPoints(new_points)
      if (old_points && onEditingFinished) {
        onEditingFinished(sortRamp(old_points), sortRamp(new_points))
      }
    },
    [points, onEditingFinished]
  )

  const selectedPoint = useMemo(() => {
    return points[selected]
  }, [points, selected])

  const EditingFinished =
    onEditingFinished !== undefined
      ? (old_point: Point, idx: number) => {
          const old_points = points.map((point, i) =>
            i == idx
              ? {
                  ...old_point
                }
              : point
          )
          onEditingFinished(sortRamp(old_points), sortRamp(points))
        }
      : undefined

  return (
    <div className={cn('flex w-full gap-4 h-20', className)}>
      <svg
        ref={svgRef}
        className="size-full rounded bg-white/10"
        onDoubleClick={(e) => addPoint({ ...getSvgCoords(e), method: points[0].method })}
        onContextMenu={(e) => {
          e.stopPropagation()
          e.preventDefault()
        }}
        // width={width}
        // height={height}
      >
        <SvgLine points={points} width={width} height={height} />
        <SvgBackground width={width} height={height} />
        {points.map((point, i) => (
          <SvgPoint
            key={i}
            point={point}
            width={width}
            height={height}
            selected={i === selected}
            onMouseDown={() => {
              oldValueRef.current = { ...point }
              if (i !== selected) {
                setSelected(i)
              }
              document.onmousemove = (e) => {
                if (e.buttons == 0) {
                  document.onmousemove = null
                  if (
                    EditingFinished &&
                    oldValueRef.current &&
                    !isSamePoint(oldValueRef.current, selectedPoint)
                  ) {
                    EditingFinished(oldValueRef.current, i)
                  }
                  oldValueRef.current = null
                  return
                }
                updatePoint({ ...getSvgCoords(e as any), method: points[i].method }, i)
              }
            }}
            onMouseUp={(e) => {
              if (document.onmousemove) {
                document.onmousemove = null
                const has_change =
                  oldValueRef.current && !isSamePoint(oldValueRef.current, selectedPoint)
                if (e.button == 2 && !has_change) {
                  removePoint(i)
                } else if (EditingFinished && has_change) {
                  EditingFinished(oldValueRef.current!, i)
                }
              }
              oldValueRef.current = null
            }}
          />
        ))}
      </svg>
      <div className="my-1 flex flex-col gap-2">
        <Select
          className="select-accent w-full bg-white/20 text-xs"
          onChange={(v) => updatePoint({ ...selectedPoint, method: v }, selected, true)}
          value={selectedPoint.method}
          options={methods}
        />

        <div className="flex flex-row gap-2">
          <label className="select-none">x</label>
          <Slider
            valueType={SliderValueType.Float}
            value={selectedPoint.pos}
            rangeMin={0}
            rangeMax={1}
            limitMin={0}
            limitMax={1}
            onValueChange={(v) => {
              updatePoint({ ...selectedPoint, pos: v }, selected)
            }}
            onEditingFinished={
              EditingFinished
                ? (v) => EditingFinished({ ...selectedPoint, pos: v }, selected)
                : undefined
            }
            className="w-20"
          />
        </div>
        <div className="flex flex-row gap-2">
          <label className="select-none">y</label>
          <Slider
            valueType={SliderValueType.Float}
            value={selectedPoint.value}
            rangeMin={0}
            rangeMax={1}
            limitMin={0}
            limitMax={1}
            onValueChange={(v) => {
              updatePoint({ ...selectedPoint, value: v }, selected)
            }}
            onEditingFinished={
              EditingFinished
                ? (v) => EditingFinished({ ...selectedPoint, value: v }, selected)
                : undefined
            }
            className="w-20"
          />
        </div>
      </div>
    </div>
  )
}
