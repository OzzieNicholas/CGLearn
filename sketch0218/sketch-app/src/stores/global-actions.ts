import { createSelectors } from '@/utils/create-selectors'
import { createNanoEvents, Emitter, Unsubscribe } from 'nanoevents'
import { create } from 'zustand'

interface Events {
  new: () => void
  open: (url?: string) => void
  save: () => void
  saveAs: () => void
  quit: () => void
  copy: () => void
  paste: () => void
  cut: () => void
  resetLayout: () => void
  jumpToNode: (path: string) => void
  jumpToLine: (line: number) => void
}

interface GlobalActionsState {
  emitter: Emitter<Events>
  block_events: boolean
}

export const useGlobalActionsStore = createSelectors(
  // eslint-disable-next-line no-unused-vars
  create<GlobalActionsState>()((_) => ({
    emitter: createNanoEvents(),
    block_events: false
  }))
)

export function emit<E extends keyof Events>(action: E, ...args: Parameters<Events[E]>) {
  const { emitter, block_events } = useGlobalActionsStore.getState()
  if (!block_events) {
    emitter.emit(action, ...args)
  }
}

export function listen<E extends keyof Events>(action: E, cb: Events[E]): Unsubscribe {
  return useGlobalActionsStore.getState().emitter.on(action, cb)
}

export const isBlockEvents = () => useGlobalActionsStore.getState().block_events

export const setBlockEvents = (block_events: boolean) =>
  useGlobalActionsStore.setState(() => ({ block_events }))
