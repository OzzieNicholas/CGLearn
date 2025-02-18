import { NNode } from '@/types/node-graph'
import { cn } from '@/utils/cn'
import { Handle, NodeProps, Position } from '@xyflow/react'
import { memo } from 'react'

function DotNode({ selected }: NodeProps<NNode>) {
  return (
    <div className="indicator flex flex-col items-center gap-2">
      <Handle id="0" type="target" position={Position.Top} className="m-0 size-3 bg-teal-500 p-0" />
      <div
        className={cn(
          'transition-colors-opacity bg-transparent border-2 rounded-full size-4 shadow-inner',
          selected ? 'border-yellow-300' : 'border-teal-400'
        )}
      />
      <Handle
        id="0"
        type="source"
        position={Position.Bottom}
        className="m-0 size-3 bg-teal-500 p-0"
      />
    </div>
  )
}

export default memo(DotNode)
