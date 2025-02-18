import RecursiveMenu from '@/components/RecursiveMenu'
import { useI18n } from '@/locales'
import { useTitleBarStore } from '@/stores/title-bar'

export default function Menus() {
  const { t } = useI18n()
  const menus = useTitleBarStore.use.menus()

  return (
    <div className="flex flex-nowrap items-center gap-0">
      {menus.map((menu) => {
        const menu_btn = (
          <div
            className="btn btn-ghost btn-xs p-1 font-thin tracking-wider text-text-primary"
            onClick={menu.action}
            key={menu.label}
          >
            {t(menu.label as any)}
          </div>
        )
        if (menu.sub_menus.length == 0) {
          return (
            <div className="p-0" key={menu.label}>
              {menu_btn}
            </div>
          )
        }
        return (
          <RecursiveMenu
            key={menu.label}
            menus={menu.sub_menus}
            modal={false}
            className="animate-fade-in"
          >
            {menu_btn}
          </RecursiveMenu>
        )
      })}
    </div>
  )
}
