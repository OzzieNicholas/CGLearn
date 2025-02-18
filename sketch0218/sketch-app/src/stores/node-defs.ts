import { nodeDefsLocalProvider, storageNameNodeDefs } from '@/providers/node-defs'
import { NodeDef, NodeDefsState } from '@/types/node-def'
import { createSelectors } from '@/utils/create-selectors'
import { loadNodeIcon } from '@/utils/node-graph'
import { create } from 'zustand'
import { persist } from 'zustand/middleware'

export const useNodeDefsStore = createSelectors(
  create<NodeDefsState>()(
    persist(
      // eslint-disable-next-line no-unused-vars
      (_) => ({
        defs: []
      }),
      {
        name: storageNameNodeDefs,
        storage: nodeDefsLocalProvider,
        onRehydrateStorage: () => async (state) => {
          if (state) {
            for (const def of state.defs) {
              if (def.icon) {
                loadNodeIcon(def.type_name, def.icon)
              }
            }
          }
        }
      }
    )
  )
)

export const addNodeDef = (def: NodeDef) => {
  useNodeDefsStore.setState((state) => ({
    defs: [...state.defs.filter((d) => d.type_name !== def.type_name), def]
  }))

  if (def.icon) {
    loadNodeIcon(def.type_name, def.icon)
  }
}

export const removeNodeDef = (type_name: string) =>
  useNodeDefsStore.setState((state) => ({
    defs: state.defs.filter((d) => d.type_name !== type_name)
  }))

export const getNodeDef = (type_name: string) => {
  return useNodeDefsStore.getState().defs.find((d) => d.type_name === type_name)
}

export const useNodeDefsStoreApi = () => {
  return useNodeDefsStore.getState()
}
