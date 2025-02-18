import {
  LayoutType,
  ParamType,
  getNodeDef,
  getNodePath,
  getNodesByGraphId,
  getParamEnumList,
  pasteNodes as pasteNodes_
} from '@/commands'
import {
  GetNodesByGraphIdResult,
  ParamItemDef,
  LayoutData,
  NodeData as NodeDataType,
  ParameterData,
  NodeDef,
  ParamItemDefType,
  Node_
} from '@/commands/index.types'

import { nodeTypesKeys } from '@/features/NodeGraph/context'
import { useNodeDefsStoreApi } from '@/stores/node-defs'
import { convertFileSrc } from '@tauri-apps/api/core'
import { iconTypes } from '@/constant/node-resource'
import { NodeKind } from '@/types/node-def'
import { Edge, NNode, NodeData } from '@/types/node-graph'
import { rgbaToHexa } from '@uiw/color-convert'
import { MakeIcon } from '@/components/Icon'
import { XYPosition } from '@xyflow/react'
import { Mutex } from 'async-mutex'
import uuid from './uuid'
import { findParamItemDef } from './parameter'

async function parseGraph(graph_id: number, src: GetNodesByGraphIdResult) {
  let defs: Record<string, NodeDef> = {}
  let node_ids: Record<string, string> = {}

  let nodes: NNode[] = []
  let edges: Edge[] = []

  const storeApi = useNodeDefsStoreApi()

  for (const node of src.nodes) {
    const data = node.node_data
    const node_id = data.id

    let def = defs[node.type_name]
    if (!def) {
      def =
        storeApi.defs.find(
          (d) => d.type_name === node.type_name && d.type_version === node.node_data.type_version
        ) || (await getNodeDef([graph_id, node.type_name, node.node_data.type_version]))
      defs[node.type_name] = def
    }

    const show_version = hasDifferentVersion(src.nodes, node.type_name)

    const new_node = generateNodeFromData(node_id, data, def, show_version)
    nodes.push(new_node)
    node_ids[data.name] = new_node.id
  }

  for (const links of src.links) {
    const target = node_ids[links.name]

    for (const link of links.node_links_in) {
      edges.push({
        id: uuid(),
        source: node_ids[link.node_name],
        sourceHandle: link.out_port_id.toString(),
        target: target,
        targetHandle: link.port_id.toString()
      })
    }
  }
  return { nodes, edges }
}

export async function loadSubGraph(
  graph_id: number,
  subgraph_id: number
): Promise<{
  nodes: NNode[]
  edges: Edge[]
}> {
  const result = await getNodesByGraphId([graph_id, subgraph_id])
  if (!result) return { nodes: [], edges: [] }
  return parseGraph(graph_id, result)
}

export async function pasteNodes(
  graph_id: number,
  subgraph_id: number,
  data: string,
  move_nodes: boolean,
  pos?: XYPosition
): Promise<{
  nodes: NNode[]
  edges: Edge[]
}> {
  const result = await pasteNodes_([
    graph_id,
    subgraph_id,
    data,
    move_nodes,
    pos ? pos.x : 0,
    pos ? pos.y : 0
  ])
  if (!result) return { nodes: [], edges: [] }
  return parseGraph(graph_id, result)
}

export function handleCompare(handle_a?: string | null, handle_b?: string | null): boolean {
  if (handle_a === handle_b) {
    return true
  }
  if ((!handle_a || handle_a === '0') && (!handle_b || handle_b === '0')) {
    return true
  }
  return false
}

export function handleIndex(handle?: string | null): number {
  if (!handle) {
    return 0
  }
  return parseInt(handle)
}

export function isSubgraphOrNda(kind?: NodeKind): boolean {
  if (!kind) {
    return false
  }
  return kind == NodeKind.Subgraph || kind == NodeKind.Nda
}

export function isSubgraphOrNdaEditable(editable?: boolean): boolean {
  if (editable === undefined || editable) {
    return true
  }
  return false
}

export async function getNodeSpacePathItems(node_space_id: number): Promise<string[]> {
  if (node_space_id != 0) {
    const path = await getNodePath([0, node_space_id])
    const items = ['/', ...path.split('/').filter((item) => item)]
    return items
  }
  return []
}

export function generateNodeFromData(
  node_id: number,
  data: NodeDataType,
  def: NodeDef,
  show_version: boolean
): NNode {
  const inputs = [...new Array(data.input_num)].map((_, i) => {
    const input = def.inputs[i]
    if (input) {
      return {
        name: input.name,
        type: input.type
      }
    }
    return {
      name: `input-${i}`,
      type: 0
    }
  })

  const outputs = [...new Array(data.output_num)].map((_, i) => {
    const output = def.outputs[i]
    if (output) {
      return {
        name: output.name,
        type: output.type
      }
    }
    return {
      name: `output-${i}`,
      type: 0
    }
  })

  return {
    id: uuid(),
    type: nodeTypesKeys.has(def.type_name) ? def.type_name : 'common',
    position: { x: data.pos_x, y: data.pos_y },
    data: {
      id: node_id,
      label: data.name,
      type_name: def.type_name,
      color: rgbaToHexa({
        r: data.color[0] * 255.0,
        g: data.color[1] * 255.0,
        b: data.color[2] * 255.0,
        a: data.color[3]
      }),
      icon: def.type_name,
      inputs,
      outputs,
      bypassed: data.bypass,
      is_display: data.display,
      logs: data.logs,

      kind: def.kind,
      category: def.category,

      editable: data.editable,
      desc: 'No description',

      version: data.type_version,
      show_version
    }
  }
}

export async function updateDefEnumList(def: ParamItemDef, graph_id: number, node: NodeData) {
  if (!def.children || !Array.isArray(def.children)) {
    return
  }

  for (const child_param_item_def of def.children) {
    if (child_param_item_def.data.item_type === ParamItemDefType.Parameter) {
      const paramDef = child_param_item_def.data.def as ParameterData
      if (paramDef.type === ParamType.Enum) {
        if (paramDef.enum_items === undefined || paramDef.enum_items.length == 0) {
          const enum_items = await getParamEnumList([graph_id, node.id, paramDef.name])
          if (enum_items.length > 0) {
            paramDef.enum_items = enum_items
          }
        }
      } else if (paramDef.type === ParamType.List) {
        await updateDefEnumList(child_param_item_def, graph_id, node)
      }
    } else if (child_param_item_def.data.def !== undefined) {
      // Layout
      await updateDefEnumList(child_param_item_def, graph_id, node)
    }
  }
}

export function initDefRootLayout(def: NodeDef): ParamItemDef[] {
  const root_parameter_def: LayoutData = {
    name: 'root',
    type: LayoutType.Vertical,
    editable: true,
    visible: true,
    builtin: true
  }

  const root = [
    {
      name: 'root',
      data: {
        item_type: ParamItemDefType.Layout,
        def: root_parameter_def
      },
      children: def.children || []
    }
  ]

  def.children = root

  return root
}

export function mergeItemDefs(
  from?: ParamItemDef[],
  to?: ParamItemDef[]
): ParamItemDef[] | undefined {
  if (!from || !to) return to

  for (let i = 0; i < from.length; i++) {
    const from_def = from[i]
    const to_def = findParamItemDef(from_def.name, to)
    if (to_def === undefined) {
      to.splice(i, 0, from_def)
    } else {
      mergeItemDefs(from_def.children, to_def.children)
    }
  }

  return to
}

const nodeIconMutex = new Mutex()

export async function loadNodeIcon(node_type_name: string, icon_path: string): Promise<void> {
  if (node_type_name.length == 0 || icon_path.length == 0) {
    return
  }

  let content: string | undefined = undefined
  try {
    content = convertFileSrc(icon_path)
  } catch (e) {
    console.log(e)
  }
  if (!content) {
    return
  }
  nodeIconMutex.runExclusive(async () => {
    iconTypes[node_type_name] = MakeIcon({ content })
  })
}
export function hasDifferentVersion(nodes: Node_[], type_name: string): boolean {
  let has_different_version = false
  nodes
    .filter((node) => node.type_name == type_name)
    .reduce((prev, curr) => {
      if (prev === -1) return curr.node_data.type_version
      if (prev !== curr.node_data.type_version) {
        has_different_version = true
      }
      return curr.node_data.type_version
    }, -1)

  return has_different_version
}
