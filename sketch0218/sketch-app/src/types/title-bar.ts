export type TitleBarBadge = {
  label: string
  color: string
  info?: string
  action?: () => void
}

export type TitleBarSubMenu = {
  label: string
  action: () => void
  shortcut?: string
  divider?: boolean
  info?: string
  sub_menus?: TitleBarSubMenu[]
}

export type TitleBarMenu = {
  label: string
  action?: () => void
  sub_menus: TitleBarSubMenu[]
}

export type TitleBarState = {
  menus: TitleBarMenu[]
  badges: TitleBarBadge[]
}
