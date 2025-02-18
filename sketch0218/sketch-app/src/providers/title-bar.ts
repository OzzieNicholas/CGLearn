import { defaultTitleBarBadges, defaultTitleBarMenus } from '@/constant/title-bar'
import { TitleBarBadge, TitleBarMenu, TitleBarState } from '@/types/title-bar'
import { PersistStorage, StorageValue } from 'zustand/middleware'

export const storageNameTitleBar = 'title-bar'

class TitleBarLocalProvider implements PersistStorage<TitleBarState> {
  menus: TitleBarMenu[]
  badges: TitleBarBadge[]

  constructor() {
    this.menus = defaultTitleBarMenus
    this.badges = defaultTitleBarBadges
  }

  getItem(name: string): StorageValue<TitleBarState> | null {
    switch (name) {
      case storageNameTitleBar:
        return { state: { menus: this.menus, badges: this.badges } }
      default:
        return null
    }
  }

  setItem(name: string, value: StorageValue<TitleBarState>) {
    switch (name) {
      case storageNameTitleBar: {
        if (value.state.menus != this.menus) {
          this.menus = value.state.menus
        }
        if (value.state.badges != this.badges) {
          this.badges = value.state.badges
        }
        break
      }
    }
  }

  removeItem(name: string) {
    switch (name) {
      case storageNameTitleBar: {
        this.menus = []
        this.badges = []
        break
      }
    }
  }
}

const titleBarLocalProvider = new TitleBarLocalProvider()

export { titleBarLocalProvider }
