import { getParamPath, getParamValueByPath, getRelativeParamPath } from '@/commands'
import { Parameter, ParameterData, ParamValue } from '@/commands/index.types'
import RecursiveMenu from '@/components/RecursiveMenu'
import { deepCopy, isNotEmptyString } from '@/utils/functions'
import { isParamValueSame } from '@/utils/parameter'
import {
  readText as clipboardRead,
  writeText as clipboardWrite
} from '@tauri-apps/plugin-clipboard-manager'
import { memo, useCallback, useContext, useMemo } from 'react'
import { useAsync } from 'react-use'
import { ParamPanelContext } from './context'

const ParameterContextMenu = ({
  name,
  param,
  paramDef,
  readonly,
  left,
  top,
  onClick,
  onAddExpression
}: {
  name: string
  param: Parameter
  paramDef: ParameterData
  readonly: boolean
  left?: number
  top?: number
  onClick: () => void
  onAddExpression: () => void
}) => {
  const { node, graph_id, node_graph } = useContext(ParamPanelContext)
  const state = useAsync(clipboardRead)

  const setParam = useCallback(
    async (new_value: ParamValue, old_value: ParamValue, type?: any) => {
      await node_graph.getState().setNodeParam(node.id, name, new_value, type)
      await node_graph
        .getState()
        .onNodeParamEditingFinished(node.label, name, old_value, new_value, type)
      param.value = new_value
    },
    [param]
  )

  const menus = useMemo(() => {
    return [
      {
        label: 'common.copy',
        action: async () => {
          return clipboardWrite(await getParamPath([graph_id, node.id, name]), {
            label: 'nous-parameter'
          })
        }
      },
      {
        label: 'menu.copy-name',
        action: async () => {
          return clipboardWrite(param.name)
        }
      },
      ...(!readonly && state.value && state.value.trim().length > 0
        ? [
            {
              label: 'common.paste',
              action: async () => {
                const value = await getParamValueByPath([graph_id, state.value!.trim()])
                if (value) {
                  setParam(value, deepCopy(param.value))
                }
              }
            },
            {
              label: 'menu.paste-as-expr',
              action: async () => {
                const path = await getRelativeParamPath([
                  graph_id,
                  node.id,
                  name,
                  state.value!.trim()
                ])
                if (path.length > 0) {
                  setParam(`return value("${path}")`, deepCopy(param.expression || ''), 'expr')
                }
              }
            }
          ]
        : []),
      ...(!readonly
        ? [
            {
              label: '',
              divider: true
            },
            isParamValueSame(param.value, paramDef.default_value)
              ? undefined
              : {
                  label: 'menu.reset-value',
                  action: async () => {
                    setParam(paramDef.default_value, deepCopy(param.value))
                  }
                },
            isNotEmptyString(param.expression)
              ? {
                  label: 'menu.delete-expr',
                  action: async () => {
                    setParam('', deepCopy(param.expression || ''), 'expr')
                  }
                }
              : {
                  label: 'menu.add-expr',
                  action: onAddExpression
                }
          ]
        : [])
    ].filter((m) => m !== undefined)
  }, [param, state])

  return (
    <div style={{ left, top }} className="absolute z-10 w-48" onClick={onClick}>
      <RecursiveMenu
        menus={menus as any}
        defaultOpen={true}
        asChild
        className="w-48 animate-fade-in"
      >
        <span className="m-0 h-0 p-0 text-[0]"></span>
      </RecursiveMenu>
    </div>
  )
}

export default memo(ParameterContextMenu)
