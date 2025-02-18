import RecursiveMenu, { type MenuTye } from '@/components/RecursiveMenu'
import { getIconTheme, iconTypes } from '@/constant/node-resource'
import { useI18n } from '@/locales'
import { useNodeDefsStoreApi } from '@/stores/node-defs'
import { NodeGraphStoreType, useNodeGraphStoreApi } from '@/stores/node-graph'
import { NodeDef } from '@/types/node-def'
import uuid from '@/utils/uuid'
import { OnConnectStartParams, XYPosition } from '@xyflow/react'
import { produce } from 'immer'
import React, { memo, useCallback, useEffect, useRef, useState } from 'react'
import { useEffectOnce } from 'react-use'
import { ContextMenuParam, nodeTypesKeys, EdgeInfo } from './context'
import { hasDifferentVersion } from '@/stores/node-graph/local'
// import Fuse from 'fuse.js'

// const fuseOptions = {
//   isCaseSensitive: false,
//   // includeScore: false,
//   shouldSort: true,
//   includeMatches: true,
//   findAllMatches: true,
//   minMatchCharLength: 1,
//   // location: 0,
//   threshold: 0.5,
//   // distance: 100,
//   // useExtendedSearch: false,
//   // ignoreLocation: false,
//   // ignoreFieldNorm: false,
//   // fieldNormWeight: 1,
//   keys: ['label']
// }

let iconCache = new Map<string, () => React.ReactElement>()

const CreateMenuIcon = (category: string, type_name?: string): (() => React.ReactElement) => {
  let key = type_name || category
  let fn = iconCache.get(key)

  if (!fn) {
    const theme = getIconTheme(category)

    fn = () => {
      const NodeIcon = iconTypes[key] || iconTypes.default
      return (
        <div className="m-0 flex size-4 items-center rounded-md p-0">
          <NodeIcon className="size-full" style={{ color: theme[1] }} />
        </div>
      )
    }
    iconCache.set(key, fn)
  }
  return fn
}

const getOrCreateMenu = (path: string, menus: MenuTye[]) => {
  let result: MenuTye = null!
  for (const name of path.split(/[|/]/)) {
    let menu = menus.find((m) => m.label === name)

    if (!menu) {
      menu = {
        label: name,
        icon: CreateMenuIcon(name, undefined),
        sub_menus: []
      }
      menus.push(menu)
    }

    if (menu.sub_menus !== undefined) {
      menus = menu.sub_menus
    }

    result = menu
  }
  return result
}

const sortMenuTree = (menus: MenuTye[]) => {
  menus.sort((a, b) => a.label.localeCompare(b.label))
  for (const menu of menus) {
    if (menu.sub_menus) {
      sortMenuTree(menu.sub_menus)
    }
  }
}

const buildMenus = (nodeDefs: NodeDef[], out_tree: MenuTye[], out_items: MenuTye[]) => {
  for (const def of nodeDefs) {
    const new_menu = {
      label: def.type_name,
      icon: CreateMenuIcon(def.category, def.type_name)
    }
    out_items.push(new_menu)
    getOrCreateMenu(def.category, out_tree).sub_menus?.push(new_menu)
  }

  sortMenuTree(out_tree)
}

export const createNode = async (
  type_name: string,
  api: NodeGraphStoreType,
  param: {
    node_pos?: XYPosition
    connection?: OnConnectStartParams
    edge?: EdgeInfo
  }
) => {
  const nodeDefs = useNodeDefsStoreApi().defs
  const def = nodeDefs.find((d) => d.type_name === type_name)
  if (!def) return []

  def.inputs = def.inputs || []
  def.outputs = def.outputs || []

  const componet_type = nodeTypesKeys.has(def.type_name) ? def.type_name : 'common'

  const show_version = hasDifferentVersion(api.getState().nodes, def.type_name)

  const node = {
    id: uuid(),
    type: componet_type,
    position: param.node_pos || { x: 0, y: 0 },
    data: {
      id: 0,
      label: def.type_name,
      type_name: def.type_name,
      color: '#FFF',
      icon: def.type_name,
      inputs: def.inputs.map((i) => ({
        name: i.name,
        type: i.type
      })),
      outputs: def.outputs.map((i) => ({
        name: i.name,
        type: i.type
      })),
      bypassed: false,
      is_display: api.getState().nodes.length == 0,
      kind: def.kind,

      category: def.category,
      desc: 'No description',

      version: def.type_version,
      show_version
    }
  }

  // begin undo group
  api.setState(
    produce((state) => {
      state.undo_redo.beginGroup('create node')
    })
  )

  const node_data_id = await api.getState().addNode(node)

  // connect node to the hang connection
  if (param.connection && param.connection.nodeId !== null) {
    if (param.connection.handleType === 'source') {
      if (node.data.inputs.length > 0) {
        await api.getState().addEdges([
          {
            id: uuid(),
            source: param.connection.nodeId,
            sourceHandle: param.connection.handleId,
            target: node.id,
            targetHandle: '0'
          }
        ])
      }
    } else {
      if (node.data.outputs.length > 0) {
        await api.getState().deleteInputEdges(param.connection.nodeId, param.connection.handleId)
        await api.getState().addEdges([
          {
            id: uuid(),
            source: node.id,
            sourceHandle: '0',
            target: param.connection.nodeId,
            targetHandle: param.connection.handleId
          }
        ])
      }
    }
  }

  // insert node to a exitsing edge
  else if (param.edge) {
    await api.getState().deleteInputEdges(param.edge.target, param.edge.targetHandle)

    let new_edges = []
    if (node.data.inputs.length > 0) {
      new_edges.push({
        id: uuid(),
        source: param.edge.source,
        sourceHandle: param.edge.sourceHandle,
        target: node.id,
        targetHandle: '0'
      })
    }
    if (node.data.outputs.length > 0) {
      new_edges.push({
        id: uuid(),
        source: node.id,
        sourceHandle: '0',
        target: param.edge.target,
        targetHandle: param.edge.targetHandle
      })
    }
    if (new_edges.length > 0) {
      await api.getState().addEdges(new_edges)
    }
  }

  // end undo group
  api.setState(
    produce((state) => {
      state.undo_redo.endGroup()
    })
  )

  return [node.id, node_data_id]
}

const ContextMenu = ({ param, close }: { param: ContextMenuParam; close: () => void }) => {
  const [defaultMenuTree, setDefaultMenuTree] = useState<MenuTye[]>([])
  const [menuTree, setMenuTree] = useState<MenuTye[]>([])
  const [searchText, setSearchText] = useState<string>('')

  const { t } = useI18n()
  const menuItems = useRef<MenuTye[]>([])
  // const fuse = useRef<Fuse<unknown>>()
  const storeApi = useNodeGraphStoreApi()

  useEffectOnce(() => {
    let items: MenuTye[] = new Array()
    let tree: MenuTye[] = new Array()
    buildMenus(useNodeDefsStoreApi().defs, tree, items)
    menuItems.current = items
    // fuse.current = new Fuse(menuItems as any, fuseOptions)
    setMenuTree(tree)
    setDefaultMenuTree(tree)

    window.addEventListener('click', close, { once: true })
    return () => {
      window.removeEventListener('click', close)
    }
  })

  const onCreateNode = useCallback(
    async (item: MenuTye) => {
      createNode(item.label, storeApi, param)
    },
    [param]
  )

  const onInputChancge = useCallback(
    (e: any) => {
      setSearchText(e.target.value)

      const input = (e.target.value as string).toLowerCase()
      if (input.length === 0) {
        setMenuTree(defaultMenuTree)
      } //else if (fuse.current)
      else {
        let menus = []

        // filter by char orders
        {
          for (const menu of menuItems.current) {
            const label = menu.label.toLowerCase()
            let last = -1
            for (const char of input) {
              const idx = label.indexOf(char, last + 1)
              if (idx != -1) {
                last = idx
              } else {
                last = -1
                break
              }
            }
            if (last !== -1) {
              menus.push(menu)
            }
          }
        }

        // sort by scores
        {
          let menusWithScore: { menu: MenuTye; score: number }[] = []

          for (const menu of menus) {
            const label = menu.label.toLowerCase()

            let score = 0

            if (label === input) {
              // same
              score = 10000
            } else if (label.startsWith(input)) {
              // starts with
              score = 5000
            } else {
              // keywords
              let last = -1
              for (const char of input) {
                const idx = label.indexOf(char, last + 1)
                if (idx > last && menu.label[idx] === menu.label[idx].toUpperCase()) {
                  last = idx
                } else {
                  last = -1
                  break
                }
              }
              if (last > 0) {
                score = 4000
              }
            }

            if (score == 0 && label.includes(input)) {
              // includes
              const idx = label.indexOf(input)
              if (menu.label[idx] === menu.label[idx].toUpperCase()) {
                score = 3000
              } else {
                score = 2000
              }
            }

            if (score == 0) {
              // complex scores
              let last = -1
              for (const char of input) {
                let find = false
                for (let i = last + 1; i < label.length; ++i) {
                  if (char === label[i]) {
                    if (last == i - 1) {
                      score += 2
                    }
                    if (menu.label[i] === menu.label[i].toUpperCase()) {
                      if (char === input[0]) {
                        score += 3
                      }
                      if (last == i - 1) {
                        score += 3
                      }
                    }
                    last = i
                    ++score
                    find = true
                    break
                  }
                }
                if (!find) {
                  break
                }
              }
            }
            if (score == 0) {
              continue
            }
            menusWithScore.push({ menu, score })
          }

          menus = menusWithScore.sort((a, b) => b.score - a.score).map((m) => m.menu)
        }

        // fuse.current.setCollection(menus)
        // setMenuTree(fuse.current.search(input).map((m) => m.item) as any)
        setMenuTree(menus)
      }
    },
    [setSearchText, defaultMenuTree]
  )

  const autoFocus = useCallback(
    (element: HTMLInputElement) => (element ? element.focus() : null),
    []
  )

  const onKeyDown = (e: any) => {
    if (e.key === 'Enter' && menuTree.length > 0 && !menuTree[0].sub_menus) {
      onCreateNode(menuTree[0])
      close()
    } else if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'a') {
      e.stopPropagation()
    }
  }

  useEffect(() => {
    setSearchText('')
    setMenuTree(defaultMenuTree)
  }, [defaultMenuTree, param])

  return (
    <div
      style={{ top: param.top, left: param.left, right: param.right, bottom: param.bottom }}
      className="absolute z-10 w-48"
      onClick={close}
    >
      <RecursiveMenu
        menus={menuTree}
        open={true}
        defaultOpen={true}
        asChild
        className="w-48 animate-fade-in"
        onClickItem={onCreateNode}
        modal={false}
        focusTriggerOnKeyPressed={true}
      >
        <input
          type="text"
          autoFocus
          placeholder={t('common.type-search-here')}
          className="input input-sm input-bordered w-full animate-fade-in border-r-0 bg-black/60 text-text-primary transition-colors-opacity selection:bg-text-select/40 hover:bg-black/80"
          onChange={onInputChancge}
          value={searchText}
          onClick={(e) => e.stopPropagation()}
          onKeyDown={onKeyDown}
          ref={autoFocus}
          autoComplete="new-password"
        />
      </RecursiveMenu>
    </div>
  )
}

export default memo(ContextMenu)
