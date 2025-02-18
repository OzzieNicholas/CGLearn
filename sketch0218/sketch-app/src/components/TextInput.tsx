import { cn } from '@/utils/cn'
import { useCallback, useEffect, useRef, useState } from 'react'

export default function TextInput({
  onChange,
  defaultValue,
  className,
  value,
  placeHolder,
  readonly
}: {
  onChange: (value: string) => void
  defaultValue?: string
  className?: string
  value?: string
  placeHolder?: string
  readonly?: boolean
}) {
  const [currentValue, setCurrentValue] = useState(value !== undefined ? value : defaultValue || '')
  const ref = useRef<HTMLInputElement>(null!)

  useEffect(() => {
    if (value !== undefined && value !== currentValue) {
      setCurrentValue(value)
    }
  }, [value])

  const finishEditing = useCallback(
    (new_value: string) => {
      if (new_value !== value) {
        onChange(new_value)
      }
    },
    [value, onChange]
  )

  return (
    <input
      type="text"
      ref={ref}
      readOnly={readonly}
      placeholder={placeHolder || defaultValue}
      className={cn(
        'transition-colors px-2 rounded outline-none bg-text-input min-w-0  selection:bg-text-select/40 hover:bg-text-input-active focus:outline-btn-active/80 focus:min-w-20 border-1 border-white/5',
        className
      )}
      value={currentValue}
      onBlur={() => finishEditing(currentValue)}
      onClick={() => {
        ref.current.focus()
      }}
      onChange={(e: any) => {
        setCurrentValue(e.target.value)
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
      autoComplete="new-password"
    />
  )
}
