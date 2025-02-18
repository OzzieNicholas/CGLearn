import { createSelectors } from '@/utils/create-selectors'
import { create } from 'zustand'

interface GlobalVariablesState {
  viewport_visible: boolean
}

export const useGlobalVariablesStore = createSelectors(
  // eslint-disable-next-line no-unused-vars
  create<GlobalVariablesState>()((_) => ({
    viewport_visible: false
  }))
)

export const useGlobalVariablesStoreApi = () => {
  return useGlobalVariablesStore
}
