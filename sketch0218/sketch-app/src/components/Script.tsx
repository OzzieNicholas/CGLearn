import { python } from '@codemirror/lang-python'
import { StreamLanguage } from '@codemirror/language'
import { shader } from '@codemirror/legacy-modes/mode/clike'
import CodeMirror, { ViewUpdate } from '@uiw/react-codemirror'
import { useEffect, useState } from 'react'
import { dslLanguage, dslWordHover } from './detail/codemirror-dsl'
import { luaLanguage, luaWordHover } from './detail/codemirror-lua'

import '@/css/codemirror.css'

export const langs = {
  dsl: () => undefined,
  python,
  lua: () => undefined,
  glsl: () => StreamLanguage.define(shader),
  hlsl: () => StreamLanguage.define(shader),
  none: () => undefined
}

type LanguageName = keyof typeof langs

function loadLanguage(name: LanguageName) {
  return langs[name] ? langs[name]() : undefined
}

export default function Script({
  lang,
  onChange,
  onBlur,
  className,
  defaultValue,
  value,
  readonly,
  autoFocus
}: {
  lang: LanguageName
  defaultValue?: string
  onChange?: (value: string) => void
  onBlur?: () => void
  className?: string
  value?: string
  readonly?: boolean
  autoFocus?: boolean
}) {
  const [currentValue, setCurrentValue] = useState(value || defaultValue || '')
  const [extensions, setExtensions] = useState(undefined)

  useEffect(() => {
    if (value !== undefined && value !== currentValue) {
      setCurrentValue(value)
    }
  }, [value])

  useEffect(() => {
    if (lang === 'dsl') {
      dslLanguage().then((dsl) => {
        setExtensions([StreamLanguage.define(dsl), dslWordHover] as any)
      })
    } else if (lang === 'lua') {
      setExtensions([StreamLanguage.define(luaLanguage()), luaWordHover] as any)
    } else if (lang === 'none') {
      setExtensions(undefined)
    } else {
      const ext = loadLanguage(lang)
      if (!ext) {
        setExtensions(undefined)
      } else {
        setExtensions([ext] as any)
      }
    }
  }, [lang])

  return (
    <CodeMirror
      className={className}
      value={currentValue}
      theme="dark"
      minHeight="1.5rem"
      maxHeight="25rem"
      extensions={extensions}
      onChange={(value: string, viewUpdate: ViewUpdate) => {
        if (viewUpdate.docChanged) {
          setCurrentValue(value)
        }
      }}
      basicSetup={{
        highlightActiveLine: false,
        foldGutter: true,
        autocompletion: true,
        searchKeymap: true,
        completionKeymap: true,
        lineNumbers: extensions !== undefined
      }}
      onBlur={
        onChange
          ? () => {
              const result = currentValue.trim().length == 0 ? '' : currentValue
              if (result !== value) onChange(result)
              if (onBlur) onBlur()
            }
          : onBlur
      }
      editable={!readonly}
      autoFocus={autoFocus}
    />
  )
}
