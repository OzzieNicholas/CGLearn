import { presets } from '@/css/presets'
import { cn } from '@/utils/cn'
import { OverlayPlacement } from '@nextui-org/aria-utils'
import { Popover, PopoverContent, PopoverTrigger } from '@nextui-org/react'
import { ColorResult, rgbaToHexa } from '@uiw/color-convert'
import React, { useEffect, useRef, useState } from 'react'
import ColorPicker_, { useColorPicker } from 'react-best-gradient-color-picker'
import { useDebounce } from 'react-use'

const ColorPicker = ({
  children,
  color,
  onChange,
  placement,
  useGradientControls,
  hideOpacity,
  className,
  onEditingFinished,
}: {
  children: React.ReactNode
  color: string
  onChange: (color: ColorResult & { css: string }) => void
  placement: OverlayPlacement
  useGradientControls?: boolean
  hideOpacity?: boolean
  className?: string
  onEditingFinished?: (old_color: string, new_color: string) => void
}) => {
  if (typeof color !== 'string') {
    color = 'rgb(255,255,255)'
  }
  const [currentColor, setCurrentColor] = useState<string>(color)
  const { valueToHex, rgbaArr } = useColorPicker(currentColor, setCurrentColor)
  const oldColorRef = useRef(currentColor)
  const dirty = useRef(false)

  const onChange_ = (new_color: string) => {
    if (new_color !== currentColor) {
      dirty.current = true
      setCurrentColor(new_color)
    }
  }

  useEffect(() => {
    if (color !== currentColor) {
      setCurrentColor(color)
    }
  }, [color])

  const [,] = useDebounce(
    () => {
      if (!dirty.current) return
      dirty.current = false
      const hex = valueToHex()
      const rgba = {
        r: rgbaArr[0],
        g: rgbaArr[1],
        b: rgbaArr[2],
        a: rgbaArr[3]
      }
      onChange({
        hex: hex,
        hexa: rgbaToHexa(rgba),
        rgb: {
          r: rgbaArr[0],
          g: rgbaArr[1],
          b: rgbaArr[2]
        },
        rgba,
        css: currentColor
      } as any)
    },
    5,
    [currentColor]
  )

  const onMouseDown = () => {
    if (onEditingFinished !== undefined) {
      oldColorRef.current = currentColor
    }
  }

  const onMouseUp = () => {
    if (onEditingFinished !== undefined && oldColorRef.current !== currentColor) {
      onEditingFinished(oldColorRef.current, currentColor)
    }
  }

  return (
    <Popover placement={placement}>
      <PopoverTrigger>{children}</PopoverTrigger>
      <PopoverContent className={cn(presets['menu-primary'], 'p-1 bg-white/10')}>
        <div
          onMouseDown={onMouseDown}
          onMouseUp={onMouseUp}
          onClick={(e) => {
            e.stopPropagation()
            e.preventDefault()
          }}
        >
          <ColorPicker_
            value={currentColor}
            onChange={onChange_}
            hideColorTypeBtns={!useGradientControls}
            // disableDarkMode={true}
            className={cn("rounded-lg !bg-transparent", className)}
            hideOpacity={hideOpacity}
          />
        </div>
      </PopoverContent>
    </Popover>
  )
}

export default ColorPicker
