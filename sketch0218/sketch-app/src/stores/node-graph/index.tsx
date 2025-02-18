import { NodeGraphState } from '@/types/node-graph'
import { ReactNode, createContext, useContext, useRef } from 'react'
import { StoreApi, useStore } from 'zustand'
import { createLocalNodeGraphStore } from './local'

export type NodeGraphStoreType = {
  (): NodeGraphState
  <U>(selector: (state: NodeGraphState) => U): U
  <U>(selector: (state: NodeGraphState) => U, equalityFn: (a: U, b: U) => boolean): U
} & StoreApi<NodeGraphState>

const StoreContext = createContext<NodeGraphStoreType>(null!)

let nodeGraphMap: Record<string, NodeGraphStoreType> = {}

export const NodeGraphStoreProvider = ({ id, children }: { id: string; children: ReactNode }) => {
  const store_ref = useRef<NodeGraphStoreType>(null!)
  if (!store_ref.current) {
    if (nodeGraphMap[id] !== undefined) {
      store_ref.current = nodeGraphMap[id]
    } else {
      // TODO: more store source
      store_ref.current = createLocalNodeGraphStore(id)
      nodeGraphMap[id] = store_ref.current
    }
  }
  return <StoreContext.Provider value={store_ref.current}>{children}</StoreContext.Provider>
}

export function useNodeGraphStore<U>(selector: (state: NodeGraphState) => U) {
  const store = useContext(StoreContext)
  if (!store) {
    throw new Error('missing NodeGraphStoreProvider')
  }
  return useStore(store, selector)
}

export function useNodeGraphStoreApi() {
  return useContext(StoreContext)
}

export function getNodeGraphStoreApi(id: string) {
  return nodeGraphMap[id]
}
