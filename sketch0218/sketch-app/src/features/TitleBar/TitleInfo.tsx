import { useTitleBarStore } from '@/stores/title-bar'
import NiceModal from '@ebay/nice-modal-react'
import { CommandPalette } from '@/features/CommandPalette'

export default function TitleInfo() {
  const info = useTitleBarStore.use.info()

  return (
    <button
      className="btn btn-ghost btn-xs font-sans	font-light text-text-primary"
      onClick={() => NiceModal.show(CommandPalette)}
    >
      <p className="truncate">{info}</p>
    </button>
  )
}
