import { getAllMeshesMemoryUsageInfo, writeTextFile } from '@/commands/index'
import { AttribMemoryUsageInfo, NodeMeshMemoryUsageInfo } from '@/commands/index.types'
import { useNodeGraphBridgeStoreApi } from '@/stores/node-graph-bridge'
import { save as saveFileDialog } from '@tauri-apps/plugin-dialog'

type NodeInfo = {
  path: string
  type: string
  port: number
  execution_time: number
}

type AttribInfo = AttribMemoryUsageInfo & {
  nodes: NodeInfo[]
}

type TopoInfo = {
  points_size: number
  vertices_size: number
  prims_size: number
  num_points: number
  num_vertices: number
  num_prims: number
  size: number
  nodes: NodeInfo[]
}

export const exportGraphDebugInfo = async () => {
  const { graph_id } = useNodeGraphBridgeStoreApi().getState()
  if (!graph_id) {
    return
  }

  // open a dialog
  const path = await saveFileDialog({
    filters: [
      {
        name: 'Json file(.json)',
        extensions: ['json']
      }
    ]
  })
  if (!path) {
    return
  }

  const info = await getAllMeshesMemoryUsageInfo([graph_id])
  if (!info) {
    return
  }
  const mesh_infos: NodeMeshMemoryUsageInfo[] = info.sort((a, b) => b.size - a.size)

  let total_size = 0

  let attrib_unique_infos: Record<number, AttribInfo> = {}
  let topo_unique_infos: Record<number, TopoInfo> = {}

  const appendAttrib = (attrib_info: AttribMemoryUsageInfo, node: NodeInfo) => {
    let item = attrib_unique_infos[attrib_info.version]
    if (item) {
      item.nodes.push(node)
    } else {
      item = {
        ...attrib_info,
        nodes: [node]
      }
      delete (item as any).version
      attrib_unique_infos[attrib_info.version] = item
    }
  }

  for (let mesh_info of mesh_infos) {
    const node = {
      path: mesh_info.path,
      type: mesh_info.type,
      port: mesh_info.port,
      execution_time: mesh_info.execution_time
    }

    let item = topo_unique_infos[mesh_info.version]
    if (item) {
      item.nodes.push(node)
    } else {
      topo_unique_infos[mesh_info.version] = {
        points_size: mesh_info.points_size,
        vertices_size: mesh_info.vertices_size,
        prims_size: mesh_info.prims_size,
        num_points: mesh_info.num_points,
        num_vertices: mesh_info.num_vertices,
        num_prims: mesh_info.num_prims,
        size: mesh_info.points_size + mesh_info.vertices_size + mesh_info.prims_size,
        nodes: [node]
      }
    }

    for (let attrib_info of mesh_info.point_attribs) {
      appendAttrib(attrib_info, node)
      delete (attrib_info as any).version
    }
    for (let attrib_info of mesh_info.vertex_attribs) {
      appendAttrib(attrib_info, node)
      delete (attrib_info as any).version
    }
    for (let attrib_info of mesh_info.prim_attribs) {
      appendAttrib(attrib_info, node)
      delete (attrib_info as any).version
    }
    for (let attrib_info of mesh_info.global_attribs) {
      appendAttrib(attrib_info, node)
      delete (attrib_info as any).version
    }

    delete (mesh_info as any).version
  }

  let attrib_infos: AttribInfo[] = []
  Object.entries(attrib_unique_infos).forEach(([, value]) => {
    total_size += value.size
    attrib_infos.push(value)
  })
  attrib_infos = attrib_infos.sort((a, b) => b.size - a.size)

  let topo_infos: TopoInfo[] = []
  Object.entries(topo_unique_infos).forEach(([, value]) => {
    total_size += value.size
    topo_infos.push(value)
  })
  topo_infos = topo_infos.sort((a, b) => b.size - a.size)

  const content = JSON.stringify(
    {
      total_size,
      topology: topo_infos,
      attribute: attrib_infos,
      mesh: mesh_infos
    },
    null,
    2
  )

  await writeTextFile(path, content)
}
