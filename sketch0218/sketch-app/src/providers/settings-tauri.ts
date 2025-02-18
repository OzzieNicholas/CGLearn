import { Store } from '@tauri-apps/plugin-store'
import { PersistStorage, StorageValue } from 'zustand/middleware'

const tauriStore = new Store('nous-settings.json')

const SettingsLocalProvider: PersistStorage<any> = {
  getItem: async (name: string): Promise<StorageValue<any> | null> => {
    return tauriStore.get(name)
  },

  setItem: async (name: string, value: StorageValue<any>) => {
    await tauriStore.set(name, value)
    await tauriStore.save()
  },

  removeItem: async (name: string) => {
    await tauriStore.delete(name)
    await tauriStore.save()
  }
}

export { SettingsLocalProvider }
