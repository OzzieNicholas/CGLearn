import { useI18n } from '@/locales'
import {
  Dialog,
  DialogButton,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle
} from './Dialog'

const TripleChoiceDialog = ({
  header,
  message,
  optionA,
  optionB,
  optionC,
  onOptionA,
  onOptionB,
  onOptionC,
  onOpenChange
}: {
  header: string
  message: string
  optionA: string
  optionB: string
  optionC: string
  onOptionA?: () => any
  onOptionB?: () => any
  onOptionC?: () => any
  onOpenChange?: (open: boolean) => void
}) => {
  const { t } = useI18n()
  return (
    <Dialog open onOpenChange={onOpenChange}>
      <DialogContent>
        <DialogHeader>
          <DialogTitle>{t(header as any)} ...</DialogTitle>
        </DialogHeader>

        <div className="flex w-full">
          <h1 className="m-auto font-bold">{t(message as any)}</h1>
        </div>

        <DialogFooter>
          <div className="z-10 flex gap-1">
            <DialogButton onClick={onOptionA}>{t(optionA as any)}</DialogButton>
            <DialogButton onClick={onOptionB}>{t(optionB as any)}</DialogButton>
            <DialogButton onClick={onOptionC}>{t(optionC as any)}</DialogButton>
          </div>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  )
}

export default TripleChoiceDialog
