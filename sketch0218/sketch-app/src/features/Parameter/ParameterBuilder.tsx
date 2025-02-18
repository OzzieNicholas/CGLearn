import { LayoutType, ParamType } from '@/commands/definer'
import {
  ParamItemDef,
  ParamItemDefType,
  LayoutData,
  NodeData as NodeDataType,
  NodeDef,
  ParamValue,
  ParameterData
} from '@/commands/index.types'
import {
  Dialog,
  DialogButton,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle
} from '@/components/Dialog'
import DropdownMenu from '@/components/DropdownMenu'
import { ResizableHandle, ResizablePanel, ResizablePanelGroup } from '@/components/Resizable'
import Script from '@/components/Script'
import TextInput from '@/components/TextInput'
import Toggle from '@/components/Toggle'
import { I32_MAX, I32_MIN } from '@/constant/number'
import { useI18n } from '@/locales'
import { useNodeGraphBridgeStoreApi } from '@/stores/node-graph-bridge'
import { NodeData } from '@/types/node-graph'
import { LayoutTemplate, ParameterTemplate } from '@/types/parameter'
import { cn, join } from '@/utils/cn'
import {
  convertFromDescription,
  generateNodeParametersDescription,
  isValidDescription,
  rebuildNodeParameters
} from '@/utils/parameter'
import NiceModal, { useModal } from '@ebay/nice-modal-react'
import {
  DropOptions,
  MultiBackend,
  NodeModel,
  Tree,
  TreeMethods,
  getBackendOptions,
  getDescendants
} from '@minoru/react-dnd-treeview'
import { Divider } from '@nextui-org/react'
import { listen } from '@tauri-apps/api/event'
import {
  writeText as clipboardWrite,
  readText as clipboardRead
} from '@tauri-apps/plugin-clipboard-manager'
import React, { useCallback, useEffect, useRef, useState } from 'react'
import { DndProvider } from 'react-dnd'
import { Else, If, Then } from 'react-if'
import { useDebounce } from 'react-use'
import { createNode } from '../NodeGraph/ContextMenu'
import { NodeGraphStoreProvider, useNodeGraphStoreApi } from '@/stores/node-graph'
import { emit as emitGlobalAction } from '@/stores/global-actions'
import { Maybe } from '@/types/maybe'
import { t } from 'i18next'

import { presets } from '@/css/presets'
import { FaCaretRight } from 'react-icons/fa'
import { IoMdAdd, IoMdRemove } from 'react-icons/io'
import { PiFolderOpenFill } from 'react-icons/pi'
import { SiTemporal } from 'react-icons/si'

const buildTreeItems = (item_def: ParamItemDef, parent: string, out_items: Record<string, any>) => {
  if (item_def.data.item_type === ParamItemDefType.Layout) {
    buildTreeLayoutItem(item_def, parent, out_items)
  } else {
    buildTreeParamItem(item_def, parent, out_items)
  }
}

const buildTreeLayoutItem = (
  layout_def: ParamItemDef,
  parent: string,
  out_items: Record<string, any>
) => {
  if (!layout_def) return

  const new_item = {
    id: layout_def.name,
    text: layout_def.name,
    parent,
    droppable: (layout_def.data.def as LayoutData).type !== LayoutType.Spacer,
    draggale: !layout_def?.data.def.builtin,
    data: {
      is_folder: layout_def.data.def.type != LayoutType.Spacer || false,
      is_custom: !layout_def?.data.def.builtin,
      is_dirty: false,
      readonly: layout_def?.data.def.builtin,
      layout_def: {
        ...layout_def.data.def,
        param_items: [],
        previous_name: layout_def.name
      }
    }
  }
  out_items.push(new_item)

  for (const item_def of layout_def.children || []) {
    buildTreeItems(item_def, new_item.id, out_items)
  }
}

const buildTreeParamItem = (
  param_def: ParamItemDef,
  parent: string,
  out_items: Record<string, any>
) => {
  if (!param_def) return

  const new_item = {
    id: param_def.name,
    text: param_def.name,
    parent,
    droppable: (param_def.data.def as ParameterData).type === ParamType.List,
    draggale: !param_def?.data.def.builtin,
    data: {
      is_folder: param_def.data.def.type === ParamType.List,
      is_custom: !param_def.data.def.builtin,
      is_dirty: false,
      readonly: param_def?.data.def.builtin,
      param_def: {
        ...param_def.data.def,
        name: param_def.name,
        previous_name: param_def.name
      }
    }
  }

  out_items.push(new_item)

  for (const item_def of param_def.children || []) {
    buildTreeItems(item_def, new_item.id, out_items)
  }
}

const convertToTemplate = (
  tree: NodeModel<CustomData>[],
  maybe_root_id: Maybe<number | string> = undefined
) => {
  const mapTreeNodeToTemplate = (child: NodeModel<CustomData>) => {
    if (child?.data?.layout_def) {
      let layout_template: LayoutTemplate = {
        is_custom: child.data.is_custom,
        is_dirty: child.data.is_dirty,
        is_new: child.data.is_new,
        ...child.data.layout_def,
        previous_name: child.data.layout_def.previous_name,
        param_items: child.data.layout_def.type != LayoutType.Spacer ? findItems(child) : [],
        item_type: 'layout',
        parent_id: child.parent.toString()
      }
      return layout_template
    } else if (child?.data?.param_def) {
      const param_template: ParameterTemplate = {
        is_custom: child.data.is_custom,
        is_dirty: child.data.is_dirty,
        is_new: child.data.is_new,
        ...child.data.param_def,
        previous_name: child.data.param_def.previous_name,
        param_items: child.data.param_def.type == ParamType.List ? findItems(child) : undefined,
        item_type: 'param',
        parent_id: child.parent.toString()
      }
      return param_template
    } else {
      return undefined
    }
  }

  const findItems = (parent: NodeModel<CustomData>) => {
    const children = tree.filter((n) => n.parent === parent.id)
    if (children === undefined) {
      return []
    }

    return children
      .map((child) => {
        return mapTreeNodeToTemplate(child)
      })
      .filter((n) => n !== undefined) as (ParameterTemplate | LayoutTemplate)[]
  }

  const root_id = maybe_root_id ? maybe_root_id : 'root'

  const root = tree.find((n) => n.id === root_id)
  if (!root || !root.data) {
    console.error('root not found, root id is: ', root_id, '; tree is: ', tree)
    return []
  }

  const result: (LayoutTemplate | ParameterTemplate)[] = []

  if (maybe_root_id !== undefined) {
    const root_template = mapTreeNodeToTemplate(root)
    if (root_template) {
      result.push(root_template)
    }
  } else {
    const children = findItems(root)
    result.push(...children)
  }

  return result
}

const rangeNumberToString = (value?: number) => {
  if (
    value === undefined ||
    value > 1e10 ||
    value < -1e10 ||
    value === I32_MAX ||
    value === I32_MIN
  ) {
    return ''
  }
  return Number.parseFloat(value.toFixed(4)).toString()
}

function ParameterBuilder({
  node_id,
  node,
  node_def,
  node_data
}: {
  node_id: number
  node: NodeData
  node_def: NodeDef
  node_data: NodeDataType
}) {
  const modal = useModal()
  const { t } = useI18n()
  const [treeData, setTreeData] = useState<NodeModel<CustomData>[]>([])
  const [selectedNode, setSelectedNode] = useState<NodeModel<CustomData> | null>(null)
  const [initialOpen, setInitialOpen] = useState<string[]>([])
  const [size, setSize] = useState<{ width: number; height: number } | null>(null)

  const ref = useRef<HTMLDivElement>(null)
  const handleRef = useRef<HTMLDivElement>(null)
  const treeRef = useRef<TreeMethods>(null)
  const dirty = useRef<boolean>(false)

  const storeApi = useNodeGraphBridgeStoreApi()
  const graphStoreApi = useNodeGraphStoreApi()

  useEffect(() => {
    const unlisten = listen<string>('RemoveNodeEvent', (event) => {
      if (!node) return
      if (parseInt(JSON.parse(event.payload).node_id) == node_id) {
        modal.remove()
      }
    })
    return () => {
      unlisten.then((fn) => fn())
    }
  }, [node])

  useEffect(() => {
    let items: NodeModel<CustomData>[] = []
    if (node_data && node_def) {
      items.push({
        id: 'root-root',
        text: 'root-root',
        parent: ''
      })

      // note: assert node_def.children![0] is root layout
      buildTreeItems(node_def.children![0], 'root-root', items)
    }
    setTreeData(items)
    setInitialOpen(items.filter((i) => i.data?.is_folder).map((i) => i.id) as any)
  }, [node_data, node_def])

  const cloneNode = useCallback(async (node_type: string, params: NodeModel<CustomData>[]) => {
    const [node_id, node_data_id] = await createNode(node_type, graphStoreApi, {
      node_pos: {
        x: node_data.pos_x,
        y: node_data.pos_y + 200
      }
    })

    if (!node_id) {
      console.error(`create node ${node.type_name} failed`)
      return
    }

    const graph_id = storeApi.getState().graph_id

    if (graph_id !== undefined) {
      rebuildNodeParameters(graph_id, node_data_id as number, convertToTemplate(params))
      modal.remove()
    }

    const suffix = '_Copy'
    const node_name = node_data.name.endsWith(suffix)
      ? `${node_data.name}${suffix}`
      : `${node.type_name}${suffix}`
    await graphStoreApi.getState().renameNode(node_id as string, node_name)

    const node_path = await graphStoreApi.getState().getNodePath(node_data_id as number)
    emitGlobalAction('jumpToNode', node_path)
  }, [])

  const handleSelect = useCallback(
    (node: NodeModel<CustomData>) => setSelectedNode(node),
    [setSelectedNode]
  )

  const handleDrop = useCallback(
    (new_tree_data: NodeModel<CustomData>[], options: DropOptions<CustomData>) => {
      setTreeData(new_tree_data)
      setSelectedNode((n) => {
        if (n?.id === options.dragSource?.id) return n
        return options.dragSource as any
      })
      dirty.current = true
    },
    [setTreeData, setSelectedNode]
  )

  const handelDelete = useCallback(
    (tree_node: NodeModel<CustomData>) => {
      if (selectedNode && selectedNode.id === tree_node.id) {
        setSelectedNode(null)
      }
      const delete_ids = [tree_node.id, ...getDescendants(treeData, tree_node.id).map((n) => n.id)]
      setTreeData(treeData.filter((n) => !delete_ids.includes(n.id)))
      dirty.current = true
    },
    [selectedNode, treeData]
  )

  const getUniqueName = useCallback((name: string, treeData: NodeModel<CustomData>[]) => {
    let names = new Set<string>()
    treeData.forEach((n) => {
      names.add(n.id as string)
      names.add(n.text)
      if (n.data?.param_def) {
        names.add(n.data.param_def.name)
      }
      if (n.data?.layout_def) {
        names.add(n.data.layout_def.name)
      }
    })
    const prefix = name
    let postfix = 1
    while (names.has(name)) {
      name = `${prefix}${postfix}`
      postfix += 1
    }
    return name
  }, [])

  const handleAdd = useCallback(
    (tree_node: NodeModel<CustomData>, param?: ParamType, layout?: LayoutType) => {
      if (param !== undefined) {
        const name = getUniqueName(ParamType[param], treeData)
        const is_list = param == ParamType.List

        setTreeData((tree) => [
          ...tree,
          {
            id: name,
            text: name,
            parent: tree_node.id,
            droppable: is_list,
            data: {
              is_folder: is_list,
              is_custom: true,
              is_dirty: true,
              is_new: true,
              param_def: {
                name: name,
                checkable: false,
                checked: undefined,
                default_value: getParamDefaultValue(param),
                editable: true,
                max_value: I32_MAX,
                min_value: I32_MIN,
                name_visible: true,
                tooltip: '',
                type: param,
                visible: true,
                builtin: false,
                param_items: []
              }
            }
          }
        ])

        if (is_list && treeRef.current) {
          treeRef.current.open(name)
        }
      }
      if (layout !== undefined) {
        const name = getUniqueName(LayoutType[layout], treeData)
        const is_spacer = layout == LayoutType.Spacer
        setTreeData((tree) => [
          ...tree,
          {
            id: name,
            text: name,
            parent: tree_node.id,
            droppable: !is_spacer,
            is_dirty: true,
            is_new: true,
            data: {
              is_folder: !is_spacer,
              is_custom: true,
              is_dirty: true,
              is_new: true,
              layout_def: {
                item_type: 'layout',
                id: 0,
                name: name,
                display_name: undefined,
                type: layout,
                editable: true,
                visible: true,
                expanded: undefined,
                param_items: [],
                builtin: false
              }
            }
          }
        ])
        if (!is_spacer && treeRef.current) {
          treeRef.current.open(name)
        }
      }
      dirty.current = true
    },
    [getUniqueName, setTreeData, treeData]
  )

  const handleRename = useCallback(
    (id: string, new_name: string) => {
      let new_node: NodeModel<CustomData> | null = null

      setTreeData((tree) => {
        const tree_node = tree.find((n) => n.id === id)
        if (tree_node && tree_node.data) {
          new_name = getUniqueName(new_name, treeData)
          if (tree_node.data.layout_def) {
            new_node = {
              ...tree_node,
              data: {
                ...tree_node.data,
                layout_def: {
                  ...tree_node.data.layout_def!,
                  name: new_name
                },
                is_dirty: true
              },
              text: new_name
            }
          } else if (tree_node.data.param_def) {
            new_node = {
              ...tree_node,
              data: {
                ...tree_node.data,
                param_def: {
                  ...tree_node.data.param_def!,
                  name: new_name
                },
                is_dirty: true
              },
              text: new_name
            }
          }
          if (new_node) {
            return tree.map((n) => (n.id === tree_node.id ? new_node! : n))
          }
        }
        return tree
      })
      if (new_node) {
        setSelectedNode(new_node)
      }
    },
    [getUniqueName, setTreeData, setSelectedNode, treeData]
  )

  const handleUpdate = useCallback(
    (id: string, data: ParameterData | LayoutData) => {
      let new_node: NodeModel<CustomData> | null = null

      setTreeData((tree) => {
        const tree_node = tree.find((n) => n.id === id)
        if (tree_node && tree_node.data) {
          if (tree_node.data.layout_def) {
            new_node = {
              ...tree_node,
              data: {
                ...tree_node.data,
                layout_def: data as LayoutData,
                is_dirty: true
              }
            }
          } else if (tree_node.data.param_def) {
            new_node = {
              ...tree_node,
              data: {
                ...tree_node.data,
                param_def: data as ParameterData,
                is_dirty: true
              }
            }
          }
          if (new_node) {
            return tree.map((n) => (n.id === tree_node.id ? new_node! : n))
          }
        }
        return tree
      })

      if (new_node) {
        setSelectedNode(new_node)
      }
    },
    [setTreeData, setSelectedNode]
  )

  const onPointerDown = useCallback((e: React.PointerEvent) => {
    if (e.buttons == 1 && ref.current !== null && handleRef.current !== null) {
      handleRef.current.setPointerCapture(e.pointerId)
    }
  }, [])

  const onPointerUp = useCallback((e: React.PointerEvent) => {
    if (handleRef.current !== null) {
      handleRef.current.releasePointerCapture(e.pointerId)
    }
  }, [])

  const onPointerMove = useCallback(
    (e: React.PointerEvent) => {
      e.preventDefault()
      e.stopPropagation()
      if (e.buttons == 1 && ref.current !== null) {
        setSize((s) => {
          const width = Math.max((s ? s.width : ref.current!.clientWidth) + e.movementX * 2, 200)
          const height = Math.max((s ? s.height : ref.current!.clientHeight) + e.movementY * 2, 200)
          return { width, height }
        })
      }
    },
    [setSize]
  )

  const handleCopy = useCallback(
    (node: NodeModel<CustomData>) => {
      const real_node_parent = node.parent
      // clear the origin parent info
      node.parent = -1
      clipboardWrite(generateNodeParametersDescription(convertToTemplate(treeData, node.id)))
      node.parent = real_node_parent
    },
    [treeData]
  )

  const handlePaste = useCallback(
    async (parent: NodeModel<CustomData>) => {
      const maybeDescription = await clipboardRead()

      if (isValidDescription(maybeDescription)) {
        const nodes: NodeModel<CustomData>[] = convertFromTemplate(
          convertFromDescription(maybeDescription, parent.id.toString())
        )

        const newTreeData: NodeModel<CustomData>[] = []

        nodes.forEach((node) => {
          // keep the same node
          // eg. if parent is root and this node is also node
          // keep the origin one
          if (node.id === parent.id) return

          const last_id = node.id

          const name = getUniqueName(node.text, [...treeData, ...newTreeData])

          node.id = name
          node.text = name
          if (node.data?.param_def) {
            node.data.param_def.name = name
          } else if (node.data?.layout_def) {
            node.data.layout_def.name = name
          }

          // assert parent is ahead of children in order
          // update descendants nodes
          if (last_id !== name && parent.id !== last_id) {
            nodes.filter((node) => node.parent === last_id).forEach((node) => (node.parent = name))
          }

          newTreeData.push(node)
        })

        setTreeData((tree) => {
          return [...tree, ...newTreeData]
        })
      }
    },
    [setTreeData, getUniqueName, treeData]
  )

  return (
    <Dialog open modal={false}>
      <DialogContent className="size-max max-h-none max-w-none">
        <DialogHeader>
          <DialogTitle className="select-none">{node_data.name}</DialogTitle>
        </DialogHeader>

        <div
          ref={ref}
          style={{
            width: size ? size.width : 800,
            height: size ? size.height : 400
          }}
        >
          <ResizablePanelGroup direction="horizontal" className="size-full">
            <ResizablePanel>
              <div className="size-full overflow-auto rounded-lg border-1 border-white/15 p-1">
                <DndProvider context={window} backend={MultiBackend} options={getBackendOptions()}>
                  <Tree
                    ref={treeRef}
                    tree={treeData}
                    rootId={'root-root'}
                    enableAnimateExpand={true}
                    onDrop={handleDrop}
                    initialOpen={initialOpen}
                    dropTargetOffset={5}
                    render={(tree_node: NodeModel<CustomData>, { depth, isOpen, onToggle }) => (
                      <TreeNode
                        tree_node={tree_node}
                        depth={depth}
                        isOpen={isOpen}
                        isSelected={tree_node.id === selectedNode?.id}
                        onToggle={onToggle}
                        onSelect={handleSelect}
                        onDelete={handelDelete}
                        onAdd={handleAdd}
                        onCopy={handleCopy}
                        onPaste={handlePaste}
                      />
                    )}
                    classes={{
                      draggingSource: 'opacity-30',
                      dropTarget: 'bg-sky-200/50',
                      placeholder: 'bg-sky-400 h-1'
                    }}
                    sort={false}
                    insertDroppableFirst={false}
                    placeholderRender={(_, { depth }) => <Placeholder depth={depth} />}
                    canDrop={(_, { dragSource, dropTarget, dropTargetId }) => {
                      if (dropTarget?.data?.readonly && dropTargetId !== 'root') {
                        return false
                      }
                      if (dropTarget?.data?.layout_def?.type === LayoutType.Stack) {
                        if (
                          dragSource?.data?.layout_def === undefined ||
                          ![LayoutType.Horizontal, LayoutType.Vertical].includes(
                            dragSource.data.layout_def.type
                          ) ||
                          dragSource?.data?.layout_def.name.length == 0
                        ) {
                          return false
                        }
                      }

                      if (dragSource?.parent === dropTargetId) {
                        return true
                      }
                    }}
                    canDrag={(node) => {
                      if (node?.id === 'root' || node?.data?.readonly) {
                        return false
                      }
                      return true
                    }}
                  />
                </DndProvider>
              </div>
            </ResizablePanel>
            <ResizableHandle className="w-1" />
            <ResizablePanel>
              <div className="size-full min-w-20 overflow-auto rounded-lg border-1 border-white/15 p-2 text-xs">
                <If
                  condition={
                    selectedNode !== null &&
                    !(selectedNode.data?.is_folder && selectedNode.id === 'root')
                  }
                >
                  <Then>
                    {() => (
                      <If condition={selectedNode?.data?.layout_def !== undefined}>
                        <Then>
                          {() => (
                            <LayoutDefEditor
                              id={selectedNode!.id as string}
                              layoutDef={selectedNode?.data?.layout_def!}
                              onRename={handleRename}
                              onUpdate={handleUpdate}
                              readonly={selectedNode?.data?.readonly}
                            />
                          )}
                        </Then>
                        <Else>
                          {() => (
                            <ParamDefEditor
                              id={selectedNode!.id as string}
                              paramDef={selectedNode?.data?.param_def!}
                              onRename={handleRename}
                              onUpdate={handleUpdate}
                              readonly={selectedNode?.data?.readonly}
                            />
                          )}
                        </Else>
                      </If>
                    )}
                  </Then>
                </If>
              </div>
            </ResizablePanel>
          </ResizablePanelGroup>
        </div>

        <DialogFooter>
          <div className="z-10 flex gap-1">
            <DialogButton
              onClick={() => {
                modal.remove()
              }}
            >
              {t('common.cancel')}
            </DialogButton>

            <DialogButton
              onClick={() => {
                cloneNode('Null', treeData)
              }}
            >
              {t('builder.clone-node')}
            </DialogButton>

            <DialogButton
              onClick={() => {
                clipboardWrite(generateNodeParametersDescription(convertToTemplate(treeData)))
              }}
            >
              {t('common.copy')}
            </DialogButton>
            <DialogButton
              onClick={() => {
                const graph_id = storeApi.getState().graph_id
                if (graph_id !== undefined) {
                  rebuildNodeParameters(graph_id, node.id, convertToTemplate(treeData))
                  modal.remove()
                }
              }}
            >
              {t('common.ok')}
            </DialogButton>
          </div>
        </DialogFooter>

        <div
          ref={handleRef}
          className="pointer-events-auto absolute bottom-0 right-0 size-8 cursor-nwse-resize"
          onPointerDown={onPointerDown}
          onPointerUp={onPointerUp}
          onPointerMove={onPointerMove}
        />
      </DialogContent>
    </Dialog>
  )
}

function ParameterBuilderWrapper(props: {
  node_id: number
  node: NodeData
  node_def: NodeDef
  node_data: NodeDataType
}) {
  return (
    <NodeGraphStoreProvider id={'node-graph-default'}>
      <ParameterBuilder {...props} />
    </NodeGraphStoreProvider>
  )
}

export default NiceModal.create(ParameterBuilderWrapper)

const TypeIcon = ({ isLayout }: { isLayout?: boolean }) => {
  if (isLayout) {
    return <PiFolderOpenFill />
  }
  return <SiTemporal />
}

type PreviousName = {
  previous_name?: string
}

type CustomData = {
  is_folder: boolean
  is_custom: boolean
  is_dirty: boolean
  is_new?: boolean
  param_def?: ParameterData & PreviousName
  layout_def?: LayoutData & PreviousName
  readonly?: boolean
}

type Props = {
  tree_node: NodeModel<CustomData>
  depth: number
  isOpen: boolean
  isSelected: boolean
  onToggle: (id: NodeModel['id']) => void
  onSelect: (node: NodeModel<CustomData>) => void
  onDelete: (node: NodeModel<CustomData>) => void
  onAdd: (node: NodeModel<CustomData>, param?: ParamType, layout?: LayoutType) => void
  onCopy: (node: NodeModel<CustomData>) => void
  onPaste: (node: NodeModel<CustomData>) => void
}

const TreeNode = (props: Props) => {
  const { id, data } = props.tree_node
  const indent = props.depth * 16

  const [hovered, setHovered] = useState<boolean>(false)
  const [adding, setAdding] = useState<boolean>(false)
  const [dragOver, setDragOver] = useState<boolean>(false)
  const [dropDownMenuOpen, setDropDownMenuOpen] = useState<boolean>(false)

  const handleSelect = () => {
    props.onSelect(props.tree_node)
  }

  useDebounce(
    () => {
      if (dragOver && !props.isOpen) {
        props.onToggle(id)
      }
    },
    500,
    [dragOver]
  )

  const getContextMenuItems = useCallback(() => {
    const isReadonlyExceptRoot =
      props.tree_node.data?.readonly === true && props.tree_node.id !== 'root'

    if (
      (props.tree_node.data?.layout_def?.type === LayoutType.Horizontal ||
        props.tree_node.data?.layout_def?.type === LayoutType.Vertical) &&
      !isReadonlyExceptRoot
    ) {
      return [t('common.copy'), t('common.paste')]
    }

    return [t('common.copy')]
  }, [props.tree_node])

  return (
    <div
      className={cn(
        presets['menu-item-primary'],
        'transition-colors-opacity duration-[10] flex gap-2 place-items-center rounded-lg h-6 text-nowrap min-w-max pr-2 text-xs',
        props.isSelected ? 'bg-btn-active/70' : 'hover:bg-white/20'
      )}
      style={{ paddingInlineStart: indent }}
      onClick={handleSelect}
      onContextMenu={() => {
        handleSelect()
        setDropDownMenuOpen(true)
      }}
      onMouseOver={() => {
        setHovered(true)
      }}
      onMouseOut={() => setHovered(false)}
      onDragLeave={
        data?.is_folder && !props.isOpen
          ? (e) => {
              e.stopPropagation()
              setDragOver(false)
            }
          : undefined
      }
      onDragEnter={
        data?.is_folder && !props.isOpen
          ? (e) => {
              e.stopPropagation()
              setDragOver(true)
            }
          : undefined
      }
    >
      <div className="flex size-3 place-items-center">
        {data?.is_folder && id !== 'root' && (
          <div
            onClick={(e: React.MouseEvent) => {
              e.stopPropagation()
              props.onToggle(id)
            }}
            className={props.isOpen ? 'rotate-90' : ''}
          >
            <FaCaretRight />
          </div>
        )}
      </div>
      <div>
        <TypeIcon isLayout={data?.is_folder} />
      </div>
      <div className=" pointer-events-none">{props.tree_node.text}</div>

      <div className="mx-auto" />
      {(hovered || props.isSelected || adding) &&
        data?.is_folder &&
        (data?.is_custom || id === 'root') && (
          <DropdownMenu
            className="max-h-60 min-w-0 overflow-auto"
            itemClassName="h-5"
            onOpenChange={setAdding}
            onClickItem={(item) => {
              const { is_layout, idx } = item.data as any
              if (is_layout) {
                props.onAdd(props.tree_node, undefined, idx as LayoutType)
              } else {
                props.onAdd(props.tree_node, idx as ParamType)
              }
            }}
            menus={
              data.layout_def?.type === LayoutType.Stack
                ? stackAllowedItems()
                : containerAllowedItems()
            }
          >
            <div className="btn btn-ghost btn-xs m-0 size-4 min-h-0 p-0 active:brightness-125 ">
              <IoMdAdd />
            </div>
          </DropdownMenu>
        )}
      {(hovered || props.isSelected) && data?.is_custom && (
        <div
          className="btn btn-ghost btn-xs m-0 size-4 min-h-0 p-0 active:brightness-125"
          onClick={() => {
            props.onDelete(props.tree_node)
          }}
        >
          <IoMdRemove />
        </div>
      )}
      <DropdownMenu
        className="max-h-60 min-w-0 overflow-auto"
        itemClassName="h-5"
        menus={getContextMenuItems()}
        open={dropDownMenuOpen}
        onOpenChange={setDropDownMenuOpen}
        onClickItem={(item) => {
          if (item.label === t('common.copy')) {
            props.onCopy(props.tree_node)
          } else if (item.label === t('common.paste')) {
            props.onPaste(props.tree_node)
          }
        }}
      >
        <></>
      </DropdownMenu>
    </div>
  )
}

const Placeholder = ({ depth }: { depth: number }) => {
  const left = depth * 16
  return <div className="absolute right-0 top-0 h-1 -translate-y-1/2" style={{ left }} />
}

const LabeledItem = ({
  children,
  label,
  className
}: {
  children: React.ReactNode
  label: string
  className?: string
}) => {
  const { t } = useI18n('parameter')

  return (
    <div className={cn('grid grid-cols-3 items-center gap-4', className)}>
      <label className="h-full truncate text-right align-middle  tracking-wide">
        {t(label as any)}
      </label>
      <div className="col-span-2 h-full self-center">{children}</div>
    </div>
  )
}

const ParamDefEditor = ({
  id,
  paramDef,
  onRename,
  onUpdate,
  readonly
}: {
  id: string
  paramDef: ParameterData
  onRename: (id: string, name: string) => void
  onUpdate: (id: string, layoutDef: ParameterData) => void
  readonly?: boolean
}) => {
  if (!paramDef) {
    return undefined
  }
  const type_name = ParamType[paramDef.type]
  const placeholder = getParamValuePlaceHolder(paramDef.type)
  return (
    <div className="flex flex-col gap-2 select-none">
      <LabeledItem label="type">
        <label>{type_name}</label>
      </LabeledItem>
      <Divider />
      <LabeledItem label="name">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.name}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.name) onRename(id, v)
          }}
        />
      </LabeledItem>
      <LabeledItem label="label">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.display_name || ''}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.display_name) {
              paramDef.display_name = v
              if (paramDef.display_name.length == 0) {
                paramDef.display_name = undefined
              }
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>
      <LabeledItem label="label-visible">
        <Toggle
          className={join(
            'checkbox-accent size-3.5',
            readonly && 'pointer-events-none  opacity-60'
          )}
          checked={paramDef.name_visible}
          onChange={(v) => {
            paramDef.name_visible = v
            onUpdate(id, { ...paramDef })
          }}
        />
      </LabeledItem>
      <LabeledItem label="visible">
        <Toggle
          className={join(
            'checkbox-accent size-3.5',
            readonly && 'pointer-events-none  opacity-60'
          )}
          checked={paramDef.visible}
          onChange={(v) => {
            paramDef.visible = v
            onUpdate(id, { ...paramDef })
          }}
        />
      </LabeledItem>
      <LabeledItem label="editable">
        <Toggle
          className={join(
            'checkbox-accent size-3.5',
            readonly && 'pointer-events-none  opacity-60'
          )}
          checked={paramDef.editable}
          onChange={(v) => {
            paramDef.editable = v
            onUpdate(id, { ...paramDef })
          }}
        />
      </LabeledItem>
      <LabeledItem label="checkable">
        <Toggle
          className={join(
            'checkbox-accent size-3.5',
            readonly && 'pointer-events-none  opacity-60'
          )}
          checked={paramDef.checkable}
          onChange={(v) => {
            paramDef.checkable = v
            if (v) {
              paramDef.checked = false
            } else if (paramDef.checked !== undefined) {
              delete paramDef.checked
            }
            onUpdate(id, { ...paramDef })
          }}
        />
      </LabeledItem>
      {paramDef.checkable && (
        <LabeledItem label="checked">
          <Toggle
            className={join(
              'checkbox-accent size-3.5',
              readonly && 'pointer-events-none  opacity-60'
            )}
            checked={paramDef.checked || false}
            onChange={(v) => {
              paramDef.checked = v
              onUpdate(id, { ...paramDef })
            }}
          />
        </LabeledItem>
      )}
      {([ParamType.Float, ParamType.Int].includes(paramDef.type) ||
        type_name.startsWith('Vec') ||
        type_name.startsWith('Mat')) && (
        <>
          <LabeledItem label="min-value">
            <TextInput
              className="w-full  rounded-md border-1 border-white/20"
              value={rangeNumberToString(paramDef.min_value)}
              readonly={readonly}
              onChange={(v) => {
                try {
                  paramDef.min_value = checkNan(parseFloat(v))
                } catch (_e) {
                  paramDef.min_value = I32_MIN
                }
                onUpdate(id, { ...paramDef })
              }}
            />
          </LabeledItem>
          <LabeledItem label="max-value">
            <TextInput
              className="w-full rounded-md border-1 border-white/20"
              value={rangeNumberToString(paramDef.max_value)}
              readonly={readonly}
              onChange={(v) => {
                try {
                  paramDef.max_value = checkNan(parseFloat(v))
                } catch (_e) {
                  paramDef.max_value = I32_MAX
                }
                onUpdate(id, { ...paramDef })
              }}
            />
          </LabeledItem>
          <LabeledItem label="min-ui-value">
            <TextInput
              className="w-full rounded-md border-1 border-white/20"
              value={rangeNumberToString(paramDef.min_ui_value)}
              readonly={readonly}
              onChange={(v) => {
                try {
                  paramDef.min_ui_value = checkNan(parseFloat(v))
                } catch (_e) {
                  paramDef.min_ui_value = undefined
                }
                onUpdate(id, { ...paramDef })
              }}
            />
          </LabeledItem>
          <LabeledItem label="max-ui-value">
            <TextInput
              className="w-full rounded-md border-1 border-white/20"
              value={rangeNumberToString(paramDef.max_ui_value)}
              readonly={readonly}
              onChange={(v) => {
                try {
                  paramDef.max_ui_value = checkNan(parseFloat(v))
                } catch (_e) {
                  paramDef.max_ui_value = undefined
                }
                onUpdate(id, { ...paramDef })
              }}
            />
          </LabeledItem>
        </>
      )}
      {paramDef.type == ParamType.Enum && (
        <LabeledItem label="enum-items">
          <TextInput
            className=" w-full rounded-md border-1 border-white/20"
            placeHolder="item0, item1, item2, ..."
            value={paramDef.enum_items ? paramDef.enum_items.toString() : ''}
            readonly={readonly}
            onChange={(v) => {
              v = v.trim()
              if (v.length == 0) {
                delete paramDef.enum_items
              } else {
                paramDef.enum_items = v.split(',').map((s) => s.trim())
              }
              onUpdate(id, { ...paramDef })
            }}
          />
        </LabeledItem>
      )}
      {!type_name.endsWith('Ramp') && paramDef.type != ParamType.Button && (
        <LabeledItem label="default-value">
          <TextInput
            className="w-full rounded-md border-1 border-white/20"
            placeHolder={placeholder}
            value={paramDef.default_value.toString()}
            readonly={readonly}
            onChange={(v) => {
              v = v.trim()
              try {
                if (
                  paramDef.type == ParamType.String ||
                  paramDef.type == ParamType.Group ||
                  paramDef.type == ParamType.Code ||
                  paramDef.type == ParamType.Mesh
                ) {
                  paramDef.default_value = v
                } else if (
                  paramDef.type == ParamType.Int ||
                  paramDef.type == ParamType.List ||
                  paramDef.type == ParamType.Enum
                ) {
                  paramDef.default_value = checkNan(parseInt(v))
                } else if (paramDef.type == ParamType.Float) {
                  paramDef.default_value = checkNan(parseFloat(v))
                } else if (paramDef.type == ParamType.Bool) {
                  if (v === 'true') {
                    paramDef.default_value = true
                  } else if (v === 'false') {
                    paramDef.default_value = false
                  } else {
                    paramDef.default_value = checkNan(parseInt(v)) > 0
                  }
                } else if (
                  paramDef.type == ParamType.Vec2 ||
                  paramDef.type == ParamType.Vec3 ||
                  paramDef.type == ParamType.Vec4 ||
                  paramDef.type == ParamType.Mat3 ||
                  paramDef.type == ParamType.Mat4 ||
                  paramDef.type == ParamType.Color
                ) {
                  const items = v.split(',').map((s) => checkNan(parseFloat(s.trim())))
                  if (paramDef.type == ParamType.Color) {
                    if (items.length < 3 || items.length > 4) {
                      throw new Error('Incorrect size of inputs')
                    }
                  } else {
                    let size = checkNan(parseInt(type_name.substring(3, 4)))
                    if (type_name.startsWith('Mat')) size = size * size
                    if (items.length != size) {
                      throw new Error('Incorrect size of inputs')
                    }
                  }
                  paramDef.default_value = items
                } else if (
                  paramDef.type == ParamType.Vec2i ||
                  paramDef.type == ParamType.Vec3i ||
                  paramDef.type == ParamType.Vec4i
                ) {
                  const items = v.split(',').map((s) => checkNan(parseInt(s.trim())))
                  const size = checkNan(parseInt(type_name.substring(3, 4)))
                  if (items.length != size) {
                    throw new Error('Incorrect size of inputs')
                  }
                  paramDef.default_value = items
                }
              } catch (e) {
                console.error(e)
                // TODO: reset value
                return
              }
              onUpdate(id, { ...paramDef })
            }}
          />
        </LabeledItem>
      )}
      <LabeledItem label="tooltip">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.tooltip}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.tooltip) {
              paramDef.tooltip = v
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>
      <LabeledItem label="expression">
        <Script
          lang="lua"
          value={paramDef.expression || ''}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.expression) {
              paramDef.expression = v
              if (paramDef.expression.length == 0) {
                paramDef.expression = undefined
              }
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>
      <Divider />
      {/* {!type_name.endsWith('Ramp') && paramDef.type != ParamType.Button && (
        <LabeledItem label="value-action">
          <TextInput
            className="w-full rounded-md border-1 border-white/20"
            value={paramDef.value_action || ''}
            readonly={readonly}
            onChange={(v) => {
              if (v !== paramDef.value_action) {
                paramDef.value_action = v
                if (paramDef.value_action.length == 0) {
                  paramDef.value_action = undefined
                }
                onUpdate(id, { ...paramDef })
              }
            }}
          />
        </LabeledItem>
      )} */}
      <LabeledItem label="visible-action">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.visible_action || ''}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.visible_action) {
              paramDef.visible_action = v
              if (paramDef.visible_action.length == 0) {
                paramDef.value_action = undefined
              }
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>
      <LabeledItem label="editable-action">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.editable_action || ''}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.editable_action) {
              paramDef.editable_action = v
              if (paramDef.editable_action.length == 0) {
                paramDef.editable_action = undefined
              }
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>

      <LabeledItem label="checked-action">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={paramDef.checked_action || ''}
          readonly={readonly}
          onChange={(v) => {
            if (v !== paramDef.checked_action) {
              paramDef.checked_action = v
              if (paramDef.checked_action.length == 0) {
                paramDef.checked_action = undefined
              }
              onUpdate(id, { ...paramDef })
            }
          }}
        />
      </LabeledItem>
    </div>
  )
}

const LayoutDefEditor = ({
  id,
  layoutDef,
  onRename,
  onUpdate,
  readonly
}: {
  id: string
  layoutDef: LayoutData
  onRename: (id: string, name: string) => void
  onUpdate: (id: string, layoutDef: LayoutData) => void
  readonly?: boolean
}) => {
  if (!layoutDef) {
    return undefined
  }

  return (
    <div className="flex flex-col gap-2 select-none">
      <LabeledItem label="type">
        <label>{LayoutType[layoutDef.type]}</label>
      </LabeledItem>
      <Divider />
      <LabeledItem label="name">
        <TextInput
          className="w-full bg-black/5 hover:bg-black/10"
          readonly={readonly}
          value={layoutDef.name}
          onChange={(v) => {
            if (v !== layoutDef.name) onRename(id, v)
          }}
        />
      </LabeledItem>
      <LabeledItem label="label">
        <TextInput
          className="w-full rounded-md border-1 border-white/20"
          value={layoutDef.display_name}
          readonly={readonly}
          onChange={(v) => {
            if (v !== layoutDef.display_name) {
              layoutDef.display_name = v.length == 0 ? undefined : v
              onUpdate(id, { ...layoutDef })
            }
          }}
        />
      </LabeledItem>
      <LabeledItem label="visible">
        <Toggle
          className={join(
            'checkbox-accent size-3.5',
            readonly && 'pointer-events-none  opacity-60'
          )}
          checked={layoutDef.visible}
          onChange={(v) => {
            layoutDef.visible = v
            onUpdate(id, { ...layoutDef })
          }}
        />
      </LabeledItem>
      {layoutDef.type != LayoutType.Spacer && (
        <>
          <LabeledItem label="editable">
            <Toggle
              className={join(
                'checkbox-accent size-3.5',
                readonly && 'pointer-events-none  opacity-60'
              )}
              checked={layoutDef.editable}
              onChange={(v) => {
                layoutDef.editable = v
                onUpdate(id, { ...layoutDef })
              }}
            />
          </LabeledItem>
          <LabeledItem label="collapsible">
            <Toggle
              className={join(
                'checkbox-accent size-3.5',
                readonly && 'pointer-events-none  opacity-60'
              )}
              checked={layoutDef.expanded !== undefined}
              onChange={(v) => {
                if (v) {
                  layoutDef.expanded = true
                } else if (layoutDef.expanded !== undefined) {
                  delete layoutDef.expanded
                }
                onUpdate(id, { ...layoutDef })
              }}
            />
          </LabeledItem>
          {layoutDef.expanded !== undefined && (
            <LabeledItem label="expanded">
              <Toggle
                className={join(
                  'checkbox-accent size-3.5',
                  readonly && 'pointer-events-none  opacity-60'
                )}
                checked={layoutDef.expanded}
                onChange={(v) => {
                  layoutDef.expanded = v
                  onUpdate(id, { ...layoutDef })
                }}
              />
            </LabeledItem>
          )}
        </>
      )}

      <Divider />
      <LabeledItem label="visible-action">
        <TextInput
          className="w-full bg-black/5 hover:bg-black/10"
          value={layoutDef.visible_action || ''}
          readonly={readonly}
          onChange={(v) => {
            if (layoutDef.visible_action !== v) {
              layoutDef.visible_action = v
              if (layoutDef.visible_action.length == 0) {
                layoutDef.visible_action = undefined
              }
              onUpdate(id, { ...layoutDef })
            }
          }}
        />
      </LabeledItem>
      {layoutDef.type != LayoutType.Spacer && (
        <LabeledItem label="editable-action">
          <TextInput
            className="w-full bg-black/5 hover:bg-black/10"
            value={layoutDef.editable_action || ''}
            readonly={readonly}
            onChange={(v) => {
              if (layoutDef.editable_action !== v) {
                layoutDef.editable_action = v
                if (layoutDef.editable_action.length == 0) {
                  layoutDef.editable_action = undefined
                }
                onUpdate(id, { ...layoutDef })
              }
            }}
          />
        </LabeledItem>
      )}
    </div>
  )
}

const getParamValuePlaceHolder = (type: ParamType) => {
  switch (type) {
    case ParamType.Bool:
      return 'true or false'
    case ParamType.Color:
      return 'r, g, b, a'
    case ParamType.Vec2:
      return 'x, y'
    case ParamType.Vec3:
      return 'x, y, z'
    case ParamType.Vec4:
      return 'x, y, z, w'
    case ParamType.Vec2i:
      return 'x, y'
    case ParamType.Vec3i:
      return 'x, y, z'
    case ParamType.Vec4i:
      return 'x, y, z, w'
    case ParamType.Enum:
      return 'index of item'
    case ParamType.List:
      return 'size of list'
    default:
      return undefined
  }
}

const getParamDefaultValue = (type: ParamType): ParamValue => {
  switch (type) {
    case ParamType.Bool:
      return false
    case ParamType.Int:
      return 0
    case ParamType.Float:
      return 0
    case ParamType.String:
      return ''
    case ParamType.Color:
      return [1, 1, 1, 1]
    case ParamType.Vec2:
      return [0, 0]
    case ParamType.Vec3:
      return [0, 0, 0]
    case ParamType.Vec4:
      return [0, 0, 0, 0]
    case ParamType.Vec2i:
      return [0, 0]
    case ParamType.Vec3i:
      return [0, 0, 0]
    case ParamType.Vec4i:
      return [0, 0, 0, 0]
    case ParamType.Mat3:
      return [1, 0, 0, 0, 1, 0, 0, 0, 1]
    case ParamType.Mat4:
      return [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
    case ParamType.Mesh:
      return ''
    case ParamType.Group:
      return ''
    case ParamType.Enum:
      return 0
    case ParamType.Button:
      return 0
    case ParamType.Code:
      return ''
    case ParamType.ScalarRamp:
      return [
        {
          pos: 0,
          value: 0,
          method: 'Linear'
        },
        {
          pos: 1,
          value: 1,
          method: 'Linear'
        }
      ]
    case ParamType.ColorRamp:
      return [
        {
          pos: 0,
          value: [0, 0, 0, 1],
          method: 'Linear'
        },
        {
          pos: 1,
          value: [1, 1, 1, 1],
          method: 'Linear'
        }
      ]
    case ParamType.List:
      return 0
    default:
      return ''
  }
}

const checkNan = (value: number) => {
  if (isNaN(value)) {
    throw new Error('Input value is Nan')
  }
  return value
}

const stackAllowedItems = () => {
  return [LayoutType.Horizontal, LayoutType.Vertical].map((item) => ({
    label: LayoutType[item] as string,
    data: {
      is_layout: true,
      idx: item as number
    }
  }))
}

const allLayoutTypeKeys = Object.keys(LayoutType).filter((key) =>
  Number.isNaN(Number(key))
) as (keyof typeof LayoutType)[]

const allParamTypeKeys = Object.keys(ParamType).filter((key) =>
  Number.isNaN(Number(key))
) as (keyof typeof ParamType)[]

const containerAllowedItems = () => {
  const allLayouts = allLayoutTypeKeys.map((key) => ({
    label: key as string,
    data: {
      is_layout: true,
      idx: LayoutType[key] as number
    }
  }))

  const allParams = allParamTypeKeys.map((key) => ({
    label: key as string,
    data: {
      is_layout: false,
      idx: ParamType[key] as number
    }
  }))

  return allLayouts.concat(allParams)
}

function convertFromTemplate(
  template: (ParameterTemplate | LayoutTemplate)[]
): NodeModel<CustomData>[] {
  const result: NodeModel<CustomData>[] = []
  for (const item of template) {
    if (item.item_type === 'param') {
      result.push(...convertFromParameterTemplate(item))
    } else {
      result.push(...convertFromLayoutTemplate(item))
    }
  }

  return result
}

function convertFromParameterTemplate(item: ParameterTemplate): NodeModel<CustomData>[] {
  const result: NodeModel<CustomData>[] = [
    {
      id: item.name,
      text: item.name,
      parent: item.parent_id || '',
      droppable: item.type === ParamType.List,
      data: {
        is_folder: item.type === ParamType.List,
        is_custom: item.is_custom,
        is_dirty: item.is_dirty,
        is_new: item.is_new,
        param_def: {
          name: item.name,
          checkable: item.checkable,
          checked: item.checked,
          default_value: item.default_value,
          editable: item.editable,
          max_value: item.max_value,
          min_value: item.min_value,
          max_ui_value: item.max_ui_value,
          min_ui_value: item.min_ui_value,

          display_name: item.display_name,
          name_visible: item.name_visible,
          tooltip: item.tooltip,
          type: item.type,
          visible: item.visible,
          builtin: false,
          enum_items: item.enum_items,

          expression: item.expression,
          value_action: item.value_action,
          visible_action: item.visible_action,
          editable_action: item.editable_action,

          checked_action: item.checked_action
        }
      }
    }
  ]

  if (item.param_items && item.param_items.length > 0) {
    result.push(...convertFromTemplate(item.param_items))
  }

  return result
}

function convertFromLayoutTemplate(item: LayoutTemplate): NodeModel<CustomData>[] {
  const is_spacer = item.type == LayoutType.Spacer

  const result: NodeModel<CustomData>[] = [
    {
      id: item.name,
      text: item.name,
      parent: item.parent_id || '',
      droppable: !is_spacer,
      data: {
        is_folder: !is_spacer,
        is_custom: item.is_custom,
        is_dirty: item.is_dirty,
        is_new: item.is_new,
        layout_def: {
          name: item.name,
          display_name: item.display_name,
          type: item.type,
          editable: item.editable,
          visible: item.visible,
          expanded: item.expanded,
          builtin: false,
          visible_action: item.visible_action,
          editable_action: item.editable_action
        }
      }
    }
  ]

  if (item.param_items.length > 0) {
    result.push(...convertFromTemplate(item.param_items))
  }

  return result
}
