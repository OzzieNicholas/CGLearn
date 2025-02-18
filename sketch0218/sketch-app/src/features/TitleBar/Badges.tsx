import Tooltip from '@/components/Tooltip'
import { useI18n } from '@/locales'
import { useTitleBarStore } from '@/stores/title-bar'
import { cn } from '@/utils/cn'

export default function Badges() {
  const { t } = useI18n()
  const badges = useTitleBarStore.use.badges()

  return (
    <div className="flex gap-1 pr-2">
      {badges.map((badge) => (
        <Tooltip content={badge.info} placement="bottom" key={badge.label}>
          <div
            className={cn(
              badge.action
                ? 'btn btn-outline btn-xs hover:bg-white/5'
                : 'badge badge-outline badge-xs',
              'z-10 h-5 min-h-0'
            )}
            style={{
              color: badge.color,
              borderColor: badge.color
            }}
            onClick={badge.action}
          >
            {t(badge.label as any)}
          </div>
        </Tooltip>
      ))}
    </div>
  )
}
