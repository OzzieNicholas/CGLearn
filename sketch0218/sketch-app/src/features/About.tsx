import {
  Dialog,
  DialogButton,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle
} from '@/components/Dialog'
import { useI18n } from '@/locales'
import { getEnv } from '@/utils/env'
import NiceModal, { useModal } from '@ebay/nice-modal-react'
import { getName, getTauriVersion, getVersion } from '@tauri-apps/api/app'
import { writeText } from '@tauri-apps/plugin-clipboard-manager'
import { useCallback, useState } from 'react'
import { useEffectOnce } from 'react-use'

function About() {
  const modal = useModal()
  const { t } = useI18n()
  const env = getEnv()

  const [infos, setInfos] = useState([])
  const [appName, setAppName] = useState('')

  const getInfos = useCallback(async () => {
    return [
      ['Version', await getVersion()],
      ['Tauri', await getTauriVersion()],
      ['OS', `${env.os} ${env.arch} ${env.os_version}`],
      ['Locale', env.locale]
    ]
  }, [])

  useEffectOnce(() => {
    getInfos().then((v) => setInfos(v as any))
    getName().then((n) => setAppName(n))
  })

  return (
    <Dialog
      open
      onOpenChange={(open) => {
        if (!open) modal.remove()
      }}
    >
      <DialogContent>
        <DialogHeader>
          <DialogTitle>
            {t('common.about')} {appName} ...
          </DialogTitle>
        </DialogHeader>

        <img src="/nous.svg" className="pointer-events-none mx-auto size-40 select-none"></img>
        <div className="grid gap-4 py-4">
          {infos.map((v, idx) => (
            <div className="grid grid-cols-2 items-center gap-4 self-center" key={idx}>
              <label className="self-center bg-transparent text-right font-bold ">
                {t(v[0]!)}
                <span>:</span>
              </label>
              <label className="col-span-1 grow self-center bg-transparent font-bold">
                {t(v[1]!)}
              </label>
            </div>
          ))}
        </div>

        <DialogFooter>
          <div className="z-10 flex gap-1">
            <DialogButton
              onClick={() => {
                writeText(infos.map((info) => `${info[0]}: ${info[1]}`).join('\n'))
                modal.remove()
              }}
            >
              {t('common.copy')}
            </DialogButton>
            <DialogButton
              onClick={() => {
                modal.remove()
              }}
            >
              {t('common.ok')}
            </DialogButton>
          </div>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export default NiceModal.create(About)
