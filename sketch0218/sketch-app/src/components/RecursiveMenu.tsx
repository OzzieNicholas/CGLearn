import { useI18n } from '@/locales'
import React, { useCallback, useRef } from 'react'
import {
  DropdownMenu,
  DropdownMenuContent,
  // DropdownMenuGroup,
  DropdownMenuItem,
  DropdownMenuLabel,
  DropdownMenuPortal,
  DropdownMenuSeparator,
  DropdownMenuShortcut,
  DropdownMenuSub,
  DropdownMenuSubContent,
  DropdownMenuSubTrigger,
  DropdownMenuTrigger
} from './DropdownMenuBase'

export type MenuTye = {
  label: string
  divider?: boolean
  action?: () => void
  shortcut?: string
  icon?: () => React.ReactElement
  sub_menus?: MenuTye[]
  data?: any
}

const MenuItem = ({
  data,
  className,
  itemClassName,
  dividerClassName,
  onClickItem
}: {
  data: MenuTye
  className?: string
  itemClassName?: string
  dividerClassName?: string
  onClickItem?: (item: MenuTye) => void
}) => {
  const { t } = useI18n()

  const onClick = useCallback(() => {
    if (onClickItem) {
      onClickItem(data)
    }
    if (data.action) {
      data.action()
    }
  }, [data, onClickItem])

  if (data.sub_menus !== undefined) {
    return (
      <DropdownMenuSub>
        <DropdownMenuSubTrigger className={itemClassName}>
          <div className="flex items-center gap-2 tracking-wide">
            {data.icon && data.icon()}
            {t(data.label as any)}
          </div>
          {data.shortcut && (
            <DropdownMenuShortcut className="pl-8">{data.shortcut}</DropdownMenuShortcut>
          )}
        </DropdownMenuSubTrigger>
        <DropdownMenuPortal>
          <DropdownMenuSubContent className={className}>
            {data.sub_menus.length > 0 ? (
              data.sub_menus.map((item, i) => (
                <div key={i}>
                  {item.label.length > 0 && (
                    <MenuItem
                      data={item}
                      itemClassName={itemClassName}
                      className={className}
                      onClickItem={onClickItem}
                    />
                  )}
                  {item.divider && <DropdownMenuSeparator className={dividerClassName} />}
                </div>
              ))
            ) : (
              <DropdownMenuLabel>{t('menu.no-items')}</DropdownMenuLabel>
            )}
          </DropdownMenuSubContent>
        </DropdownMenuPortal>
      </DropdownMenuSub>
    )
  }

  return (
    <DropdownMenuItem onClick={onClick} className={itemClassName}>
      <div className="flex items-center gap-2 tracking-wide">
        {data.icon && data.icon()}
        {t(data.label as any)}
      </div>
      {data.shortcut && (
        <DropdownMenuShortcut className="pl-8 font-thin">{data.shortcut}</DropdownMenuShortcut>
      )}
    </DropdownMenuItem>
  )
}

const RecursiveMenu = ({
  children,
  menus,
  className,
  itemClassName,
  dividerClassName,
  defaultOpen,
  open,
  asChild,
  onClickItem,
  onOpenChange,
  modal,
  focusTriggerOnKeyPressed
}: {
  children: React.ReactNode
  menus: MenuTye[]
  className?: string
  itemClassName?: string
  dividerClassName?: string
  defaultOpen?: boolean
  open?: boolean
  asChild?: boolean
  onClickItem?: (item: MenuTye) => void
  onOpenChange?: (open: boolean) => void
  modal?: boolean
  focusTriggerOnKeyPressed?: boolean
}) => {
  const { t } = useI18n()

  const triggerElement = useRef<HTMLButtonElement>(null)
  const contentElement = useRef<HTMLDivElement>(null)

  const onTriggerKeyDown = useCallback(
    (event: any) => {
      if (event.key === 'ArrowDown' && contentElement.current) {
        contentElement.current.focus()
      }
    },
    [contentElement]
  )

  const onContentKeyDown = useCallback(
    (event: React.KeyboardEvent) => {
      if (event.key === 'ArrowUp' && contentElement.current && triggerElement.current) {
        if (contentElement.current.firstChild?.firstChild === document.activeElement) {
          triggerElement.current.focus()
        }
      } else if (
        focusTriggerOnKeyPressed &&
        !event.key.startsWith('Arrow') &&
        triggerElement.current
      ) {
        triggerElement.current.focus()
      }
    },
    [contentElement, triggerElement]
  )

  return (
    <div>
      <DropdownMenu modal={modal} defaultOpen={defaultOpen} open={open} onOpenChange={onOpenChange}>
        <DropdownMenuTrigger
          asChild={asChild}
          onKeyDown={onTriggerKeyDown}
          ref={triggerElement}
          className="focus:outline-none"
        >
          {children}
        </DropdownMenuTrigger>
        <DropdownMenuContent
          className={className}
          ref={contentElement}
          onKeyDown={onContentKeyDown}
        >
          {menus.length > 0 ? (
            menus.map((item: MenuTye, i) => (
              <div key={i}>
                {item.label.length > 0 && (
                  <MenuItem
                    data={item}
                    className={className}
                    itemClassName={itemClassName}
                    onClickItem={onClickItem}
                  />
                )}
                {item.divider && <DropdownMenuSeparator className={dividerClassName} />}
              </div>
            ))
          ) : (
            <DropdownMenuLabel>{t('menu.no-items')}</DropdownMenuLabel>
          )}
        </DropdownMenuContent>
      </DropdownMenu>
    </div>
  )
}

export default RecursiveMenu
