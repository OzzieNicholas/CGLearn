import { presets } from '@/css/presets'
import { cn } from '@/utils/cn'
import { useEffect, useRef, useState } from 'react'
import {
  Select as SelectBase,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue
} from './SelectBase'

type SelectItemType = {
  value: string
  label?: string
}

const getOptionValue = (opt: SelectItemType | string) => {
  if (typeof opt === 'string') {
    return opt
  }
  return opt.value
}

const getOptionLabel = (opt: SelectItemType | string) => {
  if (typeof opt === 'string') {
    return opt
  }
  return opt.label || opt.value
}

export default function Select({
  options,
  onChange,
  className,
  defaultValue,
  value
}: {
  options: SelectItemType[] | string[]
  onChange: (value: string) => void
  className?: string
  defaultValue?: string
  value?: string
}) {
  const [currentValue, setCurrentValue] = useState(
    value || defaultValue || getOptionValue(options[0])
  )
  const [open, setOpen] = useState<boolean>(false)
  const ref = useRef<HTMLButtonElement>(null)

  useEffect(() => {
    if (value !== undefined && value != currentValue) {
      setCurrentValue(value)
    }
  }, [value])

  return (
    <SelectBase
      value={currentValue}
      onValueChange={(v) => {
        setCurrentValue(v)
        onChange(v)
      }}
      onOpenChange={setOpen}
    >
      <SelectTrigger
        ref={ref}
        className={cn(
          'self-center outline-none rounded border-1 border-white/20  hover:border-white/50 select-none w-auto h-4 focus:border-white/30',
          open && 'outline-accent',
          className
        )}
      >
        <SelectValue />
      </SelectTrigger>
      <SelectContent className={cn(presets['menu-primary'], 'm-0 p-0 min-w-0 w-auto')}>
        {options.map((opt) => (
          <SelectItem
            key={getOptionValue(opt)}
            value={getOptionValue(opt)}
            className={cn(presets['menu-item-primary'], 'tracking-wide')}
          >
            {getOptionLabel(opt)}
          </SelectItem>
        ))}
      </SelectContent>
    </SelectBase>
  )
}
