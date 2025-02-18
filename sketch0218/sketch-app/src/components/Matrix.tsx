import { isNumberSame } from '@/utils/functions'
import { useEffect, useState } from 'react'
import { SliderValueType } from './Slider'
import Vector from './Vector'

const getRow = (value: number[], size: number, row: number) => {
  let result: number[] = []
  for (let i = 0; i < size; ++i) {
    result.push(value[row + i * size])
  }
  return result
}

const setRow = (value: number[], size: number, row: number, v: number[]) => {
  let result = [...value]
  for (let i = 0; i < size; ++i) {
    result[row + i * size] = v[i]
  }
  return result
}

export const isSameMatrix = (a: number[], b: number[]) => {
  for (let i = 0; i < a.length; i++) {
    if (!isNumberSame(a[i], b[i])) {
      return false
    }
  }
  return true
}

export default function Matrix({
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
    value || Array.from(Array(size * size).keys()).map(() => 0)
  )

  useEffect(() => {
    if (value !== undefined && !isSameMatrix(value, currentValue)) {
      setCurrentValue(value)
    }
  }, [value])

  return (
    <div className="flex grow flex-col gap-1">
      {Array.from(Array(size).keys()).map((idx) => (
        <Vector
          key={idx}
          size={size}
          valueType={valueType}
          value={getRow(currentValue, size, idx)}
          minValue={minValue}
          maxValue={maxValue}
          limitMin={limitMin}
          limitMax={limitMax}
          onChange={(v) => {
            const new_value = setRow(currentValue, size, idx, v)
            if (!isSameMatrix(new_value, currentValue)) {
              setCurrentValue(new_value)
              onChange(new_value)
            }
          }}
          onEditingFinished={
            onEditingFinished !== undefined
              ? (old_value) => {
                  if (onEditingFinished !== undefined) {
                    onEditingFinished(setRow(currentValue, size, idx, old_value), currentValue)
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
