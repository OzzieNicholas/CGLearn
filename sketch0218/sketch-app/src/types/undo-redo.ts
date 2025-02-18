import { listen, pop as popCommand, push as pushCommand } from '@/stores/global-undo-redo'
import { createNanoEvents, Emitter, Unsubscribe } from 'nanoevents'

type EventName = 'push' | 'undo' | 'redo' | 'end-grouping' | 'clear'

export type UndoCommand<T = any> = {
  name: string
  data?: T
  block_signal?: boolean
  undo: (data?: T) => Promise<any>
  redo: (data?: T) => Promise<any>
}

export class GroupCommand {
  name: string
  data: UndoCommand[]
  block_signal?: boolean

  constructor(name: string) {
    this.name = name
    this.data = []
  }

  async undo(): Promise<any> {
    for (let i = this.data.length - 1; i >= 0; --i) {
      const command = this.data[i]
      const fn = command.undo
      if (fn !== undefined) {
        await fn(command.data)
      }
    }
  }

  async redo(): Promise<any> {
    for (let i = 0; i < this.data.length; ++i) {
      const command = this.data[i]
      const fn = command.redo
      if (fn !== undefined) {
        await fn(command.data)
      }
    }
  }
}

export class UndoRedo {
  name: string = ''
  undo_stack: UndoCommand[] = []
  redo_stack: UndoCommand[] = []
  grouping_depth: number = 0
  unbind: Unsubscribe | null = null
  emitter?: Emitter

  constructor(name: string) {
    this.name = name
    this.unbind = listen(this.name, (action) => {
      switch (action) {
        case 'undo':
          this.undo()
          break
        case 'redo':
          this.redo()
          break
      }
    })
  }

  canUndo(): boolean {
    return this.undo_stack.length > 0
  }

  canRedo(): boolean {
    return this.redo_stack.length > 0
  }

  async undo(): Promise<void> {
    if (this.canUndo()) {
      const command = this.undo_stack.pop()
      if (command) {
        this.redo_stack.push(command)
        await command.undo(command.data)
        if (command.block_signal) return
        this.emit('undo')
      }
    }
  }

  async redo(): Promise<void> {
    if (this.canRedo()) {
      const command = this.redo_stack.pop()
      if (command) {
        this.undo_stack.push(command)
        await command.redo(command.data)
        if (command.block_signal) return
        this.emit('redo')
      }
    }
  }

  push(command: UndoCommand, run?: boolean): void {
    if (this.redo_stack.length > 0) {
      this.redo_stack = []
    }
    if (this.grouping_depth == 0) {
      this.undo_stack.push(command)
      pushCommand(this.name, command.name)
      if (run) {
        command.redo(command.data)
        return
      }
      // console.log('pushCommand:', command.name)
    } else {
      let group = this.undo_stack[this.undo_stack.length - 1] as GroupCommand
      group.data.push(command)
      // console.log('pushToGroup:', group.name, command.name)
    }
    if (command.block_signal) return
    this.emit('push')
  }

  beginGroup(name: string): void {
    if (this.grouping_depth == 0) {
      // console.log('beginGroup:', name)
      this.push(new GroupCommand(name))
    }
    ++this.grouping_depth
  }

  endGroup(): void {
    --this.grouping_depth
    if (this.grouping_depth == 0) {
      const group_command = this.undo_stack[this.undo_stack.length - 1] as GroupCommand
      if (group_command.data.length == 0) {
        this.undo_stack.pop()
        popCommand()
        return
      } else if (group_command.data.length == 1) {
        this.undo_stack.pop()
        this.undo_stack.push(group_command.data[0])
      }
      // console.log('endGroup:', group_command.name)
    }
    if (this.grouping_depth == 0) {
      if (this.undo_stack[this.undo_stack.length - 1].block_signal) return
      this.emit('end-grouping')
    }
  }

  isGrouping(): boolean {
    return this.grouping_depth > 0
  }

  clear(): void {
    this.undo_stack = []
    this.redo_stack = []
    this.emit('clear')
  }

  listen(event: EventName, cb: () => any): Unsubscribe {
    if (!this.emitter) {
      this.emitter = createNanoEvents()
    }
    return this.emitter.on(event, cb)
  }

  emit(event: EventName): void {
    if (this.emitter) {
      this.emitter.emit(event)
    }
  }

  unlistenAll(): void {
    if (this.emitter) {
      this.emitter.events = {}
      delete this.emitter
    }
  }
}
