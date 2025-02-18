import { Icons } from '@/components/WindowControlIcons'
import { emit as emitGlobalAction } from '@/stores/global-actions'
import { setupWindowStore, useWindowStore } from '@/stores/window'
import { cn } from '@/utils/cn'
import { isMacOS } from '@/utils/env'
import { minimizeWindow, toggleWindowMaximize } from '@/utils/window-state'
import { ReactNode, useState } from 'react'
import { useEffectOnce } from 'react-use'

function Decoration({
  icon,
  callback,
  className
}: {
  icon: ReactNode
  callback: () => any
  className?: string
}) {
  return (
    <div
      className={cn(
        'btn btn-ghost btn-xs mr-0 aspect-square h-titlebar rounded-none focus:outline-none text-text-primary/90',
        className
      )}
      onClick={callback}
    >
      {icon}
    </div>
  )
}

function MacDecoration({
  icon,
  callback,
  className
}: {
  icon: ReactNode
  callback: () => any
  className?: string
}) {
  const [hovered, setHovered] = useState<boolean>(false)

  return (
    <div
      className={cn(
        'transition-colors duration-[10] rounded-full size-3 flex place-content-center place-items-center text-black/50 border border-black/15 cursor-default',
        className
      )}
      onClick={callback}
      onPointerOver={() => setHovered(true)}
      onPointerOut={() => setHovered(false)}
    >
      {hovered && icon}
    </div>
  )
}

export default function Decorations() {
  const is_macos = isMacOS()
  const is_maximized = is_macos ? false : useWindowStore.use.is_maximized()

  useEffectOnce(() => {
    if (!useWindowStore.getState().is_initialized) {
      setupWindowStore()
    }
  })

  return (
    <>
      {is_macos ? (
        <div className="z-10 mx-2 flex gap-2 self-center">
          <MacDecoration
            callback={() => emitGlobalAction('quit')}
            icon={<Icons.closeMac />}
            className="bg-[#ff544d] hover:bg-[#bf403a]"
          />
          <MacDecoration
            callback={minimizeWindow}
            icon={<Icons.minMac />}
            className="bg-[#ffbd2e] hover:bg-[#bf9122]"
          />
          <MacDecoration
            callback={toggleWindowMaximize}
            icon={<Icons.fullMac />}
            className="bg-[#28c93f] hover:bg-[#1e9930]"
          />
        </div>
      ) : (
        <div className="z-10 flex gap-0 self-center">
          <Decoration callback={minimizeWindow} icon={<Icons.minimizeWin />} />
          <Decoration
            callback={toggleWindowMaximize}
            icon={is_maximized ? <Icons.maximizeRestoreWin /> : <Icons.maximizeWin />}
          />
          <Decoration
            callback={() => emitGlobalAction('quit')}
            icon={<Icons.closeWin />}
            className="hover:bg-[#c42b1c]"
          />
        </div>
      )}
    </>
  )
}
