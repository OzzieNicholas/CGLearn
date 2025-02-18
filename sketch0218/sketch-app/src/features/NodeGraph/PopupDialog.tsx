import TripleChoiceDialog from '@/components/TripleChoiceDialog'
import { NodeGraphStoreType } from '@/stores/node-graph'
import { closeWindow } from '@/utils/window-state'
import NiceModal, { useModal } from '@ebay/nice-modal-react'

export enum PopupDialogType {
  SaveBeforeQuit,
  SaveBeforeOpen,
  SaveBeforeNew,
  PublishNda
}

const NodeGraphPopupDialog = ({
  storeApi,
  type,
  url
}: {
  storeApi: NodeGraphStoreType
  type: PopupDialogType
  url?: string
}) => {
  const modal = useModal()

  let header: string = ''
  let message: string = ''
  let optionA: string = ''
  let optionB: string = ''
  const optionC = 'common.cancel'
  let onOptionA: () => Promise<void> = undefined!
  let onOptionB: () => Promise<void> = undefined!

  const closePopupDialog = () => {
    modal.remove()
  }

  switch (type) {
    case PopupDialogType.SaveBeforeQuit: {
      header = 'common.quit'
      message = 'message.confirm-save-before-quit'
      optionA = 'message.save-and-quit'
      optionB = 'message.quit-anyway'

      onOptionA = async () => {
        await storeApi.getState().save()
        closeWindow()
      }
      onOptionB = async () => {
        closeWindow()
      }
      break
    }
    case PopupDialogType.SaveBeforeOpen: {
      header = 'common.open'
      message = 'message.confirm-save-before-open'
      optionA = 'message.save-and-open'
      optionB = 'message.open-anyway'

      onOptionA = async () => {
        closePopupDialog()
        await storeApi.getState().save()
        storeApi.getState().load(url)
      }
      onOptionB = async () => {
        closePopupDialog()
        storeApi.getState().load(url)
      }
      break
    }
    case PopupDialogType.SaveBeforeNew: {
      header = 'common.new'
      message = 'message.confirm-save-before-new'
      optionA = 'message.save-and-new'
      optionB = 'message.new-anyway'

      onOptionA = async () => {
        closePopupDialog()
        await storeApi.getState().save()
        storeApi.getState().clear()
      }
      onOptionB = async () => {
        closePopupDialog()
        storeApi.getState().clear()
      }
      break
    }
  }

  return (
    <TripleChoiceDialog
      header={header}
      message={message}
      optionA={optionA}
      optionB={optionB}
      optionC={optionC}
      onOptionA={onOptionA}
      onOptionB={onOptionB}
      onOptionC={closePopupDialog}
      onOpenChange={(open) => {
        if (!open) modal.remove()
      }}
    />
  )
}

export default NiceModal.create(NodeGraphPopupDialog)
