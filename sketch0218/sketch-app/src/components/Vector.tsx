import { isNumberSame } from '@/utils/functions'
import { useEffect, useState } from 'react'
import { Slider, SliderValueType } from './Slider'

export const isSameVector = (a: number[], b: number[]) => {
  for (let i = 0; i < a.length; i++) {
    if (!isNumberSame(a[i], b[i])) {
      return false
    }
  }
  return true
}

export default function Vector({
  size,
  valueType,
  onChange,
  className,
  value,
  minValue,
  maxValue,
  limitMin,
  limitMax,
  onEditingFinished
}: {
  size: number
  valueType: SliderValueType
  onChange: (value: number[]) => void
  className?: string
  value?: number[]
  minValue: number
  maxValue: number
  limitMin?: number
  limitMax?: number
  onEditingFinished?: (old_value: number[], new_value: number[]) => void
}) {
  const [currentValue, setCurrentValue] = useState(
    value || Array.from(Array(size).keys()).map(() => 0)
  )
  useEffect(() => {
    if (value !== undefined && !isSameVector(value, currentValue)) {
      setCurrentValue(value)
    }
  }, [value])

  return (
    <div className="flex grow gap-1">
      {Array.from(Array(size).keys()).map((idx) => (
        <Slider
          key={idx}
          valueType={valueType}
          value={currentValue[idx]}
          rangeMin={minValue}
          rangeMax={maxValue}
          limitMin={limitMin}
          limitMax={limitMax}
          onValueChange={(v) => {
            const new_value = [...currentValue]
            new_value[idx] = v
            if (!isSameVector(new_value, currentValue)) {
              onChange(new_value)
              setCurrentValue(new_value)
            }
          }}
          onEditingFinished={
            onEditingFinished !== undefined
              ? (old_value) => {
                  if (onEditingFinished !== undefined) {
                    const old_vec = [...currentValue]
                    old_vec[idx] = old_value
                    onEditingFinished(old_vec, currentValue)
                  }
                }
              : undefined
          }
          className={className}
        />
      ))}
    </div>
  )
}
