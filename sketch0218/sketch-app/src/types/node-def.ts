export enum NodeKind {
  Default = 0,
  Foreach,
  Switch,
  Subgraph,
  SubgraphInput,
  SubgraphOutput,
  Nda,
  UnrealEngine
}

export type NodePortDef = {
  name: string
  type: number
}

export type NodeDef = {
  type_name: string
  category: string
  kind: NodeKind
  inputs: NodePortDef[]
  outputs: NodePortDef[]
  icon?: string
  type_version: number
}

export type NodeDefsState = {
  defs: NodeDef[]
}
