import { getDslLanguageReference } from '@/commands'
import { DslLanguageReference } from '@/commands/index.types'
import { completeFromList, Completion } from '@codemirror/autocomplete'
import { c } from '@codemirror/legacy-modes/mode/clike'
import { hoverTooltip } from '@codemirror/view'

let _dslLanguage = { ...c }

_dslLanguage.languageData = {
  ...(_dslLanguage.languageData ? _dslLanguage.languageData : {}),
  commentTokens: { line: '//' },
  closeBrackets: { brackets: ['(', '[', '{', '"'] }
}

let ref: DslLanguageReference | undefined | null = undefined

export const dslWordHover = hoverTooltip((view, pos, side) => {
  if (!ref) return null
  let { from, to, text } = view.state.doc.lineAt(pos)
  let start = pos,
    end = pos
  while (start > from && /\w/.test(text[start - from - 1])) start--
  while (end < to && /\w/.test(text[end - from])) end++
  if ((start == pos && side < 0) || (end == pos && side > 0)) return null
  const hover = text.slice(start - from, end - from)
  const func = ref.functions.find((f) => f.name === hover)
  if (!func) return null

  return {
    pos: start,
    end,
    above: true,
    create() {
      let dom = document.createElement('div')
      dom.textContent = func.doc
      return { dom }
    }
  }
})

export const dslLanguage = async () => {
  if (ref === undefined) {
    ref = await getDslLanguageReference()
    let completions: Completion[] = []
    if (ref) {
      for (const kw of ref.keywords) {
        completions.push({ label: kw, type: 'keyword' })
      }
      for (const op of ref.operators) {
        completions.push({ label: op, type: 'operator' })
      }
      for (const t of ref.types) {
        completions.push({ label: t, type: 'type' })
        completions.push({ label: t + '@', type: 'type' })
      }
      const attribKeys = ['v', 'f', 'i', 'b', 'd', 's']
      for (const k of attribKeys) {
        completions.push({ label: k + '@', type: 'type' })
        completions.push({ label: k + '[]@', type: 'type' })
      }

      for (const func of ref.functions) {
        let info = func.doc + '\n'
        let i = 0
        for (const overload of func.overloads) {
          info += '\n' + overload
          if (i++ > 5) {
            info += '\n...'
            break
          }
        }
        completions.push({ label: func.name, type: 'function', info })
      }
      completions.push({ label: 'i@idx', type: 'variable' })

      _dslLanguage.languageData = {
        ...(_dslLanguage.languageData ? _dslLanguage.languageData : {}),
        commentTokens: { line: '//' },
        // eslint-disable-next-line quotes
        closeBrackets: { brackets: ['(', '[', '{', "'", '"'] }, // prettier-ignore
        keywords: [],
        autocomplete: completeFromList(completions)
      }

      const keywords = completions.filter((c) => c.type === 'keyword')
      const types = completions.filter((c) => c.type === 'type')
      const functions = completions.filter((c) => c.type === 'function')
      const operators = completions.filter((c) => c.type === 'operator')

      const keywordRegex = new RegExp(`\\b(${keywords.map((k) => k.label).join('|')})\\b`, 'g')
      const functionRegex = new RegExp(`\\b(${functions.map((f) => f.label).join('|')})\\s*\\(`)
      const escapeRegExp = (str: string) => {
        return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')
      }
      const operatorRegex = new RegExp(operators.map((op) => escapeRegExp(op.label)).join('|'))
      const typeNames = types.map((t) => escapeRegExp(t.label))
      const typesRegex = new RegExp(
        `(^|\\s)(${typeNames.map((t) => t + '(\\[\\])?').join('|')})(?=\\s|$)`
      )
      const attribRegex = new RegExp(
        `${typeNames.concat(attribKeys).map((t) => t + '(\\[\\])?\\@').join('|')}`
      )

      _dslLanguage.token = (stream, state) => {
        if (stream.match('//')) {
          stream.skipToEnd()
          return 'comment'
        }

        if (stream.match('i@idx')) {
          return 'color'
        }

        if (stream.match(keywordRegex)) {
          return 'keyword'
        }

        if (stream.match(functionRegex)) {
          stream.backUp(1)
          return 'function'
        }

        if (stream.match(operatorRegex)) {
          return 'operator'
        }

        if (stream.match(typesRegex) || stream.match(attribRegex)) {
          return 'typeName'
        }

        return c.token(stream, state)
      }
    } else {
      ref = null
    }
  }
  return _dslLanguage
}
