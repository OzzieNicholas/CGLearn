import DropdownMenu from '@/components/DropdownMenu'
import { AttributeSheet } from '@/features/AttributeSheet'
import { CommandPalette } from '@/features/CommandPalette'
import { NodeGraph } from '@/features/NodeGraph'
import { ParameterPanel } from '@/features/Parameter'
import { TitleBar } from '@/features/TitleBar'
import { Viewport } from '@/features/Viewport'
// import { Profiler } from '@/features/Profiler'
import { useI18n } from '@/locales'
import { emit as emitGlobalAction, listen as listenGlobalAction } from '@/stores/global-actions'
import { redo, undo } from '@/stores/global-undo-redo'
import { addBadge } from '@/stores/title-bar'
import { NodeGraphStoreProvider } from '@/stores/node-graph'
import {
  defaultBackgroundColor,
  defaultBackgroundColor_Win10,
  setLayout,
  useSettingsStore,
  useSettingsStoreApi
} from '@/stores/settings'
import { isMacOS, setupEnv } from '@/utils/env'
import NiceModal from '@ebay/nice-modal-react'
import DockLayout, {
  BoxData,
  DockContext,
  LayoutBase,
  LayoutData,
  PanelData,
  TabData,
  TabGroup
} from 'rc-dock'
import { createContext, useCallback, useContext, useEffect, useMemo, useRef, useState } from 'react'
import { useHotkeys } from 'react-hotkeys-hook'
import { useEffectOnce } from 'react-use'
import { getCurrentBackendAddr, reloadAll } from '@/commands'
import { listen } from '@tauri-apps/api/event'

import { IoMdAdd, IoMdClose } from 'react-icons/io'

import './css/rc-dock-dark.css'

function NodeGraphWithProvider() {
  return (
    <NodeGraphStoreProvider id={'node-graph-default'}>
      <NodeGraph />
    </NodeGraphStoreProvider>
  )
}

function ViewportWrapper() {
  return <Viewport />
}

function ParameterPanelWrapper() {
  return <ParameterPanel />
}

function AttributeSheetWrapper() {
  return <AttributeSheet />
}

// function ProfilerWrapper() {
//   return <Profiler />
// }

function PanelExtra({ panel_data, context }: { panel_data: PanelData; context: DockContext }) {
  let { closedTabs } = useContext(DockLayoutContext)
  const { t } = useI18n('window')

  const currentClosedTabs = getClosedTabs(context)
  if (currentClosedTabs.length != closedTabs.length) {
    closedTabs = currentClosedTabs
  }

  const numClosedTabs = closedTabs.length
  const numExistingTabs = allWindowTabNames.length - numClosedTabs

  return (
    <div className="flex">
      {numClosedTabs > 0 && (
        <DropdownMenu
          className="min-w-0 font-thin tracking-wide"
          menus={closedTabs.map((tab_name) => ({
            label: `window.${tab_name}`,
            data: tab_name
          }))}
          onClickItem={(item) => {
            const tab_name = item.data as string
            let tab = findDefaultTab(tab_name, defaultLayout.dockbox)
            if (tab) {
              tab.title = t(tab_name as any)
              context.dockMove(tab, panel_data, 'right')
            }
          }}
        >
          <div className="btn btn-ghost btn-xs min-h-0 rounded opacity-70">
            <IoMdAdd />
          </div>
        </DropdownMenu>
      )}
      {numExistingTabs > 1 && (
        <div
          className="btn btn-ghost btn-xs min-h-0 rounded"
          onClick={() => {
            if (panel_data && panel_data.activeId && numExistingTabs > 1) {
              const tab = context.find(panel_data.activeId)
              if (tab) {
                context.dockMove(tab as TabData, null, 'remove')
              }
            }
          }}
        >
          <IoMdClose className="opacity-50" />
        </div>
      )}
    </div>
  )
}

function App() {
  const [dataInitialized, setDataInitialized] = useState(false)
  const [layoutInitialized, setLayoutInitialized] = useState(false)
  const [dockLayout, setDockLayout] = useState<LayoutBase | undefined>()
  const [closedTabs, setClosedTabs] = useState<string[]>([])

  const { t } = useI18n('window')
  const dock = useRef<DockLayout>(null)

  const allInitialized = useMemo(() => {
    return dataInitialized && layoutInitialized
  }, [dataInitialized, layoutInitialized])

  // initialize
  useEffectOnce(() => {
    console.log('app initializing...')

    // init env
    setupEnv().then((env) => {
      // if win10
      if (env.os === 'windows' && parseInt(env.os_version.split('.')[2]) < 22000) {
        // if default
        if (useSettingsStoreApi().getState().background_color === defaultBackgroundColor) {
          useSettingsStoreApi().setState((state) => ({
            ...state,
            background_color: defaultBackgroundColor_Win10
          }))
        }
      }
      setDataInitialized(true)
    })

    // badges
    getCurrentBackendAddr().then((addr) => {
      if (addr && addr.length > 0) {
        addBadge({
          label: addr,
          color: '#baf2e7',
          info: `backend server: ${addr}`
        })
      }
    })

    // translate tab titles
    for (let panel of defaultLayout.dockbox.children) {
      let tabs: TabData[] | undefined = (panel as any)['tabs']
      if (tabs) {
        for (let tab of tabs) {
          tab.title = t(tab.id as any)
        }
      }
    }

    let unlisten1 = undefined

    // load layout
    if (useSettingsStore.persist.hasHydrated()) {
      const layout = useSettingsStoreApi().getState().layout
      if (layout) {
        setDockLayout(JSON.parse(layout))
      }
      setLayoutInitialized(true)
    } else {
      setLayoutInitialized(true)
      unlisten1 = useSettingsStore.persist.onFinishHydration((state) => {
        if (state.layout) {
          setDockLayout(JSON.parse(state.layout))
        }
      })
    }

    // events
    const unlisten2 = listenGlobalAction('resetLayout', () => {
      setDockLayout(defaultLayout)
      onLayoutChange(defaultLayout)
    })

    const unlisten3 = listen('ReloadGraphEvent', () => {
      reloadAll()
    })

    return () => {
      if (unlisten1) unlisten1()
      unlisten2()
      unlisten3.then((fn) => fn())
    }
  })

  const onLayoutChange = useCallback((layout: LayoutBase) => {
    setLayout(JSON.stringify(layout, replacer))
    setDockLayout(layout)
  }, [])

  useEffect(() => {
    setTimeout(() => {
      setClosedTabs(getClosedTabs(dock.current))
    }, 50)
  }, [dock, dockLayout])

  const meta_key = isMacOS() ? 'meta' : 'ctrl'
  useHotkeys(`${meta_key}+n`, () => emitGlobalAction('new'))
  useHotkeys(`${meta_key}+o`, () => emitGlobalAction('open'))
  useHotkeys(`${meta_key}+s`, () => emitGlobalAction('save'))
  useHotkeys(`${meta_key}+shift+s`, () => emitGlobalAction('saveAs'))

  useHotkeys(`${meta_key}+z`, undo)
  useHotkeys(`${meta_key}+shift+z`, redo)

  useHotkeys(`${meta_key}+x`, () => emitGlobalAction('cut'))
  useHotkeys(`${meta_key}+c`, () => emitGlobalAction('copy'))
  useHotkeys(`${meta_key}+v`, () => emitGlobalAction('paste'))
  useHotkeys(`${meta_key}+p`, () => NiceModal.show(CommandPalette))

  useEffect(() => {
    if (dock.current) {
      const update = () => {
        const all_tabs = Array.from(
          dock.current!.getRootElement().querySelectorAll('.dock-tab-btn .drag-initiator')
        )
        let ok = false
        for (let tab of all_tabs) {
          if (!tab.parentElement) {
            continue
          }
          let parent_id: string = tab.parentElement.id
          if (parent_id) {
            const tab_name = allWindowTabNames.find((n) => parent_id.endsWith(n))
            if (tab_name) {
              tab.textContent = t(tab_name as any)
              ok = true
            }
          }
        }
        return ok
      }
      setTimeout(update, 50)
    }
  }, [t, dock, allInitialized, dockLayout])

  const backgroundColor = useSettingsStore((state) => state.background_color)

  if (!allInitialized) {
    return null
  }

  return (
    <div style={{ background: backgroundColor }} className="h-screen w-screen" data-theme="dark">
      <TitleBar />
      <DockLayoutContext.Provider value={{ closedTabs }}>
        <DockLayout
          ref={dock}
          defaultLayout={defaultLayout}
          layout={dockLayout}
          onLayoutChange={onLayoutChange}
          // dropMode="edge"
          groups={groups}
          style={{
            position: 'absolute',
            left: '0.25rem',
            top: '2rem',
            right: '0.25rem',
            bottom: '0.25rem',
            background: 'transparent'
          }}
        />
      </DockLayoutContext.Provider>
    </div>
  )
}

export default App

const DockLayoutContext = createContext<{ closedTabs: string[] }>(null!)

const getClosedTabs = (context: DockContext | DockLayout | undefined | null) => {
  return context ? allWindowTabNames.filter((id) => !context.find(id)) : []
}

const findDefaultTab = (id: string, box: BoxData): TabData | undefined => {
  for (const item of box.children) {
    if ((item as BoxData).children !== undefined) {
      const tab = findDefaultTab(id, item as BoxData)
      if (tab !== undefined) {
        return tab
      }
    } else {
      for (const tab of (item as PanelData).tabs) {
        if (tab.id === id) {
          return tab
        }
      }
    }
  }
  return undefined
}

const allWindowTabNames = [
  'node-graph',
  '3d-viewport',
  'parameter-panel',
  'attribute-sheet'
  // 'performance-profiler'
]

const replacer = (key: string, value: any) => {
  if (key === 'parent') return null
  return value
}

let groups: Record<string, TabGroup> = {
  'default-group': {
    floatable: true,
    newWindow: false,
    maximizable: false,
    panelExtra: (panel_data, context) => {
      return <PanelExtra panel_data={panel_data} context={context} />
    }
  }
}

let defaultLayout: LayoutData = {
  dockbox: {
    mode: 'horizontal',
    children: [
      {
        tabs: [
          {
            id: '3d-viewport',
            title: '3d-viewport',
            content: ViewportWrapper,
            group: 'default-group',
            closable: false
          }
          // {
          //   id: 'performance-profiler',
          //   title: 'performance-profiler',
          //   content: ProfilerWrapper,
          //   group: 'default-group',
          //   closable: false
          // }
        ]
      },
      {
        tabs: [
          {
            id: 'node-graph',
            title: 'node-graph',
            content: NodeGraphWithProvider,
            group: 'default-group',
            closable: false
          }
        ]
      },
      {
        mode: 'vertical',
        children: [
          {
            tabs: [
              {
                id: 'parameter-panel',
                title: 'parameter-panel',
                content: ParameterPanelWrapper,
                group: 'default-group',
                closable: false
              }
            ]
          },
          {
            tabs: [
              {
                id: 'attribute-sheet',
                title: 'attribute-sheet',
                content: AttributeSheetWrapper,
                group: 'default-group',
                closable: false
              }
            ]
          }
        ]
      }
    ]
  }
}

window.addEventListener('contextmenu', (e) => e.preventDefault())
window.addEventListener('keydown', (e) => {
  if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'a') {
    e.preventDefault()
  } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'p') {
    e.preventDefault()
  }
})
