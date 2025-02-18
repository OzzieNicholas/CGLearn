import BackgroundGradient from '@/components/BackgroundGradient'
import ColorPicker from '@/components/ColorPicker'
import { getIconTheme, iconTypes } from '@/constant/node-resource'
import { presets } from '@/css/presets'
import { NodeParameters } from '@/features/Parameter'
import { useNodeGraphStoreApi } from '@/stores/node-graph'
import { NNode, NodeData } from '@/types/node-graph'
import { cn } from '@/utils/cn'
import { isSubgraphOrNda } from '@/utils/node-graph'
import { Transition } from '@headlessui/react'
import { Tooltip } from '@nextui-org/react'
import {
  Handle,
  NodeProps,
  NodeToolbar,
  Position,
  useReactFlow,
  useUpdateNodeInternals
  // ReactFlowState,
  // useStore
} from '@xyflow/react'
import React, {
  memo,
  useCallback,
  useContext,
  useEffect,
  useRef,
  useState,
  useMemo,
  useLayoutEffect
} from 'react'
// import { shallow } from 'zustand/shallow'
import { InteractiveContext } from '../context'
import NodeInfo from '../NodeInfo'
import NodeMessage from '../NodeMessage'

import { BsFillLightningFill } from 'react-icons/bs'
import { MdOutlineSyncDisabled } from 'react-icons/md'
import { PiFoldersLight } from 'react-icons/pi'
import { TiArrowDownOutline, TiDelete } from 'react-icons/ti'
import { VscColorMode } from 'react-icons/vsc'

// const storeSelector = (state: ReactFlowState) => ({
//   zoom: state.transform[2]
// })

const colorBadgeActive = '#55fad9'

function CommonNode({ id, data, selected, dragging }: NodeProps<NNode>) {
  // const { zoom } = useStore(storeSelector, shallow)
  const zoom = 1.0
  const { editable, hideElementToolbar } = useContext(InteractiveContext)
  const showPlaceHolder = zoom < 0.4

  const { deleteElements } = useReactFlow()
  const storeApi = useNodeGraphStoreApi()

  const updateNodeInternals = useUpdateNodeInternals()
  useLayoutEffect(() => {
    updateNodeInternals(id)
  }, [data.inputs.length])

  const toggleBypassed = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault()
      e.stopPropagation()
      storeApi.getState().setNodeBypassed(id, !data.bypassed)
    },
    [id, data]
  )

  const deleteNode_ = useCallback(
    (e: React.MouseEvent) => {
      e.preventDefault()
      e.stopPropagation()
      deleteElements({ nodes: [{ id }] })
    },
    [id]
  )

  const showTooltip = useMemo(() => {
    return dragging !== true && !hideElementToolbar
  }, [dragging, hideElementToolbar])

  const showToolbar = useMemo(() => {
    return showTooltip && data.hovered === true
  }, [showTooltip, data.hovered])

  // console.log('render node: ', data.label)

  return (
    <div className="indicator flex flex-col">
      {editable && (
        <NodeToolbar isVisible position={Position.Top} className="pointer-events-none -top-1">
          <Transition
            show={showToolbar}
            enter="transition-opacity duration-100"
            enterFrom="opacity-0"
            enterTo="opacity-100"
            leave="transition-opacity duration-200"
            leaveFrom="opacity-100"
            leaveTo="opacity-0"
          >
            <div className="flex flex-row justify-start">
              <div
                className="btn btn-circle btn-ghost pointer-events-auto m-0 size-12 min-h-0 p-0 text-white/40 	hover:text-white/80"
                onClick={(e) => {
                  e.preventDefault()
                  e.stopPropagation()
                  if (!data.is_display) storeApi.getState().setDisplayNode(id)
                }}
              >
                <BsFillLightningFill
                  className=" size-full p-3"
                  style={data.is_display ? { color: colorBadgeActive, opacity: 0.8 } : {}}
                />
              </div>
              <div
                className="btn btn-circle btn-ghost pointer-events-auto m-0 size-12 min-h-0 p-0 text-white/40 	hover:text-white/80"
                onClick={toggleBypassed}
              >
                <MdOutlineSyncDisabled
                  className="size-full p-2.5"
                  style={data.bypassed ? { color: colorBadgeActive, opacity: 0.8 } : {}}
                />
              </div>
              <div className="h-0 w-4" />
              <ColorPicker
                placement="right-start"
                color={data.color}
                onChange={(color) => {
                  storeApi.getState().setNodeColor(id, color.hexa)
                }}
                onEditingFinished={(old_color) => {
                  storeApi.getState().onColorChange(id, old_color)
                }}
              >
                <button className="btn btn-circle btn-ghost pointer-events-auto m-0 size-12 min-h-0 p-0 text-white/40 	hover:text-white/80">
                  <VscColorMode className="size-full p-3" />
                </button>
              </ColorPicker>
              <div
                className="btn btn-circle  btn-ghost pointer-events-auto m-0 size-12 min-h-0 p-0 text-white/40 	hover:text-white/80"
                onClick={deleteNode_}
              >
                <TiDelete className="size-full p-2" />
              </div>
            </div>
          </Transition>
        </NodeToolbar>
      )}

      {data.is_display && (
        <span className="indicator-item z-10 size-24 border-0 bg-transparent">
          <BsFillLightningFill
            className="left-4 size-full animate-bounce-in p-1 hover:animate-bounce-loop"
            style={{ color: colorBadgeActive }}
          />
        </span>
      )}

      {data.inputs && (
        <div className="flex h-0 w-full justify-around">
          {data.inputs.map((_, idx) => {
            return (
              <Handle
                id={idx.toString()}
                key={idx}
                type="target"
                position={Position.Top}
                className="-top-6 m-0 size-3 bg-teal-500 p-0"
              />
            )
          })}
        </div>
      )}

      <BackgroundGradient
        className={cn(
          'min-w-64 cursor-default rounded-[0.475rem] drop-shadow-md',
          dragging === true && 'cursor-grab'
        )}
        containerClassName={!data.is_display ? 'bg-transparent' : ''}
        animate={true}
        enable={data.is_display}
      >
        <div
          className="m-[0.1rem] rounded-md border-0 backdrop-blur-lg"
          style={{
            backgroundColor: data.color,
            borderWidth: '1px',
            borderColor: '#ffffff80',
            opacity: data.bypassed ? '0.5' : '1',
            outlineStyle: selected ? 'solid' : 'none',
            outlineOffset: '2px',
            outlineWidth: '2px',
            outlineColor: '#FACC15'
          }}
        >
          <Tooltip
            content={<NodeInfo data={data} />}
            className={cn(presets['menu-primary'], 'p-0')}
            placement="right"
            delay={50}
            closeDelay={50}
            motionProps={{
              variants: {
                exit: {
                  opacity: 0,
                  transition: {
                    duration: 0.1,
                    ease: 'easeIn'
                  }
                },
                enter: {
                  opacity: 1,
                  transition: {
                    duration: 0.1,
                    ease: 'easeOut'
                  }
                }
              }
            }}
            isDisabled={!showTooltip}
          >
            <span>
              {showPlaceHolder ? (
                <PlaceHolder label={data.label} />
              ) : (
                <MemoNodeContent id={id} data={data} zoom={zoom} toggleBypassed={toggleBypassed} />
              )}
            </span>
          </Tooltip>
        </div>
      </BackgroundGradient>

      {!showPlaceHolder && data.inline_param && <NodeParameters node={data} />}

      {/* {!showPlaceHolder && (
        <div className="my-1 shadow-lg">
          <NodeInfo data={data} />
        </div>
      )} */}

      {data.outputs && (
        <div className="flex h-0 w-full justify-around">
          {data.outputs.map((_, idx) => {
            return (
              <Handle
                id={idx.toString()}
                key={idx}
                type="source"
                position={Position.Bottom}
                className="-bottom-3 m-0 size-3 bg-teal-500 p-0"
              />
            )
          })}
        </div>
      )}
    </div>
  )
}

function NodeContent({
  id,
  data,
  zoom,
  toggleBypassed
}: {
  id: string
  data: NodeData
  zoom: number
  toggleBypassed: (e: React.MouseEvent) => void
}) {
  const NodeIcon = iconTypes[data.icon] || iconTypes.default
  const iconTheme = getIconTheme(data.category)
  const [editing, setEditing] = useState(false)
  const [label, setLabel] = useState(data.label)
  const [labelWidth, setLabelWidth] = useState(0)
  const storeApi = useNodeGraphStoreApi()
  const { setNodeSpace, editable } = useContext(InteractiveContext)
  const span = useRef<any>()
  const canEnter = isSubgraphOrNda(data.kind)

  useEffect(() => {
    setLabelWidth(Math.max(span.current.offsetWidth, 80) + 10)
  }, [label])

  const finishEditing = useCallback(() => {
    if (!editable) return
    setEditing(false)
    let new_label = label.trim()
    if (new_label === data.label || new_label.length == 0) {
      return
    }
    storeApi.getState().renameNode(id, new_label)
  }, [label, setEditing])

  const enterNode = useCallback(() => {
    setNodeSpace(
      {
        data: {
          id: data.id,
          kind: data.kind,
          editable: data.editable
        }
      } as any,
      undefined
    )
  }, [data, setNodeSpace])

  const toggleInlineParam = useCallback(() => {
    storeApi.getState().setNodeData(id, {
      ...data,
      inline_param: !data.inline_param
    })
  }, [data])

  // const handleKeyPress = useCallback(
  //   (e: any) => {
  //     if (e.key === 'Enter') {
  //       finishEditing()
  //     }
  //   },
  //   [finishEditing]
  // )

  return (
    <div className="m-1 flex flex-col">
      <div className="flex gap-1">
        <div
          className={cn('flex size-5 items-center rounded-md drop-shadow-md')}
          style={{ backgroundColor: iconTheme[0] }}
        >
          <NodeIcon className="w-full p-px" style={{ color: iconTheme[1] }} />
        </div>

        <div className="flex items-center gap-1">
          <div
            className={cn(
              'font-bold text-zinc-800',
              zoom > 1.6 ? 'drop-shadow-md' : 'drop-shadow-none'
            )}
          >
            <span
              ref={span}
              onClick={() => {
                if (!editable) return
                setLabel(data.label)
                setEditing(true)
              }}
              // onClick={(e) => e.stopPropagation()}
              className={editing ? 'pointer-events-none absolute opacity-0' : ''}
            >
              {editing ? label : data.label}
            </span>
            {editing && (
              <input
                type="text"
                placeholder="Type here"
                className="nodrag nopan z-10 bg-transparent outline-dashed outline-1 outline-amber-400 selection:bg-text-select/40"
                value={label}
                autoFocus={true}
                onBlur={finishEditing}
                onChange={(e: any) => {
                  setLabel(e.target.value)
                }}
                // onKeyDown={handleKeyPress}
                style={{ width: labelWidth }}
                onClick={(e) => e.stopPropagation()}
                autoComplete="new-password"
              />
            )}
          </div>

          {!data.label.toLowerCase().startsWith(data['type_name'].toLowerCase()) && (
            <div className="badge rounded-md border-0 bg-gray-100 text-xs text-gray-500">
              {data['type_name']}
            </div>
          )}

          {data.show_version && (
            <div className="badge rounded-md border-0 bg-gray-100 text-xs text-gray-500">
              {`v${data['version']}`}
            </div>
          )}
        </div>

        <div className="ml-auto" />

        <div className="flex items-center">
          <div className="badge rounded-md border-0 bg-gray-200 text-xs font-bold text-gray-500 drop-shadow-md">
            {data.category}
          </div>
        </div>
      </div>

      <div className="divider my-0" />

      <div className="m-1 flex h-4 items-center">
        <div className="max-w-48 text-xs font-bold text-gray-500 required:truncate">
          {/* {data.desc} */}
          <NodeMessage data={data} />
        </div>

        <div className="ml-auto" />

        {data.bypassed && (
          <div
            className="btn btn-square btn-ghost size-5 h-full min-h-0 min-w-0 "
            onClick={toggleBypassed}
          >
            <MdOutlineSyncDisabled className="m-0  p-0" style={{ color: colorBadgeActive }} />
          </div>
        )}
        {canEnter && (
          <div
            className="btn btn-square btn-ghost size-5 h-full min-h-0 min-w-0"
            onClick={enterNode}
          >
            <PiFoldersLight className="m-0  p-0" style={{ color: colorBadgeActive }} />
          </div>
        )}

        <div
          className="btn btn-square btn-ghost size-5 h-full min-h-0 min-w-0"
          onClick={toggleInlineParam}
        >
          <TiArrowDownOutline
            className="m-0 p-0"
            style={{
              color: data.inline_param ? colorBadgeActive : 'black',
              opacity: data.inline_param ? 1 : 0.2
            }}
          />
        </div>

        {/* {data.is_display && (
            <div className="btn btn-square btn-ghost size-5  h-full min-h-0 min-w-0">
              <BsFillLightningFill
                className="m-0 p-[1px]"
                style={{ color: colorBadgeActive }}
              />
            </div>
          )} */}
      </div>
    </div>
  )
}

const MemoNodeContent = memo(NodeContent)

function PlaceHolder({ label }: { label: string }) {
  return (
    <div className="flex h-16 w-full items-center">
      {/* <div className="skeleton h-4 shrink-0 rounded-md"></div>
      <div className="divider my-0" />
      <div className="skeleton w-full"></div> */}
      <span className="mx-auto text-3xl font-bold text-zinc-800">{label}</span>
    </div>
  )
}

export default memo(CommonNode)
