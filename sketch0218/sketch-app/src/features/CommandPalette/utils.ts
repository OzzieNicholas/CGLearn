import { searchNodes as nousSearchNodes, searchParams as nousSearchParams } from '@/commands/index'
import { SearchNodeDesc, SearchParamDesc } from '@/commands/index.types'
import { defaultCommands } from '@/constant/commands'
import { titleBarLocalProvider } from '@/providers/title-bar'
import { TitleBarSubMenu } from '@/types/title-bar'

import Fuse from 'fuse.js'

const fuseOptions = {
  isCaseSensitive: false,
  // includeScore: false,
  shouldSort: true,
  includeMatches: true,
  findAllMatches: true,
  minMatchCharLength: 1,
  // location: 0,
  threshold: 0.5
  // distance: 100,
  // useExtendedSearch: false,
  // ignoreLocation: false,
  // ignoreFieldNorm: false,
  // fieldNormWeight: 1
}

export const searchNodes = async (
  search_text: string,
  graph_id: number,
  subgraph_id: number
): Promise<SearchNodeDesc[]> => {
  const result = await nousSearchNodes([graph_id, search_text, subgraph_id])
  if (result.length < 2) {
    return result
  }
  const fuse = new Fuse(result as any, {
    ...fuseOptions,
    keys: ['name', 'type_name']
  })
  return fuse.search(search_text).map((m) => m.item as SearchNodeDesc)
}

export const searchParams = async (
  search_text: string,
  graph_id: number,
  subgraph_id: number
): Promise<SearchParamDesc[]> => {
  const result = await nousSearchParams([graph_id, search_text, subgraph_id])
  if (result.length < 2) {
    return result
  }
  const fuse = new Fuse(result as any, {
    ...fuseOptions,
    keys: ['match_name']
  })
  return fuse.search(search_text).map((m) => m.item as SearchParamDesc)
}

export type SearchCommandDesc = {
  label: string
  shortcut?: string
  action?: () => void
  info?: string
  key: string
}

export const searchCommands = (search_text: string): SearchCommandDesc[] => {
  let menus: {
    label: string
    shortcut?: string
    action?: () => void
    info?: string
    sub_menus?: TitleBarSubMenu[]
  }[] = [...titleBarLocalProvider.menus, ...defaultCommands]
  let result: SearchCommandDesc[] = []

  const search_text_lower_case = search_text.toLowerCase()

  while (menus.length > 0) {
    const menu = menus.pop()
    if (menu) {
      if (menu.action !== undefined) {
        let key = menu.label.toLowerCase()
        const idx = key.lastIndexOf('.')
        if (idx >= 0) {
          key = key.substring(idx + 1)
        }
        if (key.includes(search_text_lower_case)) {
          result.push({
            label: menu.label,
            shortcut: menu.shortcut,
            action: menu.action,
            info: menu.info,
            key
          })
        }
      }
      if (menu.sub_menus !== undefined && menu.sub_menus.length > 0) {
        menus.push(...menu.sub_menus)
      }
    }
  }

  const fuse = new Fuse(result as any, {
    ...fuseOptions,
    keys: ['key']
  })
  return fuse.search(search_text).map((m) => m.item as SearchCommandDesc)
}
