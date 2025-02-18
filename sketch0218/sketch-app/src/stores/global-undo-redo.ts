import { createSelectors } from '@/utils/create-selectors'
import { createNanoEvents, Emitter } from 'nanoevents'
import { create } from 'zustand'
import { isBlockEvents } from './global-actions'
import { useSettingsStoreApi } from './settings'

type GlobalUndoRedoItem = {
  sub_stack_name: string
  desc: string
}

interface GlobalUndoRedoState {
  undo_stack: GlobalUndoRedoItem[]
  redo_stack: GlobalUndoRedoItem[]
  emitter: Emitter
}

export const useGlobalUndoRedoStore = createSelectors(
  // eslint-disable-next-line no-unused-vars
  create<GlobalUndoRedoState>()((_) => ({
    undo_stack: [],
    redo_stack: [],
    emitter: createNanoEvents()
  }))
)

export const canUndo = () => {
  useGlobalUndoRedoStore.getState().undo_stack.length > 0 && !isBlockEvents()
}

export const canRedo = () =>
  useGlobalUndoRedoStore.getState().redo_stack.length > 0 && !isBlockEvents()

export const undo = () => {
  if (isBlockEvents()) {
    return
  }

  const state = useGlobalUndoRedoStore.getState()
  if (state.undo_stack.length > 0) {
    const last_idx = state.undo_stack.length - 1
    const cmd = state.undo_stack[last_idx]
    const sub_stack_name = cmd.sub_stack_name

    // console.log('undoCommand: ', cmd.sub_stack_name, cmd.desc)

    useGlobalUndoRedoStore.setState((state) => ({
      redo_stack: [...state.redo_stack, cmd],
      undo_stack: [...state.undo_stack.slice(0, last_idx)]
    }))

    state.emitter.emit(sub_stack_name, 'undo')
  }
}

export const redo = () => {
  if (isBlockEvents()) {
    return
  }

  const state = useGlobalUndoRedoStore.getState()
  if (state.redo_stack.length > 0) {
    const last_idx = state.redo_stack.length - 1
    const cmd = state.redo_stack[last_idx]
    const sub_stack_name = cmd.sub_stack_name

    // console.log('redoCommand: ', cmd.sub_stack_name, cmd.desc)

    useGlobalUndoRedoStore.setState((state) => ({
      undo_stack: [...state.undo_stack, cmd],
      redo_stack: [...state.redo_stack.slice(0, last_idx)]
    }))

    state.emitter.emit(sub_stack_name, 'redo')
  }
}

export const push = (sub_stack_name: string, desc: string) => {
  // console.log('pushCommand: ', sub_stack_name, desc)

  useGlobalUndoRedoStore.setState((state) => {
    const max_depth = useSettingsStoreApi().getState().undo_history_max_size
    const undo_stack =
      state.undo_stack.length >= max_depth
        ? state.undo_stack.slice(state.undo_stack.length - max_depth + 1)
        : state.undo_stack
    return {
      undo_stack: [...undo_stack, { sub_stack_name, desc }],
      redo_stack: []
    }
  })
}

export const pop = () => {
  // console.log('popCommand')

  useGlobalUndoRedoStore.setState((state) => ({
    undo_stack: [...state.undo_stack.slice(0, state.undo_stack.length - 1)]
  }))
}

export const listen = (sub_stack_name: string, cb: (action: 'undo' | 'redo') => void) => {
  // console.log('listenCommand: ', sub_stack_name)

  if (useGlobalUndoRedoStore.getState().emitter.events[sub_stack_name] !== undefined) {
    useGlobalUndoRedoStore.getState().emitter.events[sub_stack_name] = []
  }
  return useGlobalUndoRedoStore.getState().emitter.on(sub_stack_name, cb)
}
