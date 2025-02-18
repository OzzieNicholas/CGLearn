import { SearchParamDesc } from '@/commands/index.types'
import { getIconTheme, iconTypes } from '@/constant/node-resource'
import { useI18n } from '@/locales'
import { cn } from '@/utils/cn'
import { memo, useMemo } from 'react'

const ParamItem = ({
  paramDesc,
  onClick,
  active
}: {
  paramDesc: SearchParamDesc
  onClick: () => void
  active: boolean
}) => {
  const { t } = useI18n()
  const iconTheme = useMemo(() => getIconTheme(paramDesc.node_category), [paramDesc.node_category])
  const NodeIcon = iconTypes[paramDesc.node_type_name] || iconTypes.default

  return (
    <div
      className={cn(
        'px-3 py-1.5 flex gap-2 items-center hover:bg-white/5 transition-colors-opacity tracking-wide text-center w-full hover:cursor-pointer',
        active && 'bg-white/10'
      )}
      onClick={onClick}
    >
      <div className="grid w-full grid-cols-4 items-center gap-2">
        <div className="col-span-1">
          <div className="text-xs text-btn-active/60">{t('command-palette.display-name')}</div>
          <div className="text-sm text-text-primary">
            {paramDesc.display_name || paramDesc.name}
          </div>
        </div>

        <div className="col-span-1">
          <div className="text-xs text-btn-active/60">{t('common.path')}</div>
          <div className="text-sm text-text-primary/50">
            {paramDesc.node_path}/<span className="text-text-primary">{paramDesc.name}</span>
          </div>
        </div>

        <div className="col-span-1">
          <div className="text-xs text-btn-active/60">{t('parameter.type')}</div>
          <div className="text-sm text-text-primary/50">{paramDesc.type}</div>
        </div>

        <div className="col-span-1 flex-col items-center">
          <div className="text-xs text-btn-active/60">{t('command-palette.node')}</div>
          <div className="flex place-content-center items-center gap-2 text-text-primary/50">
            <NodeIcon
              className="m-0 size-3.5 min-h-3.5 min-w-3.5 p-0"
              style={{ color: iconTheme[1] }}
            />
            <span className="text-sm text-text-primary/50 ">{paramDesc.node_name}</span>
          </div>
        </div>
      </div>
    </div>
  )
}

export default memo(ParamItem)
