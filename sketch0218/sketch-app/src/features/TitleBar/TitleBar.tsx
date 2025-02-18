import { isMacOS } from '@/utils/env'
import Badges from './Badges'
import Controls from './Controls'
import Menus from './Menus'
import TitleInfo from './TitleInfo'

export function TitleBar() {
  const is_macos = isMacOS()

  return (
    <div className="flex w-full">
      {is_macos && <Controls />}
      <div className="navbar h-titlebar min-h-0 bg-transparent pr-0">
        <div className="navbar-start" data-tauri-drag-region>
          <img
            className="pointer-events-none mr-1 size-4 select-none"
            src="/nous.svg"
            alt="nous logo"
          />
          <div className="z-10 ">
            <Menus />
          </div>
        </div>

        <div className="navbar-center z-10">
          <TitleInfo />
        </div>

        <div className="navbar-end mr-0 pr-0" data-tauri-drag-region>
          <Badges />
        </div>
      </div>
      {!is_macos && <Controls />}
    </div>
  )
}
