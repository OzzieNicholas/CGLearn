import {
  Dialog,
  DialogButton,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle
} from '@/components/Dialog'
import NumberInput, { NumberValueType } from '@/components/NumberInput'
import TextInput from '@/components/TextInput'
import { useI18n } from '@/locales'
import { NodeGraphStoreType } from '@/stores/node-graph'
import { NNode, NdaSaveParams } from '@/types/node-graph'
import NiceModal, { useModal } from '@ebay/nice-modal-react'
import { save as saveFileDialog } from '@tauri-apps/plugin-dialog'
import { useState } from 'react'
import { FaFolder } from 'react-icons/fa'
import { useEffectOnce } from 'react-use'

const NodePublisher = ({ node, storeApi }: { node: NNode; storeApi: NodeGraphStoreType }) => {
  const modal = useModal()

  const { t } = useI18n()
  const [params, setParams] = useState<NdaSaveParams>({
    type_name: '',
    category: '',
    url: '',
    type_version: 1
  })

  const closePopupDialog = () => {
    modal.remove()
  }

  useEffectOnce(() => {
    storeApi
      .getState()
      .getNdaSaveParams(node.id)
      .then((v) => {
        if (v !== null) {
          setParams(v)
        } else {
          closePopupDialog()
        }
      })
  })

  return (
    <Dialog
      open={modal.visible}
      onOpenChange={(open) => {
        if (!open) modal.remove()
      }}
    >
      <DialogContent>
        <DialogHeader>
          <DialogTitle className="select-none">{t('node-graph.publish-node')} ...</DialogTitle>
        </DialogHeader>

        <div className="grid gap-4 py-4">
          <div className="grid grid-cols-4 items-center gap-4">
            <label className="select-none self-center bg-transparent text-right font-bold">
              {t('node-graph.node-name')}
            </label>

            <TextInput
              className="col-span-3 rounded-md border-1 border-white/20"
              value={params.type_name}
              onChange={(type_name) => setParams({ ...params, type_name })}
            />
          </div>

          <div className="grid grid-cols-4 items-center gap-4">
            <label className="select-none self-center bg-transparent text-right font-bold">
              {t('node-graph.node-version')}
            </label>

            <NumberInput
              valueType={NumberValueType.Int}
              className="col-span-3 rounded-md border-1 border-white/20"
              value={params.type_version}
              limitMin={0}
              onChange={(type_version) => {
                setParams({ ...params, type_version })
              }}
            />
          </div>

          <div className="grid grid-cols-4 items-center gap-4">
            <label className="select-none self-center bg-transparent text-right font-bold">
              {t('node-graph.category')}
            </label>

            <TextInput
              className="col-span-3 rounded-md border-1 border-white/20"
              value={params.category}
              onChange={(category) => setParams({ ...params, category })}
            />
          </div>

          <div className="grid select-none grid-cols-4 items-center gap-4">
            <label className="self-center bg-transparent text-right font-bold">
              {t('common.save-path')}
            </label>

            <div className="col-span-3 flex h-8 items-center">
              <TextInput
                className="w-full rounded-md border-1 border-white/20"
                value={params.url}
                onChange={(url) => setParams({ ...params, url })}
              />

              <div
                className="btn btn-ghost btn-xs ml-1 h-full min-h-0 p-1"
                onClick={() => {
                  // open a dialog
                  saveFileDialog({
                    defaultPath: params.url,
                    filters: [
                      {
                        name: 'nous node file',
                        extensions: ['nda', 'node']
                      }
                    ]
                  }).then((path) => {
                    if (!path) {
                      return
                    }
                    setParams({ ...params, url: path })
                  })
                }}
              >
                <FaFolder className="size-full text-yellow-400" />
              </div>
            </div>
          </div>
        </div>

        <DialogFooter>
          <div className="flex gap-1">
            <DialogButton
              onClick={() => {
                storeApi.getState().publishNda(node.id, params)
                closePopupDialog()
              }}
              disabled={
                params.type_name.length == 0 ||
                params.category.length == 0 ||
                params.url.length == 0
              }
            >
              {t('common.save')}
            </DialogButton>
            <DialogButton onClick={closePopupDialog}>{t('common.cancel')}</DialogButton>
          </div>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export default NiceModal.create(NodePublisher)
