import { getCurrentWindow } from '@tauri-apps/api/window'
import { StateFlags, saveWindowState } from '@tauri-apps/plugin-window-state'

export async function saveWindowStateWhileNotMinimized() {
  const minimized = await getCurrentWindow().isMinimized()
  if (minimized) {
    return
  }

  await saveWindowState(StateFlags.ALL)
}

export const minimizeWindow = async () => await getCurrentWindow().minimize()

export const toggleWindowMaximize = async () => await getCurrentWindow().toggleMaximize()

export const closeWindow = async () => await getCurrentWindow().close()
