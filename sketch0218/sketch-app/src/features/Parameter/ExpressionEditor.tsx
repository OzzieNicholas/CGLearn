import { Parameter } from '@/commands/index.types'
import Script from '@/components/Script'
import { isNotEmptyString } from '@/utils/functions'

function ExpressionEditor({
  param,
  readonly,
  onChange,
  onBlur
}: {
  param: Parameter
  onChange?: (value: string) => void
  onBlur?: () => void
  readonly?: boolean
}) {
  return (
    <div className="flex min-w-52">
      <Script
        className="size-full text-sm"
        value={param.expression}
        lang="lua"
        onChange={onChange}
        onBlur={onBlur}
        readonly={readonly}
        autoFocus={!readonly && isNotEmptyString(param.expression)}
      />
    </div>
  )
}

export default ExpressionEditor
