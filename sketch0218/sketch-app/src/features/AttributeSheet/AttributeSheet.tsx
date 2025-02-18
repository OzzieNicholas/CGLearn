import { batchGetNodeOutputMeshAttrs } from '@/commands'
import Select from '@/components/Select'
import { presets } from '@/css/presets'
import { useI18n } from '@/locales'
import {
  NodeGraphBridgeState,
  setNode,
  useNodeGraphBridgeStore,
  useNodeGraphBridgeStoreApi
} from '@/stores/node-graph-bridge'
import { AttribInfo as NousAttribInfo } from '@/types/node-graph'
import { cn, join } from '@/utils/cn'
import { Dropdown, DropdownItem, DropdownMenu, DropdownTrigger, Tab, Tabs } from '@nextui-org/react'
import { listen } from '@tauri-apps/api/event'
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { Else, If, Then } from 'react-if'
import { useDebounce, useEffectOnce } from 'react-use'
import AutoSizer from 'react-virtualized-auto-sizer'
import { VariableSizeGrid } from 'react-window'
import InfiniteLoader from 'react-window-infinite-loader'

import { FaArrowDownLong, FaArrowUpLong } from 'react-icons/fa6'
import { MdFilterList } from 'react-icons/md'
import { VscGoToSearch } from 'react-icons/vsc'

import { emit as emitGlobalAction, listen as listenGlobalAction } from '@/stores/global-actions'
import NumberInput, { NumberValueType } from '@/components/NumberInput'

type AttribInfo = {
  display_name: string
  name: string
  type: string
  num_components: number
  component_idx: number
}

type MeshInfo = {
  num_points: number
  num_prims: number
  num_vertices: number
  point: AttribInfo[]
  vertex: AttribInfo[]
  prim: AttribInfo[]
  global: AttribInfo[]
}

type AttribsFilter = Record<string, string[]>

const selector = (state: NodeGraphBridgeState) => ({
  node: state.node,
  node_graph: state.node_graph
})

let widthCache: Record<string, Record<string, number>> = {}

function getAttribs<T extends keyof MeshInfo>(data: MeshInfo, key: T, filter?: AttribsFilter) {
  const result = (data[key] as AttribInfo[]).sort((a, b) => a.name.localeCompare(b.name))
  if (!filter) return result
  const keys: string[] = filter[key] || []
  return result.filter((a) => !keys.includes(a.name))
}

function getFilterSelectedAttribs(data: MeshInfo, cls: string, filter: AttribsFilter) {
  const keys: string[] = filter[cls] || []
  let names = new Set<string>()
  let result = []
  for (const attrib of getAttribs(data, cls as any) as AttribInfo[]) {
    if (!names.has(attrib.name) && !keys.includes(attrib.name)) {
      names.add(attrib.name)
      result.push(attrib.name)
    }
  }
  return result
}

function getFilterAllAttribs(data: MeshInfo, cls: string) {
  let names = new Set()
  let result = []
  for (const attrib of getAttribs(data, cls as any) as AttribInfo[]) {
    if (!names.has(attrib.name)) {
      names.add(attrib.name)
      result.push(attrib.name)
    }
  }
  return result.sort((a, b) => a.localeCompare(b))
}

function getNumElements(data: MeshInfo, key: string) {
  if (key === 'point') return data.num_points
  if (key === 'prim') return data.num_prims
  if (key === 'vertex') return data.num_vertices
  return 1
}

function AttribClassSheet({
  attribClass,
  numElements,
  attribs,
  displayIndex,
  lineHeight = 20
}: {
  attribClass: string
  numElements: number
  attribs: AttribInfo[]
  displayIndex: number
  lineHeight?: number
}) {
  const { node, node_graph } = useNodeGraphBridgeStore(selector)
  const [cache, setCache] = useState<Record<number, string>>({})
  const [sortParam, setSortParam] = useState<{
    name: string
    component_idx: number
    type: 'up' | 'down'
  } | null>(null)
  const [colWidthDirty, setColWidthDirty] = useState<boolean>(false)

  const colWidth = useRef<Record<number, number>>({})

  const infiniteLoaderRef = useRef<InfiniteLoader>(null)
  const gridRef = useRef<VariableSizeGrid<any> | null>(null)

  const numCols = attribs.length + 1
  const numRows = numElements + 1
  const numItems = numCols * numRows

  const batchParams = useMemo(() => {
    let sort_params = undefined
    if (sortParam) {
      if (
        attribs.find(
          (a) => a.name === sortParam.name && a.component_idx == sortParam.component_idx
        ) !== undefined
      ) {
        sort_params = sortParam
      }
    }
    return [
      node_graph!.getState().getNodeGraphId(),
      node!.id,
      displayIndex,
      attribClass,
      {
        begin: 0,
        end: 0,
        full_precision: false,
        attrib_names: attribs.map((a) => a.name),
        component_indices: attribs.map((a) => a.component_idx),
        sort_params
      }
    ]
  }, [attribs, attribClass, node, sortParam])

  useEffect(() => {
    let init: Record<number, string> = {}
    // fill first row with attrib names
    init[0] = ''
    for (let col = 1; col < numCols; ++col) {
      init[col] = attribs[col - 1].display_name
    }

    setCache(init)
  }, [attribs, numElements, sortParam])

  // useEffect(() => {
  //   if (infiniteLoaderRef.current) {
  //     infiniteLoaderRef.current.resetloadMoreItemsCache(true)
  //     setTimeout(() => {
  //       infiniteLoaderRef.current!.resetloadMoreItemsCache(true)
  //     }, 10)
  //   }
  // }, [attribs, numElements])

  useDebounce(
    () => {
      if (infiniteLoaderRef.current) {
        infiniteLoaderRef.current.resetloadMoreItemsCache(true)
      }
    },
    100,
    [infiniteLoaderRef, attribs, numElements, sortParam]
  )

  useEffect(() => {
    if (gridRef.current) {
      gridRef.current.resetAfterIndices({
        columnIndex: 0,
        rowIndex: 0,
        shouldForceUpdate: true
      })
    }
  }, [colWidthDirty])

  useEffectOnce(() => {
    const unlisten = listenGlobalAction('jumpToLine', (line: number) => {
      if (gridRef.current && line >= 0) {
        gridRef.current.scrollToItem({ columnIndex: 0, rowIndex: line + 1, align: 'start' })
      }
    })

    return unlisten
  })

  //返回第index列的宽度
  const getColumnWidth = useCallback(
    (index: number) => {
      let w = colWidth.current[index]
      if (w !== undefined) return w
      if (index == 0) w = numItems.toString().length * 10
      else {
        const cache = widthCache[attribClass]
        if (cache) {
          w = cache[attribs[index - 1].display_name]
        }
        if (!w) {
          w = Math.max(attribs[index - 1].display_name.length * 10, 50)
        }
      }
      colWidth.current[index] = w
      return w
    },
    [attribs, numItems]
  )

  // 返回行高度
  const getRowHeight = useCallback(() => {
    return lineHeight
  }, [])

  // 渲染每个单元格
  const Cell = ({
    columnIndex,
    rowIndex,
    style
  }: {
    columnIndex: number
    rowIndex: number
    style: React.CSSProperties
  }) => {
    const v = cache[rowIndex * numCols + columnIndex]

    return (
      <div
        style={{
          ...style,
          left: (style.left as number) + 2,
          top: (style.top as number) + 2,
          width: (style.width as number) - 2,
          height: (style.height as number) - 2
        }}
        className={join(
          'bg-white/5 text-xs flex items-center justify-center rounded font-thin selection:bg-text-select/40 truncate hover:bg-white/20',
          rowIndex == 0 && 'text-blue-300 select-none cursor-pointer',
          columnIndex == 0 && 'text-text-select/80',
          rowIndex == 0 && columnIndex == 0 && 'opacity-0 pointer-events-none'
        )}
        onMouseMove={
          rowIndex == 0
            ? (e) => {
                if (e.buttons == 1 && e.movementX != 0) {
                  colWidth.current[columnIndex] += e.movementX
                  let pre = widthCache[attribClass] || {}
                  pre[attribs[columnIndex - 1].display_name] = colWidth.current[columnIndex]
                  widthCache[attribClass] = pre
                  setColWidthDirty((v) => !v)
                }
              }
            : undefined
        }
        onClick={
          rowIndex == 0
            ? () => {
                const attrib = attribs[columnIndex - 1]
                if (
                  sortParam === null ||
                  sortParam.name !== attrib.name ||
                  sortParam.component_idx != attrib.component_idx
                ) {
                  setSortParam({
                    name: attrib.name,
                    component_idx: attrib.component_idx,
                    type: 'up'
                  })
                } else if (sortParam.type === 'down') {
                  setSortParam(null)
                } else {
                  setSortParam({
                    name: attrib.name,
                    component_idx: attrib.component_idx,
                    type: 'down'
                  })
                }
              }
            : undefined
        }
      >
        {v !== undefined ? v : '...'}
        {rowIndex == 0 &&
          columnIndex > 0 &&
          sortParam !== null &&
          sortParam.name === attribs[columnIndex - 1].name &&
          sortParam.component_idx === attribs[columnIndex - 1].component_idx && (
            <div className="text-text-select/80">
              {sortParam.type === 'down' ? <FaArrowDownLong /> : <FaArrowUpLong />}
            </div>
          )}
      </div>
    )
  }

  // 动态加载属性值
  const loadMoreItems = useCallback(
    (start: number, stop: number) => {
      const row_start = Math.max(1, Math.floor(start / numCols))
      const row_stop = Math.min(numRows, Math.ceil((stop + 1) / numCols))

      let params = batchParams
      params[4] = {
        ...(params[4] as any),
        begin: row_start - 1,
        end: row_stop - 1
      }

      const updater = async () => {
        const batch_data = await batchGetNodeOutputMeshAttrs(params as any)
        let values: Record<number, string> = {}

        if (batch_data === null) {
        } else if (numCols * (row_stop - row_start) == batch_data.length) {
          let idx = 0
          for (let col = 0; col < numCols; ++col) {
            for (let row = row_start; row < row_stop; ++row) {
              values[row * numCols + col] = batch_data[idx]
              idx += 1
            }
          }
        }

        setCache((cache) => ({ ...cache, ...values }))
        // console.log(row_start, row_stop)
      }

      return updater()
    },
    [attribs, numElements, setCache, batchParams]
  )

  // 这判断属性值是否已加载
  const isItemLoaded = (index: number) => {
    return cache[index] !== undefined
  }

  if (
    (attribs.length === 0 && numElements === 0) ||
    (attribs.length === 0 && attribClass === 'global')
  )
    return <NoData />

  if (attribs.length === 0)
    return <NoAttributes attribClass={attribClass} numElements={numElements} />

  return (
    <AutoSizer>
      {({ height, width }) => (
        <InfiniteLoader
          ref={infiniteLoaderRef}
          isItemLoaded={isItemLoaded}
          itemCount={numItems}
          loadMoreItems={loadMoreItems}
          threshold={50 * numCols}
        >
          {({ onItemsRendered, ref }) => (
            <VariableSizeGrid
              columnCount={numCols}
              columnWidth={getColumnWidth}
              height={height}
              rowCount={numRows}
              rowHeight={getRowHeight}
              width={width}
              onItemsRendered={(gridProps) => {
                onItemsRendered({
                  visibleStartIndex: gridProps.visibleRowStartIndex * numCols,
                  visibleStopIndex: Math.min(numRows, 1 + gridProps.visibleRowStopIndex) * numCols,
                  overscanStartIndex: gridProps.overscanRowStartIndex * numCols,
                  overscanStopIndex: Math.min(numRows, 1 + gridProps.overscanRowStopIndex) * numCols
                })
              }}
              ref={(r) => {
                ref(r)
                gridRef.current = r
              }}
            >
              {Cell}
            </VariableSizeGrid>
          )}
        </InfiniteLoader>
      )}
    </AutoSizer>
  )
}

export function AttributeSheet() {
  const { node, node_graph } = useNodeGraphBridgeStore(selector)
  const [meshInfo, setMeshInfo] = useState<MeshInfo | null>(null)
  const [autoUpdateFlag, setAutoUpdateFlag] = useState(false)
  const [displayIndex, setDisplayIndex] = useState(0)
  const [selectedTab, setSelectedTab] = useState<string>('point')
  const [attribsFilter, setAttribsFilter] = useState<AttribsFilter>({})
  const { t } = useI18n()
  const autoUpdateFlagRef = useRef(autoUpdateFlag)

  const isValid = node !== null && node.id != 0 && meshInfo !== null
  const storeApi = useNodeGraphBridgeStoreApi()

  const [lineIndex, setLineIndex] = useState<number>(0)

  useEffectOnce(() => {
    const unlisten1 = listen<string>('RemoveNodeEvent', (event) => {
      const current_node = storeApi.getState().node
      if (!current_node) return
      const node_id = parseInt(JSON.parse(event.payload).node_id)
      if (node_id == current_node.id) {
        setNode(null)
      }
    })

    const unlisten2 = listen<string>('ExecuteNodeEvent', (event) => {
      const current_node = storeApi.getState().node
      if (!current_node) return
      const node_id = parseInt(JSON.parse(event.payload).node_id)

      // if change this node
      if (node_id == current_node.id) {
        autoUpdateFlagRef.current = !autoUpdateFlagRef.current
        setAutoUpdateFlag(autoUpdateFlagRef.current)
      }
    })

    return () => {
      unlisten1.then((fn) => fn())
      unlisten2.then((fn) => fn())
    }
  })

  const currentDisplayIndex = useMemo(() => {
    if (!node) return 0
    return displayIndex >= node.outputs.length ? 0 : displayIndex
  }, [displayIndex, node])

  useDebounce(
    () => {
      if (node && node_graph) {
        node_graph
          .getState()
          .getNodeInfo(node.id, currentDisplayIndex)
          .then((v) => {
            if (v && v.mesh) {
              setMeshInfo({
                ...v.mesh,
                point: expandAttribInfo(v.mesh.point),
                vertex: expandAttribInfo(v.mesh.vertex),
                prim: expandAttribInfo(v.mesh.prim),
                global: expandAttribInfo(v.mesh.global)
              })
            } else {
              setMeshInfo(null)
            }
          })
      } else {
        setMeshInfo(null)
      }
    },
    50,
    [node?.id, autoUpdateFlag, currentDisplayIndex]
  )

  useEffect(() => {
    emitGlobalAction('jumpToLine', lineIndex ?? 0)
  }, [lineIndex])

  return (
    <If condition={isValid}>
      <Then>
        {() => (
          <div className="size-full pb-7">
            <div className="absolute right-0 m-1 flex gap-2">
              {node!.outputs.length > 1 && (
                <Select
                  className="select-accent bg-white/20 text-xs"
                  onChange={(v) => setDisplayIndex(parseInt(v))}
                  value={currentDisplayIndex.toString()}
                  options={Array.from(Array(node!.outputs.length).keys()).map((i) => i.toString())}
                />
              )}
              <Dropdown className={cn(presets['menu-primary'], 'tracking-wide font-thin min-w-0')}>
                <DropdownTrigger>
                  <div
                    className={join(
                      'btn btn-xs btn-ghost min-h-0 btn-square rounded',
                      attribsFilter[selectedTab] ? ' text-btn-active' : ' text-btn-primary'
                    )}
                  >
                    <MdFilterList className="m-0 size-full p-1" />
                  </div>
                </DropdownTrigger>
                <DropdownMenu
                  aria-label="light"
                  className="p-0"
                  variant="light"
                  itemClasses={{
                    base: cn(presets['menu-item-primary'], 'h-6'),
                    title: 'text-text-primary'
                  }}
                  closeOnSelect={false}
                  selectionMode="multiple"
                  selectedKeys={getFilterSelectedAttribs(meshInfo!, selectedTab, attribsFilter)}
                  onSelectionChange={(keys) => {
                    if (keys === 'all') {
                      setAttribsFilter((filter) => {
                        delete filter[selectedTab]
                        return {
                          ...filter
                        }
                      })
                    } else {
                      setAttribsFilter((filter) => {
                        const new_names = getFilterAllAttribs(meshInfo!, selectedTab).filter(
                          (v) => !keys.has(v)
                        )
                        if (new_names.length == 0) {
                          if (filter[selectedTab]) {
                            delete filter[selectedTab]
                          }
                        } else {
                          filter[selectedTab] = new_names
                        }
                        return {
                          ...filter
                        }
                      })
                    }
                  }}
                >
                  {getFilterAllAttribs(meshInfo!, selectedTab).map((key) => (
                    <DropdownItem key={key}>{key}</DropdownItem>
                  ))}
                </DropdownMenu>
              </Dropdown>
              <Dropdown className={cn(presets['menu-primary'], 'tracking-wide font-thin min-w-0')}>
                <DropdownTrigger>
                  <span
                    className={join(
                      'btn btn-xs btn-ghost min-h-0 btn-square rounded',
                      attribsFilter[selectedTab] ? ' text-btn-active' : ' text-btn-primary'
                    )}
                  >
                    <VscGoToSearch className="m-0 size-full p-1" />
                  </span>
                </DropdownTrigger>
                <DropdownMenu
                  aria-label="input"
                  className="p-0"
                  variant="light"
                  closeOnSelect={false}
                  itemClasses={{
                    base: cn(
                      // presets['menu-item-primary'],
                      'h-6 m-0 p-0 border border-white/0 bg-transparent'
                    )
                  }}
                >
                  <DropdownItem>
                    <NumberInput
                      className="w-24 h-6 text-xs text-text-primary [appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none"
                      valueType={NumberValueType.Int}
                      defaultValue={lineIndex}
                      value={lineIndex}
                      limitMin={0}
                      limitMax={getNumElements(meshInfo!, selectedTab) - 1}
                      step={1}
                      placeholder={`0 - ${getNumElements(meshInfo!, selectedTab) - 1}`}
                      onChange={(v) => {
                        setLineIndex(v)
                      }}
                    />
                  </DropdownItem>
                </DropdownMenu>
              </Dropdown>
            </div>
            <Tabs
              aria-label="attribute sheet"
              className="w-full justify-center overflow-hidden"
              fullWidth={false}
              variant="solid"
              size="sm"
              color="secondary"
              radius="md"
              classNames={{
                tabList: 'bg-white/10',
                cursor: 'w-full bg-btn-active/60',
                tabContent: 'text-text-primary/80 select-none',
                panel: 'size-full'
              }}
              tabIndex={['point', 'vertex', 'prim', 'global'].indexOf(selectedTab)}
              onSelectionChange={(key) => {
                setSelectedTab(key as string)
                setLineIndex(0)
              }}
            >
              {['point', 'vertex', 'prim', 'global'].map((cls) => {
                return (
                  <Tab
                    key={cls}
                    title={
                      <div className="flex items-center space-x-2">
                        <span>{t(('node-graph.node-info.' + cls) as any)}</span>
                      </div>
                    }
                  >
                    <AttribClassSheet
                      attribClass={cls}
                      numElements={getNumElements(meshInfo!, cls)}
                      attribs={getAttribs(meshInfo!, cls as any, attribsFilter)}
                      displayIndex={currentDisplayIndex}
                    />
                  </Tab>
                )
              })}
            </Tabs>
          </div>
        )}
      </Then>
      <Else>{() => <NoData />}</Else>
    </If>
  )
}

const NoData = () => {
  const { t } = useI18n()
  return (
    <div className="flex size-full justify-center">
      <label className="select-none self-center opacity-40">
        {t('tooltip.attribute-sheet.no-data-to-display')}
      </label>
    </div>
  )
}

const NoAttributes = ({
  numElements,
  attribClass
}: {
  numElements: number
  attribClass: string
}) => {
  const { t } = useI18n()
  return (
    <div className="flex size-full justify-center">
      <label className="select-none self-center opacity-40">
        {t('tooltip.attribute-sheet.no-attributes')
          .replace('$NUM_ELEMENTS', numElements.toString())
          .replace('$ATTRIB_CLASS', attribClass)}
      </label>
    </div>
  )
}

const numComponentsCache: Record<string, number> = {}

const attribNumComponents = (type_name: string): number => {
  let result = numComponentsCache[type_name]
  if (result !== undefined) {
    return result
  }
  result = 1

  if (type_name.endsWith('[]')) {
  } else if (type_name.endsWith(']')) {
    if (type_name.substring(type_name.length - 3, type_name.length - 2) == '[') {
      result = parseInt(type_name.substring(type_name.length - 2, type_name.length - 1))
    } else {
      result = parseInt(type_name.substring(type_name.length - 3, type_name.length - 1))
    }
  }
  numComponentsCache[type_name] = result
  return result
}

const expandAttribInfo = (attribs: NousAttribInfo[]): AttribInfo[] => {
  const result: AttribInfo[] = []
  for (const a of attribs) {
    const num = attribNumComponents(a.type)
    for (let i = 0; i < num; i++) {
      result.push({
        display_name: a.name + (num > 1 ? `[${i}]` : ''),
        name: a.name,
        type: a.type,
        num_components: num,
        component_idx: i
      })
    }
  }
  return result
}
