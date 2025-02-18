import { cn } from '@/utils/cn'
import { useEffect, useState } from 'react'

export default function Toggle({
  onChange,
  className,
  defaultChecked,
  checked
}: {
  onChange: (checked: boolean) => void
  className?: string
  defaultChecked?: boolean
  checked?: boolean
}) {
  const [value, setValue] = useState(checked !== undefined ? checked : defaultChecked || false)

  useEffect(() => {
    if (checked !== undefined && checked != value) {
      setValue(checked)
    }
  }, [checked])

  return (
    <input
      type="checkbox"
      className={cn('self-center checkbox rounded border-1 border-white/30', className)}
      checked={value}
      onChange={(e) => {
        setValue(e.target.checked)
        onChange(e.target.checked)
      }}
    />
  )
}
