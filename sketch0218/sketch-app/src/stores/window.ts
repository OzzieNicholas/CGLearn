import { createSelectors } from '@/utils/create-selectors'
import { getCurrentWindow } from '@tauri-apps/api/window'
import { saveWindowStateWhileNotMinimized } from '@/utils/window-state'
import { create } from 'zustand'
import { isMacOS } from '@/utils/env'

interface WindowState {
  is_initialized: boolean
  is_maximized: boolean
}

export const useWindowStore = createSelectors(
  // eslint-disable-next-line no-unused-vars
  create<WindowState>()((_) => ({
    is_initialized: false,
    is_maximized: false
  }))
)

let setWindowStateTimer: ReturnType<typeof setTimeout> | null = null

export const setupWindowStore = async () => {
  if (useWindowStore.getState().is_initialized) {
    return
  }

  useWindowStore.setState({
    is_initialized: true,
    is_maximized: await getCurrentWindow().isMaximized()
  })

  if (isMacOS()) {
    // FIXME: callback function will be called infinitely.
    return
  }

  // listen resize event
  getCurrentWindow().onResized(async () => {
    const is_maximized = await getCurrentWindow().isMaximized()
    if (is_maximized !== useWindowStore.getState().is_maximized) {
      useWindowStore.setState({ is_maximized })
    }

    if (!is_maximized) {
      if (setWindowStateTimer !== null) {
        clearTimeout(setWindowStateTimer)
        setWindowStateTimer = null
      }
      setWindowStateTimer = setTimeout(() => {
        saveWindowStateWhileNotMinimized()
        if (setWindowStateTimer !== null) {
          clearTimeout(setWindowStateTimer)
        }
      }, 500)
    }
  })

  getCurrentWindow().onCloseRequested(saveWindowStateWhileNotMinimized)
}
