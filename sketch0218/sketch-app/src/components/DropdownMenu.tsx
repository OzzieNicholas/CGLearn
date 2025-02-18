import { cn } from '@/utils/cn'
import React from 'react'
import RecursiveMenu from './RecursiveMenu'

export type MenuTye = {
  label: string
  divider?: boolean
  action?: () => void
  shortcut?: string
  icon?: () => React.ReactElement
  data?: any
}

const convertMenus = (menus: MenuTye[] | string[]): MenuTye[] => {
  if (menus.length == 0) {
    return []
  }
  if (typeof menus[0] === 'string') {
    return (menus as string[]).map((label) => ({
      label
    }))
  }
  return menus as MenuTye[]
}

const DropdownMenu = ({
  children,
  menus,
  className,
  itemClassName,
  dividerClassName,
  defaultOpen,
  open,
  asChild,
  onClickItem,
  onOpenChange
}: {
  children: React.ReactNode
  menus: MenuTye[] | string[]
  className?: string
  itemClassName?: string
  dividerClassName?: string
  defaultOpen?: boolean
  open?: boolean
  asChild?: boolean
  onClickItem?: (item: MenuTye) => void
  onOpenChange?: (open: boolean) => void
}) => {
  return (
    <RecursiveMenu
      menus={convertMenus(menus)}
      open={open}
      defaultOpen={defaultOpen}
      className={cn('animate-fade-in', className)}
      itemClassName={itemClassName}
      dividerClassName={dividerClassName}
      asChild={asChild}
      onClickItem={onClickItem}
      onOpenChange={onOpenChange}
    >
      {children}
    </RecursiveMenu>
  )
}

export default DropdownMenu
