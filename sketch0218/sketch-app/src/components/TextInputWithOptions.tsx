import DropdownMenu from '@/components/DropdownMenu'
import { useEffect, useState } from 'react'
import { IoMdArrowDropdown } from 'react-icons/io'
import TextInput from './TextInput'

const onSelectOption = (value: string, option: string) => {
  if (option.length > 0) {
    let values = value
      .trim()
      .split(' ')
      .filter((v) => v.length > 0)
    const new_values = values.filter((v) => v !== option)
    if (values.length !== new_values.length) {
      if (new_values.length == 0) return ''
      return new_values.join(' ')
    } else {
      values.push(option)
      return values.join(' ')
    }
  }
  return value
}

export default function TextInputWithOptions({
  onChange,
  options,
  defaultValue,
  className,
  value,
  placeHolder
}: {
  onChange: (value: string) => void
  options: () => Promise<string[]>
  defaultValue?: string
  className?: string
  value?: string
  placeHolder?: string
}) {
  const [currentOptions, setCurrentOptions] = useState<string[]>([])
  const [currentValue, setCurrentValue] = useState(value !== undefined ? value : defaultValue || '')

  useEffect(() => {
    if (value !== undefined && value !== currentValue) {
      setCurrentValue(value)
    }
  }, [value])

  return (
    <div className="relative flex w-full">
      <TextInput
        defaultValue={defaultValue}
        onChange={(v) => {
          setCurrentValue(v)
          onChange(v)
        }}
        className={className}
        value={currentValue}
        placeHolder={placeHolder}
      />
      <DropdownMenu
        className="min-w-0 font-thin tracking-wide"
        menus={currentOptions}
        onOpenChange={(open) => {
          if (open) {
            options().then((data) => {
              setCurrentOptions(data)
            })
          } else {
            setCurrentOptions([])
          }
        }}
        onClickItem={(item) => {
          const v = onSelectOption(currentValue, item.label)
          setCurrentValue(v)
          onChange(v)
        }}
      >
        <div className="btn btn-square btn-ghost btn-xs absolute right-0 top-0 h-5 min-h-0 min-w-0 rounded opacity-70">
          <IoMdArrowDropdown />
        </div>
      </DropdownMenu>
    </div>
  )
}
