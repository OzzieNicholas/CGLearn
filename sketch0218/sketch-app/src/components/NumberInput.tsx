import { cn } from '@/utils/cn'
import { useCallback, useEffect, useRef, useState } from 'react'

export enum NumberValueType {
  Float,
  Int
}

export default function NumberInput({
  valueType,
  onChange,
  defaultValue,
  className,
  value,
  limitMin,
  limitMax,
  step,
  placeholder
}: {
  valueType: NumberValueType
  onChange: (value: number) => void
  defaultValue?: number
  className?: string
  value?: number
  limitMin?: number
  limitMax?: number
  step?: number
  placeholder?: string
}) {
  const [currentValue, setCurrentValue] = useState(value !== undefined ? value : defaultValue || 0)
  const ref = useRef<HTMLInputElement>(null!)

  useEffect(() => {
    if (value !== undefined && value !== currentValue) {
      setCurrentValue(value)
    }
  }, [value])

  const finishEditing = useCallback(
    (new_value: number) => {
      if (new_value !== value) {
        onChange(new_value)
      }
    },
    [value, onChange]
  )

  return (
    <input
      type="number"
      ref={ref}
      className={cn(
        'transition-colors px-2 rounded outline-none bg-text-input min-w-0 w-16 selection:bg-text-select/40 hover:bg-text-input-active focus:outline-btn-active/80 focus:min-w-16 border-1 border-white/5',
        className
      )}
      placeholder={placeholder}
      step={step}
      min={limitMin}
      max={limitMax}
      value={currentValue}
      onBlur={() => finishEditing(currentValue)}
      onClick={() => {
        ref.current.focus()
      }}
      onChange={(e) => {
        let new_value = e.target.valueAsNumber
        if (limitMin !== undefined) {
          new_value = Math.max(limitMin, new_value)
        }
        if (limitMax !== undefined) {
          new_value = Math.min(limitMax, new_value)
        }
        if (valueType == NumberValueType.Int) {
          new_value = Math.ceil(new_value)
        }

        setCurrentValue(new_value)

        const nativeEvent = e.nativeEvent as InputEvent
        if (nativeEvent.inputType === undefined) {
          finishEditing(new_value)
        }
      }}
      onKeyDown={(e) => {
        if (e.key == 'Enter') {
          e.stopPropagation()
          e.preventDefault()
          finishEditing(currentValue)
        } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'a') {
          e.stopPropagation()
        }
      }}
      required={true}
    />
  )
}
