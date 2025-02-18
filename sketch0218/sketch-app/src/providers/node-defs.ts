import { getAllNodeDefs } from '@/commands'
import { NodeDef, NodeDefsState } from '@/types/node-def'
import { PersistStorage, StorageValue } from 'zustand/middleware'

export const storageNameNodeDefs = 'node-defs'

class NodeDefsLocalProvider implements PersistStorage<NodeDefsState> {
  defs: NodeDef[]

  constructor() {
    this.defs = []
  }

  async getItem(name: string) {
    switch (name) {
      case storageNameNodeDefs: {
        const defs = await getAllNodeDefs()
        this.defs = defs
          .filter((d) => d.kind !== undefined)
          .map((d) => {
            return {
              type_name: d.type_name,
              kind: d.kind,
              category: d.category,
              inputs: d.inputs,
              outputs: d.outputs,
              icon: d.icon,
              type_version: d.type_version
            }
          })
        return { state: { defs: this.defs } }
      }
      default:
        return null
    }
  }

  setItem(name: string, value: StorageValue<NodeDefsState>) {
    switch (name) {
      case storageNameNodeDefs: {
        if (value.state.defs != this.defs) {
          this.defs = value.state.defs
        }
        break
      }
    }
  }

  removeItem(name: string) {
    switch (name) {
      case storageNameNodeDefs: {
        this.defs = []
        break
      }
    }
  }
}

const nodeDefsLocalProvider = new NodeDefsLocalProvider()

export { nodeDefsLocalProvider }
