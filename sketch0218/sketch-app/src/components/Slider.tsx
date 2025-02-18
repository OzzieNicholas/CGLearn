import { cn } from '@/utils/cn'
import { isNumberSame } from '@/utils/functions'
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react'

export enum SliderValueType {
  Float,
  Int
}

const moveSpeed = 0.2

export function Slider({
  valueType,
  onValueChange,
  onEditingFinished,
  rangeMin,
  rangeMax,
  limitMin,
  limitMax,
  defaulValue,
  value,
  className,
  step
}: {
  valueType: SliderValueType
  onValueChange?: (value: number) => void
  onEditingFinished?: (old_value: number, new_value: number) => void
  rangeMin: number
  rangeMax: number
  limitMin?: number
  limitMax?: number
  defaulValue?: number
  value?: number
  className?: string
  step?: number
}) {
  const [editing, setEditing] = useState(false)
  const [currentValue, setCurrentValue] = useState(value || defaulValue || 0)
  const valueRef = useRef(currentValue)
  const oldValueRef = useRef(currentValue)
  const [sliding, setSliding] = useState(false)
  // const [fontSize, setFontSize] = useState('1rem')
  const pendingEdit = useRef('')
  const slidingDelta = useRef(0)
  const dirty = useRef(false)
  const ref = useRef<any>(null)

  const applyLimit = useCallback(
    (new_value: number) => {
      if (limitMin !== undefined && new_value < limitMin) {
        new_value = limitMin
      }
      if (limitMax !== undefined && new_value > limitMax) {
        new_value = limitMax
      }
      if (isNumberSame(new_value, valueRef.current)) {
        return undefined
      }
      return new_value
    },
    [limitMin, limitMax]
  )

  const applyRangeLimit = useCallback(
    (new_value: number) => {
      if (rangeMin !== undefined && new_value < rangeMin) {
        new_value = rangeMin
      }
      if (rangeMax !== undefined && new_value > rangeMax) {
        new_value = rangeMax
      }
      if (isNumberSame(new_value, valueRef.current)) {
        return undefined
      }
      return new_value
    },
    [rangeMin, rangeMax]
  )

  const finishEditing = useCallback(() => {
    setEditing(false)
    const v = pendingEdit.current.trim()
    pendingEdit.current = ''
    if (v.length == 0) {
      return
    }
    try {
      let new_value = parseFloat(v)
      if (isNaN(new_value)) return
      if (valueType == SliderValueType.Int) {
        new_value = Math.round(new_value)
      }

      new_value = applyLimit(new_value) as any
      if (new_value === undefined) {
        return
      }

      setCurrentValue(new_value)
      valueRef.current = new_value
      if (onValueChange !== undefined) {
        onValueChange(new_value)
      }
      if (onEditingFinished !== undefined) {
        onEditingFinished(oldValueRef.current, new_value)
      }
    } catch (_) {}
  }, [onValueChange, onEditingFinished])

  const step_ = useMemo(() => {
    if (step === undefined) {
      return (rangeMax - rangeMin) / 100.0
    }
    return step
  }, [step, rangeMin, rangeMax])

  const onMouseDown = useCallback((e: React.MouseEvent) => {
    setSliding(true)
    dirty.current = false
    slidingDelta.current = 0
    oldValueRef.current = valueRef.current
    e.stopPropagation()
  }, [])

  const onMouseUp = useCallback(() => {
    setSliding(false)
    document.exitPointerLock()
    if (dirty.current) {
      if (onEditingFinished !== undefined) {
        onEditingFinished(oldValueRef.current, valueRef.current)
      }
    } else {
      setEditing(true)
    }
  }, [onEditingFinished])

  const progress = () => {
    const v = currentValue - rangeMin
    const p = v / (rangeMax - rangeMin)
    return Math.min(Math.max(Math.round(p * 100), 0), 100).toString() + '%'
  }

  const onMouseMove = useCallback(
    (e: React.MouseEvent) => {
      if (sliding && e.movementX != 0) {
        if (ref.current && document.pointerLockElement !== ref.current) {
          ref.current.requestPointerLock({ unadjustedMovement: true })
        }
        const btn = e.button != 0 ? e.button : e.buttons
        let btnSpeed = 1
        switch (btn) {
          case 2:
            btnSpeed = 5
            break
          case 4:
            btnSpeed = 10
            break
        }

        slidingDelta.current += e.movementX * moveSpeed * btnSpeed
        if (Math.abs(slidingDelta.current) > 1) {
          let new_value = valueRef.current + step_ * Math.floor(slidingDelta.current)

          if (valueType == SliderValueType.Int) {
            new_value = Math.round(new_value)
            if (new_value === valueRef.current) {
              return
            }
          }

          if (currentValue > rangeMax || currentValue < rangeMin) {
            new_value = applyLimit(new_value) as any
          } else {
            new_value = applyRangeLimit(new_value) as any
          }

          if (new_value !== undefined) {
            setCurrentValue(new_value)
            valueRef.current = new_value
            if (onValueChange !== undefined) {
              onValueChange(new_value)
            }
            dirty.current = true
          }

          slidingDelta.current = 0
        }
      }
    },
    [sliding, onValueChange, rangeMax, rangeMin]
  )

  // const onRefChange = useCallback((node: any) => {
  //   ref.current = node
  //   if (node) {
  //     setFontSize(Math.round(node.clientHeight * 0.8).toString() + 'px')
  //   }
  // }, [])

  useEffect(() => {
    if (value !== undefined) {
      setCurrentValue(value)
      valueRef.current = value
    }
  }, [value])

  return (
    <div className={className}>
      {editing ? (
        <input
          type="text"
          ref={ref}
          placeholder={currentValue.toString()}
          className={cn(
            'px-2 size-full rounded-lg text-text-primary outline-none bg-text-input-active selection:bg-text-select/40 min-w-0 focus:outline-btn-active/80 focus:min-w-20'
          )}
          autoComplete="new-password"
          // style={{
          //   fontSize
          // }}
          defaultValue={currentValue}
          onBlur={finishEditing}
          onFocus={(e) => e.target.select()}
          autoFocus={true}
          onChange={(e: any) => {
            pendingEdit.current = e.target.value
          }}
          onKeyDown={(e) => {
            if (e.key == 'Enter') {
              e.stopPropagation()
              e.preventDefault()
              finishEditing()
            } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'a') {
              e.stopPropagation()
            }
          }}
        />
      ) : (
        <div
          ref={ref}
          className={cn('size-full rounded-lg bg-white/10 cursor-ew-resize')}
          onMouseDown={onMouseDown}
          onMouseUp={onMouseUp}
          onMouseMove={onMouseMove}
        >
          <div className="flex size-full rounded-lg bg-btn-active/40" style={{ width: progress() }}>
            <span
              className="select-none self-center px-2"
              // style={{
              //   fontSize
              // }}
            >
              {Number.parseFloat(currentValue.toFixed(3))}
            </span>
          </div>
        </div>
      )}
    </div>
  )
}
