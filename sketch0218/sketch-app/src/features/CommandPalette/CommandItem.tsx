import { useI18n } from '@/locales'
import { cn } from '@/utils/cn'
import { memo } from 'react'
import { SearchCommandDesc } from './utils'

const CommandItem = ({
  cmdDesc,
  onClick,
  active
}: {
  cmdDesc: SearchCommandDesc
  onClick: () => void
  active: boolean
}) => {
  const { t } = useI18n()
  return (
    <div
      className={cn(
        'px-3 py-1.5 flex gap-2 items-center hover:bg-white/5 transition-colors-opacity tracking-wide text-center w-full hover:cursor-pointer',
        active && 'bg-white/10'
      )}
      onClick={onClick}
    >
      <div className="grid w-full grid-cols-4 items-center">
        <div className="col-span-2">{t(cmdDesc.label as any)}</div>
        <div className="col-span-1 grow text-sm text-text-primary/60">{cmdDesc.info}</div>
        <div className="col-span-1 text-text-primary/50">{cmdDesc.shortcut}</div>
      </div>
    </div>
  )
}

export default memo(CommandItem)
