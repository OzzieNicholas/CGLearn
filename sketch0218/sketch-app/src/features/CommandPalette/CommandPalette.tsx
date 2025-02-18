import { SearchNodeDesc, SearchParamDesc } from '@/commands/index.types'
import { Dialog, DialogContent, DialogTitle } from '@/components/Dialog'
import { useI18n } from '@/locales'
import { emit as emitGlobalAction } from '@/stores/global-actions'
import { useNodeGraphBridgeStoreApi } from '@/stores/node-graph-bridge'
import { join } from '@/utils/cn'
import { delay } from '@/utils/functions'
import NiceModal, { useModal } from '@ebay/nice-modal-react'
import { useCallback, useEffect, useRef, useState } from 'react'
import { Case, If, Switch, Then } from 'react-if'
import { useDebounce } from 'react-use'
import CommandItem from './CommandItem'
import NodeItem from './NodeItem'
import ParamItem from './ParamItem'
import { SearchCommandDesc, searchCommands, searchNodes, searchParams } from './utils'

import { CiSearch } from 'react-icons/ci'
import { FaCircleNodes } from 'react-icons/fa6'
import { HiOutlineCommandLine } from 'react-icons/hi2'
import { PiListDashesThin } from 'react-icons/pi'
import { CiViewTable } from 'react-icons/ci'

const Spacer = () => <div className="h-px w-full bg-white/10" />

const filterKeywords: Record<string, string> = {
  n: 'node',
  node: 'node',
  p: 'parameter',
  parameter: 'parameter',
  param: 'parameter',
  c: 'command',
  command: 'command',
  cmd: 'command'
}

type SearchType = 'node' | 'parameter' | 'command' | 'line'

type Filter = {
  name: SearchType
  enabled: boolean
  items: any[]
  icon: React.ReactNode
}

const doSearch = async (search_text: string, filters: Filter[]) => {
  const storeApi = useNodeGraphBridgeStoreApi()
  const { node_graph, graph_id } = storeApi.getState()
  const node_space =
    node_graph !== undefined && graph_id != 0 ? node_graph.getState().node_space : null

  filters = await Promise.all(
    filters.map(async (f) => {
      if (!f.enabled) {
        return {
          ...f,
          items: []
        }
      }

      if (f.name === 'node') {
        if (!node_space) {
          return {
            ...f,
            items: []
          }
        }
        return {
          ...f,
          items: await searchNodes(search_text, graph_id!, node_space.id)
        }
      } else if (f.name === 'parameter') {
        if (!node_space) {
          return {
            ...f,
            items: []
          }
        }
        return {
          ...f,
          items: await searchParams(search_text, graph_id!, node_space.id)
        }
      } else if (f.name === 'command') {
        return {
          ...f,
          items: searchCommands(search_text)
        }
      } else if (f.name === 'line') {
        const lineIndex = parseInt(search_text)

        if (!isNaN(lineIndex)) {
          return {
            ...f,
            items: [lineIndex]
          }
        }
      }
      return f
    })
  )
  return filters
}

const hasItem = (filters: Filter[]) => {
  return filters.some((f) => f.enabled && f.items.length > 0)
}

let filterState = {
  node: true,
  parameter: false,
  command: true,
  line: false
}

export const CommandPalette = NiceModal.create(() => {
  const modal = useModal()
  const { t } = useI18n()
  const inputRef = useRef<HTMLInputElement>(null)

  const [filters, setFilters] = useState<Filter[]>([
    {
      name: 'node',
      enabled: filterState['node'],
      items: [],
      icon: <FaCircleNodes className="p-0.5" />
    },
    { name: 'parameter', enabled: filterState['parameter'], items: [], icon: <PiListDashesThin /> },
    { name: 'command', enabled: filterState['command'], items: [], icon: <HiOutlineCommandLine /> },
    { name: 'line', enabled: filterState['line'], items: [], icon: <CiViewTable /> }
  ])
  const [searchText, setSearchText] = useState<string>('')

  const [activeItem, setActiveItem] = useState<{
    type: SearchType | 'none'
    idx: number
  }>({
    type: 'none',
    idx: 0
  })

  const [,] = useDebounce(
    () => {
      const space = searchText.indexOf(' ')
      let tempFilters = filters.map((f) => f)

      let currentSearchText = searchText

      if (space > 0) {
        // update filter
        const keyword = searchText.substring(0, space).toLowerCase()
        if (filterKeywords[keyword] !== undefined) {
          const filterName = filterKeywords[keyword]
          tempFilters.forEach((f) => {
            if (f.enabled) {
              if (f.name !== filterName) {
                f.enabled = false
              }
            } else if (f.name === filterName) {
              f.enabled = true
            }
          })

          currentSearchText = searchText.substring(space + 1)
        }
      }

      doSearch(currentSearchText, tempFilters).then((newFilters) => {
        setFilters(newFilters)
      })
    },
    100,
    [searchText]
  )

  useEffect(() => {
    let item: typeof activeItem = {
      type: 'none',
      idx: 0
    }
    for (const filter of filters) {
      if (filter.enabled && filter.items.length > 0) {
        item.type = filter.name
        break
      }
    }
    setActiveItem(item)
  }, [filters, setActiveItem])

  const onClickItem = useCallback(
    (type: SearchType, item: SearchNodeDesc | SearchParamDesc | SearchCommandDesc | number) => {
      modal.remove()

      delay(100).then(() => {
        switch (type) {
          case 'node':
            item = item as SearchNodeDesc
            emitGlobalAction('jumpToNode', item.path)
            break
          case 'parameter':
            item = item as SearchParamDesc
            emitGlobalAction('jumpToNode', item.node_path)
            break
          case 'line':
            item = item as number
            emitGlobalAction('jumpToLine', item)
            break
          case 'command':
            item = item as SearchCommandDesc
            if (item.action !== undefined) {
              item.action()
            }
            break
        }
      })
    },
    [modal]
  )

  return (
    <Dialog
      open={modal.visible}
      // modal={false}
      onOpenChange={(open) => {
        if (!open) {
          modal.remove()
        }
      }}
    >
      <DialogContent
        className="top-[7%] mx-auto w-2/5 min-w-[30rem] max-w-2xl translate-y-0 gap-0 overflow-hidden rounded-xl border-1 border-solid  border-white/10 p-0 py-3"
        // style={{
        //   background:
        //     'linear-gradient(320deg,rgba(27, 27, 27, 0.8) 0 %,rgba(20, 20, 20, 0.9) 100 %)'
        // }}
        onKeyDownCapture={() => {
          if (inputRef.current && inputRef.current !== document.activeElement) {
            inputRef.current.focus()
          }
        }}
      >
        <DialogTitle />

        {/* search bar */}
        <div className="flex items-center gap-1 px-3 pb-3">
          <CiSearch className="size-6 opacity-50" />

          <input
            type="text"
            autoFocus
            ref={inputRef}
            placeholder={t('common.type-search-here')}
            className="rounded border-none bg-transparent px-1 text-lg tracking-wide text-text-primary outline-none selection:bg-text-select/40"
            onKeyDown={(e) => {
              if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'a') {
                e.stopPropagation()
              } else if (e.key === 'Escape') {
                // close modal
                e.stopPropagation()
                modal.remove()
              } else if (e.key === 'Enter') {
                // run command
                e.stopPropagation()
                e.preventDefault()
                const filter = filters.find(
                  (f) => f.enabled && f.name == activeItem.type && f.items.length > activeItem.idx
                )
                if (filter != undefined) {
                  onClickItem(filter.name, filter.items[activeItem.idx])
                }
              } else if (e.key === 'ArrowUp' || e.key === 'ArrowDown') {
                // navigate
                const enabledFilters = filters.filter((f) => f.enabled && f.items.length > 0)
                let filterIdx = enabledFilters.findIndex((f) => f.name == activeItem.type)
                if (filterIdx >= 0) {
                  let filter = enabledFilters[filterIdx]

                  const step = e.key.endsWith('Up') ? -1 : 1
                  let newIdx = activeItem.idx + step

                  if (newIdx >= 0 && newIdx < filter.items.length) {
                    setActiveItem({
                      type: filter.name,
                      idx: newIdx
                    })
                  } else {
                    filterIdx = (filterIdx + step + enabledFilters.length) % enabledFilters.length
                    filter = enabledFilters[filterIdx]
                    newIdx = step > 0 ? 0 : filter.items.length - 1
                    setActiveItem({
                      type: filter.name,
                      idx: newIdx
                    })
                  }
                }
              }
            }}
            onChange={(e) => {
              setSearchText(e.target.value)
            }}
          />
        </div>
        <If condition={filters.length > 0}>
          <Then>
            <Spacer />

            {/* filters */}
            {/* <span className="px-3 py-1 text-text-primary/40 text-sm font-thin font-sans">
              I'm looking for...
            </span> */}

            <div className="flex gap-2 p-3">
              {filters.map((filter) => (
                <div
                  className={join(
                    'rounded-3xl px-3 py-1 active:bg-btn-active/80 transition-colors-opacity cursor-pointer select-none border-1 border-white/20 text-xs flex items-center gap-1',
                    filter.enabled
                      ? 'bg-btn-active/50 hover:bg-btn-active/60'
                      : 'bg-white/5 hover:bg-white/10'
                  )}
                  onClick={() => {
                    let search = false
                    let tempFilters = filters.map((f) => {
                      if (f.name === filter.name) {
                        f.enabled = !f.enabled
                        if (f.enabled) {
                          search = true
                        }
                        filterState[f.name] = f.enabled
                      }
                      return f
                    })
                    if (search) {
                      doSearch(searchText, tempFilters).then((newFilters) => {
                        setFilters(newFilters)
                      })
                    } else {
                      setFilters(tempFilters)
                    }
                  }}
                  key={filter.name}
                >
                  {filter.icon}
                  <span className="text-text-primary/80">
                    {t(('command-palette.' + filter.name) as any)}
                  </span>
                </div>
              ))}
            </div>

            {/* search results */}
            <If condition={hasItem(filters)}>
              <Then>
                <div className="max-h-screen overflow-y-auto">
                  {filters
                    .filter((f) => f.enabled && f.items.length > 0)
                    .map((filter, idx1) => (
                      <Switch key={idx1}>
                        <Case condition={filter.name === 'node'}>
                          {() => (
                            <>
                              <span className="flex items-center gap-1 px-3 text-sm text-text-primary/30">
                                {filter.icon} <span>{t('command-palette.node')}</span>
                              </span>
                              <div className="max-h-[240px] overflow-y-auto">
                                {filter.items.map((item, idx2) => (
                                  <NodeItem
                                    nodeDesc={item}
                                    onClick={() => onClickItem(filter.name, item)}
                                    active={
                                      activeItem.type == filter.name && activeItem.idx == idx2
                                    }
                                    key={idx2}
                                  />
                                ))}
                              </div>
                            </>
                          )}
                        </Case>
                        <Case condition={filter.name === 'parameter'}>
                          {() => (
                            <>
                              <span className="flex items-center gap-1 px-3 text-sm text-text-primary/30">
                                {filter.icon} <span>{t('command-palette.parameter')}</span>
                              </span>
                              <div className="max-h-[240px] overflow-y-auto">
                                {filter.items.map((item, idx2) => (
                                  <ParamItem
                                    paramDesc={item}
                                    onClick={() => onClickItem(filter.name, item)}
                                    active={
                                      activeItem.type == filter.name && activeItem.idx == idx2
                                    }
                                    key={idx2}
                                  />
                                ))}
                              </div>
                            </>
                          )}
                        </Case>
                        <Case condition={filter.name === 'command'}>
                          {() => (
                            <>
                              <span className="flex items-center gap-1 px-3 text-sm text-text-primary/30">
                                {filter.icon} <span>{t('command-palette.command')}</span>
                              </span>
                              <div className="max-h-[150px] overflow-y-auto">
                                {filter.items.map((item, idx2) => (
                                  <CommandItem
                                    cmdDesc={item}
                                    onClick={() => onClickItem(filter.name, item)}
                                    active={
                                      activeItem.type == filter.name && activeItem.idx == idx2
                                    }
                                    key={idx2}
                                  />
                                ))}
                              </div>
                            </>
                          )}
                        </Case>
                        <Case condition={filter.name === 'line'}>
                          {() => (
                            <>
                              <span className="flex items-center gap-1 px-3 text-sm text-text-primary/30">
                                {filter.icon} <span>{t('command-palette.line')}</span>
                                <div
                                  className="py-1.5 px-3 text-sm text-text-primary/50"
                                  onClick={() => {
                                    onClickItem(filter.name, filter.items[0])
                                  }}
                                >
                                  {filter.items[0]}
                                </div>
                              </span>
                            </>
                          )}
                        </Case>
                      </Switch>
                    ))}
                </div>
              </Then>
              {/* <Else>
                <div className="pt-3 text-lg text-text-primary/50 text-center">
                  {t('menu.no-items')}
                </div>
              </Else> */}
            </If>
          </Then>
        </If>
      </DialogContent>
    </Dialog>
  )
})
