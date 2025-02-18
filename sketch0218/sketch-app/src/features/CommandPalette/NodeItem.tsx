import { SearchNodeDesc } from '@/commands/index.types'
import { getIconTheme, iconTypes } from '@/constant/node-resource'
import { useI18n } from '@/locales'
import { cn } from '@/utils/cn'
import { memo, useMemo } from 'react'

const NodeItem = ({
  nodeDesc,
  onClick,
  active
}: {
  nodeDesc: SearchNodeDesc
  onClick: () => void
  active: boolean
}) => {
  const { t } = useI18n()
  const iconTheme = useMemo(() => getIconTheme(nodeDesc.category), [nodeDesc.category])
  const NodeIcon = iconTypes[nodeDesc.type_name] || iconTypes.default
  // const displayTypeName = useMemo(
  //   () => !nodeDesc.name.toLocaleLowerCase().startsWith(nodeDesc.type_name.toLocaleLowerCase()),
  //   [nodeDesc]
  // )
  const displayTypeName = true

  return (
    <div
      className={cn(
        'px-3 py-1.5 flex gap-2 items-center hover:bg-white/5 transition-colors-opacity tracking-wide text-center w-full hover:cursor-pointer',
        active && 'bg-white/10'
      )}
      onClick={onClick}
    >
      <div className="grid w-full grid-cols-4 items-center">
        <div className="col-span-1 flex place-content-center items-center gap-2">
          <NodeIcon className="m-0 size-4 min-h-4 min-w-4 p-0" style={{ color: iconTheme[1] }} />
          <span>{nodeDesc.name}</span>
        </div>

        <div className="col-span-1">
          <div className="text-xs text-btn-active/60">{t('common.path')}</div>
          <div className="text-sm text-text-primary/50">{nodeDesc.path}</div>
        </div>

        {displayTypeName && (
          <div className="col-span-1">
            <div className="text-xs text-btn-active/60">{t('command-palette.type-name')}</div>{' '}
            <div className="text-sm text-text-primary/50">{nodeDesc.type_name}</div>
          </div>
        )}

        <div className="col-span-1">
          <div className="text-xs text-btn-active/60">{t('node-graph.category')}</div>
          <div className="text-sm text-text-primary/50">{nodeDesc.category}</div>
        </div>
      </div>
    </div>
  )
}

export default memo(NodeItem)
