import { rebuildNodeParameters as rebuildNodeParameters_ } from '@/commands'
import {
  LayoutStrTypeMap,
  LayoutTypeStrMap,
  ParamStrTypeMap,
  ParamType,
  ParamTypeStrMap
} from '@/commands/definer'
import {
  ColorRampValue,
  ParamItemType,
  ParamItemDef,
  ParamItemDefType,
  Parameter,
  ParamValue,
  ScalarRampValue
} from '@/commands/index.types'
import {
  Description,
  LayoutDescription,
  LayoutTemplate,
  ParameterDescription,
  ParameterTemplate
} from '@/types/parameter'
import { isEmptyString, isNumberSame } from './functions'
import { I32_MAX, I32_MAX_LIMIT, I32_MIN, I32_MIN_LIMIT } from '@/constant/number'

export const isColorRampSame = (a: ColorRampValue[], b: ColorRampValue[]) => {
  if (a.length == b.length) {
    for (let i = 0; i < a.length; ++i) {
      const v1 = a[i]
      const v2 = b[i]
      if (
        !isNumberSame(v1.pos, v2.pos) ||
        v1.method !== v2.method ||
        v1.value.length != v2.value.length
      ) {
        return false
      }
      for (let j = 0; j < v1.value.length; ++j) {
        if (!isNumberSame(v1.value[j], v2.value[j])) {
          return false
        }
      }
    }
    return true
  }
  return false
}

const isPointSame = (a: ScalarRampValue, b: ScalarRampValue) => {
  return isNumberSame(a.pos, b.pos) && isNumberSame(a.value, b.value) && a.method === b.method
}

export const isScalarRampSame = (a: ScalarRampValue[], b: ScalarRampValue[]) => {
  if (a.length == b.length) {
    for (let i = 0; i < a.length; ++i) {
      if (!isPointSame(a[i], b[i])) {
        return false
      }
    }
    return true
  }
  return false
}

export const isParamValueSame = (a: ParamValue, b: ParamValue) => {
  if (a === b) return true
  if (typeof a === 'number') {
    return isNumberSame(a, b as number)
  } else if (typeof a === 'string' || typeof a === 'boolean') {
    return a === b
  } else if (a.length != (b as any).length) {
    return false
  } else if (a.length == 0) {
    return true
  } else if (typeof a[0] === 'number') {
    for (let i = 0; i < a.length; ++i) {
      if (!isNumberSame(a[i] as number, (b as any)[i] as number)) {
        return false
      }
    }
    return true
  } else if (typeof a[0].value === 'number') {
    return isScalarRampSame(a as ScalarRampValue[], b as ScalarRampValue[])
  } else {
    return isColorRampSame(a as ColorRampValue[], b as ColorRampValue[])
  }
}

export const generateNodeParametersDescription = (
  templates: (ParameterTemplate | LayoutTemplate)[]
): string => {
  // console.log(templates)
  const description = templateListToDescriptionList(templates)
  return JSON.stringify(description, undefined, 2)
}

export const rebuildNodeParameters = (
  graph: number,
  node_id: number,
  templates: (ParameterTemplate | LayoutTemplate)[]
) => {
  rebuildNodeParameters_([graph, node_id, { param_items: templates }])
  // console.log(templates)
}

function parameterTemplateToDescription(template: ParameterTemplate): ParameterDescription {
  const min_value =
    template.min_value === undefined ||
    template.min_value === I32_MIN ||
    template.min_value === I32_MIN_LIMIT ||
    template.min_value < -1e10 ||
    Number.isNaN(template.min_value)
      ? undefined
      : Number.parseFloat(template.min_value.toFixed(3))

  const max_value =
    template.max_value === undefined ||
    template.max_value === I32_MAX ||
    template.max_value === I32_MAX_LIMIT ||
    template.max_value > 1e10 ||
    Number.isNaN(template.max_value)
      ? undefined
      : Number.parseFloat(template.max_value.toFixed(4))

  const min_ui_value =
    template.min_ui_value === undefined ||
    template.min_ui_value === I32_MIN ||
    template.min_ui_value === I32_MIN_LIMIT ||
    template.min_ui_value < -1e10 ||
    Number.isNaN(template.min_ui_value)
      ? undefined
      : Number.parseFloat(template.min_ui_value.toFixed(4))

  const max_ui_value =
    template.max_ui_value === undefined ||
    template.max_ui_value === I32_MAX ||
    template.max_ui_value === I32_MAX_LIMIT ||
    template.max_ui_value > 1e10 ||
    Number.isNaN(template.max_ui_value)
      ? undefined
      : Number.parseFloat(template.max_ui_value.toFixed(4))

  return {
    id: template.type === ParamType.List ? 'list' : 'param',
    name: template.name,
    type: ParamTypeStrMap[template.type],
    display_name: isEmptyString(template.display_name) ? undefined : template.display_name,
    default_value: template.default_value,
    items: template.enum_items,
    checked: template.checked_action !== undefined ? template.checked_action : template.checked,
    editable: template.editable_action !== undefined ? template.editable_action : template.editable,
    visible: template.visible_action !== undefined ? template.visible_action : template.visible,
    tooltip: template.tooltip,
    min_value,
    max_value,
    min_ui_value,
    max_ui_value,
    name_visible: template.name_visible,
    parent_list: undefined,
    expression: template.expression,
    content:
      template.type === ParamType.List && template.param_items
        ? templateListToDescriptionList(template.param_items)
        : undefined
  }
}

function layoutTemplateToDescription(template: LayoutTemplate): LayoutDescription {
  const display_name =
    template.display_name !== undefined && template.display_name.length > 0
      ? template.display_name
      : undefined

  return {
    id: LayoutTypeStrMap[template.type],
    content: templateListToDescriptionList(template.param_items),
    name: template.name,
    display_name,
    editable: template.editable_action !== undefined ? template.editable_action : template.editable,
    visible: template.visible_action !== undefined ? template.visible_action : template.visible,
    expanded: template.expanded,
    parent_list: undefined
  }
}

function templateToDescription(template: ParameterTemplate | LayoutTemplate): Description {
  return template.item_type === 'param'
    ? parameterTemplateToDescription(template)
    : layoutTemplateToDescription(template)
}

export function templateListToDescriptionList(
  template_list: (ParameterTemplate | LayoutTemplate)[]
): Description[] {
  return template_list.map(templateToDescription)
}

export const findParamItemDef = (name: string, defs?: ParamItemDef[]): ParamItemDef | undefined => {
  if (defs === undefined) {
    return undefined
  }

  for (const def of defs) {
    if (def.data.def.name === name) {
      return def
    }

    if (def.children !== undefined) {
      const result = findParamItemDef(name, def.children)
      if (result) return result
    }
  }
  return undefined
}

export const findChildParamItemDef = (
  name: string,
  def: ParamItemDef
): ParamItemDef | undefined => {
  if (def?.children?.length === 0) return undefined
  return findParamItemDef(name, def.children)
}

export const findParamItem = (
  name: string,
  param_items: ParamItemType[]
): ParamItemType | undefined => {
  for (const item of param_items) {
    const item_def = findParamItemDef(item.def_name)
    if (item_def?.data.item_type === ParamItemDefType.Layout) {
      if (item.def_name === name) return item
    } else {
      const parameter = item as Parameter
      if (parameter.name === name) return item
    }

    if (item.param_items) {
      const result = findParamItem(name, item.param_items)
      if (result) return result
    }
  }
}

export function isValidDescription(maybeTemplate: string): boolean {
  // TODO: check chould be better
  try {
    const description = JSON.parse(maybeTemplate)
    if (Array.isArray(description)) {
      return true
    }
  } catch (_e) {
    console.error('is not a valid description', _e)
    return false
  }
  return false
}

export function convertFromDescription(
  description: string,
  parent_id: string
): (ParameterTemplate | LayoutTemplate)[] {
  const description_list = JSON.parse(description) as Description[]
  return descriptionListToTemplateList(description_list, parent_id)
}

function descriptionListToTemplateList(
  description_list: Description[],
  parent_id?: string
): (ParameterTemplate | LayoutTemplate)[] {
  return description_list.map((description) => {
    return descriptionToTemplate(description, parent_id)
  })
}

function descriptionToTemplate(
  description: Description,
  parent_id?: string
): ParameterTemplate | LayoutTemplate {
  if (description.id === 'param' || description.id === 'list') {
    return parameterDescriptionToTemplate(description, parent_id)
  } else {
    return layoutDescriptionToTemplate(description as LayoutDescription, parent_id)
  }
}

function parameterDescriptionToTemplate(
  description: ParameterDescription,
  parent_id?: string
): ParameterTemplate {
  let checkable = false
  if (typeof description.checked !== 'undefined') {
    if (typeof description.checked === 'boolean') {
      checkable = true
    }
  }

  return {
    item_type: 'param',
    checkable: checkable,
    checked: typeof description.checked === 'boolean' ? description.checked : undefined,
    default_value: description.default_value,
    editable: typeof description.editable === 'boolean' ? description.editable : true,
    max_value: description.max_value ?? undefined,
    min_value: description.min_value ?? undefined,
    max_ui_value: description.max_ui_value ?? undefined,
    min_ui_value: description.min_ui_value ?? undefined,
    name: description.name,
    display_name: description.display_name === null ? undefined : description.display_name,
    name_visible: description.name_visible || true,
    tooltip: description.tooltip || '',
    type: ParamStrTypeMap[description.type],
    visible: typeof description.visible === 'boolean' ? description.visible : true,
    enum_items: description.items ?? undefined,
    expression: description.expression || undefined,
    value_action: undefined, // TODO
    visible_action: typeof description.visible === 'string' ? description.visible : undefined,
    editable_action: typeof description.editable === 'string' ? description.editable : undefined,
    checked_action: typeof description.checked === 'string' ? description.checked : undefined,
    param_items: description.content
      ? descriptionListToTemplateList(description.content, description.name)
      : [],

    is_custom: true,
    is_dirty: false,
    is_new: true,
    parent_id: parent_id
  }
}
function layoutDescriptionToTemplate(
  description: LayoutDescription,
  parent_id?: string
): LayoutTemplate {
  return {
    item_type: 'layout',
    name: description.name,
    display_name: description.display_name || '',
    type: LayoutStrTypeMap[description.id],
    editable: typeof description.editable === 'boolean' ? description.editable : true,
    visible: typeof description.visible === 'boolean' ? description.visible : true,
    expanded: typeof description.expanded === 'boolean' ? description.expanded : undefined,
    param_items: description.content
      ? descriptionListToTemplateList(description.content, description.name)
      : [],
    visible_action: typeof description.visible === 'string' ? description.visible : undefined,
    editable_action: typeof description.editable === 'string' ? description.editable : undefined,

    is_custom: true,
    is_dirty: false,
    is_new: true,
    parent_id: parent_id
  }
}
