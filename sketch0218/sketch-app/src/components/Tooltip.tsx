import { presets } from '@/css/presets'
import { useI18n } from '@/locales'
import { cn } from '@/utils/cn'
import { OverlayPlacement } from '@nextui-org/aria-utils'
import { Tooltip as Tooltip_, TooltipProps } from '@nextui-org/react'
import React from 'react'

const Tooltip = ({
  children,
  className,
  content,
  showArrow,
  placement,
  ...props
}: {
  children: React.ReactNode
  className?: string
  content?: string
  showArrow?: boolean
  placement?: OverlayPlacement
  props?: TooltipProps
}) => {
  const { t } = useI18n()
  return (
    <Tooltip_
      {...props}
      showArrow={showArrow}
      delay={500}
      content={content && t(content as any)}
      placement={placement}
      className={cn(presets['menu-primary'], 'whitespace-pre-wrap', className)}
      hidden={!content}
      closeDelay={10}
    >
      {children}
    </Tooltip_>
  )
}

export default Tooltip
