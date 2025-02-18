import { IconType } from 'react-icons'
import { MdOutlineFitbit } from 'react-icons/md'
import { PiArrowsSplitBold } from 'react-icons/pi'
import { RiBox2Fill } from 'react-icons/ri'
import { TbBrandMatrix, TbUniverse } from 'react-icons/tb'

export const iconThemes = [
  ['rgb(254 202 202)', 'rgb(248 113 113)'], //   'red
  ['rgb(254 215 170)', 'rgb(251 146 60)'], //   'orange
  ['rgb(253 230 138)', 'rgb(251 191 36)'], //   'amber
  ['rgb(253 230 138)', 'rgb(251 191 36)'], //   'yellow
  ['rgb(217 249 157)', 'rgb(163 230 53)'], //   'lime
  ['rgb(187 247 208)', 'rgb(74 222 128)'], //   'green
  ['rgb(167 243 208)', 'rgb(52 211 153)'], //   'emerald
  ['rgb(153 246 228)', 'rgb(45 212 191)'], //   'teal
  ['rgb(165 243 252)', 'rgb(34 211 238)'], //   'cyan
  ['rgb(186 230 253)', 'rgb(56 189 248)'], //   'sky
  ['rgb(191 219 254)', 'rgb(96 165 250)'], //   'blue
  ['rgb(199 210 254)', 'rgb(129 140 248)'], //   'indigo
  ['rgb(221 214 254)', 'rgb(167 139 250)'], //   'violet
  ['rgb(233 213 255)', 'rgb(192 132 252)'], //   'purple
  ['rgb(245 208 254)', 'rgb(232 121 249)'], //   'fuchsia
  ['rgb(251 207 232)', 'rgb(244 114 182)'], //   'pink
  ['rgb(254 205 211)', 'rgb(251 113 133)'], //   'rose
  ['rgb(226 232 240)', 'rgb(148 163 184)'], //   'slate
  ['rgb(231 229 228)', 'rgb(168 162 158)'] //   'stone
]

export const iconTypes: Record<string, IconType> = {
  default: TbUniverse,
  Cube: RiBox2Fill,
  Transform: TbBrandMatrix,
  SplitElement: PiArrowsSplitBold,
  Scatter: MdOutlineFitbit
}

export let categoryTheme: Record<string, number> = {
  default: 3,
  Geometry: 5,
  Modifier: 15,
  Polygon: 9
}

const leftColors = iconThemes.map((_, i) => i)

export function getIconTheme(node_category: string): string[] {
  let theme = categoryTheme[node_category]
  if (theme !== undefined) {
    return iconThemes[theme]
  }
  if (leftColors.length == 0) {
    theme = Math.floor(Math.random() * iconThemes.length)
  } else {
    const select = Math.floor(Math.random() * leftColors.length)
    theme = leftColors[select]
    leftColors.slice(select, 1)
  }
  categoryTheme[node_category] = theme
  return iconThemes[theme]
}
