import { SettingsLocalProvider } from '@/providers/settings-tauri'
import { createSelectors } from '@/utils/create-selectors'
import { BackgroundVariant, SelectionMode } from '@xyflow/react'
import { EdgeStyle } from '@/types/node-graph'
import { produce } from 'immer'
import { pathEqual } from 'path-equal'
import { create } from 'zustand'
import { persist, subscribeWithSelector } from 'zustand/middleware'

export interface SettingsState {
  // app
  language: string
  layout?: string
  background_color: string // css color
  undo_history_max_size: number

  // node-graph
  node_graph: {
    auto_execution: boolean
    parallel_execution_nodes: boolean
    snap_to_grid: boolean
    minimap: boolean
    selection_mode: SelectionMode
    background?: BackgroundVariant
    background_color: string // css color
    edge_style: EdgeStyle
  }

  // viewport
  viewport: {
    background_color: number[] //rgba,
    show_wireframe: boolean
    wireframe_color: number[] //rgba
    near_plane: number
    far_plane: number
    point_size: number
    point_color: number[]
    index_size: number
    show_point_index: boolean
    point_index_color: number[] //rgba
    show_point_normal: boolean
    point_normal_color: number[]
    point_normal_length: number
    show_prim_index: boolean
    prim_index_color: number[] //rgba
    show_prim_normal: boolean
    prim_normal_color: number[]
    prim_normal_length: number
    disable_light: boolean
    show_isolate_point: boolean
    force_show_point_default_color: boolean
  }

  // project
  recent_projects: string[]
}

export const defaultBackgroundColor = 'rgba(255,255,255,0)'
export const defaultBackgroundColor_Win10 = 'rgba(45,45,45,0.9)'

export const useSettingsStore = createSelectors(
  create<SettingsState>()(
    subscribeWithSelector(
      persist(
        // eslint-disable-next-line no-unused-vars
        (_) => ({
          // app
          language: 'en',
          background_color: 'rgba(255,255,255,0)',
          undo_history_max_size: 50,

          // node-graph
          node_graph: {
            auto_execution: true,
            parallel_execution_nodes: true,
            snap_to_grid: true,
            minimap: false,
            selection_mode: SelectionMode.Partial,
            background: BackgroundVariant.Dots,
            background_color: defaultBackgroundColor,
            edge_style: EdgeStyle.Bezier
          },

          // viewport
          viewport: {
            background_color: [0.18, 0.18, 0.18, 1.0],
            show_wireframe: false,
            wireframe_color: [0.18, 0.18, 0.18, 1.0],

            index_size: 15.0,

            point_size: 2,
            point_color: [1, 0.8, 0.6],
            show_point_index: false,
            point_index_color: [0.25, 0.75, 0.75, 1.0],
            show_point_normal: false,
            point_normal_color: [1, 1, 0],
            point_normal_length: 0.5,

            show_prim_index: false,
            prim_index_color: [1.0, 0.5, 1.0, 1.0],
            show_prim_normal: false,
            prim_normal_color: [0, 1, 1],
            prim_normal_length: 0.5,
            disable_light: false,
            show_isolate_point: true,
            force_show_point_default_color: false,

            near_plane: 0.01,
            far_plane: 1000.0
          },

          // project
          recent_projects: []
        }),
        {
          name: 'settings',
          // storage: createJSONStorage(() => localStorage),
          storage: SettingsLocalProvider,
          version: 6,
          migrate: (state: any, version: number) => {
            if (version < 1) {
              if (state && state.background_color === 'rgba(50,47,67,0.9)')
                state.background_color = defaultBackgroundColor_Win10
            }
            if (version < 2) {
              state.node_graph.parallel_execution_nodes = true
            }
            if (version < 3) {
              state.node_graph.edge_style = EdgeStyle.Bezier
            }
            if (version < 5) {
              state.viewport.background_color = [0.18, 0.18, 0.18, 1.0]
            }
            if (version < 6) {
              state.viewport.near_plane = 0.01
              state.viewport.far_plane = 1000
            }

            return state
          }
        }
      )
    )
  )
)

export const useSettingsStoreApi = () => {
  return useSettingsStore
}

export const setAppLanguage = (language: string) => {
  useSettingsStore.setState({ language })
}

export const setNodeGraphAutoExecution = (auto_execution: boolean) =>
  useSettingsStore.setState(
    produce((state) => {
      state.node_graph.auto_execution = auto_execution
    })
  )

export const setNodeGraphSnapToGrid = (snapToGrid: boolean) =>
  useSettingsStore.setState(
    produce((state) => {
      state.node_graph.snap_to_grid = snapToGrid
    })
  )

export const setNodeGraphselectionMode = (selectionMode: SelectionMode) =>
  useSettingsStore.setState(
    produce((state) => {
      state.node_graph.selection_mode = selectionMode
    })
  )

export const setNodeGraphBackground = (background: BackgroundVariant) =>
  useSettingsStore.setState(
    produce((state) => {
      state.node_graph.background = background
    })
  )

export const setMinimapVisible = (visible: boolean) =>
  useSettingsStore.setState(
    produce((state) => {
      state.node_graph.minimap = visible
    })
  )

export const addRecentProject = (url: string) => {
  const { recent_projects } = useSettingsStore.getState()

  const max_size = 10
  const current_size = recent_projects.length

  if (current_size == 0) {
    useSettingsStore.setState({ recent_projects: [url] })
  } else if (pathEqual(url, recent_projects[current_size - 1])) {
    return
  }

  let new_projects = recent_projects.filter((p) => !pathEqual(p, url)).concat(url)
  if (new_projects.length > max_size) {
    new_projects.splice(0, 1)
  }

  useSettingsStore.setState({ recent_projects: new_projects })
}

export const setLayout = (layout: string) => useSettingsStore.setState({ layout })