import i18n from '@/locales'
import { storageNameTitleBar, titleBarLocalProvider } from '@/providers/title-bar'
import { TitleBarBadge, TitleBarMenu, TitleBarState, TitleBarSubMenu } from '@/types/title-bar'
import { createSelectors } from '@/utils/create-selectors'
import { produce } from 'immer'
import { create } from 'zustand'
import { persist } from 'zustand/middleware'

// TODO: support menu location
// TODO: support badge location

export const useTitleBarStore = createSelectors(
  create<TitleBarState & { info: string }>()(
    persist(
      // eslint-disable-next-line no-unused-vars
      (_) => ({
        menus: [],
        badges: [],
        info: i18n.t('common.untitled')
      }),
      {
        name: storageNameTitleBar,
        storage: titleBarLocalProvider,
        partialize: (state) => ({ menus: state.menus, badges: state.badges })
      }
    )
  )
)

// menu
export const addMenu = (menu: TitleBarMenu) => {
  if (!hasMenu(menu.label)) {
    useTitleBarStore.setState((state) => ({ menus: [...state.menus, menu] }))
  } else {
    console.error(`menu '${menu.label}' already exists`)
  }
}

export const addSubMenu = (menu_label: string, sub_menu: TitleBarSubMenu) => {
  if (!hasMenu(menu_label)) {
    addMenu({
      label: menu_label,
      sub_menus: [sub_menu]
    })
  } else if (!hasSubMenu(menu_label, sub_menu.label)) {
    useTitleBarStore.setState(
      produce((state) => {
        state.menus
          .find((menu: TitleBarSubMenu) => menu.label === menu_label)
          ?.sub_menus.push(sub_menu)
      })
    )
  } else {
    console.error(`menu '${menu_label}/${sub_menu.label}' already exists`)
  }
}

export const removeMenu = (menu_label: string) =>
  useTitleBarStore.setState((state) => ({
    menus: [...state.menus.filter((menu) => menu.label !== menu_label)]
  }))

export const removeSubMenu = (menu_label: string, sub_menu_label: string) => {
  const memu = useTitleBarStore.getState().menus.find((menu) => menu.label === menu_label)
  if (memu) {
    const idx = memu.sub_menus.findIndex((m) => m.label === sub_menu_label)
    if (idx >= 0) {
      useTitleBarStore.setState(
        produce((state) => {
          state.menus
            .find((menu: TitleBarSubMenu) => menu.label === menu_label)
            ?.sub_menus.splice(idx, 1)
        })
      )
    }
  } else {
    console.error(`menu '${menu_label}' not found`)
  }
}

export const hasMenu = (menu_label: string) =>
  useTitleBarStore.getState().menus.some((menu) => menu.label === menu_label)

export const hasSubMenu = (menu_label: string, sub_menu_label: string) => {
  const memu = useTitleBarStore.getState().menus.find((menu) => menu.label === menu_label)
  if (memu) {
    return memu.sub_menus.some((m) => m.label === sub_menu_label)
  }
  return false
}

export const invokeMenuAction = (menu_label: string, sub_menu_label: string) => {
  const memu = useTitleBarStore.getState().menus.find((menu) => menu.label === menu_label)
  if (memu) {
    const sub_menu = memu.sub_menus.find((m) => m.label === sub_menu_label)
    if (sub_menu) {
      sub_menu.action()
    } else {
      console.error(`menu '${menu_label}/${sub_menu_label}' not found`)
    }
  } else {
    console.error(`menu '${menu_label}' not found`)
  }
}

// badge
export const addBadge = (badge: TitleBarBadge) => {
  if (!hasBadge(badge.label)) {
    useTitleBarStore.setState((state) => ({ badges: [...state.badges, badge] }))
  } else {
    console.error(`badge '${badge.label}' already exists`)
  }
}

export const removeBadge = (label: string) => {
  useTitleBarStore.setState((state) => ({
    badges: state.badges.filter((b) => b.label === label)
  }))
}

export const hasBadge = (label: string) => {
  return useTitleBarStore.getState().badges.some((b) => b.label === label)
}

export const invokeBadgeAction = (label: string) => {
  useTitleBarStore
    .getState()
    .badges.find((b) => b.label === label)
    ?.action?.()
}

// info
export const setTitleBarInfo = (info: string) => useTitleBarStore.setState({ info })
