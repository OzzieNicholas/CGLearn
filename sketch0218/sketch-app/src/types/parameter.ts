import { LayoutType, ParamType } from '@/commands/definer'
import { ParamValue } from '@/commands/index.types.ts'
import { LayoutTypeStr, ParamTypeStr } from './../commands/definer'
import { Maybe } from './maybe'

type Extra = {
  is_custom: boolean
  is_dirty: boolean
  is_new?: boolean
  parent_id?: string
}

export type ParameterTemplate = {
  readonly item_type: 'param'
  checkable: boolean
  checked?: boolean
  default_value: ParamValue
  editable: boolean
  max_value?: number
  min_value?: number
  max_ui_value?: number
  min_ui_value?: number
  name: string
  // for backend rebuild replacement
  previous_name?: string
  display_name?: string
  name_visible: boolean
  tooltip: string
  type: ParamType
  visible: boolean
  enum_items?: string[]
  expression?: string
  value_action?: string
  visible_action?: string
  editable_action?: string
  checked_action?: string
  param_items?: (ParameterTemplate | LayoutTemplate)[] // for list type
} & Extra

export type LayoutTemplate = {
  readonly item_type: 'layout'
  name: string
  // for backend rebuild replacement
  display_name?: string
  previous_name?: string
  type: LayoutType
  editable: boolean
  visible: boolean
  expanded?: boolean
  param_items: (ParameterTemplate | LayoutTemplate)[]
  visible_action?: string
  editable_action?: string
} & Extra

export type Expression = string

export interface ParameterDescription {
  readonly id: 'param' | 'list'
  name: string
  type: ParamTypeStr
  display_name: Maybe<string>
  default_value: Maybe<any>
  items: Maybe<string[]>
  checked: Maybe<boolean | Expression>
  editable: Maybe<boolean | Expression>
  visible: Maybe<boolean | Expression>
  tooltip: Maybe<string>
  min_value: Maybe<number>
  max_value: Maybe<number>
  min_ui_value: Maybe<number>
  max_ui_value: Maybe<number>
  name_visible: Maybe<boolean>
  parent_list: Maybe<string[]>
  expression: Maybe<string>
  content?: Description[]
}

export interface LayoutDescription {
  id: LayoutTypeStr
  content: Description[]
  name: string
  display_name: Maybe<string>
  editable: Maybe<boolean | Expression>
  visible: Maybe<boolean | Expression>
  expanded: Maybe<boolean | Expression>
  parent_list: Maybe<string[]>
}

export type Description = ParameterDescription | LayoutDescription
