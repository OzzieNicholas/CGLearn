import { GetNodeInputMeshesAttribs, setLayoutCollapsed } from '@/commands'
import { LayoutType, ParamActionType, ParamType } from '@/commands/definer'
import {
  AttributeType,
  ColorRampValue,
  ParamItemDef,
  LayoutData,
  ParameterData,
  ParamItemType,
  Parameter,
  ParamValue,
  ScalarRampValue,
  Layout,
  ParamItemDefType
} from '@/commands/index.types'
import ColorPicker from '@/components/ColorPicker'
import ColorRamp from '@/components/ColorRamp'
import Matrix from '@/components/Matrix'
import NumberInput, { NumberValueType } from '@/components/NumberInput'
import ScalarRamp from '@/components/ScalarRamp'
import Script from '@/components/Script'
import Select from '@/components/Select'
import { Slider, SliderValueType } from '@/components/Slider'
import TextInput from '@/components/TextInput'
import TextInputWithOptions from '@/components/TextInputWithOptions'
import Toggle from '@/components/Toggle'
import Tooltip from '@/components/Tooltip'
import Vector from '@/components/Vector'
import { cn } from '@/utils/cn'
import { colorResultToRgbaf, deepCopy, rgbafToHexa } from '@/utils/functions'
import { findChildParamItemDef, findParamItemDef, isParamValueSame } from '@/utils/parameter'
import { Tooltip as CustomTooltip, Spacer, Tab, Tabs } from '@nextui-org/react'
import React, { useCallback, useContext, useEffect, useMemo, useState } from 'react'
import { Case, Default, Else, If, Switch, Then } from 'react-if'
import { ParamPanelContext } from './context'
import ExpressionEditor from './ExpressionEditor'

import { BiReset } from 'react-icons/bi'
import { CgCodeSlash } from 'react-icons/cg'
import { MdKeyboardArrowDown, MdKeyboardArrowLeft } from 'react-icons/md'
import { I32_MAX_LIMIT, I32_MIN_LIMIT } from '@/constant/number'

export const isItemVisible = (item: ParamItemType): boolean => {
  return item.visible
}

function LabeledItem({
  children,
  label,
  name,
  tool,
  className,
  tooltip,
  labelVisible,
  horizontal
}: {
  children: React.ReactNode
  label: string
  name: string
  tool: React.ReactNode
  className?: string
  tooltip?: string
  labelVisible?: boolean
  horizontal?: boolean
}) {
  return (
    <Tooltip
      placement="left"
      content={label + (label !== name ? ` (${name})` : '') + (tooltip ? ':\n' + tooltip : '')}
      className="max-w-52"
    >
      <div
        className={cn(
          'rounded h-full transition-colors hover:bg-white/15 flex-grow',
          horizontal ? 'flex gap-2' : 'grid grid-cols-3 gap-4',
          className
        )}
      >
        <div className="flex gap-2">
          {tool}
          {!horizontal && labelVisible && <div className="mx-auto" />}
          {labelVisible && (
            <label
              className="select-none self-center truncate text-right opacity-80"
              style={{
                fontSize: '0.8rem'
              }}
            >
              {label}
            </label>
          )}
        </div>
        <div className="col-span-2 grow self-center align-middle">{children}</div>
      </div>
    </Tooltip>
  )
}

function computeRange(
  min_value: number,
  max_value: number,
  min_ui_value?: number,
  max_ui_value?: number
) {
  if (min_value > max_value) {
    ;[min_value, max_value] = [max_value, min_value]
  }

  if (min_ui_value === undefined) {
    if (min_value < -1e10) {
      min_ui_value = -10
    } else {
      min_ui_value = min_value
    }
  }

  if (max_ui_value === undefined) {
    if (max_value > 1e10) {
      if (min_value < 0) {
        max_ui_value = min_ui_value + 20
      } else {
        max_ui_value = min_ui_value + 10
      }
    } else {
      max_ui_value = max_value
    }
  }

  if (min_ui_value < min_value) {
    min_ui_value = min_value
  }
  if (max_ui_value > max_value) {
    max_ui_value = max_value
  }

  return [min_value, max_value, min_ui_value, max_ui_value]
}

function ParamItem({
  name,
  label,
  param,
  paramDef
}: {
  name: string
  label: string
  param: Parameter
  paramDef: ParameterData
}) {
  const { node, graph_id, setChange, node_graph } = useContext(ParamPanelContext)
  const [payload, setPayload] = useState<any>(null)

  useEffect(() => {
    if (
      paramDef.type === ParamType.Enum &&
      Array.isArray(paramDef.enum_items) &&
      paramDef.enum_items?.length > 0
    ) {
      setPayload(paramDef.enum_items)
    } else if (paramDef.type === ParamType.Color) {
      setPayload(param.value)
    }
  }, [name, param.value, paramDef.type, paramDef.enum_items])

  const size = useMemo(() => {
    switch (paramDef.type) {
      case ParamType.Vec2:
        return 2
      case ParamType.Vec3:
        return 3
      case ParamType.Vec4:
        return 4
      case ParamType.Vec2i:
        return 2
      case ParamType.Vec3i:
        return 3
      case ParamType.Vec4i:
        return 4
      case ParamType.Mat3:
        return 3
      case ParamType.Mat4:
        return 4
    }
    return 1
  }, [paramDef.type])

  const valueType = useMemo(() => {
    switch (paramDef.type) {
      case ParamType.Int:
        return SliderValueType.Int
      case ParamType.Float:
        return SliderValueType.Float
      case ParamType.Vec2:
        return SliderValueType.Float
      case ParamType.Vec3:
        return SliderValueType.Float
      case ParamType.Vec4:
        return SliderValueType.Float
      case ParamType.Vec2i:
        return SliderValueType.Int
      case ParamType.Vec3i:
        return SliderValueType.Int
      case ParamType.Vec4i:
        return SliderValueType.Int
      case ParamType.Mat3:
        return SliderValueType.Float
      case ParamType.Mat4:
        return SliderValueType.Float
      case ParamType.List:
        return SliderValueType.Int
    }
    return 1
  }, [paramDef.type])

  const onSetValue = useCallback(
    async (new_value: ParamValue) => {
      if (param.value == new_value) return

      let dirty = false
      if (paramDef.type == ParamType.List) {
        dirty = true
      } else if (paramDef.type != ParamType.Button) {
        if (
          isParamValueSame(paramDef.default_value, param.value) !=
          isParamValueSame(paramDef.default_value, new_value)
        ) {
          dirty = true
        }
      }

      let old_value: ParamValue | undefined = undefined
      if (
        paramDef.type == ParamType.Bool ||
        paramDef.type == ParamType.Enum ||
        paramDef.type == ParamType.Code ||
        paramDef.type == ParamType.String ||
        paramDef.type == ParamType.Group ||
        paramDef.type == ParamType.Mesh ||
        paramDef.type == ParamType.List
      ) {
        old_value = deepCopy(param.value)
      }

      if (paramDef.type === ParamType.Int) {
        if ((new_value as number) > I32_MAX_LIMIT) {
          new_value = I32_MAX_LIMIT
        } else if ((new_value as number) < I32_MIN_LIMIT) {
          new_value = I32_MIN_LIMIT
        }
      }

      param.value = new_value

      setChange({
        name: name,
        action: ParamActionType.Value,
        dirty
      })
      await node_graph.getState().setNodeParam(node.id, name, new_value)

      if (old_value !== undefined) {
        await onEditingFinished(old_value, new_value)
      }

      if (paramDef.type == ParamType.Color) {
        setPayload(new_value)
      }
    },
    [param]
  )

  const onEditingFinished = useCallback(
    (old_value: ParamValue, new_value: ParamValue) => {
      node_graph.getState().onNodeParamEditingFinished(node.label, name, old_value, new_value)
    },
    [param]
  )

  return (
    <div
      className={cn(
        'self-center flex-grow flex',
        paramDef.type == ParamType.Code && 'overflow-hidden'
      )}
    >
      <Switch>
        <Case condition={paramDef.type == ParamType.Bool}>
          {() => (
            <Toggle
              className="nodrag nopan checkbox-accent size-3.5"
              defaultChecked={param.value as boolean}
              checked={param.value as boolean}
              onChange={onSetValue}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.Int || paramDef.type == ParamType.Float}>
          {() => {
            const [limit_min, limit_max, range_min, range_max] = computeRange(
              paramDef.min_value ?? 0,
              paramDef.max_value ?? 0,
              paramDef.min_ui_value,
              paramDef.max_ui_value
            )
            return (
              <Slider
                valueType={valueType}
                value={param.value as number}
                rangeMin={range_min}
                rangeMax={range_max}
                limitMin={limit_min}
                limitMax={limit_max}
                step={paramDef.step}
                onValueChange={onSetValue}
                className="nodrag nopan flex h-4 w-full grow text-sm"
                onEditingFinished={onEditingFinished}
              />
            )
          }}
        </Case>
        <Case condition={paramDef.type == ParamType.List}>
          {() => {
            return (
              <NumberInput
                valueType={NumberValueType.Int}
                defaultValue={param.value as number}
                value={param.value as number}
                limitMin={0}
                step={1}
                onChange={onSetValue}
                className="nodrag nopan"
              />
            )
          }}
        </Case>
        <Case
          condition={
            paramDef.type == ParamType.Vec2 ||
            paramDef.type == ParamType.Vec3 ||
            paramDef.type == ParamType.Vec4 ||
            paramDef.type == ParamType.Vec2i ||
            paramDef.type == ParamType.Vec3i ||
            paramDef.type == ParamType.Vec4i
          }
        >
          {() => {
            const [limit_min, limit_max, range_min, range_max] = computeRange(
              paramDef.min_value ?? 0,
              paramDef.max_value ?? 0,
              paramDef.min_ui_value,
              paramDef.max_ui_value
            )
            return (
              <Vector
                size={size}
                valueType={valueType}
                value={param.value as number[]}
                minValue={range_min}
                maxValue={range_max}
                limitMin={limit_min}
                limitMax={limit_max}
                onChange={onSetValue}
                className="nodrag nopan flex h-4 w-full grow text-sm"
                onEditingFinished={onEditingFinished}
              />
            )
          }}
        </Case>
        <Case condition={paramDef.type == ParamType.Mat3 || paramDef.type == ParamType.Mat4}>
          {() => {
            const [limit_min, limit_max, range_min, range_max] = computeRange(
              paramDef.min_value ?? 0,
              paramDef.max_value ?? 0,
              paramDef.min_ui_value,
              paramDef.max_ui_value
            )
            return (
              <Matrix
                size={size}
                valueType={valueType}
                value={param.value as number[]}
                minValue={range_min}
                maxValue={range_max}
                limitMin={limit_min}
                limitMax={limit_max}
                onChange={onSetValue}
                className="nodrag nopan flex h-4 w-full grow text-sm"
                onEditingFinished={onEditingFinished}
              />
            )
          }}
        </Case>
        <Case
          condition={
            paramDef.type == ParamType.Group ||
            (paramDef.type == ParamType.String && groupParamKeys.has(label))
          }
        >
          {() => (
            <TextInputWithOptions
              defaultValue={param.value as string}
              value={param.value as string}
              onChange={onSetValue}
              className="nodrag nopan w-full grow"
              options={() => getInputGroups(graph_id, node.id)}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.String && attribParamKeys.has(label)}>
          {() => (
            <TextInputWithOptions
              defaultValue={param.value as string}
              value={param.value as string}
              onChange={onSetValue}
              className="nodrag nopan w-full grow"
              options={() => getInputAttribs(graph_id, node.id)}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.String || paramDef.type == ParamType.Mesh}>
          {() => (
            <TextInput
              defaultValue={param.value as string}
              value={param.value as string}
              onChange={onSetValue}
              className="nodrag nopan w-full grow"
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.Color}>
          {() => (
            <ColorPicker
              placement="left-start"
              color={rgbafToHexa(param.value as number[])}
              onChange={(c) => {
                onSetValue(colorResultToRgbaf(c))
              }}
              onEditingFinished={onEditingFinished}
            >
              <button
                className="nodrag nopan btn btn-ghost h-4 min-h-0 w-12 rounded-md border-1 border-black/50"
                style={{
                  backgroundColor: rgbafToHexa(
                    payload ? (payload as number[]) : (param.value as number[])
                  )
                }}
              />
            </ColorPicker>
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.Enum}>
          {() => (
            <Select
              className="nodrag nopan select-accent bg-white/20 text-xs"
              onChange={(v) => onSetValue((payload as string[]).indexOf(v))}
              value={payload ? payload[param.value as number] : '...'}
              options={payload || [{ value: '...' }]}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.Button}>
          {() => (
            <button
              className="nodrag nopan btn btn-ghost h-5 min-h-0 rounded-md border-1 border-white/20 bg-white/20 text-xs"
              onClick={() => onSetValue(!param.value as boolean)}
            >
              {label}
            </button>
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.Code}>
          {() => (
            <Script
              className="nodrag nopan w-full text-sm"
              value={param.value as string}
              lang={node.type_name.toLowerCase() as any}
              onChange={onSetValue}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.ScalarRamp}>
          {() => (
            <ScalarRamp
              value={param.value as ScalarRampValue[]}
              onChange={onSetValue}
              className="nodrag nopan"
              onEditingFinished={onEditingFinished}
            />
          )}
        </Case>
        <Case condition={paramDef.type == ParamType.ColorRamp}>
          {() => (
            <ColorRamp
              value={param.value as ColorRampValue[]}
              onChange={onSetValue}
              className="nodrag nopan"
              onEditingFinished={onEditingFinished}
            />
          )}
        </Case>
        <Default>{() => <label>{label}</label>}</Default>
      </Switch>
    </div>
  )
}

function Param({
  name,
  param,
  className,
  horizontal,
  readonly
}: {
  name: string
  param: Parameter
  className?: string
  horizontal?: boolean
  readonly?: boolean
}) {
  const { node, node_def, node_graph, setChange, showContextMenu } = useContext(ParamPanelContext)
  const paramDef = findParamItemDef(param.def_name, node_def.children)!.data.def! as ParameterData
  const [checked, setChecked] = useState<boolean>(!paramDef.checkable || param.checked)
  const [openExprEditor, setOpenExprEditor] = useState<boolean | undefined>(undefined)

  if (!readonly && !param.editable) {
    readonly = true
  }

  const tool = () => {
    return (
      <>
        {((param.expression?.length && param.expression?.length > 0) || openExprEditor) && (
          <CustomTooltip
            isOpen={openExprEditor}
            placement="bottom"
            content={
              <ExpressionEditor
                param={param}
                readonly={readonly || !checked}
                onBlur={() => {
                  if (openExprEditor) {
                    setOpenExprEditor(false)
                  }
                }}
                onChange={
                  readonly || !checked
                    ? undefined
                    : async (value: string) => {
                        const old_value = deepCopy(param.expression)
                        await node_graph.getState().setNodeParam(node.id, name, value, 'expr')
                        await node_graph
                          .getState()
                          .onNodeParamEditingFinished(node.label, name, old_value, value, 'expr')
                        param.expression = value
                      }
                }
              />
            }
            className="m-0 rounded-sm bg-white/30 p-[2px]"
            onClick={() => {
              if (!openExprEditor) setOpenExprEditor(true)
            }}
            closeDelay={50}
          >
            <div
              className={cn(
                'btn btn-ghost btn-circle min-h-0 size-5 border-1 border-btn-active/30 hover:border-btn-active/80'
              )}
              onMouseEnter={() => {
                if (openExprEditor === false) setOpenExprEditor(undefined)
              }}
            >
              <CgCodeSlash className="size-full p-px text-btn-active/80" />
            </div>
          </CustomTooltip>
        )}
        {paramDef.checkable && (
          <Toggle
            className={cn('size-3.5', readonly && 'pointer-events-none')}
            defaultChecked={param.checked}
            onChange={async (checked) => {
              setChecked(checked)
              param.checked = checked
              setChange({
                name: name,
                action: ParamActionType.Checked
              })
              await node_graph.getState().setNodeParam(node.id, name, checked, 'check')
              await node_graph
                .getState()
                .onNodeParamEditingFinished(node.label, name, !checked, checked, 'check')
            }}
          />
        )}
      </>
    )
  }

  const label = paramDef.display_name || paramDef.name

  return (
    <div
      className={cn(
        'relative flex gap-1 flex-row',
        shrinkTypes.has(paramDef.type) ? 'flex-shrink' : 'flex-grow',
        (readonly || !checked) && 'opacity-60'
      )}
      onContextMenu={(e) => {
        // e.preventDefault()
        showContextMenu({
          param_name: name,
          param,
          param_def: paramDef,
          readonly: readonly || !checked,
          pos: {
            x: e.clientX,
            y: e.clientY
          },
          onAddExpression: () => {
            setOpenExprEditor(true)
          }
        })
      }}
    >
      <LabeledItem
        label={label}
        name={param.name}
        tooltip={paramDef.tooltip}
        className={cn(className, param.expression && 'bg-purple-500/20 hover:bg-purple-500/40')}
        labelVisible={paramDef.name_visible && paramDef.type != ParamType.Button}
        horizontal={horizontal}
        tool={tool()}
      >
        <div
          className={cn('flex gap-1 w-full', (readonly || !param.checked) && 'pointer-events-none')}
        >
          <ParamItem name={name} param={param} paramDef={paramDef} label={label} />
          {paramDef.type != ParamType.Button &&
            !isParamValueSame(paramDef.default_value, param.value) &&
            !horizontal && (
              <div
                className="btn btn-circle btn-ghost size-5 min-h-0 self-center"
                onClick={async () => {
                  setChange({
                    name: name,
                    action: ParamActionType.Value,
                    dirty: true
                  })
                  const old_value = deepCopy(param.value)
                  await node_graph.getState().setNodeParam(node.id, name, paramDef.default_value)
                  await node_graph
                    .getState()
                    .onNodeParamEditingFinished(node.label, name, old_value, paramDef.default_value)
                  param.value = paramDef.default_value
                }}
              >
                <BiReset className="size-full p-px text-btn-active/80" />
              </div>
            )}
        </div>
      </LabeledItem>
    </div>
  )
}

function VLayout({
  layout,
  itemDef,
  readonly
}: {
  layout: Layout
  itemDef: ParamItemDef
  readonly?: boolean
}) {
  const { graph_id, node } = useContext(ParamPanelContext)
  const layoutData = itemDef.data.def as LayoutData
  const collapsible = layoutData.expanded !== undefined && itemDef.name.length > 0
  const [collapsed, setCollapsed] = useState<boolean>(layout.collapsed)

  const label = layoutData.display_name || itemDef.name

  return (
    <div className="flex w-full flex-col gap-1.5">
      {collapsible && (
        <div
          className={cn(
            'transition-colors w-full bg-white/15 text-center rounded-lg cursor-pointer hover:bg-white/20 flex active:bg-white/30',
            collapsed && 'bg-bg-white/10 hover:bg-white/15'
          )}
          onClick={() => {
            const new_collapsed = !collapsed
            setCollapsed(new_collapsed)
            setLayoutCollapsed([graph_id, node.id, itemDef.name, new_collapsed])
          }}
        >
          <div className="ml-auto w-5"></div>
          <label className="pointer-events-none select-none text-center">{label}</label>
          <div className=" ml-auto w-5 self-center">
            {collapsed ? <MdKeyboardArrowLeft /> : <MdKeyboardArrowDown />}
          </div>
        </div>
      )}
      {!collapsed &&
        layout.param_items
          ?.filter((item) => isItemVisible(item))
          .map((item, idx) => {
            const child_item_def = findParamItemDef(item.def_name, itemDef.children)

            if (child_item_def === undefined) {
              return <div key={idx}>Can not find child item def!</div>
            }

            if (child_item_def.data.item_type == ParamItemDefType.Layout) {
              return (
                <LayoutParameters
                  key={idx}
                  layout={item as Layout}
                  itemDef={child_item_def}
                  readonly={readonly}
                />
              )
            } else {
              return (
                <ParamLayout
                  key={idx}
                  parameter={item as Parameter}
                  itemDef={child_item_def}
                  readonly={readonly}
                />
              )
            }
          })}
    </div>
  )
}

function ParamLayout({
  parameter,
  itemDef,
  readonly
}: {
  parameter: Parameter
  itemDef: ParamItemDef
  readonly?: boolean
}) {
  return (
    <>
      <Param name={parameter.name} param={parameter} className="w-full" readonly={readonly} />
      {parameter.param_items &&
        Array.isArray(parameter.param_items) &&
        parameter.param_items.length > 0 &&
        parameter.param_items.map((item, idx) => {
          if (item.visible === false) return null

          const item_def = findParamItemDef(item.def_name, itemDef.children)

          return item_def?.data.item_type === ParamItemDefType.Layout ? (
            <LayoutParameters
              key={idx}
              layout={item as Layout}
              itemDef={item_def}
              readonly={readonly}
            />
          ) : (
            <ParamLayout
              key={idx}
              parameter={item as Parameter}
              itemDef={item_def!}
              readonly={readonly}
            />
          )
        })}
    </>
  )
}

function HLayout({
  layout,
  itemDef,
  readonly
}: {
  layout: Layout
  itemDef: ParamItemDef
  readonly?: boolean
}) {
  return (
    <div className="flex w-full flex-row gap-1">
      {layout.param_items
        ?.filter((item) => isItemVisible(item))
        .map((item, idx1) => {
          const child_item_def = findParamItemDef(item.def_name, itemDef.children)

          if (child_item_def === undefined) {
            return <div key={idx1}>Can not find child item def!</div>
          }

          if (child_item_def?.data.item_type == ParamItemDefType.Layout) {
            const item_def = findChildParamItemDef(item.def_name, itemDef)
            return (
              <LayoutParameters
                key={idx1}
                layout={item as Layout}
                itemDef={item_def}
              />
            )
          } else {
            const parameter = item as Parameter

            return (
              <React.Fragment key={idx1}>
                <Param
                  name={parameter.name}
                  param={parameter}
                  horizontal={true}
                  readonly={readonly}
                />
                {parameter.param_items &&
                  Array.isArray(parameter.param_items) &&
                  parameter.param_items.length > 0 &&
                  parameter.param_items.map((item2, idx2) => {
                    const item_def = findParamItemDef(item.def_name, child_item_def.children)

                    return item_def?.data.item_type === ParamItemDefType.Layout ? (
                      <LayoutParameters
                        key={idx2}
                        layout={item2 as Layout}
                        itemDef={item_def}
                        readonly={readonly}
                      />
                    ) : (
                      <Param
                        key={idx2}
                        name={(item2 as Parameter).name}
                        param={item2 as Parameter}
                        readonly={readonly}
                      />
                    )
                  })}
              </React.Fragment>
            )
          }
        })}
    </div>
  )
}

function TabLayout({
  layout,
  itemDef,
  readonly
}: {
  layout: Layout
  itemDef: ParamItemDef
  readonly?: boolean
}) {
  return (
    <Tabs
      aria-label={itemDef.name}
      className="w-full justify-center"
      fullWidth={false}
      variant="solid"
      size="sm"
      color="secondary"
      radius="lg"
      classNames={{
        tabList: 'bg-white/10',
        cursor: 'w-full bg-btn-active/60',
        tabContent: 'text-text-primary/80'
      }}
    >
      {layout.param_items
        ?.filter((item) => {
          const item_def = findChildParamItemDef(item.def_name, itemDef)
          return item_def?.data.item_type == ParamItemDefType.Layout && isItemVisible(item)
        })
        .map((item, idx) => {
          item = item as Layout
          const item_def = findChildParamItemDef(item.def_name, itemDef)
          return (
            <Tab
              key={item_def?.name || idx}
              title={
                <div className="flex items-center space-x-2">
                  <span>{item_def?.data.def.display_name || item_def?.name}</span>
                </div>
              }
            >
              <LayoutParameters layout={item} itemDef={item_def} readonly={readonly} />
            </Tab>
          )
        })}
    </Tabs>
  )
}

export function LayoutParameters({
  layout,
  itemDef,
  className,
  readonly
}: {
  layout: Layout
  itemDef?: ParamItemDef
  className?: string
  readonly?: boolean
}) {
  const is_root = itemDef === undefined
  const { node_def } = useContext(ParamPanelContext)

  if (!itemDef) {
    itemDef = findParamItemDef(layout.def_name, node_def.children)
  }

  if (!itemDef) {
    return <div>Can not find layout def!</div>
  }

  readonly = readonly !== undefined ? readonly : !layout.editable

  return (
    <If condition={itemDef.data.def.type != LayoutType.Spacer}>
      <Then>
        {() => (
          <div
            className={cn(
              'w-full p-[1px] px-1',
              !is_root && 'bg-menu-bg/5 rounded-md border-1 border-text-primary/5',
              className
            )}
          >
            <Switch>
              <Case condition={itemDef.data.def.type === LayoutType.Vertical}>
                {() => <VLayout layout={layout} itemDef={itemDef} readonly={readonly} />}
              </Case>
              <Case condition={itemDef.data.def.type === LayoutType.Horizontal}>
                {() => <HLayout layout={layout} itemDef={itemDef} readonly={readonly} />}
              </Case>
              <Case condition={itemDef.data.def.type === LayoutType.Stack}>
                {() => <TabLayout layout={layout} itemDef={itemDef} readonly={readonly} />}
              </Case>
            </Switch>
          </div>
        )}
      </Then>
      <Else>{() => <Spacer className="w-full px-2" />}</Else>
    </If>
  )
}

const attribClsNames: AttributeType[] = ['point', 'vertex', 'prim', 'global']

const getInputAttribs = async (graph_id: number, node_id: number) => {
  const result = await GetNodeInputMeshesAttribs([graph_id, node_id])
  if (result.length == 0) return []
  let attrib_names = new Set<string>()
  for (const input_attribs of result) {
    for (const cls of attribClsNames) {
      const attribs = input_attribs[cls]
      if (attribs !== undefined && attribs.length > 0) {
        for (const attrib of attribs) {
          attrib_names.add(attrib.name)
        }
      }
    }
  }
  return Array.from(attrib_names)
}

const getInputGroups = async (graph_id: number, node_id: number) => {
  const result = await GetNodeInputMeshesAttribs([graph_id, node_id])
  if (result.length == 0) return []
  let group_names = new Set<string>()
  for (const input_attribs of result) {
    for (const cls of attribClsNames) {
      const attribs = input_attribs[cls]
      if (attribs !== undefined && attribs.length > 0) {
        for (const attrib of attribs) {
          if (attrib.type === 'bool' || attrib.type.endsWith('group')) {
            group_names.add(attrib.name)
          }
        }
      }
    }
  }
  return Array.from(group_names)
}

const groupParamKeys = new Set([
  'Group',
  'Group Name',
  'Group Names',
  'Groups',
  'Point Group',
  'Prim Group',
  'Primitive Group',
  'Vertex Group',
  'Edge Group',
  'Point Groups',
  'Prim Groups',
  'Primitive Groups',
  'Vertex Groups',
  'Edge Groups'
])

const attribParamKeys = new Set([
  'Attribute',
  'Attribute Name',
  'Attribute Names',
  'Attributes',
  'Point Attribute',
  'Prim Attribute',
  'Primitive Attribute',
  'Vertex Attribute',
  'Global Attribute',
  'Point Attributes',
  'Prim Attributes',
  'Primitive Attributes',
  'Vertex Attributes',
  'Global Attributes'
])

const shrinkTypes = new Set<ParamType>([
  ParamType.Bool,
  ParamType.Button,
  ParamType.Color,
  ParamType.Enum,
  ParamType.List
])
