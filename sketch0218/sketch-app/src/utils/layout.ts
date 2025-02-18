import { useNodeGraphStoreApi } from '@/stores/node-graph'
import { Edge, Node as Node_, getNodesBounds, useReactFlow } from '@xyflow/react'
import ELK from 'elkjs/lib/elk.bundled.js'
import { useCallback } from 'react'

const elk = new ELK()

function getConnectedEdges(nodes: Node_[], edges: Edge[]) {
  const nodeIds = new Set()
  nodes.forEach((node) => {
    nodeIds.add(node.id)
  })

  return edges.filter((edge) => nodeIds.has(edge.source) && nodeIds.has(edge.target))
}

const useLayoutedElements = () => {
  const { getNodes, getEdges, fitView } = useReactFlow()
  const storeApi = useNodeGraphStoreApi()

  const defaultOptions = {
    'elk.direction': 'DOWN',
    'elk.layered.spacing.nodeNodeBetweenLayers': 100,
    'elk.spacing.nodeNode': 80
  }

  const layoutedElements = useCallback((layered: boolean = true) => {
    const layoutOptions = {
      ...defaultOptions,
      'elk.algorithm': layered ? 'org.eclipse.elk.layered' : 'org.eclipse.elk.force'
    }
    let graph = {
      id: 'root',
      layoutOptions: layoutOptions,
      children: getNodes(),
      edges: getEdges()
    }

    for (let n of graph.children) {
      n.width = n.measured?.width
      n.height = n.measured?.height
    }

    const selected = graph.children.filter((n) => n.selected)
    if (selected.length > 1) {
      graph.children = selected
    } else if (graph.children.length < 2) {
      return
    }

    const bounds = getNodesBounds(graph.children, { nodeOrigin: [0, 0] })
    const center = [bounds.x, bounds.y]

    const layout_all = selected.length <= 1

    if (!layout_all) {
      graph.edges = getConnectedEdges(graph.children, graph.edges)
    }

    elk.layout(graph as any).then(({ children }) => {
      // By mutating the children in-place we saves ourselves from creating a
      // needless copy of the nodes array.
      ;(children as any[]).forEach((node) => {
        node.position = { x: node.x + center[0], y: node.y + center[1] }
      })

      storeApi.getState().setNodesPosition(children as any)
    })

    if (layout_all) {
      fitView()
    }
  }, [])

  return { layoutedElements }
}

export default useLayoutedElements
